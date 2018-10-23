#ifndef TaskScheduler_H
#define TaskScheduler_H

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
-> Name:  TaskScheduler.h
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "OSLibrary.h"
#include "PCBList.h"

class TaskScheduler {
    private:
    	PCBList* PCBLists_[NUM_PRIORITIES];

    public:
        TaskScheduler();
        void AddProcess(pcb_t* new_pcb, priority_t priority);
        pcb_t* GetNextPCB();
};


#endif /* TaskScheduler_H */
