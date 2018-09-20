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

CommandCenter::CommandCenter() {
  FunctionTable[0] = &CommandCenter::TimeCommand;
  FunctionTable[1] = &CommandCenter::DateCommand;
  FunctionTable[2] = &CommandCenter::AlarmCommand;
}

void CommandCenter::HandleCommand(std::string command_str) {
  // ParseCommand

  ToUpper(command_str);

  // Max number of tokens should be two. One command and one argument
  std::string token_array[MAX_NUM_TOKENS] = { "" };


  // TODO: Rewrite maybe? Search for breaks and add substrings?
  for (int i = 0, j = 0; i < command_str.length(); i++ ) {
    // Should only have 2 tokens
    if (j == MAX_NUM_TOKENS) {
      // TODO: Print error msg here
      Monitor::GetMonitor()->PrintMsg(" ? (ERROR ON COMMAND >>" + command_str + "<<)" + new_line);
      std::cout << "[CommandCenter::HandleCommand] Error: Too many tokens >>" << command_str << "<<\n";
      return;
    }
    if (command_str[i] == ' ') j++;
    else token_array[j] += command_str[i];
  }

  for (int i = 0; i < MAX_NUM_TOKENS; i++) std::cout << ">>" << token_array[i] << "<<\n";

  // Call Correct Command
  for (int i = 0; i < NUM_VALID_COMMANDS; i++) {
    if (valid_commands[i] == token_array[0] ) {
      (this->*FunctionTable[i])(token_array[1]);
      break;
    }
  }

}

void CommandCenter::TimeCommand(std::string arg) {
  std::cout << "TIME COMMAND\n";

}

void CommandCenter::DateCommand(std::string arg) {
  std::cout << "DATE COMMAND\n";

}

void CommandCenter::AlarmCommand(std::string arg) {
  std::cout << "ALARM COMMAND\n";

}

void CommandCenter::ToUpper(std::string& str) {
  for (int i = 0; i < str.length(); i++) {
    if (str[i] >= 97 && str[i] <= 122) str[i] -= 32;
  }
}

CommandCenter* CommandCenter::GetCommandCenter() {
    // if (!CommandCenterInstance) CommandCenterInstance = new CommandCenter;
    return CommandCenterInstance_;
}
