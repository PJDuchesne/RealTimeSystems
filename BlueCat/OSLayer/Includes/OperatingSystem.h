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

#include "OSLibrary.h"
#include "TaskScheduler.h"
#include "KernelFunctions.h"

extern void MonitorProcess();
extern void DummpyProcess2();
extern void DummpyProcess3();

// Function pointer typedef
typedef void (*process_t)();

class OperatingSystem {
    private:
        static OperatingSystem* OperatingSystemInstance_;
        TaskScheduler* TaskScheduler_;
        pcb_t* CurrentPCB_;

        void RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name);
        void InitStackFrame(stack_frame_t* new_sf);
        void KickStart();
    public:
        OperatingSystem();
        void QuantumTick();
        pcb_t* GetNextPCB();
        pcb_t* GetCurrentPCB();
        void Inialize();

        static OperatingSystem* GetOperatingSystem();
};

#endif /* OperatingSystem_H */
