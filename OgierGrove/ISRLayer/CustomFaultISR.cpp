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
-> Name: CustomFaultISR.cpp
-> Date: Nov 18, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <ISRLayer/Includes/CustomFaultISR.h>

/*
    Function: CustomFaultISR
    Brief: Used to debug the operating system by accessing singletons 
           and stepping through their states
*/
void Custom_Fault_ISR() {
    std::cout << "\n\n**** Custom_Fault_ISR - ENTERING INFINITY **** \n\n\n";

    // Enter an infinite loop (That can be exited by debugger)
    // bool end_first_loop = false;
    // while(1) 
    // {
    //     if (end_first_loop) break;
    // }

    // Access singletons here
    OperatingSystem* OperatingSystemInstance = OperatingSystem::GetOperatingSystem();
    DataLinkLayer* DataLinkLayerInstance = DataLinkLayer::GetDataLinkLayer();

    #if DEBUGGING_TRAIN >= 1
    OperatingSystemInstance->StackDebug();
    std::cout << "Crashed with on Process   >>" << OperatingSystemInstance->GetCurrentPCB()->name << "<<\n";
    std::cout << "Crashed with quantum_flag >>" << int(OperatingSystemInstance->DEBUGGING_quantum_flag_) << "<<\n";
    #endif

    while(1)
    {

    }
}
