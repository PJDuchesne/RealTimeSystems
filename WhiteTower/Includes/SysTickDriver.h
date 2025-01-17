#ifndef SysTickDriver_H
#define SysTickDriver_H

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
-> Name:  SysTickDriver.h
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "ISRMsgHandler.h"

// SysTick Registers
// SysTick Control and Status Register (STCTRL)
#define ST_CTRL_R   (*((volatile unsigned long *)0xE000E010))  
// SysTick Reload Value Register (STRELOAD)
#define ST_RELOAD_R (*((volatile unsigned long *)0xE000E014))  

// SysTick defines 
#define ST_CTRL_COUNT      0x00010000  // Count Flag for STCTRL
#define ST_CTRL_CLK_SRC    0x00000004  // Clock Source for STCTRL
#define ST_CTRL_INTEN      0x00000002  // Interrupt Enable for STCTRL
#define ST_CTRL_ENABLE     0x00000001  // Enable for STCTRL

// Maximum period
// This value was based on a drift text done with an initial 
// assumed clock speed of 2^24 hz. The clock was found to be ~16.2 MHz
#define MAX_WAIT           0x18B7DD   // 1/10th of a second, after calibration

// Forward Declaration
class ISRMsgHandler;

class SysTickDriver {
    private:
        static SysTickDriver* SysTickDriverInstance_;
        ISRMsgHandler *ISRMsgHandlerInstance_;

        void SysTickStart();
        void SysTickStop();
        void SysTickEnable(bool enable);

    public:
        SysTickDriver();
        void SingletonGrab();
        void SysTickHandler();
        static SysTickDriver* GetSysTickDriver();
};

#endif /* SysTickDriver_H */
