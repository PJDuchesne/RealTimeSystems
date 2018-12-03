#ifndef CustomFaultISR_H
#define CustomFaultISR_H

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
-> Name: CustomFaultISR.h
-> Date: Nov 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <iostream>
#include <string>

#include <OSLayer/Includes/OperatingSystem.h>
#include <TrainLayers/Includes/DataLinkLayer.h>
#include <TrainLayers/Includes/TrainController.h>
#include <TrainLayers/Includes/TrainCommandCenter.h>

void Custom_Fault_ISR();

#endif /* CustomFaultISR_H */
