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

/*
    Function: TaskScheduler
    Brief: Constructor for the TaskScheduler, which creates the PCBLists used in the OS
*/
TaskScheduler::TaskScheduler() {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        PCBLists_[i] = new PCBList;
    }
}

/*
    Function: ~TaskScheduler
    Brief: Destructor for the TaskScheduler, which destroys the PCBLists used in the OS
*/
TaskScheduler::~TaskScheduler() {
    for (int i = 0; i <= MAX_PRIORITY; i++) {
        delete PCBLists_[i];
    }
}

/*
    Function: AddProcess
    Input: new_pcb: PCB to add to the relevent PCBList
    Brief: Adds a process to the PCBList indicated in its priority
*/
void TaskScheduler::AddProcess(pcb_t* new_pcb) {
    PCBLists_[new_pcb->priority]->AddPCB(new_pcb);
}

/*
    Function: GetNextPCB
    Output: <Return Value>: Pointer to next PCB in queue
    Brief: Finds the next PCB in queue by iterating from the highest priority to lowest
*/
pcb_t* TaskScheduler::GetNextPCB() {
    // Iterate through each PCBList and return the first that is not full
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) return PCBLists_[i]->NextPCB();
    }

    // If all queues are empty, this will return an uncaught 0 (NULL), which should crash pretty fast
    return 0;
}

/*
    Function: GetCurrentPCB
    Output: <Return Value>: Pointer to current PCB in queue
    Brief: Finds the current PCB in queue by iterating from the highest priority to lowest
           Note: This assumes that the kernel hasn't mucked around with queues during THIS quantum
*/
pcb_t* TaskScheduler::GetCurrentPCB() {
    // Iterate through each PCBList and return the first that is not full
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) return PCBLists_[i]->CurrentPCB();
    }

    // If all queues are empty, this will return an uncaught 0 (NULL), which should crash pretty fast
    return 0;
}

/*
    Function: DeleteCurrentPCB
    Brief: Finds the current PCB in queue by iterating from the highest priority to lowest and deletes it
           Note: This assumes that the kernel hasn't mucked around with queues during THIS quantum
*/
void TaskScheduler::DeleteCurrentPCB() {
    for (int i = MAX_PRIORITY; i >= 0; i--) {
        if (!PCBLists_[i]->IsEmpty()) {
            PCBLists_[i]->DeleteCurrentPCB();
            break;
        }
    }
}

/*
    Function: DiagnosticsDisplay
    Output: display_output: String for the output to be displayed with
    Brief: TaskScheduler (Middle) portion of debugging function that queries each PCBList for
           its processes and a brief description.
*/
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
