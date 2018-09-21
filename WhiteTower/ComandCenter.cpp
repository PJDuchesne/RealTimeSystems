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
CommandCenter *CommandCenter::CommandCenterInstance_ = new CommandCenter;

void CommandCenter::TimeCommand(std::string arg) {
  smh_t parsed_arg;

  if (arg != "") {
    if (!ParseTimeArg(arg, parsed_arg)) {
      Monitor::GetMonitor()->PrintErrorMsg("Malformed Arguments: " + arg);
      return;
    }
    else TimeHandler::GetTimeHandler()->SetTime(parsed_arg);
  }

  TimeHandler::GetTimeHandler()->PrintCurrentTime();

}

void CommandCenter::DateCommand(std::string arg) {
  dmy_t parsed_arg;

  if (arg != "") {
    if (!ParseDateArg(arg, parsed_arg)) {
      Monitor::GetMonitor()->PrintErrorMsg("Malformed Arguments: " + arg);
      return;
    }
    else TimeHandler::GetTimeHandler()->SetDate(parsed_arg);
  }

  TimeHandler::GetTimeHandler()->PrintCurrentDate();

}

void CommandCenter::AlarmCommand(std::string arg) {
  smh_t parsed_arg;

  if (arg == "") {
    TimeHandler::GetTimeHandler()->ClearAlarm();
    Monitor::GetMonitor()->PrintMsg("\r\nAlarm cleared");
  }
  else {
    if (!ParseTimeArg(arg, parsed_arg)) {
      Monitor::GetMonitor()->PrintErrorMsg("Malformed Arguments: " + arg);
      return;
    }
    TimeHandler::GetTimeHandler()->SetAlarm(parsed_arg);
  }
}

void CommandCenter::ZooCommand(std::string arg) {
  std::string animal;

  // Choose an animal at "random" (By using modulus and the current time)
  if (arg == "") animal = zoo[TimeHandler::GetTimeHandler()->GetCurrentTime().raw_time_ % (ZOO_SIZE)];
  else {
    Monitor::GetMonitor()->PrintErrorMsg("Unnecessary Argument for Zoo: " + arg);
    return;
  }

  bool end_flag = false;
  int16_t next_endline;
  std::string substr;
  while (1) {
    next_endline = animal.find_first_of("\n");
    if (next_endline == std::string::npos) {
      // On last line
      end_flag = true;
      substr = animal;
    }
    else {
      // Get substring
      substr = animal.substr(0, next_endline + 1); // +1 to include '\n'
      // Slice off that portion
      animal = animal.substr(next_endline + 1); // +1 to exclude '\n'
    }
    Monitor::GetMonitor()->PrintMsg(substr + "\r");
    if (end_flag) break;
  }
}

void CommandCenter::ToUpper(std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] >= 97 && str[i] <= 122) str[i] -= 32;
  }
}

// Format SS:MM:HH (Variable width (0+) on all 3)
bool CommandCenter::ParseTimeArg(std::string &input, smh_t &output) {

  int8_t length = input.length();
  if (length > 8) return false;

  int8_t first_colon = input.find_first_of(":");
  int8_t last_colon  = input.find_last_of(":");

  // If 0 or 1 colons (If 3+ then one of the stoi will fail)
  if (first_colon == last_colon) return false;

  // Flag used to check if SafeStoi returns properly
  int8_t substr_len;
  int stoi_tmp;

  // Hour (Variable size of 0 to 2)
  substr_len = first_colon; 
  if (substr_len > 2) return false;
  if (substr_len == 0) output.hour = 0;
  else {
    stoi_tmp = SafeStoi(input.substr(0, substr_len));
    if (stoi_tmp == -1) return false;
    else output.hour = stoi_tmp;
  }

  // Min (Variable size of 0 to 2)
  substr_len = last_colon - first_colon - 1;; 
  if (substr_len > 2) return false;
  if (substr_len == 0) output.min = 0;
  else {
    stoi_tmp = SafeStoi(input.substr(first_colon + 1, substr_len));
    if (stoi_tmp == -1) return false;
    else output.min = stoi_tmp;
  }

  // Sec (Variable size of 0 to 2)
  substr_len = length - last_colon - 1;
  if (substr_len > 2) return false;
  if (substr_len == 0) output.sec = 0;
  else {
    stoi_tmp = SafeStoi(input.substr(last_colon + 1, substr_len));
    if (stoi_tmp == -1) return false;
    else output.sec = stoi_tmp;
  }

  return TimeHandler::GetTimeHandler()->CheckValidTime(output);
}

// Format DD-MMM-YYYY (Varible width (1+) on DD and YYYY)
bool CommandCenter::ParseDateArg(std::string &input, dmy_t &output) {
  int8_t length = input.length();
  if (length > 11) return false;

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

  // Month
  stoi_tmp = -1;
  substr_len = last_dash - first_dash - 1;
  if (substr_len != 3) return false;
  std::string month_str = input.substr(first_dash + 1,substr_len);
  ToUpper(month_str); 
  for (int i = 0; i < MAX_MONTH + 2; i++) {
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

  return TimeHandler::GetTimeHandler()->CheckValidDate(output);
}

int CommandCenter::SafeStoi(std::string input_substr) {
  for (int i = 0; i < input_substr.length(); i++) {
    if (input_substr[i] < '0' || input_substr[i] > '9') return -1;
  }
  return std::stoi(input_substr);
}

CommandCenter::CommandCenter() {
  FunctionTable[0] = &CommandCenter::TimeCommand;
  FunctionTable[1] = &CommandCenter::DateCommand;
  FunctionTable[2] = &CommandCenter::AlarmCommand;
  FunctionTable[3] = &CommandCenter::ZooCommand;
}

void CommandCenter::HandleCommand(std::string command_str) {
  ToUpper(command_str);

  // Max number of tokens should be two. One command and one argument
  std::string token_array[MAX_NUM_TOKENS] = { "" };

  // TODO: Possibly Rewrite. Search for breaks and add substrings?
  for (int i = 0, j = 0; i < command_str.length(); i++ ) {
    // Should only have 2 tokens
    if (j == MAX_NUM_TOKENS) {
      Monitor::GetMonitor()->PrintMsg(" ? (Malformed Command: Too many arguments >>" + command_str + "<<)" + NEW_LINE);
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
  if (!flag) Monitor::GetMonitor()->PrintErrorMsg("Invalid Command: " + command_str);

}

CommandCenter* CommandCenter::GetCommandCenter() {
    // if (!CommandCenterInstance) CommandCenterInstance = new CommandCenter;
    return CommandCenterInstance_;
}
