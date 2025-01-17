#ifndef TrainCommandCenter_H
#define TrainCommandCenter_H

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
-> Name:  TrainCommandCenter.h
-> Date: Nov 26, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"
#include <ISRLayer/Includes/GlobalMailboxes.h>

#include "TrainMonitor.h"

#define NUM_VALID_TRAIN_COMMANDS  8
const std::string valid_train_commands[NUM_VALID_TRAIN_COMMANDS] = {
  "TRAIN",
  "SWITCH",
  "SENSOR",
  "QUEUERESET",
  "REFRESH",
  "INIT",
  "TRAINGO",
  "KICK",
};

// Forward Declarations
class TrainMonitor;

class TrainCommandCenter {
    private:
        static TrainCommandCenter* TrainCommandCenterInstance_;
        typedef void (TrainCommandCenter::*FunctionPtr)(std::string arg);
        FunctionPtr FunctionTable[NUM_VALID_TRAIN_COMMANDS];
        std::string TokenizedArgs_[MAX_NUM_TRAIN_ARGUMENTS];

        // Individual Commands
        void TrainCommand(std::string args);
        void SwitchCommand(std::string args);
        void SensorCommand(std::string arg);
        void QueueResetCommand(std::string arg);
        void RefreshCommand(std::string arg);
        void InitCommand(std::string arg);
        void TrainGoCommand(std::string arg);
        void KickTrainCommand(std::string arg);
        void SudoStopCommand(std::string arg);
        void StopCommand(std::string arg);

        // Internal helpers
        void ToUpper(std::string &str);
        int SafeStoi(std::string input_substr);
        void SendErrorMsg(std::string msg);
        int TokenizeArguments(std::string &msg);

    public:
        TrainCommandCenter();

        void HandleCommand(std::string command_str);

        // Packet Sending Functions for Each Command (Public API)
        void SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q = TRAIN_MONITOR_MB);
        void SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q = TRAIN_MONITOR_MB);
        void SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q = TRAIN_MONITOR_MB);
        void SendSensorQueueReset(uint8_t src_q = TRAIN_MONITOR_MB);

        static TrainCommandCenter* GetTrainCommandCenter();
};

#endif /* TrainCommandCenter_H */
