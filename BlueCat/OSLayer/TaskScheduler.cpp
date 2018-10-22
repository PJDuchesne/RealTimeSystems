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
-> Name:  TaskScheduler.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TaskScheduler.h"

TaskScheduler::TaskScheduler() {
	for (int i = 0; i < NUM_PRIORITIES; i++) {
		PCBLists_[i] = new PCBList;
	}
}

void TaskScheduler::AddProcess(pcb_t* new_pcb, priority_t priority) {
	PCBLists_[priority - 1]->AddPCB(new_pcb);
}

pcb_t* TaskScheduler::GetNextPCB() {
	// Iterate through each PCBList and return the first that is not full
	for (int i = NUM_PRIORITIES - 1; i >= 0; i--) {
		if (!PCBLists_[i]->IsEmpty()) return PCBLists_[i]->NextPCB();
	}

	// If all queues are empty, this will return an uncaught 0 (NULL), which should crash pretty fast
	return 0;
}
