#ifndef TrainProcesses_H
#define TrainProcesses_H

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
-> Name:  TrainProcesses.h
-> Date: Nov 14, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"

// Layers
#include "DataLinkLayer.h"
#include "PhysicalLayer.h"
#include "TrainCommandApplication.h"
#include "TrainCommandCenter.h"
#include "TrainMonitor.h"
#include "TrainTimeServer.h"
#include "TrainController.h"

// Testing functions
void TestLayers();   // Utilizes all three layers to test functionality

// Physical layer loops
void PhysicalLayerUARTLoopEntry();
void PhysicalLayerPacketLoopEntry();

// Data link layer loop
void DataLinkLayerLoopEntry();

// Train Application layer loop
void TrainCommandApplicationLoopEntry();

// Train Monitor Layer loop
void TrainMonitorLoopEntry();

// Train Time Server for handling DLL timeouts
void TrainTimeServer();

// Train Controller for routing the train(s) to various destinations
void TrainControllerLoopEntry();

#endif /* TrainProcesses_H */
