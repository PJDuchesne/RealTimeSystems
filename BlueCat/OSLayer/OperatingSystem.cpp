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
-> Name:  OperatingSystem.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/OperatingSystem.h"
#include "Includes/KernelFunctions.h"

// Singleton Instance
OperatingSystem *OperatingSystem::OperatingSystemInstance_ = 0;

/*
    Function:OperatingSystem 
    Brief: Constructor for the command center, initializes the FunctionTable
*/
OperatingSystem::OperatingSystem() {
    // TODO: Add OS init codes
}

/*
    Function: SingletonGrab
    Brief: Setup function for the OperatingSystem to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void OperatingSystem::SingletonGrab() {
    // TODO: Add singletons to grab (If any)
}

/*
    Function: GetOperatingSystem
    Output: OperatingSystemInstance_: Pointer to the OperatingSystem Singleton
    Brief: Get function for the OperatingSystem singleton
*/
OperatingSystem* OperatingSystem::GetOperatingSystem() {
    if (!OperatingSystemInstance_) OperatingSystemInstance_ = new OperatingSystem;
    return OperatingSystemInstance_;
}
