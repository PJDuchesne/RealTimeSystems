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
#include <OSLayer/Includes/OSLibrary.h>
#include <OSLayer/Includes/ProcessCalls.h>

#define NUM_ZONES 20

// Direction
#define STAY 4 // TODO: Find a more elegant solution?
#define CW__ CW
#define CCW_ CCW

#define IDC_ CCW_ // Direction doesn't Matter. Pick either

// Action (NOP, SWITCH STR, SWITCH DIV)
#define NOP_ 0
#define STR_ STR
#define DIV_ DIV

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

typedef struct train_state {
    bool is_moving;
    bool en_route;
    uint8_t current_zone;
} train_state_t;

class TrainController {
    private:
        static TrainController* TrainControllerInstance_;

        train_state_t fast_train; // Train "2"

    public:
        TrainController();

        void TrainControllerLoop();

        static TrainController* GetTrainController();
};

#endif /* TrainController_H */
