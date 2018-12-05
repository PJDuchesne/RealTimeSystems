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

#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// Flag to turn on 
#define RESEND_TIMERS 0

#define HEX(x) " " << std::setw(2) << std::setfill('0') << std::hex << int(x) << " " << std::dec

#define DELAY(x) i_DELAY = 0; while(i_DELAY++ < x) {}

#define UART_PHYSICAL_LAYER_MB_SIZE 100

#define MAX_PACKET_SIZE 5

#define NUM_HALL_SENSORS 24

#define NUM_ZONES 20
#define NUM_TRAINS 2

#define NO_ZONE    255
#define NO_TRAIN   255
#define NO_INDEX   255
#define NO_HALL    0

#define ALL 255 // Used to acknowledge all sensor resets or throw all switches

// Macros to perform modulus 8 increments and decrements
#define MOD8PLUS1(x) (x = (x + 1) % 8)
#define MOD8MINUS1(x) (x = (x == 0 ? 7 : x - 1))

// Raw Msg types
#define SENSOR      '\xA0'
#define SENSORQUEUE '\xAA'
#define TRAIN       '\xC2'
#define SWITCH      '\xE2'

typedef enum ctrl_msg_type {
    HALL_SENSOR = 0x80,  // 2 Bytes:      1 for this enum, 1 for control block
    SWITCH_CHANGE,       // 2 Bytes:      1 for this enum, 1 for control block
    ZONE_CMD,            // 3 Bytes:      1 for this enum, 1 for the train number and 1 for the train zone
    ZONE_CHANGE,         // 4 Bytes:      1 for this enum, 1 for the train number, 1 for new zone, and 1 for old zone
    PARTIAL_ZONE_CHANGE, // 3 Bytes:      1 for this enum, 1 for the train number, 1 for partial zone
    TRAIN_GO_CMD,        // 3+ Bytes:     1 for this enum, 1 for train num, and at least 1 more for destination(s)
    KICK_CMD,            // 2 Bytes:      1 for this enum, 1 for train num
    SUDO_STOP_CMD,       // 2 Bytes:      1 for this enum, 1 for train num
    STOP_CMD,            // 2 Bytes:      1 for this enum, 1 for train num
} ctrl_msg_type_t;

enum train_layer_mailboxes {
    UART_PHYSICAL_LAYER_MB     = 200,
    PACKET_PHYSICAL_LAYER_MB   = 201,
    DATA_LINK_LAYER_MB         = 202,
    TRAIN_APPLICATION_LAYER_MB = 203,
    TRAIN_TIME_SERVER_MB       = 204,
    TRAIN_MONITOR_MB           = 205,
    TRAIN_CONTROLLER_MB        = 206,
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
            union { // 3 bytes
                train_msg_t msg;
                struct 
                {
                    // packet_type_t code;
                    uint8_t code;
                    uint8_t arg1;
                    uint8_t arg2;
                };
            };
        };
        uint8_t tmp_array[5]; // For debugging
    };
} packet_t; // 5 bytes

typedef struct train_alarm {
    uint8_t ns;
    bool set_flag;
} train_alarm_t;

uint8_t MsgLengthFromCode(uint8_t msg_code);

typedef enum train_direction {
    CW   = 0,
    CCW  = 1,
    STAY = 2 // To signify that the train is currently not moving (Or shouldn't move)
} train_direction_t;

const std::string train_direction_strings[3] {
    "CW",
    "CCW",
    "IDLE"
};

typedef enum switch_direction {
    DIVERTED   = 0,
    STRAIGHT   = 1,
    NOT_NEEDED = 2,
} switch_direction_t;

#define DIV DIVERTED
#define STR STRAIGHT

typedef struct train_settings
{
    uint8_t speed : 4;
    uint8_t dir : 4;
} train_settings_t;

#define MAX_NUM_TRAIN_TOKENS      3
#define MAX_NUM_TRAIN_ARGUMENTS   3

// Number of arguments for each command
#define NUM_TRAIN_ARGS    3
#define NUM_SWITCH_ARGS   2
#define NUM_SENSOR_ARGS   1
#define NUM_QUEUE_ARGS    0

#define NUM_SCREEN_ROWS   24

/* OLD UI: With Switch numbers and hall sensor number */
// const std::string TrainScreen[NUM_SCREEN_ROWS] = {
//   "################################################################################",
//   "#                      20                              19                      #",
//   "#                    ==##==============================##==                    #",
//   "#             14    //    13       12      11       10    \\\\    9              #",
//   "#          ===##====---===##=======##---===##=======##===---====##===          #",
//   "#         //         S1               // S2              S3         \\\\         #",
//   "#        //             ==##=======##==                              \\\\        #",
//   "#    15 ##                24       23                                 ## 8     #",
//   "#       ||                                                            ||       #",
//   "#       ||      Train 1: Spd: XX  Dir: XXXX  Dst: XXXX                ||       #",
//   "#       ||      Train 2: Spd: XX  Dir: XXXX  Dst: XXXX                ||       #",
//   "#       ||                                                            ||       #",
//   "#    16 ##                                 21       22                ## 7     #",
//   "#        \\\\                              ==##=======##==             //        #",
//   "#         \\\\  1     S6    2        3 S5 // 4        5     S4        //         #",
//   "#          ===##====---===##=======##===---##=======##===---====##===          #",
//   "#                   \\\\                                    //    6              #",
//   "#  Hall   ##         ==##==============================##==                    #",
//   "#  Switch -- or //     17                              18                      #",
//   "#______________________________________________________________________________#",
//   "#                                            TX:                               #",
//   "#  >                                                                           #",
//   "#                                                                              #",
//   "################################################################################"
// };

/* New UI: With just zones */
const std::string TrainScreen[NUM_SCREEN_ROWS] = {
  "################################################################################",
  "#                                       19                                     #",
  "#                    ==##==============================##==                    #",
  "#                13 //         12      11       10      9 \\\\                   #",
  "#          ===##====---===##=======##---===##=======##===---====##===          #",
  "#      14 //                   18     //                            \\\\  8      #",
  "#        //             ==##=======##==                              \\\\        #",
  "#       ##                                                            ##       #",
  "#       ||                                                            ||       #",
  "#    15 ||      Train 1: Spd: XX  Dir: XXXX  Dst: XXXX                || 7     #",
  "#       ||      Train 2: Spd: XX  Dir: XXXX  Dst: XXXX                ||       #",
  "#       ||                                                            ||       #",
  "#       ##                                     17                     ##       #",
  "#        \\\\                              ==##=======##==             //        #",
  "#      0  \\\\         1         2      3 //      4          5        //  6      #",
  "#          ===##====---===##=======##===---##=======##===---====##===          #",
  "#                   \\\\                  16                //                   #",
  "#                    ==##==============================##==                    #",
  "#                                                                              #",
  "#______________________________________________________________________________#",
  "#                                            TX:                               #",
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
    WHITE,
    NO_COLOR
} color_t;

const color_t train_colors[NUM_TRAINS] {
    GREEN,
    YELLOW
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
#define VT_100_RESET        "\e[0m" 
#define VT_100_SHOW_CURSOR  "\e[?25h" 
#define VT_100_HIDE_CURSOR  "\e[?25l" 

#define MAX_NUM_SENSORS 24

// [Sensor #][0] -> Row
// [Sensor #][1] -> Column
const uint8_t sensor_locations[MAX_NUM_SENSORS + 1][2] {
    {0,   0}, // 0 -> DOES NOT EXIST
    {16, 15}, // 1
    {16, 27}, // 2
    {16, 36}, // 3
    {16, 44}, // 4
    {16, 53}, // 5
    {16, 65}, // 6
    {13, 71}, // 7
    {8,  71}, // 8 
    {5,  65}, // 9
    {5,  53}, // 10
    {5,  44}, // 11
    {5,  36}, // 12
    {5,  27}, // 13
    {5,  15}, // 14
    {8,   9}, // 15
    {13,  9}, // 16
    {18, 24}, // 17
    {18, 56}, // 18
    {3,  56}, // 19
    {3,  24}, // 20
    {14, 44}, // 21
    {14, 53}, // 22
    {7,  36}, // 23
    {7,  27}, // 24
};

#define MAX_NUM_SWITCHES 6

// [Sensor #][Diverted (0) -> Straight (1)][Row (0) -> Column (1)]
const uint8_t switch_locations[MAX_NUM_SWITCHES + 1][2][2] {
//  DIVERTED   STRAIGHT
    { {0,  0},  {0,  0 } }, // 0 --> DOES NOT EXIST
    { {4,  21}, {5,  21} }, // 1
    { {6,  39}, {5,  38} }, // 2
    { {4,  59}, {5,  58} }, // 3
    { {17, 59}, {16, 58} }, // 4
    { {15, 41}, {16, 41} }, // 5
    { {17, 21}, {16, 21} }, // 6
};

const std::string switch_strings[MAX_NUM_SWITCHES + 1][2] {
    // DIV   STR
    { "",     ""   }, // 0 --> DOES NOT EXIST
    { "//",   "---"}, // 1
    { "//",   "---"}, // 2
    { "\\\\", "---"}, // 3
    { "//",   "---"}, // 4
    { "//",   "---"}, // 5
    { "\\\\", "---"}, // 6
};

#define MAX_COLUMNS_PER_ZONE 4

// For swiches (Zones 1, 3, 5, 9, 11, and 13):
// the 0 index is the STR branch
// the 1-3 indexes are the DIVERTED branch
const uint8_t zone_locations[NUM_ZONES][MAX_COLUMNS_PER_ZONE][3] {
    { {14, 10,  2}, {15, 11,  2}, {16, 12,  3}, { 0,  0,  0} }, // 0
    { {16, 17, 10}, {16, 17,  4}, {17, 21,  2}, {18, 22,  2} }, // 1  // SW 6
    { {16, 29,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 2
    { {16, 38,  6}, {16, 38,  3}, {15, 41,  2}, {14, 42,  2} }, // 3  // SW 5
    { {16, 46,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 4
    { {16, 55, 10}, {16, 61,  4}, {17, 59,  2}, {18, 58,  2} }, // 5  // SW 4
    { {16, 67,  3}, {15, 69,  2}, {14, 70,  2}, { 0,  0,  0} }, // 6
    { { 9, 71,  2}, {10, 71,  2}, {11, 71,  2}, {12, 71,  2} }, // 7
    { { 5, 67,  3}, { 6, 69,  2}, { 7, 70,  2}, { 0,  0,  0} }, // 8
    { { 5, 55, 10}, { 5, 61,  4}, { 4, 59,  2}, { 3, 58,  2} }, // 9  // SW 3
    { { 5, 46,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 10
    { { 5, 38,  6}, { 5, 41,  3}, { 6, 39,  2}, { 7, 38,  2} }, // 11 // SW 2
    { { 5, 29,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 12
    { { 5, 17, 10}, { 5, 17,  4}, { 4, 21,  2}, { 3, 22,  2} }, // 13 // SW 1
    { { 5, 12,  3}, { 6, 11,  2}, { 7, 10,  2}, { 0,  0,  0} }, // 14
    { { 9,  9,  2}, {10,  9,  2}, {11,  9,  2}, {12,  9,  2} }, // 15
    { {18, 26, 30}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 16
    { {14, 46,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 17
    { { 7, 29,  7}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 18
    { { 3, 26, 30}, { 0,  0,  0}, { 0,  0,  0}, { 0,  0,  0} }, // 19
};

const uint8_t switch_zones[MAX_NUM_SWITCHES] { 1, 3, 5, 9, 11, 13 };

// Used to display train information to the center of the monitor
const uint8_t train_info_locations[NUM_TRAINS][5] {
    {10, 17, 31, 40, 51}, 
    {11, 17, 31, 40, 51},
};

#define RESET_VT_100_CODE "\e[0m"

#define TRAIN_1_CLR 42 // Train 1 is Green
#define TRAIN_2_CLR 43 // Train 2 is Yellow

#define LIT_HALL_SENSOR_CLR 44 // Active hall sensors are blue

#define CMD_PROMPT_START_ROW 22
#define CMD_PROMPT_START_COL 6

#define CLEAR_SCREEN     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"

typedef struct switch_ctrl {
    uint8_t num : 4;
    uint8_t req_state : 4;
} switch_ctrl_t; // 1 byte long

typedef struct switch_msg {
    uint8_t type;
    switch_ctrl_t ctrl;
} switch_msg_t;

typedef struct train_ctrl {
    uint8_t num : 2;
    uint8_t speed : 4;
    uint8_t dir : 2;
} train_ctrl_t;

typedef struct sensor_msg_t {
    uint8_t type;
    uint8_t state : 1;
    uint8_t num : 7;
} sensor_msg_t;

#define SWITCH_BUFFER_SIZE 4 // Should never be spamming requests enough to fill this
#define TRAIN_BUFFER_SIZE 8

#endif /* TrainLibrary_H */
