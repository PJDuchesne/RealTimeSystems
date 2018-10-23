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
// #include <ISRLayer/Includes/GlobalConfig.h>

// Singleton Instance
OperatingSystem *OperatingSystem::OperatingSystemInstance_ = 0;

/*
    Function:OperatingSystem 
    Brief: Constructor for the OS, which creates a new TaskScheduler
*/
OperatingSystem::OperatingSystem() {
    TaskScheduler_ = new TaskScheduler;
    CurrentPCB_ = 0;
}


void OperatingSystem::Inialize() {
    // Register all initial processes
    RegProc(&MonitorProcess, 123, P_THREE, "Monitor");
    RegProc(&DummpyProcess2, 456, P_THREE, "DummpyProcess2");
    RegProc(DummpyProcess3, 789, P_THREE, "DummpyProcess3");

    // Pass control to first process
    KickStart();
}

void OperatingSystem::RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name) {
    // Make new pcb
    pcb_t* new_pcb = new pcb();

    // Allocate stack
    new_pcb->stack_start = (uint32_t)(new uint32_t[STACKSIZE]);

    // Add stack pointer
    new_pcb->stack_ptr = (uint32_t)(new_pcb->stack_start + STACKSIZE*sizeof(uint32_t) - sizeof(stack_frame_t));

    // Add pid and name
    new_pcb->pid = pid;
    new_pcb->name = name;

    // Initialize the stackframe within this block of memory, registers and entry point
    stack_frame_t* new_sf = (stack_frame_t*)new_pcb->stack_ptr;
    InitStackFrame(new_sf);
    new_sf->pc = (uint32_t) entry_point;

    // Set PSR to thumb mode
    new_sf->psr = THUMB_MODE;
    new_sf->lr = 0;

    // Initialize quantum number to 0 for diagnostics
    new_pcb->q_count = 0;

    // Initialize to 0 to find issues
    new_pcb->next = 0;
    new_pcb->prev = 0;

    // Add to PCBList queue
    TaskScheduler_->AddProcess(new_pcb, priority);
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
    CurrentPCB_ = GetNextPCB();
    set_PSP(CurrentPCB_->stack_ptr);
    // restore_registers();
    SVC();
}

pcb_t* OperatingSystem::GetNextPCB() {
    return TaskScheduler_->GetNextPCB();
}

pcb_t* OperatingSystem::GetCurrentPCB() {
    return CurrentPCB_;
}

void OperatingSystem::QuantumTick() {

    // pcb_t* old_pcb = CurrentPCB_;
    // while (1) {
    //     std::cout << "[WTF] >>" << GetNextPCB()->name << "<<\n";
    // }
    CurrentPCB_->q_count++;

    // 1: Save registers
    save_registers();
    // 2: Set Stack Pointer on current PCB
    CurrentPCB_->stack_ptr = get_PSP();
    // 3: Get new process PCB and set stack pointer from it
    CurrentPCB_ = GetNextPCB();
    set_PSP(CurrentPCB_->stack_ptr);
    // 4: Restore_Registers
    restore_registers();
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
