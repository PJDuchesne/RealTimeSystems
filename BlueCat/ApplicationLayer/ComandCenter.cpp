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
-> Name:  CommandCenter.cpp
-> Date: Sept 19, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/CommandCenter.h"

// Singleton Instance
CommandCenter *CommandCenter::CommandCenterInstance_ = 0;

/*
    Function: TimeCommand
    Input:  arg: Command argument to be parsed
    Brief: Handles the Time command by setting and/or printing the current time
*/
void CommandCenter::TimeCommand(std::string arg) {
    smh_t parsed_arg;

    if (arg != "") {
        if (!ParseTimeArg(arg, parsed_arg)) {
            MonitorInstance_->PrintErrorMsg("Malformed Arguments: >>" + arg);
            return;
        }
        else TimeHandlerInstance_->SetTime(parsed_arg);
    }

    TimeHandlerInstance_->PrintCurrentTime();
}

/*
    Function: DateCommand
    Input:  arg: Command argument to be parsed
    Brief: Handles the date command by setting and/or printing the current date
*/
void CommandCenter::DateCommand(std::string arg) {
    dmy_t parsed_arg;

    if (arg != "") {
        if (!ParseDateArg(arg, parsed_arg)) {
            MonitorInstance_->PrintErrorMsg("Malformed Arguments: >>" + arg);
            return;
        }
        else TimeHandlerInstance_->SetDate(parsed_arg);
    }

    TimeHandlerInstance_->PrintCurrentDate();
}

/*
    Function: AlarmCommand
    Input:  arg: Command argument to be parsed
    Brief: Handles the date command by setting or clearing the current alarm
*/
void CommandCenter::AlarmCommand(std::string arg) {
    alarm_t new_alarm = zero_alarm;
    smh_t parsed_arg;

    if (arg == "") {
        // If alarm is active, clear it
        if (TimeHandlerInstance_->CheckAlarmActive()) TimeHandlerInstance_->SetAlarm(new_alarm);
        else MonitorInstance_->PrintMsg("\r\nNo alarm to clear");
    }
    else {
        if (!ParseTimeArg(arg, parsed_arg)) {
            MonitorInstance_->PrintErrorMsg("Malformed Arguments: >>" + arg);
            return;
        }
        if (parsed_arg == zero_smh) { // zero_smh meaning 00:00:00
            MonitorInstance_->PrintErrorMsg("ERROR: Alarm cannot be set 0 seconds from now");
            return;
        }
        new_alarm.alarm_time = parsed_arg; // This is the relative time, not actual alarm time
        new_alarm.is_active = true;
        TimeHandlerInstance_->SetAlarm(new_alarm); // Actual alarm time calculated in SetAlarm()
    }
}

void CommandCenter::DiagCommand(std::string arg) {
    if (arg != "") {
        MonitorInstance_->PrintErrorMsg("ERROR: Diag does not take arguments");
        return;
    }
    std::string DisplayString;
    OSInstance_->DiagnosticsDisplay(DisplayString);
    MonitorInstance_->PrintMsg(DisplayString);
}

/*
    Function: ToUpper
    Input:  str: String to make upper case 
    Output: str: String with capitalized alphabetical characters
    Brief: Translate an input string from any case to upper case, allows case insensitive input
*/
void CommandCenter::ToUpper(std::string& str) {
    for (int i = 0; i < str.length(); i++) {
        if (str[i] >= 'a' && str[i] <= 'z') str[i] -= ASCII_CASE_OFFSET;
    }
}

/*
    Function: ParseTimeArg
    Input:  input: Input string to parse into a time structure
    Output: output: Output time structure containing the parsed string values
    Brief: Parses the input string from the format SS:MM::HH into an output structure for
           elsewhere in the code. Accepts variable width input from 0 to 2 characters on
           each field, autofilling blank fields to 0.
*/
bool CommandCenter::ParseTimeArg(std::string &input, smh_t &output) {
    output = zero_smh;

    int8_t length = input.length();
    if (length > MAX_TIME_STR_LEN) return false;

    int8_t first_colon = input.find_first_of(":");
    int8_t last_colon  = input.find_last_of(":");

    if (first_colon == last_colon) last_colon = std::string::npos;

    // Flag used to check if SafeStoi returns properly
    int8_t substr_len;
    int stoi_tmp;

    // Hour (Variable size of 0 to 2)
    substr_len = (first_colon == std::string::npos ? length : first_colon);
    if (substr_len > 2) return false;
    if (substr_len == 0) output.hour = 0;
    else {
        stoi_tmp = SafeStoi(input.substr(0, substr_len));
        if (stoi_tmp == -1) return false;
        else output.hour = stoi_tmp;
    }

    if (first_colon == std::string::npos || length == first_colon + 1) {
        return TimeHandlerInstance_->CheckValidTime(output);
    }

    // Min (Variable size of 0 to 2)
    substr_len = (last_colon == std::string::npos ? length : last_colon) - first_colon - 1;; 
    if (substr_len > 2) return false;
    if (substr_len == 0) output.min = 0;
    else {
        stoi_tmp = SafeStoi(input.substr(first_colon + 1, substr_len));
        if (stoi_tmp == -1) return false;
        else output.min = stoi_tmp;
    }

    if (last_colon == std::string::npos) return TimeHandlerInstance_->CheckValidTime(output);

    // Sec (Variable size of 0 to 2)
    substr_len = length - last_colon - 1;
    if (substr_len > 2) return false;
    if (substr_len == 0) output.sec = 0;
    else {
        stoi_tmp = SafeStoi(input.substr(last_colon + 1, substr_len));
        if (stoi_tmp == -1) return false;
        else output.sec = stoi_tmp;
    }

    return TimeHandlerInstance_->CheckValidTime(output);
}

/*
    Function: ParseDateArg
    Input:  input: Input string to parse into a date structure
    Output: output: Output date structure containing the parsed string values
    Brief: Parses the input string from the format DD-MMM-YYYY into an output structure for
           elsewhere in the code. Accepts variable width input from 1 to 4 characters depending
           on the field. Takes into account the variable number of days per month (inc. leap years).
*/
bool CommandCenter::ParseDateArg(std::string &input, dmy_t &output) {
    int8_t length = input.length();
    if (length > MAX_DATE_STR_LEN) return false;

    int8_t first_dash = input.find_first_of("-");
    int8_t last_dash  = input.find_last_of("-");

    if (first_dash == last_dash) return false;

    // Flag used to check if SafeStoi returns properly
    int16_t stoi_tmp;
    int8_t substr_len;

    // Day (Variable size of 1 to 2)
    substr_len = first_dash;
    if (substr_len == 0 || substr_len > 2) return false;
    else {
        stoi_tmp = SafeStoi(input.substr(0, substr_len));
        if (stoi_tmp == -1) return false;
        else output.day = stoi_tmp;
    }

    // Month (Fixed size of 3)
    stoi_tmp = -1;
    substr_len = last_dash - first_dash - 1;
    if (substr_len != 3) return false;
    std::string month_str = input.substr(first_dash + 1,substr_len);
    ToUpper(month_str); 
    for (int i = 0; i < MAX_MONTH + 1; i++) {
        if (month_str == months_str[i]) {
            stoi_tmp = i;
            break;
        }
    }
    if (stoi_tmp == -1) return false;
    output.month = stoi_tmp;

    // Year (Variable size of 1 to 4)
    substr_len = length - last_dash - 1;
    if (substr_len == 0 || substr_len > 4) return false;
    else {
        stoi_tmp = SafeStoi(input.substr(last_dash + 1, substr_len));
        if (stoi_tmp == -1) return false;
        else output.year = stoi_tmp;
    }

    return TimeHandlerInstance_->CheckValidDate(output);
}

/*
    Function: ParseDateArg
    Input:  input_substr: Input string to parse from character inters to an integer value
    Output: (stoi result): Output integer
    Brief: Wrapper function for std::stoi to check that all input characters are valid integer characters
*/
int CommandCenter::SafeStoi(std::string input_substr) {
    for (int i = 0; i < input_substr.length(); i++) {
        if (input_substr[i] < '0' || input_substr[i] > '9') return -1;
    }
    return std::stoi(input_substr);
}

/*
    Function: CommandCenter
    Brief: Constructor for the command center, initializes the FunctionTable
*/
CommandCenter::CommandCenter() {
    FunctionTable[0] = &CommandCenter::TimeCommand;
    FunctionTable[1] = &CommandCenter::DateCommand;
    FunctionTable[2] = &CommandCenter::AlarmCommand;
    FunctionTable[3] = &CommandCenter::DiagCommand;
}

/*
    Function: SingletonGrab
    Brief: Setup function for the CommandCenter to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void CommandCenter::SingletonGrab() {
    MonitorInstance_ = Monitor::GetMonitor();
    TimeHandlerInstance_ = TimeHandler::GetTimeHandler();
    OSInstance_ = OperatingSystem::GetOperatingSystem();
}

/*
    Function: HandleCommand
    Input:  command_str: Input string to tokenize and call command function on
    Brief: Breaks the input string into tokens and calls the correct command based on the first token
*/
void CommandCenter::HandleCommand(std::string command_str) {
    ToUpper(command_str);

    // Max number of tokens should be two. One command and one argument
    std::string token_array[MAX_NUM_TOKENS] = { "" };

    for (int i = 0, j = 0; i < command_str.length(); i++ ) {
        // Should only have 2 tokens
        if (j == MAX_NUM_TOKENS) {
            MonitorInstance_->PrintErrorMsg("Malformed Command: Too many arguments: >>" + command_str);
            return;
        }
        if (command_str[i] == ' ') j++;
        else token_array[j] += command_str[i];
    }

    bool flag = false;
    for (int i = 0; i < NUM_VALID_COMMANDS; i++) {
        if (valid_commands[i] == token_array[0] ) {
            (this->*FunctionTable[i])(token_array[1]);
            flag = true;
            break;
        }
    }
    if (!flag) MonitorInstance_->PrintErrorMsg("Invalid Command: " + command_str);
}

/*
    Function: GetCommandCenter
    Output: CommandCenterInstance_: Pointer to the CommandCenter Singleton
    Brief: Get function for the CommandCenter singleton
*/
CommandCenter* CommandCenter::GetCommandCenter() {
    if (!CommandCenterInstance_) CommandCenterInstance_ = new CommandCenter;
    return CommandCenterInstance_;
}
