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

/*
    Function: SysTickStart
    Brief: Set the clock source to internal and enable the counter to interrupt
*/
void SysTickDriver::SysTickStart() {
    ST_CTRL_R |= ST_CTRL_CLK_SRC | ST_CTRL_ENABLE;
}

/*
    Function: SysTickStop
    Brief: Clear the enable bit to stop the counter
           (Not used in the code, but was included in example code)
*/
void SysTickDriver::SysTickStop() {
    ST_CTRL_R &= ~(ST_CTRL_ENABLE);
}

// If true, enable SysTick
// Else, disable SysTick
/*
    Function: SysTickStop
    Input: enable: True to enable SysTick and false to disable it
    Brief: Used to enable or disable the SysTick interrupt
*/
void SysTickDriver::SysTickEnable(bool enable) {
    if (enable) ST_CTRL_R |= ST_CTRL_INTEN;  // Set the interrupt bit in STCTRL
    else ST_CTRL_R &= ~(ST_CTRL_INTEN);      // Clear the interrupt bit in STCTRL
}

/*
    Function: SingletonGrab
    Brief: Setup function for the SysTickDriver to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void SysTickDriver::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
}

/*
    Function: SysTickDriver
    Brief: Constructor for SysTickDriver, which sets up the SysTick interrupt on startup
*/
SysTickDriver::SysTickDriver() {
    ST_RELOAD_R = MAX_WAIT; // Set period to 10 sec
    SysTickEnable(true);
    SysTickStart();
}

/*
    Function: SysTickHandler
    Brief: SysTick ISR, which queues the interrupt into the ISR message queue with the MsgHandler
*/
void SysTickDriver::SysTickHandler() {
    ISRMsgHandlerInstance_->QueueISRMsg(SYSTICK, char());
}

/*
    Function: GetSysTickDriver
    Output: SysTickDriverInstance_: Pointer to the SysTickDriver Singleton
    Brief: Get function for the SysTickDriver singleton
*/
SysTickDriver* SysTickDriver::GetSysTickDriver() {
    if (!SysTickDriverInstance_) SysTickDriverInstance_ = new SysTickDriver;
    return SysTickDriverInstance_;
}
