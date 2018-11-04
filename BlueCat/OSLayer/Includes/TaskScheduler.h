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
        PCBList* PCBLists_[MAX_PRIORITY + 1];

    public:
        TaskScheduler();
        void AddProcess(pcb_t* new_pcb);
        pcb_t* GetNextPCB();
        pcb_t* GetCurrentPCB();
        void DeleteCurrentPCB();

        void DiagnosticsDisplay(std::string &display_output);
};


#endif /* TaskScheduler_H */
