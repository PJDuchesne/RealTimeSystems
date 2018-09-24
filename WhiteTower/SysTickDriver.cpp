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
-> Name:  SysTickDriver.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/SysTickDriver.h"
#include <iostream>

// Singleton Instance
SysTickDriver *SysTickDriver::SysTickDriverInstance_ = 0;

void SysTickDriver::SysTickStart() {
    // Set the clock source to internal and enable the counter to interrupt
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

void SysTickDriver::SysTickStop() {
    // Clear the enable bit to stop the counter
    ST_CTRL_R &= ~(ST_CTRL_ENABLE); 
}

// If true, enable SysTick
// Else, disable SysTick
void SysTickDriver::SysTickEnable(bool enable) {
    if (enable) {
        // Set the interrupt bit in STCTRL
        ST_CTRL_R |= ST_CTRL_INTEN;
    }
    else {
        // Clear the interrupt bit in STCTRL
        ST_CTRL_R &= ~(ST_CTRL_INTEN);  
    }
}

void SysTickDriver::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
}

// Constructor, which initializes SysTick on startup
SysTickDriver::SysTickDriver() {
    // Set period to 10 sec
    ST_RELOAD_R = MAX_WAIT - 1;
    SysTickEnable(true);
    SysTickStart();
}

void SysTickDriver::SysTickHandler() {
    ISRMsgHandlerInstance_->QueueMsg(SYSTICK, char());
}

SysTickDriver* SysTickDriver::GetSysTickDriver() {
    if (!SysTickDriverInstance_) SysTickDriverInstance_ = new SysTickDriver;
    return SysTickDriverInstance_;
}
