#ifndef TrainLibrary_H
#define TrainLibrary_H

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
-> Name:  TrainLibrary.h
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

// TODO: Remove any unncessary includes
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

// For cleaning printing hex characters
#include <iomanip>
#define HEX(x) " " << std::setw(2) << std::setfill('0') << std::hex << int(x) << " " << std::dec

#define DELAY(x) i_DELAY = 0; while(i_DELAY++ < x) {}

#define UART_PHYSICAL_LAYER_MB_SIZE 100

#define MAX_PACKET_SIZE 5
#define EMPTY_MAILBOX 0

#define ALL 255 // Used to acknowledge all sensor resets or throw all switches

// Macros to perform modulus 8 increments and decrements
#define MOD8PLUS1(x) (x = (x + 1) % 8)
#define MOD8MINUS1(x) (x = (x == 0 ? 7 : x - 1)) // TODO: Make the MINUS one more efficient

enum train_layer_mailboxes {
    UART_PHYSICAL_LAYER_MB     = 200,
    PACKET_PHYSICAL_LAYER_MB   = 201,
    DATA_LINK_LAYER_MB         = 202,
    TRAIN_APPLICATION_LAYER_MB = 203,
    TRAIN_TIME_SERVER_MB       = 204,
    TRAIN_MONITOR_MB           = 205,
    TEST_PROCESS_MB = 210 // For future testing
};

typedef enum packet_type {
    DATA_PT   = 0,
    ACK_PT    = 1,
    NACK_PT   = 2,
    UNUSED_PT = 3 // Actually used internally as a NULL value of packets
} packet_type_t;

typedef struct control 
{
    union {
        struct {
            uint8_t nr : 3;
            uint8_t ns : 3;
            packet_type_t type : 2;
        };
        uint8_t all;
    };
} control_t;

typedef struct ack_control
{
    control_t control;
    uint8_t empty;
} ack_control_t;

typedef struct train_msg {
    packet_type_t code;
    uint8_t arg1;
    uint8_t arg2;
} train_msg_t; // 3 bytes

typedef struct packet {
    union {
        struct 
        {
            control_t control_block; // 1 byte
            uint8_t length; // 1 byte
            // TODO: Is this necessary?
            // Anonymous union for direct access
            union { // 3 bytes
                train_msg_t msg;
                struct 
                {
                    packet_type_t code;
                    uint8_t arg1;
                    uint8_t arg2;
                };
            };
        };
        uint8_t tmp_array[5]; // For debugging
    };
    // control_t control_block; // 1 byte
    // uint8_t length; // 1 byte
    // // TODO: Is this necessary?
    // // Anonymous union for direct access
    // union { // 3 bytes
    //     train_msg_t msg;
    //     struct 
    //     {
    //         packet_type_t code;
    //         uint8_t arg1;
    //         uint8_t arg2;
    //     };
    // };
} packet_t; // 5 bytes

typedef struct train_alarm {
    uint8_t ns;
    bool set_flag;
} train_alarm_t;

uint8_t MsgLengthFromCode(uint8_t msg_code);

typedef enum train_direction {
    CW = 0,
    CCW = 8 // 8 because that's the value within its nibble
} train_direction_t;

typedef enum switch_direction {
    DIVERTED = 0,
    STRAIGHT = 1
} switch_direction_t;

typedef struct train_settings
{
    uint8_t speed : 4;
    train_direction_t direction : 4;
} train_settings_t;

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

// To match tables below
enum {
    ROW = 0,
    COL = 1
};

#define NUM_FONT_COLORS       8
#define NUM_BACKGROUND_COLORS 8

// Font & Background Colors Colors
typedef enum {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
} color_t;

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

// [Sensor #][Diverted (0) -> Straight (1)][Row (0) -> Column (1)]
const uint8_t switch_locations[MAX_NUM_SWITCHES + 1][2][2] {
    {{0,  0},  {0,  0 }}, // 0 --> DOES NOT EXIST
    {{5,  22}, {6,  22}}, // 1
    {{7,  40}, {6,  39}}, // 2
    {{5,  59}, {6,  58}}, // 3
    {{17, 59}, {16, 58}}, // 4
    {{15, 42}, {16, 42}}, // 5
    {{17, 22}, {16, 22}}, // 6
};

#define RESET_VT_100_CODE "\e[0m"

#define TRAIN_1_CLR 42 // Train 1 is Green
#define TRAIN_2_CLR 43 // Train 2 is Yellow

#define LIT_HALL_SENSOR_CLR 44 // Active hall sensors are blue

#define CMD_PROMPT_START_ROW    22
#define CMD_PROMPT_START_COLUMN 6

#define CLEAR_SCREEN     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"

#endif /* TrainLibrary_H */
