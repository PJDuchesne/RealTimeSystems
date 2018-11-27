#ifndef OperatingSystem_H
#define OperatingSystem_H

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
-> Name:  OperatingSystem.h
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Kernel.h"
#include "OSLibrary.h"
#include "PostOffice.h"

#include "TaskScheduler.h"
#include "KernelCalls.h"
#include "ProcessCalls.h"
#include "Processes.h"

#include <TrainLayers/Includes/TrainProcesses.h>

// Function pointer typedef
typedef void (*process_t)();

// Forward Declaration
class PostOffice;

class OperatingSystem {
    private:
        static OperatingSystem* OperatingSystemInstance_;
        TaskScheduler* TaskScheduler_;
        PostOffice* PostOfficeInstance_;
        pcb_t* current_pcb_;

        void InitStackFrame(stack_frame_t* new_sf);
        void KickStart();

    public:
        OperatingSystem();
        void RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name);
        void QuantumTick();
        void QueuePCB(pcb_t* new_pcb);
        pcb_t* GetCurrentPCB();
        pcb_t* GetNextPCB();
        void DeleteCurrentPCB();
        void Inialize();

        void DiagnosticsDisplay(std::string &display_output);
       
        #if DEBUGGING_TRAIN >= 1
        void SetKernelDebugFlag(uint8_t flag_num, uint32_t raise_flag);
        void SetKernelVoidPtr(uint8_t flag_num, void* void_ptr);
        volatile uint8_t DEBUGGING_quantum_flag_; // TODO: Delete (Used to debug OS)
        volatile uint8_t flag_array[24];
        volatile void* void_ptr_array[8];
        volatile pcb_t* debugging_pcbs[20] = {0};
        volatile uint32_t pcb_stack_sizes[20] = {0};
        volatile int pcb_idx = 0;
        void StackDebug();
        #endif

        static OperatingSystem* GetOperatingSystem();
};

#endif /* OperatingSystem_H */
