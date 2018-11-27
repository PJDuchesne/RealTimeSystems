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

#include "TrainMonitor.h"

#include <ISRLayer/Includes/GlobalMailboxes.h> // TODO: Might cause a dependency loop
#include "TrainLibrary.h"

#define MAX_NUM_TRAIN_TOKENS      3
#define MAX_NUM_TRAIN_ARGUMENTS   3

// Number of arguments for each command
#define NUM_TRAIN_ARGS    3
#define NUM_SWITCH_ARGS   2
#define NUM_SENSOR_ARGS   1
#define NUM_QUEUE_ARGS    0

#define NUM_SCREEN_ROWS   24

const std::string TrainScreen[NUM_SCREEN_ROWS] = {
  "################################################################################",
  "#                                                                              #",
  "#                       20                             19                      #",
  "#                     ==##=============================##==                    #",
  "#              14    //    13       12      11       10   \\\\     9             #",
  "#            ==##====---===##=======##---===##=======##==---=====##==          #",
  "#          //       S1                 // S2              S3        \\\\         #",
  "#         //           ====##=======##==                             \\\\        #",
  "#     15 ##                24       23                                ## 8     #",
  "#        ||                                                           ||       #",
  "#        ||      Train 1: Spd, Dir                                    ||       #",
  "#        ||      Train 2: Spd, Dir                                    ||       #",
  "#     16 ##                                 21       22               ## 7     #",
  "#         \\\\                              ==##=======##====          //        #",
  "#          \\\\       S6     2      3   S5 //  4       5    S4        //         #",
  "#            ==##====---===##=======##===---##=======##==---=====##==          #",
  "#              1     \\\\                                   //      6            #",
  "#  Hall   ##          ==##=============================##==                    #",
  "#  Switch -- or //      17                             18                      #",
  "#______________________________________________________________________________#",
  "#                                                                              #",
  "#  >                                                                           #",
  "#                                                                              #",
  "################################################################################"
};

#define NUM_VALID_TRAIN_COMMANDS  5
const std::string valid_train_commands[NUM_VALID_TRAIN_COMMANDS] = {
  "TRAIN",
  "SWITCH",
  "SENSOR",
  "QUEUERESET",
  "REFRESH"
};

// To match tables below
enum {
    ROW = 0,
    COL = 1
};

#define NUM_FONT_COLORS       8
#define NUM_BACKGROUND_COLORS 8

// Font & Background Colors Colors
enum {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

const std::string VT_100_FONT_COLORS[NUM_FONT_COLORS] {
    "\e[1;30m",
    "\e[1;31m",
    "\e[1;32m",
    "\e[1;33m",
    "\e[1;34m",
    "\e[1;35m",
    "\e[1;36m",
    "\e[1;37m"
};

const std::string VT_100_BR_COLORS[NUM_BACKGROUND_COLORS] {
    "\e[40m",
    "\e[41m",
    "\e[42m",
    "\e[43m",
    "\e[44m",
    "\e[45m",
    "\e[46m",
    "\e[47m"
};

#define VT_100_RESET    "\e[0m" 

#define MAX_NUM_SENSORS 24

// [Sensor #][0] -> Row
// [Sensor #][1] -> Column
const uint8_t sensor_locations[MAX_NUM_SENSORS + 1][2] {
    {0,   0}, // 0 -> DOES NOT EXIST
    {16, 16}, // 1
    {16, 28}, // 2
    {16, 37}, // 3
    {16, 45}, // 4
    {16, 54}, // 5
    {16, 66}, // 6
    {13, 71}, // 7
    {9,  71}, // 8 
    {6,  66}, // 9
    {6,  54}, // 10
    {6,  45}, // 11
    {6,  37}, // 12
    {6,  28}, // 13
    {6,  16}, // 14
    {9,  10}, // 15
    {13, 10}, // 16
    {18, 25}, // 17
    {18, 56}, // 18
    {4,  56}, // 19
    {4,  25}, // 20
    {14, 45}, // 21
    {14, 54}, // 22
    {8,  37}, // 23
    {8,  28}, // 24
};

#define MAX_NUM_SWITCHES 6

// [Sensor #][Straight (0) -> Diverted (1)][Row (0) -> Column (1)]
const uint8_t switch_locations[MAX_NUM_SWITCHES + 1][2][2] {
    {{0,  0},  {0,  0 }}, // 0 --> DOES NOT EXIST
    {{6,  22}, {5,  22}}, // 1
    {{6,  39}, {7,  40}}, // 2
    {{6,  58}, {5,  59}}, // 3
    {{16, 58}, {17, 59}}, // 4
    {{16, 42}, {15, 42}}, // 5
    {{16, 22}, {17, 22}}, // 6
};

#define RESET_VT_100_CODE "\e[0m"

#define TRAIN_1_CLR 42 // Train 1 is Green
#define TRAIN_2_CLR 43 // Train 2 is Yellow

#define LIT_HALL_SENSOR_CLR 44 // Active hall sensors are blue

#define CMD_PROMPT_START_ROW    22
#define CMD_PROMPT_START_COLUMN 6

#define CLEAR_SCREEN     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"

// Forward Declarations
class TrainMonitor;

class TrainCommandCenter {
    private:
        static TrainCommandCenter* TrainCommandCenterInstance_;
        typedef void (TrainCommandCenter::*FunctionPtr)(std::string arg);
        FunctionPtr FunctionTable[NUM_VALID_TRAIN_COMMANDS];

        TrainMonitor *TrainMonitorInstance_;

        bool cup_reset;

        std::string TokenizedArgs_[MAX_NUM_TRAIN_ARGUMENTS];

        // Individual Commands
        void TrainCommand(std::string args);
        void SwitchCommand(std::string args);
        void SensorCommand(std::string arg);
        void QueueResetCommand(std::string arg);
        void RefreshCommand(std::string arg);

        // Packet Sending Functions for Each Command
        void SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q = MONITOR_MB);
        void SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q = MONITOR_MB);
        void SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q = MONITOR_MB);
        void SendSensorQueueReset(uint8_t src_q = MONITOR_MB);

        // Internal helpers
        void ToUpper(std::string &str);
        int SafeStoi(std::string input_substr);
        void SendErrorMsg(std::string msg);
        int TokenizeArguments(std::string &msg);

    public:
        TrainCommandCenter();
        void SingletonGrab();

        void HandleCommand(std::string command_str);

        static TrainCommandCenter* GetTrainCommandCenter();
};

#endif /* TrainCommandCenter_H */
