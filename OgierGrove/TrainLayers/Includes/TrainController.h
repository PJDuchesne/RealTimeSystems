#ifndef TrainController_H
#define TrainController_H

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
-> Name:  TrainController.h
-> Date: Nov 29, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"
#include "TrainMonitor.h" // Includes the CommandCenter
// #include "TrainCommandCenter.h"
#include <OSLayer/Includes/OSLibrary.h>
#include <OSLayer/Includes/ProcessCalls.h>
#include <OSLayer/Includes/RingBuffer.h>

// Direction
#define CW__ CW
#define CCW_ CCW

#define IDC_ CCW_ // Direction doesn't Matter. Pick either

// Action (NOP, SWITCH STR, SWITCH DIV)
#define DIV_ DIV        // 0
#define STR_ STR        // 1
#define NOP_ NOT_NEEDED // 2

// 0) Direction     ()
// 1) Switch Action (switch_direction_t)
// 2) Switch Num    (1 to 6)
const uint8_t routing_table[NUM_ZONES][NUM_ZONES][3] {
// FROM  TO>   0                 1                 2                 3                 4                 5                 6                 7                 8                 9                 10                11                12                13                14                15                16                17                18                19   
/* 0  */    { {STAY, NOP_, 0},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {CCW_, STR_, 6},  {IDC_, STR_, 6},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, DIV_, 6},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0} },
/* 1  */    { {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0} },
/* 2  */    { {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {CCW_, STR_, 5},  {IDC_, STR_, 5},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, DIV_, 5},  {IDC_, NOP_, 0},  {CW__, NOP_, 0} },
/* 3  */    { {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0} },
/* 4  */    { {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0} },
/* 5  */    { {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0} },
/* 6  */    { {CW__, STR_, 4},  {CW__, STR_, 4},  {CW__, STR_, 4},  {CW__, STR_, 4},  {CW__, STR_, 4},  {CW__, STR_, 4},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, DIV_, 4},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0} },
/* 7  */    { {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0} },
/* 8  */    { {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CCW_, STR_, 3},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, DIV_, 3} },
/* 9  */    { {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0} },
/* 10 */    { {CCW_, STR_, 2},  {CCW_, STR_, 2},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, STR_, 2},  {CCW_, STR_, 2},  {CCW_, STR_, 2},  {CCW_, STR_, 2},  {CCW_, STR_, 2},  {CW__, NOP_, 0},  {IDC_, NOP_, 0},  {CCW_, DIV_, 2},  {CW__, NOP_, 0} },
/* 11 */    { {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {IDC_, NOP_, 0} },
/* 12 */    { {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0} },
/* 13 */    { {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CCW_, NOP_, 0} },
/* 14 */    { {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {CW__, STR_, 1},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, DIV_, 1} },
/* 15 */    { {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {IDC_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0},  {STAY, NOP_, 0},  {CCW_, NOP_, 0},  {CCW_, NOP_, 0},  {CW__, NOP_, 0},  {CW__, NOP_, 0} },
/* 16 */    { {CW__, DIV_, 6},  {CW__, DIV_, 6},  {CW__, DIV_, 6},  {CW__, DIV_, 6},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4},  {CW__, DIV_, 6},  {CW__, DIV_, 6},  {CW__, DIV_, 6},  {CW__, DIV_, 6},  {STAY, NOP_, 0},  {CW__, DIV_, 6},  {CCW_, DIV_, 4},  {CCW_, DIV_, 4} },
/* 17 */    { {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5},  {STAY, NOP_, 5},  {CCW_, DIV_, 5},  {CCW_, DIV_, 5} },
/* 18 */    { {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {CW__, DIV_, 2},  {STAY, NOP_, 2},  {CW__, DIV_, 2} },
/* 19 */    { {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CW__, DIV_, 3},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CCW_, DIV_, 1},  {CW__, DIV_, 3},  {STAY, NOP_, 0} },
};

#define MAX_ZONES_PER_TRAIN 2

#define MAX_DIFF_SENSORS 3 

typedef struct sensor_trigger {
    union {
        struct
        {
            uint8_t sensor_num;
            uint8_t num_triggers : 4;
            uint8_t age : 4;
        };
        uint16_t reset; // Reset to 0
    };
} sensor_trigger_t;

typedef struct train_state {
    uint8_t current_zone;

    // 0) Sensor # that has triggered recently
    // 1) Number of triggers that has happened
    sensor_trigger_t triggered_sensors[MAX_DIFF_SENSORS];

    union {
        struct
        {
            uint8_t last_hall_triggered;
            uint8_t second_last_hall_triggered;
        };
        uint16_t last_two_hall_triggers;
    };

    // Number, direction, and speed
    train_ctrl_t train_ctrl;

    uint8_t current_dst;
    uint8_t default_speed;

} train_state_t;

#define MAX_NUM_SENSORS_FROM_A_ZONE_FOR_A_DIRECTION 2

// 1) Current Zone
// 2) Current Train Direction
// 3) Possible Hall Sensors (From 1 to 24, 0 is NULL), only 2 possible options
const uint8_t possible_halls[NUM_ZONES][2][MAX_NUM_SENSORS_FROM_A_ZONE_FOR_A_DIRECTION] {
//         CW         CCW
/* 0  */ { {16, NO_HALL},  {1,  NO_HALL} },
/* 1  */ { {1,  NO_HALL},  {2,  17}      },
/* 2  */ { {2,  NO_HALL},  {3,  NO_HALL} },
/* 3  */ { {3,  NO_HALL},  {4,  21}      },
/* 4  */ { {4,  NO_HALL},  {5,  NO_HALL} },
/* 5  */ { {5,  18     },  {6,  NO_HALL} },
/* 6  */ { {6,  NO_HALL},  {7,  NO_HALL} },
/* 7  */ { {7,  NO_HALL},  {8,  NO_HALL} },
/* 8  */ { {8,  NO_HALL},  {9,  NO_HALL} },
/* 9  */ { {9,  NO_HALL},  {10, 19}      },
/* 10 */ { {10, NO_HALL},  {11, NO_HALL} },
/* 11 */ { {11, NO_HALL},  {12, 23}      },
/* 12 */ { {12, NO_HALL},  {13, NO_HALL} },
/* 13 */ { {13, 20     },  {14, NO_HALL} },
/* 14 */ { {14, NO_HALL},  {15, NO_HALL} },
/* 15 */ { {15, NO_HALL},  {16, NO_HALL} },
/* 16 */ { {17, NO_HALL},  {18, NO_HALL} },
/* 17 */ { {22, NO_HALL},  {21, NO_HALL} },
/* 18 */ { {23, NO_HALL},  {24, NO_HALL} },
/* 19 */ { {19, NO_HALL},  {20, NO_HALL} },
};

#define ER 255 // Error state

// 1) Hall Triggered
// 2) Current Train Direction
// 3) Possible Hall Sensors (From 1 to 24, 0 is NULL), only 2 possible options
const uint8_t possible_zones[NUM_HALL_SENSORS + 1][2] {
//         CW  CCW
/* 0  */ { ER, ER }, // This sensor does not exist
/* 1  */ { 0,   1 },
/* 2  */ { 1,   2 },
/* 3  */ { 2,   3 },
/* 4  */ { 3,   4 },
/* 5  */ { 4,   5 },
/* 6  */ { 5,   6 },
/* 7  */ { 6,   7 },
/* 8  */ { 7,   8 },
/* 9  */ { 8,   9 },
/* 10 */ { 9,  10 },
/* 11 */ { 10, 11 },
/* 12 */ { 11, 12 },
/* 13 */ { 12, 13 },
/* 14 */ { 13, 14 },
/* 15 */ { 14, 15 },
/* 16 */ { 15,  0 },
/* 17 */ {  1, 16 },
/* 18 */ { 16,  5 },
/* 19 */ {  9, 19 },
/* 20 */ { 19, 13 },
/* 21 */ {  3, 17 },
/* 22 */ { 17, ER }, // Not a feasible state
/* 23 */ { 11, 18 },
/* 24 */ { 18, ER }, // Not a feasible state
};

#define NUM_DANGER_ZONES 6 // Equal to the number of switches

// Number of danger zones
// [0] -> Zone num
// [1] -> Direction
const uint8_t routing_needed_zones[NUM_DANGER_ZONES][2] {
    {  0, CCW },
    {  2, CCW },
    {  6, CCW },
    {  8, CCW },
    { 10, CCW },
    { 14, CCW },
};

class TrainCommandCenter;
class TrainMonitor;

class TrainController {
    private:
        static TrainController* TrainControllerInstance_;

        TrainCommandCenter* TrainCommandCenterInstance_;
        TrainMonitor* TrainMonitorInstance_;

        RingBuffer<switch_ctrl_t> *switch_msg_buffer_;
        RingBuffer<train_ctrl_t> *train_msg_buffer_;

        train_state_t trains_[NUM_TRAINS];

        void SetSwitch(switch_ctrl_t ctrl);
        void CmdTrain(train_ctrl_t train_ctrl);
        void StopTrain(uint8_t train_num);
        uint8_t WhichTrain(uint8_t hall_sensor_num);
        void HandleZoneChange(uint8_t hall_sensor_num, uint8_t train_num);

        void CheckIfRoutingNeeded(uint8_t train_num); // Calls RouteTrain for trains en_route
        void RouteTrain(uint8_t train_num); // Routes from any zone to any other zone using routing_table


        void HandleSensorTrigger(char* msg_body, uint8_t msg_len);

    public:
        TrainController();
        ~TrainController();

        void TrainControllerLoop();

        void SingletonGrab();

        static TrainController* GetTrainController();
};

#endif /* TrainController_H */
