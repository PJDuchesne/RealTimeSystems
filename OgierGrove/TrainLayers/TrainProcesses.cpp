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

void PhysicalLayerUARTLoopEntry() {
    PhysicalLayer::GetPhysicalLayer()->UARTMailboxLoop();
}

void PhysicalLayerPacketLoopEntry() {
    PhysicalLayer::GetPhysicalLayer()->PacketMailboxLoop();
}

void DataLinkLayerLoopEntry() {
    DataLinkLayer::GetDataLinkLayer()->MailboxLoop();
}

void TrainCommandApplicationLoopEntry() {
    TrainCommandApplication::GetTrainCommandApplication()->MailboxLoop();
}

void TrainMonitorLoopEntry() {
    TrainMonitor::GetTrainMonitor()->CentralLoop();
}

void TrainTimeServer() {
    TrainTimeServer::GetTrainTimeServer()->TrainTimeServerLoop();
}

void TrainControllerLoopEntry() {
    TrainController::GetTrainController()->TrainControllerLoop();
}
