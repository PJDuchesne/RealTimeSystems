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
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        PCBLists_[i] = new PCBList;
    }
}

void TaskScheduler::AddProcess(pcb_t* new_pcb) {
    PCBLists_[new_pcb->priority]->AddPCB(new_pcb);
}

pcb_t* TaskScheduler::GetNextPCB() {
    // Iterate through each PCBList and return the first that is not full
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) return PCBLists_[i]->NextPCB();
    }

    // If all queues are empty, this will return an uncaught 0 (NULL), which should crash pretty fast
    return 0;
}

pcb_t* TaskScheduler::GetCurrentPCB() {
    // Iterate through each PCBList and return the first that is not full
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) return PCBLists_[i]->CurrentPCB();
    }

    // If all queues are empty, this will return an uncaught 0 (NULL), which should crash pretty fast
    return 0;
}

void TaskScheduler::DeleteCurrentPCB() {
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) {
            PCBLists_[i]->DeleteCurrentPCB();
            break;
        }
    }
}

void TaskScheduler::DiagnosticsDisplay(std::string &display_output) {
    std::stringstream tmp_ss;

    display_output += "\n\n[DiagnosticsDisplay] Printing PCBLists and their Contents";
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        tmp_ss.str(std::string());
        tmp_ss.clear();
        tmp_ss << "\nPriority >>" << i+1 << "<<: ";
        display_output += tmp_ss.str();
        PCBLists_[i]->DiagnosticsDisplay(display_output);
    }
    display_output += "\n";
}
