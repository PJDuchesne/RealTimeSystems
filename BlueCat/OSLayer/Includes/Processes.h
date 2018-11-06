#ifndef Processes_H
#define Processes_H

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
-> Name:  Processes.h
-> Date: Nov 5, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

#define TINY_WAIT  500000
#define SHORT_WAIT 1000000
#define LONG_WAIT  3000000

void MonitorProcessEntry();

void EndlessProcess();
void ShortProcess();
void LongProcess();

void NiceTestProcess();

void SendProcess();
void RecvProcess();

void SendConstantRate();
void RecvNonBlockingProcess();

void ReverseString();

void IdleProcess();

#endif /* Processes_H */
