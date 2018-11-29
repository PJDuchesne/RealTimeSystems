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

// Included for testing in TestSwitches()
#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

#define SHORT_DELAY      1000000
#define LESS_SHORT_DELAY 3000000

// Tests layers!
void TestLayers() {
    // TODO: Send mail to this mailbox from the monitor, and then send commands based on that mail
    // Bind a mailbox
    PBind(TEST_PROCESS_MB, ONE_CHAR);

    // For delay
    int i_DELAY;
    DELAY(SHORT_DELAY)

    // for(int i = 0; i < 3; i++) {
    //     TrainCommandCenter::GetTrainCommandCenter()->SendSwitchCommand(6, DIVERTED, TEST_PROCESS_MB);
    //     DELAY(SHORT_DELAY)
    //     TrainCommandCenter::GetTrainCommandCenter()->SendSwitchCommand(6, STRAIGHT, TEST_PROCESS_MB);
    //     DELAY(SHORT_DELAY)
    // }

    // TrainCommandCenter::GetTrainCommandCenter()->SendSensorQueueReset(TEST_PROCESS_MB);

    // DELAY(LESS_SHORT_DELAY)

    // TrainCommandCenter::GetTrainCommandCenter()->SendSwitchCommand(ALL, STRAIGHT, TEST_PROCESS_MB);

    // DELAY(LESS_SHORT_DELAY)

    TrainCommandCenter::GetTrainCommandCenter()->SendTrainCommand(2, 15, CW, TEST_PROCESS_MB);

    // for(int i = 0; i < 5; i++) {
    //     TrainCommandCenter::GetTrainCommandCenter()->SendTrainCommand(2, 4, CCW, TEST_PROCESS_MB);
    //     DELAY(LESS_SHORT_DELAY)
    //     TrainCommandCenter::GetTrainCommandCenter()->SendTrainCommand(2, 4, CW, TEST_PROCESS_MB);
    //     DELAY(LESS_SHORT_DELAY)
    // }

    // // For now, manual testing!
    // TrainCommandCenter::GetTrainCommandCenter()->SendSwitchCommand(ALL, DIVERTED, TEST_PROCESS_MB);

    // No semi-colon needed! Woot woot

    // TrainCommandCenter::GetTrainCommandCenter()->SendTrainCommand(2, 15, CCW, TEST_PROCESS_MB);

    while(1) {}
}

void PhysicalLayerUARTLoopEntry() {
    ISRMsgHandler::GetISRMsgHandler(); // TODO: Put these in one central location, like the kernal startup
    PhysicalLayer::GetPhysicalLayer()->UARTMailboxLoop();
}


void PhysicalLayerPacketLoopEntry() {
    ISRMsgHandler::GetISRMsgHandler();
    PhysicalLayer::GetPhysicalLayer()->PacketMailboxLoop();
}

void DataLinkLayerLoopEntry() {
    ISRMsgHandler::GetISRMsgHandler();
    DataLinkLayer::GetDataLinkLayer()->MailboxLoop();
}

void TrainCommandApplicationLoopEntry() {
    ISRMsgHandler::GetISRMsgHandler();
    TrainCommandApplication::GetTrainCommandApplication()->MailboxLoop();
}

void TrainMonitorLoopEntry() {
    ISRMsgHandler::GetISRMsgHandler();
    TrainMonitor::GetTrainMonitor()->CentralLoop();
}

void TrainTimeServer() {
    ISRMsgHandler::GetISRMsgHandler();
    TimeServer::GetTimeServer()->TrainTimeServerLoop();
}


