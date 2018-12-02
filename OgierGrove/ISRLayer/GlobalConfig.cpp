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
-> Name: GlobalConfig.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/GlobalConfig.h"

/*
    Function: InterruptMasterEnable
    Brief: Makes an assembly call to enable interupts globally
*/
void InterruptMasterEnable() {
    __asm("    cpsie   i");
}

/*
    Function: InterruptEnable
    Brief: General purpose function to access and enable specific interrupts
*/
void InterruptEnable(unsigned long InterruptIndex) {
    /* Indicate to CPU which device is to interrupt */
    if(InterruptIndex < 32) NVIC_EN0_R = 1 << InterruptIndex; // Enable the interrupt in the EN0 Register
    else NVIC_EN1_R = 1 << (InterruptIndex - 32); // Enable the interrupt in the EN1 Register
}

/*
    Function: SingletonSetup
    Brief: Calls SingletonGrab for each singleton in the project. This both instantiating the Singleton by
           calling its GetInstance() function for the first time and sets the pointers to other singletons
           for future use. This also prevents any possibility of a the empty singleton pointers in each
           singleton to be used before being setup.
*/
void SingletonSetup() {
    SysTickDriver::GetSysTickDriver()->SingletonGrab();
    Monitor::GetMonitor()->SingletonGrab(); // TODO: Remove this when the new monitor is fully used.
    CommandCenter::GetCommandCenter()->SingletonGrab();
    TimeHandler::GetTimeHandler()->SingletonGrab();
}

/*
    Function: CustomFaultISR
    Brief: Used to debug the operating system by accessing singletons and steping through their states
*/
void CustomFaultISR() {
    std::cout << "\n\n**** FaultISR - ENTERING INFINITY **** \n\n\n";

    while(1) 
    {

    }
}
