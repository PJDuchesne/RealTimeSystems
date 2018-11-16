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

#include "Includes/TrainLibrary.h"

// Included for testing in TestSwitches()
#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

void TestSwitches() {
    // Test mailbox is 200
    // PBind(200, ZERO_CHAR); // TODO: Why does this crash??

    std::cout << "TestSwitches(): Starting\n";
    // Make a packet

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

    // Switches
    // switchFrame[2] = '\xe0';            // Payload
    // switchFrame[3] = '\xff';            // ""
    // switchFrame[4] = '\x00';            // "" -> Straight
    // switchFrame[4] = '\x01';            // "" -> Diverted

    // Train Speed
    switchFrame[2] = '\xc0';          // Payload
    switchFrame[3] = '\x01';          // Train #2
    switchFrame[4] = '\x0F';         // "" -> CW, speed 8

    int i = 0;
    bool flag = false;

    while (1) {
        // Delay
        // Toggle

        // Train 1
        ISRMsgHandler::GetISRMsgHandler()->QueueOutputPacket((char *)switchFrame, 5);

        // // // train 2
        // switchFrame[3] = '\x02';   // Train #1
        // switchFrame[4] = '\x08';         // "" -> CW, speed 8

        // control_block.ns++;
        // switchFrame[0] = control_block.all; // Control
        // ISRMsgHandler::GetISRMsgHandler()->QueueOutputPacket((char *)switchFrame, 5);



        // switchFrame[0] = control_block.all; // Control
        // if (flag) {
        //     // switchFrame[4] = '\x00';            // "" -> Straight
        //     ISRMsgHandler::GetISRMsgHandler()->QueueOutputPacket((char *)switchFrame, 5);
        //     flag = false;
        // }
        // else {
        //     // switchFrame[4] = '\x01';                // "" -> Diverted
        //     ISRMsgHandler::GetISRMsgHandler()->QueueOutputPacket((char *)switchFrame, 5);
        //     flag = true;
        // }

        control_block.ns++;

        break;

        i = 0;
        while (i++ < 10000000) {

        }
    }

    while (1) {}
}

void PhysicalLayerUARTLoopEntry() {
    PhysicalLayer::GetPhysicalLayer()->UARTMailboxLoop();
}


void PhysicalLayerPacketLoopEntry() {
    PhysicalLayer::GetPhysicalLayer()->PacketMailboxLoop();
}


