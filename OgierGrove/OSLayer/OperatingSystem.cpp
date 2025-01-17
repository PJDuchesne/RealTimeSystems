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

#if DEBUGGING_TRAIN >= 1
#include <ISRLayer/Includes/CustomFaultISR.h>
#endif

// Singleton Instance
OperatingSystem *OperatingSystem::OperatingSystemInstance_ = 0;

/*
    Function: OperatingSystem 
    Brief: Constructor for the OS, which creates a new TaskScheduler.
*/
OperatingSystem::OperatingSystem() {
    TaskScheduler_ = new TaskScheduler;

    // Initialize PostOffice singleton and store reference for future use
    PostOfficeInstance_ = PostOffice::GetPostOffice();
}

/*
    Function: Inialize
    Brief: Initializes the OS by registering processes and then kickstarting the first in queue.
*/
void OperatingSystem::Inialize() {
    uint8_t test_case = 7;

    // Initialize ISR_MSG_Handler Singleton
    ISRMsgHandler::GetISRMsgHandler();

    std::cout << "Launching things\n";

    bool idle_needed_flag = true;
    switch (test_case) {
        case 1: // Default running with monitor and several empty processes
            RegProc(&EndlessProcess, 20, P_TWO,   "Endless_2");
            RegProc(&EndlessProcess, 30, P_THREE, "Endless_3");
            RegProc(&EndlessProcess, 40, P_FOUR,  "Endless_4");
            RegProc(&EndlessProcess, 50, P_FIVE,  "Endless_5");
            RegProc(&EndlessProcess, 60, P_FIVE,  "Endless_6");
            RegProc(&MonitorProcessEntry, 123, P_FIVE, "Monitor");
            break;
        case 2: // Testing termination
            RegProc(&ShortProcess, 30, P_THREE, "ShortProcess_3");
            RegProc(&ShortProcess, 40, P_FOUR,  "ShortProcess_4");
            RegProc(&LongProcess, 50, P_FIVE,   "LongProcess_5");
            RegProc(&LongProcess, 60, P_FIVE,   "LongProcess_6");           
            // Add monitor to catch diagnostics messages
            RegProc(&MonitorProcessEntry, 123, P_FIVE, "Monitor");
            break;
        case 3: // Testing NICE
            RegProc(&LongProcess, 30, P_THREE, "LongProcess_3");
            RegProc(&LongProcess, 40, P_FOUR, "LongProcess_4");
            RegProc(&NiceTestProcess, 50, P_FIVE, "NiceTestProcess");
            // Add monitor to catch diagnostics messages
            RegProc(&MonitorProcessEntry, 123, P_FIVE, "Monitor");
            break;
        case 4: // Test non-blocking message passing
            RegProc(&MonitorProcessEntry, 123, P_FIVE, "Monitor");
            RegProc(&SendConstantRate, 25, P_THREE, "SendConstantRate");
            RegProc(&RecvNonBlockingProcess, 50, P_THREE, "RecvNonBlockingProcess");
            break;
        case 5: // Test blocking message passing
            RegProc(&MonitorProcessEntry, 123, P_THREE, "Monitor");
            RegProc(&SendProcess, 70, P_THREE, "SendProcess");
            RegProc(&RecvProcess, 80, P_THREE, "RecvProcess");
            break;
        case 6: // Test UART passing message to monitor and then replying
            RegProc(&MonitorProcessEntry, 123, P_THREE, "Monitor");
            RegProc(&ReverseString, 124, P_THREE, "ReverseString");
            break;
        case 7: // Trains!
            RegProc(&TrainMonitorLoopEntry, 205, P_THREE, "TrainMonitor");

            // Add physical layer loops
            RegProc(&PhysicalLayerUARTLoopEntry,   200, P_THREE, "PhysicalLayerUARTLoop");
            RegProc(&PhysicalLayerPacketLoopEntry, 201, P_THREE, "PhysicalLayerPacketLoop");

            // Add data link layer loop
            RegProc(&DataLinkLayerLoopEntry, 202, P_THREE, "DataLinkLayerLoop");

            // Add Train Command loop (Application Layer)
            RegProc(&TrainCommandApplicationLoopEntry, 203, P_THREE, "DataLinkLayerLoop");

            // Add new time server to perform DLL timeouts
            RegProc(&TrainTimeServer, 204, P_THREE, "TrainTimeServer");

            // Add Train Controller loop entry
            RegProc(&TrainControllerLoopEntry, 206, P_THREE, "TrainController");

            break;
        default:
            idle_needed_flag = false;
            RegProc(&IdleProcess, 1, P_ONE, "IdleProcess");
            break;
    }

    if (idle_needed_flag) RegProc(&EndlessProcess, 1, P_ONE, "IdleProcess");

    // Pass control to first process
    KickStart();
}

/*
    Function: RegProc
    Inputs: entry_point: A pointer to the position in memory of the process itself
            pid: A process identification number to attach to the process
            priority: The priority for the process to start with (1-5)
            name: String name for the process (Essentially used for debugging)
    Brief: Registers a process by allocating its PCB and stack before queueing it to it's PCBList.
*/
void OperatingSystem::RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name) {
    // Make new pcb
    pcb_t* new_pcb = new pcb();

    // Allocate stack and store its pointer (For eventual termination)
    new_pcb->stack_start = new uint32_t[STACKSIZE]; 

    // Add stack pointer
    new_pcb->stack_ptr = ((uint32_t)new_pcb->stack_start + STACKSIZE*sizeof(uint32_t) - sizeof(stack_frame_t));

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

    #if DEBUGGING_TRAIN >= 1

    debugging_pcbs[pcb_idx++] = new_pcb;

    #endif
}

/*
    Function: InitStackFrame
    Input: sf: A pointer to the stack frame to update
    Brief: Initializes the stack frame to known values (Primarily used for debugging).
*/
void OperatingSystem::InitStackFrame(stack_frame_t* sf) {
    sf->r0 = 0;
    sf->r1 = 1;
    sf->r2 = 2;
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

/*
    Function: KickStart
    Brief: Sets the process stack pointer to the next PCB in queue and then
           makes and SVC call to start it (In conjunction with code within 
           the SCV call itself).
*/
void OperatingSystem::KickStart() {
    set_PSP(GetNextPCB()->stack_ptr);
    SVC();
}

/*
    Function: GetCurrentPCB
    Output: <return value>: Pointer to the current PCB
    Brief: Getter function for the current PCB by querying the TaskScheduler.
*/
pcb_t* OperatingSystem::GetCurrentPCB() {
    return current_pcb_;
}

/*
    Function: GetNextPCB
    Output: <return value>: Pointer to the next PCB
    Brief: Getter function for the next PCB by querying the TaskScheduler.
*/
pcb_t* OperatingSystem::GetNextPCB() {
    current_pcb_ = TaskScheduler_->GetNextPCB();
    return current_pcb_;
}

/*
    Function: DeleteCurrentPCB
    Brief: Function to ask the TaskScheduler to delete the current PCB.
*/
void OperatingSystem::DeleteCurrentPCB() {
    TaskScheduler_->DeleteCurrentPCB();
}

/*
    Function: QuantumTick
    Brief: Called on every SYSTick (~100 Hz) to save the current process state
           and start the next process in queue.
*/
void OperatingSystem::QuantumTick() {
    #if DEBUGGING_TRAIN >= 1
    DEBUGGING_quantum_flag_ = 1;
    #endif
    // Fetch CurrentPCB for function

    // For debugging / statistics
    current_pcb_->q_count++;

    // 1: Save registers
    save_registers();
    // 2: Set Stack Pointer on current PCB
    current_pcb_->stack_ptr = get_PSP();

    assert(current_pcb_->stack_ptr > (uint32_t)current_pcb_->stack_start); // Else there has been a stack overflow

    // 3: Get new process PCB and set stack pointer from it
    set_PSP(GetNextPCB()->stack_ptr);
    // 4: Restore_Registers
    restore_registers();
    #if DEBUGGING_TRAIN >= 1
    DEBUGGING_quantum_flag_ = 0;
    #endif
}

/*
    Function: QueuePCB
    Input: new_pcb: Pointer to the PCB to queue
    Brief: Queues the provided PCB by passing it down to the TaskScheduler
*/
void OperatingSystem::QueuePCB(pcb_t* new_pcb) {
    TaskScheduler_->AddProcess(new_pcb);
}

/*
    Function: DiagnosticsDisplay
    Output: display_output: String for the output to be displayed with
    Brief: OperatingSystem (Top) Portion Debugging function that returns a diagnostics string of
           the current processes present in all 5 PCBLists in the system. Used heavily for debugging.
*/
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

#if DEBUGGING_TRAIN >= 1
#define ALL 255
void OperatingSystem::SetKernelDebugFlag(uint8_t flag_num, uint32_t raise_flag) {
    if (flag_num == ALL) for(int i = 0; i < 16; i++) flag_array[i] = raise_flag;
    else if (flag_num >= 16) { // 16-24 reserved for storing uint32_t values
        assert(flag_num < 24);
        flag_array[flag_num] = raise_flag;
    }
    else { // 0-15 reserved for storing booleans
        assert(raise_flag <= 1);
        flag_array[flag_num] = raise_flag;
    }
}

void OperatingSystem::SetKernelVoidPtr(uint8_t flag_num, void* void_ptr) {
    if (flag_num == ALL) for(int i = 0; i < 8; i++) void_ptr_array[i] = 0;
    else {
        assert(flag_num < 8);
        void_ptr_array[flag_num] = void_ptr;
    }
}

#if DEBUGGING_TRAIN >= 1


#endif
void OperatingSystem::StackDebug() {
    for (int i = 0; i < pcb_idx; i++) {
        pcb_stack_sizes[i] = (debugging_pcbs[i]->stack_ptr - (uint32_t)debugging_pcbs[i]->stack_start);
    }

    pcb_stack_sizes[19] = STACKSIZE*sizeof(uint32_t);
}
#endif
