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

#include "Includes/TrainCommandCenter.h"

// Singleton Instance
TrainCommandCenter *TrainCommandCenter::TrainCommandCenterInstance_ = 0;


void TrainCommandCenter::TrainCommand(std::string args) { // Two arguments
    int tmp_code = TokenizeArguments(args);
    if (tmp_code == -1 || tmp_code != 3) {
        SendErrorMsg("[TrainCommandCenter::TrainCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    uint8_t train_num     = SafeStoi(TokenizedArgs_[0]);
    uint8_t train_speed   = SafeStoi(TokenizedArgs_[1]);
    uint8_t train_dir_int = SafeStoi(TokenizedArgs_[2]);

    if ((train_num == 0 || train_num > 2) || train_speed > 15 || train_dir_int > 1) {
        SendErrorMsg("[TrainCommandCenter::TrainCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }

    train_direction_t train_dir = (train_dir_int ? CCW : CW);

    // SendTrainCommand(train_num, train_speed, train_dir);
}

void TrainCommandCenter::SwitchCommand(std::string args) { // Two arguments
    int tmp_code = TokenizeArguments(args);
    if (tmp_code == -1 || tmp_code != 2) {
        SendErrorMsg("[TrainCommandCenter::SwitchCommand] Error! Malformed Command (due to number of args)!\n");
        return;
    }

    uint8_t switch_num     = SafeStoi(TokenizedArgs_[0]);
    uint8_t switch_dir_int = SafeStoi(TokenizedArgs_[1]);

    if (((switch_num == 0 || switch_num > 6) && switch_num != 255) || switch_dir_int > 1) {
        SendErrorMsg("[TrainCommandCenter::SwitchCommand] Error! Malformed Command (due to numbers)!\n");
        return;
    }

    switch_direction_t switch_dir = (switch_dir_int ? DIVERTED : STRAIGHT);

    TrainMonitorInstance_->VisuallySetSwitch(switch_num, switch_dir); // TODO: Move this into the response code
    SendSwitchCommand(switch_num, switch_dir);
}

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

void TrainCommandCenter::QueueResetCommand(std::string arg) {
    if (arg != "") {
        SendErrorMsg("[TrainCommandCenter::QueueResetCommand] Error! Queue Command does not take arguments\n");
        return;
    }

    SendSensorQueueReset();
}

void TrainCommandCenter::RefreshCommand(std::string arg) {
    if (arg != "") {
        SendErrorMsg("[TrainCommandCenter::QueueResetCommand] Error! Refresh Command does not take arguments\n");
        return;
    }

    // Slightly different than just InitializeScreen
    TrainMonitorInstance_->InitializeScreen();
}

void TrainCommandCenter::SendErrorMsg(std::string msg) {

}

int TrainCommandCenter::TokenizeArguments(std::string &arg) {
    // Zero previous arguments (Not strictly necessary, but avoids any future complications)
    for (int i = 0; i < MAX_NUM_TRAIN_ARGUMENTS; i++) TokenizedArgs_[i] = "";

    // Tokenize input
    int token_idx = 0;
    for (int i = 0; i < arg.length(); i++ ) {
        // Should only have 2 tokens
        if (token_idx == MAX_NUM_TRAIN_TOKENS) {
            return -1;
        }
        if (arg[i] == ' ') token_idx++;
        else TokenizedArgs_[token_idx] += arg[i];
    }

    return token_idx + 1; // Return number of arguments
}

// Construct a message to command a train and send to TrainCommand mailbox
void TrainCommandCenter::SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q) {
    static char msg_body[3];

    msg_body[0] = '\xC0';
    msg_body[1] = train_num;
    ((train_settings_t *)(&msg_body[2]))->speed = speed;
    ((train_settings_t *)(&msg_body[2]))->direction = direction;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandCenter::SendTrainCommand(): Warning! PSend Failed\n";
    }
}

// Construct a message to throw switch(es) and send to TrainCommand mailbox
void TrainCommandCenter::SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q) {
    static char msg_body[3];

    msg_body[0] = '\xE0';
    msg_body[1] = switch_num;
    msg_body[2] = direction;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandCenter::SendSwitchCommand(): Warning! PSend Failed\n";
    }
}

void TrainCommandCenter::SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q) {
    static char msg_body[2];

    msg_body[0] = '\xA2';
    msg_body[1] = sensor_number;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 2)) {
        std::cout << "TrainCommandCenter::SendSensorAcknowledge(): Warning! PSend Failed\n";
    }
}

// Construct a message to reset sensor queue and send to TrainCommand mailbox
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
    Function: ParseDateArg
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
}

/*
    Function: SingletonGrab
    Brief: Setup function for the TrainCommandCenter to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void TrainCommandCenter::SingletonGrab() {
    TrainMonitorInstance_ = TrainMonitor::GetTrainMonitor();
}

/*
    Function: HandleCommand
    Input:  command_str: Input string to tokenize and call command function on
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
