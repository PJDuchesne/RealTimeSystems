#ifndef CommandCenter_H
#define CommandCenter_H

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
-> Name:  CommandCenter.h
-> Date: Sept 19, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Monitor.h"
#include "TimeHandler.h"
#include "TimeDefinitions.h"

#include <OSLayer/Includes/OperatingSystem.h>
#include <TrainLayers/Includes/TrainProcesses.h>

#define ASCII_CASE_OFFSET   32
#define NUM_VALID_COMMANDS  6
#define MAX_NUM_TOKENS      2
#define MAX_TIME_STR_LEN    8
#define MAX_DATE_STR_LEN    11

enum CommandTokens {
  COMMAND,
  ARGS
};

const std::string valid_commands[NUM_VALID_COMMANDS] = {
  "TIME",
  "DATE",
  "ALARM",
  "DIAG",
  "REVERSE",
  "TRAINYARD"
};

// Forward Declarations
class Monitor;
class TimeHandler;
class OperatingSystem;

class CommandCenter {
    private:
        static CommandCenter* CommandCenterInstance_;
        typedef void (CommandCenter::*FunctionPtr)(std::string arg);
        FunctionPtr FunctionTable[NUM_VALID_COMMANDS];

        Monitor *MonitorInstance_;
        TimeHandler *TimeHandlerInstance_;
        OperatingSystem *OSInstance_;

        // Individual Commands
        void TimeCommand(std::string arg);
        void DateCommand(std::string arg);
        void AlarmCommand(std::string arg);
        void DiagCommand(std::string arg);
        void ReverseCommand(std::string arg);
        void TrainYardCommand(std::string arg);

        // Internal helpers
        void ToUpper(std::string &str);
        bool ParseTimeArg(std::string &input, smh_t &output);
        bool ParseDateArg(std::string &input, dmy_t &output);
        int SafeStoi(std::string input_substr);

    public:
        CommandCenter();
        void SingletonGrab();

        void HandleCommand(std::string command_str);

        static CommandCenter* GetCommandCenter();
};

#endif /* CommandCenter_H */
