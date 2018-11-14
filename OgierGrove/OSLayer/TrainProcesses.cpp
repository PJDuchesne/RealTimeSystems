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
-> Name:  TrainProcesses.cpp
-> Date: Nov 14, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainProcesses.h"

void TestSwitches() {
    std::cout << "TestSwitches(): Starting\n";
    // Make a packet

    int i = 0;

    while (i < 1000000) { i++; }

    std::cout << "TestSwitches(): Sending\n";

    // TODO: use constructor
    control_t control_block;
    control_block.nr = 0;
    control_block.ns = 0;
    control_block.type = DATA_PT;

    // Or just do this
    // switchFrame[0] = control(0, 0, DATA_PT); // Control

    // Make frame
    unsigned char switchFrame[5];
    switchFrame[0] = control_block.all; // Control
    switchFrame[1] = 3;                 // Length
    switchFrame[2] = '\xe0';            // Payload
    switchFrame[3] = '\xff';            // ""
    switchFrame[4] = '\x00';            // "" -> Straight
    // switchFrame[4] = "\x01";         // "" -> Diverted

    // Shove packet out UART1
    ISRMsgHandler::GetISRMsgHandler()->QueueOutputMsg((char *)switchFrame, 5, UART1);

    std::cout << "TestSwitches(): Ending\n";

    // Stay around to diagnostics
    while (1) {

    }
}

