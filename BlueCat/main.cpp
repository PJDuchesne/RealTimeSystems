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
-> Name:  main.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

// To allow global configurations (i.e. Master ISR setups, Singleton, etc.)
#include <ISRLayer/Includes/GlobalConfig.h>
#include "OSLayer/Includes/OperatingSystem.h"

/*
    Function: main
    Brief: Initializes interrupts and passes control to the Monitor
*/
int main(void)
{
    // Pass control to OS
    OperatingSystem::GetOperatingSystem()->Inialize();
}
