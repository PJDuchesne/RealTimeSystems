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
-> Name:  OperatingSystem.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/OperatingSystem.h"

// Singleton Instance
OperatingSystem *OperatingSystem::OperatingSystemInstance_ = 0;

/*
    Function:OperatingSystem 
    Brief: Constructor for the OS, which creates a new TaskScheduler
*/
OperatingSystem::OperatingSystem() {
    TaskScheduler_ = new TaskScheduler;
}


void OperatingSystem::Inialize() {
    // Register all initial processes
    RegProc(&MonitorProcess, 123, P_THREE, "Monitor");
    RegProc(&DummpyProcess2, 456, P_FIVE, "DummpyProcess2");
    // RegProc(&DummpyProcess3, 789, P_THREE, "DummpyProcess3");

    // Pass control to first process
    KickStart();
}

void OperatingSystem::RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name) {
    // Make new pcb
    pcb_t* new_pcb = new pcb();

    // Allocate stack and store its pointer (For eventual termination)
    new_pcb->stack_start = new uint32_t[STACKSIZE]; 

    // Add stack pointer
    new_pcb->stack_ptr = (uint32_t)(new_pcb->stack_start + STACKSIZE*sizeof(uint32_t) - sizeof(stack_frame_t));

    // Add pid and name
    new_pcb->pid = pid;
    new_pcb->name = name;
    new_pcb->priority = priority;

    // Initialize the stackframe within this block of memory, registers and entry point
    stack_frame_t* new_sf = (stack_frame_t*)new_pcb->stack_ptr;
    InitStackFrame(new_sf);
    new_sf->pc = (uint32_t) entry_point;

    // Set PSR to thumb mode
    new_sf->psr = THUMB_MODE;

    // Set LR to the TerminateProcess entry point
    new_sf->lr = (uint32_t) PTerminateProcess;

    // Initialize quantum number to 0 for diagnostics
    new_pcb->q_count = 0;

    // Initialize to 0 to find issues
    new_pcb->next = 0;
    new_pcb->prev = 0;

    // Add to PCBList queue
    QueuePCB(new_pcb);
}

void OperatingSystem::InitStackFrame(stack_frame_t* sf) {
    sf->r0 = 0;
    sf->r1 = 1;
    sf->r2 = 77;
    sf->r3 = 3;
    sf->r4 = 4;
    sf->r5 = 5;
    sf->r6 = 6;
    sf->r7 = 7;
    sf->r8 = 8;
    sf->r9 = 9;
    sf->r10 = 10;
    sf->r11 = 11;
    sf->r12 = 12;
}

void OperatingSystem::KickStart() {
    // Force start new process
    set_PSP(GetNextPCB()->stack_ptr);
    SVC();
}

pcb_t* OperatingSystem::GetCurrentPCB() {
    return TaskScheduler_->GetCurrentPCB();
}

pcb_t* OperatingSystem::GetNextPCB() {
    return TaskScheduler_->GetNextPCB();
}

void OperatingSystem::DeleteCurrentPCB() {
    TaskScheduler_->DeleteCurrentPCB();
}

void OperatingSystem::QuantumTick() {
    // Fetch CurrentPCB for function
    pcb_t* CurrentPCB = TaskScheduler_->GetCurrentPCB();

    CurrentPCB->q_count++;

    // 1: Save registers
    save_registers();
    // 2: Set Stack Pointer on current PCB
    CurrentPCB->stack_ptr = get_PSP();
    // 3: Get new process PCB and set stack pointer from it
    CurrentPCB = GetNextPCB();
    set_PSP(CurrentPCB->stack_ptr);
    // 4: Restore_Registers
    restore_registers();
}

void OperatingSystem::QueuePCB(pcb_t* new_pcb) {
    TaskScheduler_->AddProcess(new_pcb);
}

void OperatingSystem::DiagnosticsDisplay(std::string &display_output) {
    display_output = "";
    TaskScheduler_->DiagnosticsDisplay(display_output);
}

/*
    Function: GetOperatingSystem
    Output: OperatingSystemInstance_: Pointer to the OperatingSystem Singleton
    Brief: Get function for the OperatingSystem singleton
*/
OperatingSystem* OperatingSystem::GetOperatingSystem() {
    if (!OperatingSystemInstance_) OperatingSystemInstance_ = new OperatingSystem;
    return OperatingSystemInstance_;
}
