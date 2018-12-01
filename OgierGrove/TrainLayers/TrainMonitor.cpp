/*
__/\\\\\\\\\\\\\_____/\\\\\\\\\\\__/\\\\\\\\\\\\____        
 _\/\\\/////////\\\__\/////\\\///__\/\\\////////\\\__       
  _\/\\\_______\/\\\______\/\\\_____\/\\\______\//\\\_      
   _\/\\\\\\\\\\\\\/_______\/\\\_____\/\\\_______\/\\\_     
    _\/\\\/////////_________\/\\\_____\/\\\_______\/\\\_    
     _\/\\\__________________\/\\\_____\/\\\_______\/\\\_   
      _\/\\\___________/\\\___\/\\\_____\/\\\_______/\\\__  
       _\/\\\__________\//\\\\\\\\\______\/\\\\\\\\\\\\/___
        _\///____________\/////////_______\////////////_____
-> Name:  TrainMonitor.cpp
-> Date: Nov 26, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainMonitor.h"
#include "Includes/TrainCommandCenter.h"

// Singleton Instance
TrainMonitor *TrainMonitor::TrainMonitorInstance_ = 0;

/*
    Function: CheckMessageHandler
    Brief: Checks the ISR message queue and handles message if one is present,
       constantly polled from CentralLoop
*/
void TrainMonitor::CheckMessageHandler() {
    static MsgType_t type = NONE;
    static char data = char();

    // Check the ISR message queue and handle the result
    // NOTE: This is a blocking process, but will trigger at ~100Hz
    ISRMsgHandlerInstance_->GetFromISRQueue(type, data);
    switch (type) {
        case NONE:
            // Meaning the queue is empty
            break;
        case UART:
            InputUARTChar(data);
            break;
        case SYSTICK:
            break;
        default:
            std::cout << "[CheckMessageHandler()] SWITCH TABLE ERROR: >>" << type << "<<\n";
            break;
    }

    uint8_t src_q;
    uint32_t msg_len;
    static char msg_body[SMALL_LETTER];

    // If there is a message, print it
    if (PRecv(src_q, TRAIN_MONITOR_MB, &msg_body, msg_len, false)) {
        if(msg_len >= 1) {
            switch(msg_body[0]) {
                case '\xA0': // Hall sensor update from DLL
                    assert(msg_len == 2 || msg_len == 3);
                    VisuallySetHallSensor(msg_body[1], (msg_len == 3 ? true : false));
                    break;
                case '\xC0':
                    assert(msg_len == 3);
                    VisuallyUpdateTrainInfo(msg_body[1], ((train_settings_t *)(&msg_body[2]))->speed, (train_direction_t)((train_settings_t *)(&msg_body[2]))->dir);
                    break;
                case ZONE_CHANGE: // Zone change update from the train controller
                    assert(msg_len == 4);
                    VisuallySetTrainLocation(msg_body[1], msg_body[2], msg_body[3]);
                    break;
                case TRAIN_GO_CMD: // Train Speed/Dir change from controller
                    assert(msg_len == 3);
                    VisuallyUpdateTrainDst(msg_body[1], msg_body[2]);
                    break;
                default:
                    // TODO: Some sort of error/warning here
                    break;
            }
        }
    }
}

/*
    Function: TrainMonitor
    Brief: Constructor for the TrainMonitor class, which initializes the data_buffer_ on startup
*/
TrainMonitor::TrainMonitor() {
    CupReset_ = false;
}

/*
    Function: ~TrainMonitor
    Brief: Destructor for the TrainMonitor class, which deletes the data_buffer_ on shutdown
*/
TrainMonitor::~TrainMonitor() {
}

/*
    Function: SingletonGrab
    Brief: Setup function for the TrainMonitor to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void TrainMonitor::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
}

/*
    Function: CentralLoop
    Brief: 
*/
void TrainMonitor::CentralLoop() {
    // SingletonGrab(); // Done in startup code now.

    // Bind TrainMonitor queue
    PBind(ISR_MSG_HANDLER_MB, ONE_CHAR, ISR_QUEUE_SIZE);
    PBind(TRAIN_MONITOR_MB, SMALL_LETTER); // Default mailbox size of 16

    // Initialize screen with GUI
    InitializeScreen();

    // Loop Forever
    while(1)
    {
        CheckMessageHandler();
    }
}

void TrainMonitor::InputUARTChar(char input_char) {
    // Print uart to screen using CUP
    static char single_char[1]; // Used to output a character
    static std::string command_string = ""; // Used to build command
    static uint8_t command_string_len = 0;
    static int8_t escape_mode = -1;

    // If cup position has been reset, 
    if (CupReset_) {
        // Reset all VT-100 commands
        ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);

        // Output cup command to reset cursor position
        PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COLUMN + command_string_len);

        // Clear flag for future use
        CupReset_ = false;
    }

    // VT-100 Input Code handling: If in escape mode, skip next two characters
    //      Escape mode is denoted by escape_mode equalling 0 or 1
    if (escape_mode != -1) {
        // However, if one of the skipped chacters is a number, skip an extra one
        if (input_char >= '0' && input_char <= '9') return;
        if (++escape_mode == 2) escape_mode = -1;
        return;
    }

    switch (input_char) {
        case 0x08: // Backspace
        case 0x7F: // Delete
            // Do not delete if there is nothing in the buffer
            if (command_string_len != 0) {
                // Update command_string
                command_string.pop_back();
                command_string_len--;

                // Output DEL character to screen
                single_char[0] = '\x7F';
                ISRMsgHandlerInstance_->QueueOutputMsg(single_char, UART0);
            }

            break;
        case 0x0D: // Enter (Carriage Return)
            // Handle command
            TrainCommandCenter::GetTrainCommandCenter()->HandleCommand(command_string);

            // Update command string
            command_string = "";
            command_string_len = 0;
            CupReset_ = true;

            // Reset the prompt
            ResetCommandLine();
            break;

        case 0x1B: // Escape
            // Enter backslash mode, ignoring next two characters
            escape_mode = 0;
            break;

        default:  // All other characters, add to buffer if not already full
            if (command_string_len < TRAIN_DATA_BUFFER_SIZE) {
                // Update command string
                command_string += input_char;
                command_string_len++;

                // Output character
                single_char[0] = input_char;
                ISRMsgHandlerInstance_->QueueOutputMsg(single_char, UART0);
            }
            break;
    }
}

void TrainMonitor::ResetCommandLine() {
    PrintCup(CMD_PROMPT_START_ROW, 0);

    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);
    ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[CMD_PROMPT_START_ROW - 1] + "\r", UART0);

    PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COLUMN);
}

void TrainMonitor::PrintDefaultScreen() {
    std::stringstream sstream;
    sstream << "\n\r" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    for(int i = 0; i < NUM_SCREEN_ROWS; i++) {
        PrintCup(i + 1, 0);
        ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[i] + "\r", UART0);
    }
}

void TrainMonitor::PrintCup(int row, int col) {
    std::stringstream sstream;
    sstream << "\e[" << row << ";" << col << "H";
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
}

void TrainMonitor::InitializeScreen() {
    ISRMsgHandlerInstance_->QueueOutputMsg(CLEAR_SCREEN, UART0);
    ISRMsgHandlerInstance_->QueueOutputMsg(CLEAR_SCREEN, UART0);

    // Print default screen
    PrintDefaultScreen();

    std::stringstream sstream;

    // Highlight trains and set their information

    // TODO: Make this a loop, everything should be functionized by now

    // Train 1 is green
    PrintCup(train_info_locations[0][0], train_info_locations[0][1]);
    sstream << VT_100_BR_COLORS[train_colors[0]] << VT_100_FONT_COLORS[BLACK] << "Train 1" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    VisuallyUpdateTrainInfo(1, 0, STAY);
    VisuallyUpdateTrainDst(1, NO_ZONE);

    sstream.str(std::string());
    sstream.clear();

    // Train 2 is Yellow
    PrintCup(train_info_locations[1][0], train_info_locations[1][1]);
    sstream << VT_100_BR_COLORS[train_colors[1]] << VT_100_FONT_COLORS[BLACK] << "Train 2" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    VisuallyUpdateTrainInfo(2, 0, STAY);
    VisuallyUpdateTrainDst(2, NO_ZONE);

    // To correspond with initial state of the trainset
    VisuallySetSwitch(255, STRAIGHT);

    // Set prompt to starting position 
    PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COLUMN);
}

void TrainMonitor::VisuallySetHallSensor(uint8_t sensor_num, bool status) {
    std::stringstream sstream;

    // Set all sensors high or low
    if (sensor_num == 255) {

        // Set VT-100 color just once
        if (status) sstream << VT_100_BR_COLORS[BLUE];
        else sstream << VT_100_RESET;

        ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

        for (int i = 1; i <= MAX_NUM_SENSORS; i++) {
            // Set cursor at correct location
            TrainMonitor::PrintCup(sensor_locations[i][ROW], sensor_locations[i][COL]);

            // 1) Add characters to update
            // 2) Reset VT-100 codes
            ISRMsgHandlerInstance_->QueueOutputMsg("##", UART0);
        }
    }
    else if (sensor_num != 0 && sensor_num <= 24) { // Else set individual sensor
        // Set cursor at correct location
        TrainMonitor::PrintCup(sensor_locations[sensor_num][ROW], sensor_locations[sensor_num][COL]);

        // 1) Set background color depending on status
        if (status) sstream << VT_100_BR_COLORS[BLUE];
        else sstream << VT_100_RESET;
        // 2) Add characters to update
        // 3) Reset VT-100 codes
        sstream << "##" << VT_100_RESET;
        ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    }
    else {
        // TODO: Error state
    }

    // Set cup reset flag
    CupReset_ = true;
}

void TrainMonitor::VisuallySetSwitch(uint8_t switch_num, switch_direction_t dir) {
    std::stringstream sstream;

    // Reset all VT_100 Codes
    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);

    // Set all sensors high or low
    if (switch_num == 255) {
        for (int i = 1; i <= MAX_NUM_SWITCHES; i++) {
            switch_states[i] = dir;

            /* Handle straight portion first */

            // Set cursor at correct location
            TrainMonitor::PrintCup(switch_locations[i][STRAIGHT][ROW], switch_locations[i][STRAIGHT][COL]);

            if (dir == STRAIGHT) ISRMsgHandlerInstance_->QueueOutputMsg("---", UART0);
            else ISRMsgHandlerInstance_->QueueOutputMsg("   ", UART0);

            /* Handle diverted portion second */

            // Set cursor at correct location
            TrainMonitor::PrintCup(switch_locations[i][DIVERTED][ROW], switch_locations[i][DIVERTED][COL]);


            if (dir == DIVERTED) {
                switch (i) {
                    case 1:
                    case 2:
                    case 4:
                    case 5:
                        ISRMsgHandlerInstance_->QueueOutputMsg("//", UART0);
                        break;
                    case 3:
                    case 6:
                        ISRMsgHandlerInstance_->QueueOutputMsg("\\\\", UART0);
                        break;
                    default: // No possible error state
                        break;
                }
            }
            else ISRMsgHandlerInstance_->QueueOutputMsg("  ", UART0);

        }
    }
    else if (switch_num != 0 && switch_num <= 6) { // Else set individual switch
        switch_states[switch_num] = dir;

        /* Handle straight portion first */

        // Set cursor at correct location
        TrainMonitor::PrintCup(switch_locations[switch_num][STRAIGHT][ROW], switch_locations[switch_num][STRAIGHT][COL]);

        if (dir == STRAIGHT) ISRMsgHandlerInstance_->QueueOutputMsg("---", UART0);
        else ISRMsgHandlerInstance_->QueueOutputMsg("   ", UART0);

        /* Handle diverted portion second */

        // Set cursor at correct location
        TrainMonitor::PrintCup(switch_locations[switch_num][DIVERTED][ROW], switch_locations[switch_num][DIVERTED][COL]);


        if (dir == DIVERTED) {
            switch (switch_num) {
                case 1:
                case 2:
                case 4:
                case 5:
                    ISRMsgHandlerInstance_->QueueOutputMsg("//", UART0);
                    break;
                case 3:
                case 6:
                    ISRMsgHandlerInstance_->QueueOutputMsg("\\\\", UART0);
                    break;
                default: // No possible error state
                    break;
            }
        }
        else ISRMsgHandlerInstance_->QueueOutputMsg("  ", UART0);

    }
    else {
        // TODO: Error state
    }

    CupReset_ = true;
}

void TrainMonitor::VisuallyUpdateTrainInfo(uint8_t train_num, uint8_t speed, train_direction_t dir) {
    train_num--;
    assert(train_num < NUM_TRAINS);

    std::stringstream sstream;

    // Print speed
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][2]);
    sstream << "  " << '\x7F' << '\x7F' << (int)speed;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    sstream.str("");
    sstream.clear();

    // Print Direction
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][3]);
    sstream << "    " << '\x7F' << '\x7F' << '\x7F' << '\x7F' << train_direction_strings[dir];
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
}

void TrainMonitor::VisuallyUpdateTrainDst(uint8_t train_num, uint8_t dst) {
    train_num--;
    assert(train_num < NUM_TRAINS);

    std::stringstream sstream;

    // Print new destination 
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][4]);
    sstream << "    " << '\x7F' << '\x7F' << '\x7F' << '\x7F';
    if (dst != NO_ZONE) sstream << (int)dst;
    else sstream << "NONE";
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
}

void TrainMonitor::VisuallySetTrainLocation(uint8_t train_num, uint8_t new_zone, uint8_t prev_zone) {
    // Clear old zone (If not NO_ZONE)
    if (prev_zone < NUM_ZONES && prev_zone != NO_ZONE) PaintZone(prev_zone, NO_COLOR);

    // Color new zone
    assert(new_zone != NO_ZONE); //This should never be set to NO_ZONE
    if (new_zone < NUM_ZONES) PaintZone(new_zone, train_colors[train_num]);
}

void TrainMonitor::PaintZone(uint8_t zone, color_t color) {

    // Select background color (if any)
    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);
    if (color != NO_COLOR) {
        ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_BR_COLORS[color], UART0);
        ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_FONT_COLORS[BLACK], UART0);
    }

    uint8_t switch_num = IsSwitchZone(zone);

    // Handle differently if this is a zone that contains a switch
    if (switch_num) {
        assert(switch_states[switch_num] != NOT_NEEDED); // Ensure this is a valid state

        // If the switch is currently straight, highlight the straight portion
        if(switch_states[switch_num] == STRAIGHT) {
            // Subtracting by 1 to go from the 1 indexed CUP notation to 0 indexed strings within TrainScreen
            PrintCup(zone_locations[zone][0][0], zone_locations[zone][0][1]);
            ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[zone_locations[zone][0][0] - 1].substr(zone_locations[zone][0][1] - 1, zone_locations[zone][0][2]), UART0);
        }
        else { // Else highlight the diverted portion
            for(uint8_t i = 1; i < MAX_COLUMNS_PER_ZONE; i++) {
                PrintCup(zone_locations[zone][i][0], zone_locations[zone][i][1]);
                ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[zone_locations[zone][i][0] - 1].substr(zone_locations[zone][i][1] - 1, zone_locations[zone][i][2]), UART0);
            }
        }
    }
    else { // Else, not a switch zone, proceed normally
        for(uint8_t i = 0; i < MAX_COLUMNS_PER_ZONE; i++) {
            if (zone_locations[zone][i][0] == 0) break;
            PrintCup(zone_locations[zone][i][0], zone_locations[zone][i][1]);
            ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[zone_locations[zone][i][0] - 1].substr(zone_locations[zone][i][1] - 1, zone_locations[zone][i][2]), UART0);
        }
    }
}

// Essentially a macro
uint8_t TrainMonitor::IsSwitchZone(uint8_t zone) {
    switch (zone) {
        case 1:
            return 6;
        case 3:
            return 5;
        case 5:
            return 4;
        case 9:
            return 3;
        case 11:
            return 2;
        case 13:
            return 1;
        default:
            return 0;
    }
}

#define STATUS_ROW 3
#define STATUS_COL 5
void TrainMonitor::UpdateCommandStatus(color_t color) {
    PrintCup(STATUS_ROW, STATUS_COL);

    std::stringstream sstream;
    sstream << VT_100_BR_COLORS[color] << "CMD" << VT_100_RESET;

    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    CupReset_ = true;
}

/*
    Function: GetTrainMonitor
    Output: TrainMonitorInstance_: Pointer to the TrainMonitor Singleton
    Brief: Get function for the TrainMonitor singleton
*/
TrainMonitor* TrainMonitor::GetTrainMonitor() {
    if (!TrainMonitorInstance_) TrainMonitorInstance_ = new TrainMonitor;
    return TrainMonitorInstance_;
}
