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
    std::string tmp;

    // If there is a message, handle it
    if (PRecv(src_q, TRAIN_MONITOR_MB, &msg_body, msg_len, false)) {
        if(msg_len >= 1) {
            switch(msg_body[0]) {
                case '\xC0':
                    assert(msg_len == 3);
                    VisuallyUpdateTrainInfo(msg_body[1], ((train_settings_t *)(&msg_body[2]))->speed, (train_direction_t)((train_settings_t *)(&msg_body[2]))->dir);

                    // Display command to screen
                    VisuallyDisplayTX(msg_body, 3);
                    break;
                case HALL_SENSOR:
                    assert(msg_len == 2);
                    VisuallySetHallSensor(((sensor_msg_t *)msg_body)->num, ((sensor_msg_t *)msg_body)->state);
                    break;
                case SWITCH_CHANGE:
                    assert(msg_len == 2);
                    VisuallySetSwitch(((switch_msg_t *)msg_body)->ctrl.num, (switch_direction_t)(((switch_msg_t *)msg_body)->ctrl.req_state));
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
                    std::cout << "TrainMonitor::CheckMessageHandler(): Invalid switch case <<" << HEX(msg_body[0]) << " <<\n";
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

    for(uint8_t i = 0; i <= MAX_NUM_SWITCHES; i++) switch_states[i] = STRAIGHT;
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

/*
    Function: InputUARTChar
    Brief: Handles inputting a character and displaying it on the monitor
*/
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
        ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);
        ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_SHOW_CURSOR, UART0);

        // Output cup command to reset cursor position
        PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COL + command_string_len);

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
            CupResetFlag();

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

/*
    Function: ResetCommandLine
    Brief: Function to reset the state of the command line
*/
void TrainMonitor::ResetCommandLine() {
    PrintCup(CMD_PROMPT_START_ROW, 0);

    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);
    ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[CMD_PROMPT_START_ROW - 1] + "\r", UART0);

    PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COL);
}

/*
    Function: PrintDefaultScreen
    Brief: Function to print the default screen seen in the train library
*/
void TrainMonitor::PrintDefaultScreen() {
    std::stringstream sstream;
    sstream << "\n\r" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    for(int i = 0; i < NUM_SCREEN_ROWS; i++) {
        PrintCup(i + 1, 0);
        ISRMsgHandlerInstance_->QueueOutputMsg(TrainScreen[i] + "\r", UART0);
    }
}

/*
    Function: PrintCup
    Brief: Sends the CUP command to position the cursor to the given
        row and column
    Inputs: row: Row to set cursor to
            col: Column to set cursor to
*/
void TrainMonitor::PrintCup(int row, int col) {
    static std::stringstream sstream;
    sstream << "\e[" << row << ";" << col << "H";
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
    sstream.str("");
    sstream.clear();
}

/*
    Function: CupResetFlag
    Brief: Raises the cup reset flag, which signifies whether or not to cup
        to the command line on a new input or not. This also hides the cursor
*/
void TrainMonitor::CupResetFlag() {
    ISRMsgHandler::GetISRMsgHandler()->QueueOutputMsg(VT_100_HIDE_CURSOR, UART0);
    CupReset_ = true;
}

/*
    Function: InitializeScreen
    Brief: Called upon startup to initialize the screen to with the train set
        GUI in the correct state (All switches STRAIGHT and hall sensors off)
*/
void TrainMonitor::InitializeScreen() {
    ISRMsgHandlerInstance_->QueueOutputMsg(CLEAR_SCREEN, UART0);

    // Print default screen
    PrintDefaultScreen();

    std::stringstream sstream;

    // Highlight trains and set their information

    // Train 1 is green
    PrintCup(train_info_locations[0][0], train_info_locations[0][1]);
    sstream << VT_100_FONT_COLORS[train_colors[0]] << "Train 1" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    sstream.str(std::string());
    sstream.clear();

    // Train 2 is Yellow
    PrintCup(train_info_locations[1][0], train_info_locations[1][1]);
    sstream << VT_100_FONT_COLORS[train_colors[1]] << "Train 2" << VT_100_RESET;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    // To correspond with initial state of the trainset
    VisuallySetSwitch(255, STRAIGHT);

    // Set prompt to starting position 
    PrintCup(CMD_PROMPT_START_ROW, CMD_PROMPT_START_COL);
}

/*
    Function: VisuallySetHallSensor
    Brief: Function to visually set a hall sensor
    Input: sensor_num: Sensor to trigger or clear
           status: Boolean to set the sensor high (blue) or low (reset)
*/
void TrainMonitor::VisuallySetHallSensor(uint8_t sensor_num, bool status) {
    std::stringstream sstream;

    // Set all sensors high or low
    if (sensor_num == 255) {

        // Set VT-100 color just once
        if (status) sstream << VT_100_FONT_COLORS[BLUE];
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
        if (status) sstream << VT_100_FONT_COLORS[BLUE];
        else sstream << VT_100_RESET;
        // 2) Add characters to update
        // 3) Reset VT-100 codes
        sstream << "##" << VT_100_RESET;
        ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    }

    // Set cup reset flag
    CupResetFlag();
}

/*
    Function: VisuallySetSwitch
    Brief: Function to visually set one or many hall sensors
    Input: switch_num: Switch number to trigger
           dir: Direction to set the sensor to
*/
void TrainMonitor::VisuallySetSwitch(uint8_t switch_num, switch_direction_t dir) {
    std::stringstream sstream;

    assert(dir != NOT_NEEDED);

    // Reset all VT_100 Codes
    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);

    // Set all sensors high or low
    if (switch_num == 255) {
        for (uint8_t i = 1; i <= MAX_NUM_SWITCHES; i++) SetIndividualSwitch(i, dir);
    } // Or set one individual switch
    else if (switch_num != 0 && switch_num <= 6) SetIndividualSwitch(switch_num, dir);

    CupResetFlag();
}

/*
    Function: SetIndividualSwitch
    Brief: Function to visually set an individual hall sensor
    Input: switch_num: Switch number to trigger
           dir: Direction to set the sensor to
*/
void TrainMonitor::SetIndividualSwitch(uint8_t switch_num, switch_direction_t dir) {
    /* Handle straight portion first */
    TrainMonitor::PrintCup(switch_locations[switch_num][STRAIGHT][ROW], switch_locations[switch_num][STRAIGHT][COL]);

    // Save state of the switch
    switch_states[switch_num] = dir;

    // If straight, draw straight portion, else clear it
    if (dir == STRAIGHT) ISRMsgHandlerInstance_->QueueOutputMsg(switch_strings[switch_num][STRAIGHT], UART0);
    else ISRMsgHandlerInstance_->QueueOutputMsg("   ", UART0);

    /* Handle diverted portion second */
    TrainMonitor::PrintCup(switch_locations[switch_num][DIVERTED][ROW], switch_locations[switch_num][DIVERTED][COL]);

    if (dir == DIVERTED) ISRMsgHandlerInstance_->QueueOutputMsg(switch_strings[switch_num][DIVERTED], UART0);
    else ISRMsgHandlerInstance_->QueueOutputMsg("  ", UART0);
}

/*
    Function: VisuallyUpdateTrainInfo
    Brief: Updates the status information on the monitor to display the current speed and direction of
        a given train
    Input: train_num: Train number to display
           speed: Speed of the train to display
           dir: Direction of the train
*/
void TrainMonitor::VisuallyUpdateTrainInfo(uint8_t train_num, uint8_t speed, train_direction_t dir) {
    train_num--;
    assert(train_num < NUM_TRAINS);

    std::stringstream sstream;

    // Print speed
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][2]);
    sstream << VT_100_RESET << "  " << '\x7F' << '\x7F' << (int)speed;
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    sstream.str("");
    sstream.clear();

    // Print Direction
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][3]);
    sstream << "    " << '\x7F' << '\x7F' << '\x7F' << '\x7F' << train_direction_strings[dir];
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
}

/*
    Function: VisuallyUpdateTrainDst
    Brief: Updates the status information on the monitor to display the current destination
        of a given train
    Input: train_num: Train number to display
           dst: Destination zone of the train
*/
void TrainMonitor::VisuallyUpdateTrainDst(uint8_t train_num, uint8_t dst) {
    // train_num--;
    assert(train_num < NUM_TRAINS);

    std::stringstream sstream;

    // Print new destination 
    PrintCup(train_info_locations[train_num][0], train_info_locations[train_num][4]);
    sstream << VT_100_RESET << "    " << '\x7F' << '\x7F' << '\x7F' << '\x7F';
    if (dst != NO_ZONE) sstream << (int)dst;
    else sstream << "NONE";
    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);
}

/*
    Function: VisuallySetTrainLocation
    Brief: Function to visually show the train's location on the track
    Input: train_num: Train number to display
           new_zone: The zone to highlight
           prev_zone: The zone to clear
*/
void TrainMonitor::VisuallySetTrainLocation(uint8_t train_num, uint8_t new_zone, uint8_t prev_zone) {
    // Clear old zone (If not NO_ZONE)
    if (prev_zone < NUM_ZONES && prev_zone != NO_ZONE) PaintZone(prev_zone, NO_COLOR);

    // Color new zone
    assert(new_zone != NO_ZONE); //This should never be set to NO_ZONE
    if (new_zone < NUM_ZONES) PaintZone(new_zone, train_colors[train_num]);
}

/*
    Function: PaintZone
    Brief: Function to actually 'paint' a zone a given color
    Input: zone: Zone number to paint (or clear)
           color: VT-100 color to set the zone to
*/
void TrainMonitor::PaintZone(uint8_t zone, color_t color) {

    // Select background color (if any)
    ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_RESET, UART0);
    if (color != NO_COLOR) ISRMsgHandlerInstance_->QueueOutputMsg(VT_100_FONT_COLORS[color], UART0);

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

/*
    Function: IsSwitchZone
    Brief: Function that checks if a given zone is a switch zone or not
    Input: zone: Zone to check
    Output: uint8_t: The switch number (1-6) or 0 for not a switch zone
*/
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

#define TX_ROW 21
#define TX_COL 50
#define MAX_TX_NUM_CHARS 29

/*
    Function: VisuallyDisplayTX
    Brief: This function visually displays a given packet on the monitor in the TX
        position
    Input: msg: Message to display
           msg_len: Length of message
*/
void TrainMonitor::VisuallyDisplayTX(char* msg, uint8_t msg_len) {
    msg_len = MIN(msg_len, MAX_TX_NUM_CHARS); // Arbitrary number of characters

     // Delete old msg
    PrintCup(TX_ROW, TX_COL);
    ISRMsgHandler::GetISRMsgHandler()->QueueOutputMsg(VT_100_RESET, UART0);
    ISRMsgHandler::GetISRMsgHandler()->QueueOutputMsg("                            ", UART0);

     // Put new msg
    PrintCup(TX_ROW, TX_COL);
    static std::stringstream sstream;
    for (uint8_t i = 0; i < msg_len; i++) sstream << HEX(msg[i]);
    ISRMsgHandler::GetISRMsgHandler()->QueueOutputMsg(sstream.str(), UART0);
    sstream.str("");
    sstream.clear();
}

#define STATUS_ROW 3
#define STATUS_COL 5

/*
    Function: UpdateCommandStatus
    Brief: This function visually updates the status of a given command, typically
        green for a valid command and red for an invalid command
    Input: color: Message to display
*/
void TrainMonitor::UpdateCommandStatus(color_t color) {
    PrintCup(STATUS_ROW, STATUS_COL);

    std::stringstream sstream;
    sstream << VT_100_BR_COLORS[color] << "CMD" << VT_100_RESET;

    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    CupResetFlag();
}

#define TC_STATUS_ROW 4
#define TC_STATUS_COL 5

/*
    Function: UpdateTCCommandStatus
    Brief: This function visually updates the monitor with the state of the
        last train command sent, typically the color of the train for movement
        and red for stopping. This was used to debug things to tell when a
        stop/go command was sent
    Input: color: Message to display
*/
void TrainMonitor::UpdateTCCommandStatus(color_t color) {
    PrintCup(TC_STATUS_ROW, TC_STATUS_COL);

    std::stringstream sstream;
    sstream << VT_100_BR_COLORS[color] << "CMD" << VT_100_RESET;

    ISRMsgHandlerInstance_->QueueOutputMsg(sstream.str(), UART0);

    CupResetFlag();
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
