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
-> Name:  TrainCommandCenter.cpp
-> Date: Nov 26, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

/* General Note:
    
    This is essentially the same command center from the previous 2 assignments,
    the only difference is the commands themselves and the logic to cleanly
    create any of the of 4 controller packets

 */

#include "Includes/TrainCommandCenter.h"

// Singleton Instance
TrainCommandCenter *TrainCommandCenter::TrainCommandCenterInstance_ = 0;

/*
    Function: TrainCommand
    Brief: This function gives the user manual control over sending commands
        to the train. This was used for testing before the train controller
        logic was put in place.
    Input: Args in the form of an unparsed string (three arguments)
        1) Train num (1 or 2)
        2) Train speed (0 to 15)
        3) Train direction (CW or CCW)
*/
void TrainCommandCenter::TrainCommand(std::string args) {
    int tmp_code = TokenizeArguments(args);
    if (tmp_code == -1 || tmp_code != 3) {
        SendErrorMsg("[TrainCommandCenter::TrainCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    uint8_t train_num     = SafeStoi(TokenizedArgs_[0]);
    uint8_t train_speed   = SafeStoi(TokenizedArgs_[1]);

    if ((train_num == 0 || train_num > 2) || train_speed > 15) {
        SendErrorMsg("[TrainCommandCenter::TrainCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }

    train_direction_t train_dir;
    if (TokenizedArgs_[2] == "CW") train_dir = CW;
    else if (TokenizedArgs_[2] == "CCW") train_dir = CCW;
    else {
        SendErrorMsg("[TrainCommandCenter::TrainCommand] Error! Malformed Command (due to direction)!\n");
        return;
    }

    SendTrainCommand(train_num, train_speed, train_dir);
}

/*
    Function: SwitchCommand
    Brief: This function gives the user manual control over sending commands
        to the switches. This was used for testing before the train controller
        logic was put in place.
    Input: Arg in the form of an unparsed string (Two Arguments)
        1) Switch Number (1 to 6, or 255 for all)
        2) Switch state (STR or DIV)
*/
void TrainCommandCenter::SwitchCommand(std::string args) { // Two arguments
    int tmp_code = TokenizeArguments(args);
    if (tmp_code == -1 || tmp_code != 2) {
        SendErrorMsg("[TrainCommandCenter::SwitchCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    uint8_t switch_num = SafeStoi(TokenizedArgs_[0]);

    if ((switch_num == 0 || switch_num > 6) && switch_num != 255) {
        SendErrorMsg("[TrainCommandCenter::SwitchCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }

    switch_direction_t switch_dir;
    if (TokenizedArgs_[1] == "DIV") switch_dir = DIVERTED;
    else if (TokenizedArgs_[1] == "STR") switch_dir = STRAIGHT;
    else {
        SendErrorMsg("[TrainCommandCenter::SwitchCommand] Error! Malformed Command (due to direction)!\n");
        return;
    }

    SendSwitchCommand(switch_num, switch_dir);
}

/*
    Function: SensorCommand
    Brief: This function gives the user manual control over sending reset commands
        to the hall sensors. This was used for testing before the train controller
        logic was put in place.
    Input: Arg in the form of an unparsed string (1 argument)
        1) 
*/
void TrainCommandCenter::SensorCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);
    if (tmp_code == -1 || tmp_code != 1) {
        SendErrorMsg("[TrainCommandCenter::SensorCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    uint8_t sensor_num = SafeStoi(TokenizedArgs_[0]);

    if(sensor_num == 0 || (sensor_num > 24 && sensor_num != 255)) {
        SendErrorMsg("[TrainCommandCenter::SensorCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }
    
    SendSensorAcknowledge(sensor_num);
}

/*
    Function: QueueResetCommand
    Brief: This function gives the user manual control over sending the hall queue reset
        command to the Atmega. This was used for testing before the train controller
        logic was put in place.
    Input: Args in the form of an unparsed string (Should be empty)
*/
void TrainCommandCenter::QueueResetCommand(std::string arg) {
    if (arg != "") {
        SendErrorMsg("[TrainCommandCenter::QueueResetCommand] Error! Queue Command does not take arguments\n");
        return;
    }

    SendSensorQueueReset();
}

/*
    Function: RefreshCommand
    Brief: This function gives the user manual control over refreshing the UI screen
        in case something went wrong. This was useful when testing the UI and not so much
        after the UI was stabilized
    Input: Args in the form of an unparsed string (Should be empty)
*/
void TrainCommandCenter::RefreshCommand(std::string arg) {
    if (arg != "") {
        SendErrorMsg("[TrainCommandCenter::QueueResetCommand] Error! Refresh Command does not take arguments\n");
        return;
    }

    TrainMonitor::GetTrainMonitor()->InitializeScreen();
}

/*
    Function: InitCommand
    Brief: This function initializes the train's internal state to a given zone and
        updates the monitor accordingly
    Input: Args in the form of an unparsed string (One argument)
        1) Train Num (1 or 2)
*/
void TrainCommandCenter::InitCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);

    if (tmp_code != 2) {
        SendErrorMsg("[TrainCommandCenter::InitCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    static uint8_t init_msg[3];

    init_msg[0] = ZONE_CMD;
    init_msg[1] = SafeStoi(TokenizedArgs_[0]) - 1; // Train #: Decremented to match table in control system
    init_msg[2] = SafeStoi(TokenizedArgs_[1]);     // Zone Number

    if ( (init_msg[1] >= NUM_TRAINS) || (init_msg[2] > NUM_ZONES) ) {
        SendErrorMsg("[TrainCommandCenter::InitCommand] Error! Malformed Command (due to switch destination)!\n");
        return;
    }

    // If correctly formed, send a message to the controller to initialize the train
    PSend(TRAIN_MONITOR_MB, TRAIN_CONTROLLER_MB, (void *)init_msg, 3);
}

/*
    Function: TrainGoCommand
    Brief: This function gets the train to go to the given destination using the routing table
    Input: Args in the form of an unparsed string: (Two Arguments)
        1) Train num (1 or 2)
        2) Destination Zone (0 to 19)
*/
void TrainCommandCenter::TrainGoCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);

    if (tmp_code != 2) {
        SendErrorMsg("[TrainCommandCenter::TrainGoCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    static uint8_t train_go_msg[3];

    train_go_msg[0] = TRAIN_GO_CMD;
    train_go_msg[1] = SafeStoi(TokenizedArgs_[0]) - 1; // Train #: Decremented to match table in control system
    train_go_msg[2] = SafeStoi(TokenizedArgs_[1]);     // Destination zone Number

    for (uint8_t i = 0; i < MAX_NUM_SWITCHES; i++) {
        if (train_go_msg[2] == switch_zones[i]) {
            SendErrorMsg("[TrainCommandCenter::TrainGoCommand] Error! Malformed Command (due to numbers)!\n");
            return;
        }
    }

    if ( (train_go_msg[1] >= NUM_TRAINS) || (train_go_msg[2] > NUM_ZONES) ) {
        SendErrorMsg("[TrainCommandCenter::TrainGoCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }

    // If correctly formed, send a message to the controller to start the train routing
    PSend(TRAIN_MONITOR_MB, TRAIN_CONTROLLER_MB, (void *)train_go_msg, 3);
}

/*
    Function: KickTrainCommand
    Brief: This function attempts ot jumpstart the train if the current command
        has failed. This was useful due ot he inconsistent nature of the
        trains when responding to commands
    Input: Arg in the form of an unparsed string: (one Argument)
        1) Train num (1 or 2)
*/
void TrainCommandCenter::KickTrainCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);

    if (tmp_code != 1) {
        SendErrorMsg("[TrainCommandCenter::KickTrainCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    static uint8_t train_go_msg[2];

    train_go_msg[0] = KICK_CMD;
    train_go_msg[1] = SafeStoi(TokenizedArgs_[0]) - 1; // Train #: Decremented to match table in control system

    // If correctly formed, send a message to the controller to start the train routing
    PSend(TRAIN_MONITOR_MB, TRAIN_CONTROLLER_MB, (void *)train_go_msg, 2);
}

/*
    Function: SudoStopCommand
    Brief: This function directly sends a command to stop the train, it also
        updates the train controller so the state is updated
    Input: Arg in the form of an unparsed string: (one Argument)
        1) Train num (1 or 2)
*/
void TrainCommandCenter::SudoStopCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);

    if (tmp_code != 1) {
        SendErrorMsg("[TrainCommandCenter::KickTrainCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    // Directly stop the train
    SendTrainCommand(SafeStoi(TokenizedArgs_[0]), 0, CW);

    // Update the controller so it is aware of state
    static uint8_t stop_msg[2];

    stop_msg[0] = SUDO_STOP_CMD;
    stop_msg[1] = SafeStoi(TokenizedArgs_[0]) - 1; // Train #: Decremented to match table in control system

    // If correctly formed, send a message to the controller to start the train routing
    PSend(TRAIN_MONITOR_MB, TRAIN_CONTROLLER_MB, (void *)stop_msg, 2);

}

/*
    Function: StopCommand
    Brief: This function sends a message to the controller to stop the train
        when it enters the next zone. This was a step in the direction of
        halting and re-routing trains
    Input: Arg in the form of an unparsed string: (one Argument)
        1) Train num (1 or 2)
*/
void TrainCommandCenter::StopCommand(std::string arg) {
    int tmp_code = TokenizeArguments(arg);

    if (tmp_code != 1) {
        SendErrorMsg("[TrainCommandCenter::KickTrainCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    // Politely ask the controller to stop
    static uint8_t stop_msg[2];

    stop_msg[0] = STOP_CMD;
    stop_msg[1] = SafeStoi(TokenizedArgs_[0]) - 1; // Train #: Decremented to match table in control system

    // If correctly formed, send a message to the controller to start the train routing
    PSend(TRAIN_MONITOR_MB, TRAIN_CONTROLLER_MB, (void *)stop_msg, 2);
}

/*
    Function: SendErrorMsg
    Brief: This function is used to indicate that an input command was invalid
        due to the reasoning given in the message. This simply updates an error
        status on the UI in the upper left to indicate to the user that their
        command was invalid.
    Input: msg (Not actually used)
*/
void TrainCommandCenter::SendErrorMsg(std::string msg) {
    TrainMonitor::GetTrainMonitor()->UpdateCommandStatus(RED);
}

/*
    Function: TokenizeArguments
    Brief: This function tokenizes the input arguments of a command into
        variables stored within the class (to save allocation/deallocation times),
        while returning the number of arguments found.
    Input: arg: The argument string to parse
    Output: int: The number of arguments found
            TokenizedArgs_: The arguments themselves, tokenized into this array
*/
int TrainCommandCenter::TokenizeArguments(std::string &arg) {
    // Zero previous arguments (Not strictly necessary, but avoids any future complications)
    for (int i = 0; i < MAX_NUM_TRAIN_ARGUMENTS; i++) TokenizedArgs_[i] = "";

    // Tokenize input
    int token_idx = 0;
    for (int i = 0; i < arg.length(); i++ ) {
        if (token_idx == MAX_NUM_TRAIN_TOKENS) {
            return -1;
        }
        if (arg[i] == ' ') token_idx++;
        else TokenizedArgs_[token_idx] += arg[i];
    }

    return token_idx + 1; // Return number of arguments
}

/*
    Function: SendTrainCommand
    Brief: Construct a message to command a train and send to TrainCommand mailbox
    Input: train_num: Number of train to command (1 or 2)
           speed: Speed to set train to (0 to 15)
           direction: Direction to set train to (CW or CCW)
           srq_q: Mailbox that this message came from (Defaults to the TRAIN_MONITOR_MB)
*/
void TrainCommandCenter::SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q) {
    // Direction doesn't matter if the speed is set to 0
    if (direction == STAY) assert(speed == 0);

    static char msg_body[3];

    uint8_t tmp_num = src_q;

    msg_body[0] = '\xC0';
    msg_body[1] = train_num;
    ((train_settings_t *)(&msg_body[2]))->speed = speed;
    ((train_settings_t *)(&msg_body[2]))->dir = (direction == CW ? CW : 8*CCW); // Technically this should be an 8 within this nibble

    if (!PSend(tmp_num, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandCenter::SendTrainCommand(): Warning! PSend Failed\n";
    }

    // Also update monitor with train status
    PSend(tmp_num, TRAIN_MONITOR_MB, msg_body, 3);
}

/*
    Function: SendSwitchCommand
    Brief: Construct a message to throw switch(es) and send to TrainCommand mailbox
    Input: switch_num: Number of train to command (1 to 6 or 255)
           direction: Direction to set switch to (STRAIGHT or DIVERTED)
           srq_q: Mailbox that this message came from (Defaults to the TRAIN_MONITOR_MB)
*/
void TrainCommandCenter::SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q) {
    static char msg_body[3];

    msg_body[0] = '\xE0';
    msg_body[1] = switch_num;
    msg_body[2] = direction;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandCenter::SendSwitchCommand(): Warning! TRAIN_APPLICATION_LAYER_MB PSend Failed\n";
    }

    // Update monitor with new switch state
    switch_msg_t switch_msg;
    switch_msg.type = SWITCH_CHANGE;
    switch_msg.ctrl.num = switch_num;
    switch_msg.ctrl.req_state = direction;
    if (!PSend(src_q, TRAIN_MONITOR_MB, &switch_msg, 2)) {
        std::cout << "TrainCommandCenter::SendSwitchCommand(): Warning! TRAIN_MONITOR_MB PSend Failed\n";
    }
}

/*
    Function: SendSensorAcknowledge
    Brief: Construct a message to acknowledge a sensor trigger
    Input: sensor_number: Number of train to command (1 to 24)
           srq_q: Mailbox that this message came from (Defaults to the TRAIN_MONITOR_MB)
*/
void TrainCommandCenter::SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q) {
    static char msg_body[2];

    msg_body[0] = '\xA2';
    msg_body[1] = sensor_number;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 2)) {
        std::cout << "TrainCommandCenter::SendSensorAcknowledge(): Warning! PSend Failed\n";
    }
}

/*
    Function: SendSensorQueueReset
    Brief: Construct a message to reset sensor queue and send to TrainCommand mailbox
    Input: srq_q: Mailbox that this message came from (Defaults to the TRAIN_MONITOR_MB)
*/
void TrainCommandCenter::SendSensorQueueReset(uint8_t src_q) {
    static char msg_body[1];

    msg_body[0] = '\xA8';

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 1)) {
        std::cout << "TrainCommandCenter::SendSensorQueueReset(): Warning! PSend Failed\n";
    }
}

/*
    Function: ToUpper
    Input:  str: String to make upper case 
    Output: str: String with capitalized alphabetical characters
    Brief: Translate an input string from any case to upper case, allows case insensitive input
*/
void TrainCommandCenter::ToUpper(std::string& str) {
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= 'a' && str[i] <= 'z') str[i] -= ASCII_CASE_OFFSET;
    }
}

/*
    Function: SafeStoi
    Input:  input_substr: Input string to parse from character inters to an integer value
    Output: (stoi result): Output integer
    Brief: Wrapper function for std::stoi to check that all input characters are valid integer characters
*/
int TrainCommandCenter::SafeStoi(std::string input_substr) {
    for (int i = 0; i < input_substr.length(); i++) {
        if (input_substr[i] < '0' || input_substr[i] > '9') return -1;
    }
    return std::stoi(input_substr);
}

/*
    Function: TrainCommandCenter
    Brief: Constructor for the command center, initializes the FunctionTable
*/
TrainCommandCenter::TrainCommandCenter() {
    FunctionTable[0] = &TrainCommandCenter::TrainCommand;
    FunctionTable[1] = &TrainCommandCenter::SwitchCommand;
    FunctionTable[2] = &TrainCommandCenter::SensorCommand;
    FunctionTable[3] = &TrainCommandCenter::QueueResetCommand;
    FunctionTable[4] = &TrainCommandCenter::RefreshCommand;
    FunctionTable[5] = &TrainCommandCenter::InitCommand;
    FunctionTable[6] = &TrainCommandCenter::TrainGoCommand;
    FunctionTable[7] = &TrainCommandCenter::KickTrainCommand;
}

/*
    Function: HandleCommand
    Input: command_str: Input string to tokenize and call command function on
    Brief: Breaks the input string into tokens and calls the correct command based on the first token
*/
void TrainCommandCenter::HandleCommand(std::string command_str) {
    ToUpper(command_str);

    // Max number of tokens should be two. One command and one argument
    std::string token_array[MAX_NUM_TRAIN_TOKENS] = { "" };

    // Tokenize the command string
    for (int i = 0, j = 0; i < command_str.length(); i++ ) {
        // Should only have 2 tokens
        if (j == MAX_NUM_TRAIN_TOKENS) {
            SendErrorMsg("Malformed Command: Too many arguments: >>" + command_str);
            return;
        }
        if (command_str[i] == ' ' && j == 0) j++; // Only allow 2 tokens
        else token_array[j] += command_str[i];
    }

    // Call the correct command
    bool flag = false;
    for (int i = 0; i < NUM_VALID_TRAIN_COMMANDS; i++) {
        if (valid_train_commands[i] == token_array[0] ) {
            (this->*FunctionTable[i])(token_array[1]);
            flag = true;
            break;
        }
    }
    if (!flag) SendErrorMsg("Invalid Command: " + command_str);
}

/*
    Function: GetTrainCommandCenter
    Output: TrainCommandCenterInstance_: Pointer to the TrainCommandCenter Singleton
    Brief: Get function for the TrainCommandCenter singleton
*/
TrainCommandCenter* TrainCommandCenter::GetTrainCommandCenter() {
    if (!TrainCommandCenterInstance_) TrainCommandCenterInstance_ = new TrainCommandCenter;
    return TrainCommandCenterInstance_;
}
