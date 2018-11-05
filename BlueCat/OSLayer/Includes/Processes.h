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

#define TINY_WAIT  500000	// ~1 second alone
#define SHORT_WAIT 1000000  // ~5 seconds alone
#define LONG_WAIT  3000000	// ~15 seconds alone

void MonitorProcessEntry();

void EndlessProcess();
void ShortProcess();
void LongProcess();

void NiceTestProcess();

void DummpyProcess3();
void DummpyProcess4();

void ReverseString();

void IdleProcess();

// TESTPLAN RUNDOWN:
/*

1) Solo Monitor with full functionality
2) Duo: Monitor + Empty Function to demonstrate switching of same priority
3) Duo: Monitor + ~10 second function at higher priority to demonstrate 
                    termination and activation of monitor
4) Hop/Jump: 2 processes at higher priorities than monitor jumping to lower levels 
5) Message Blocking:
6) 
7)
8)
9)
10)

*/

/*

Major Components of A2
1) Process Switching
2) Process Priority
3) GETID
4) NICE
5) Process Termination
6) Message passing
    - Blocking/Waking
    - 

*/

#endif /* Processes_H */
