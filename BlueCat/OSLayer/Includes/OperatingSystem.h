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
#include "PostOffice.h"

extern void MonitorProcess();
extern void DummpyProcess2();
extern void DummpyProcess3();

// Function pointer typedef
typedef void (*process_t)();

// Forward Declaration
class PostOffice;

class OperatingSystem {
    private:
        static OperatingSystem* OperatingSystemInstance_;
        TaskScheduler* TaskScheduler_;
        PostOffice* PostOfficeInstance_;

        void RegProc(process_t entry_point, uint32_t pid, priority_t priority, std::string name);
        void InitStackFrame(stack_frame_t* new_sf);
        void KickStart();
    public:
        OperatingSystem();
        void QuantumTick();
        void QueuePCB(pcb_t* new_pcb);
        pcb_t* GetCurrentPCB();
        pcb_t* GetNextPCB();
        void DeleteCurrentPCB();
        void Inialize();

        void DiagnosticsDisplay(std::string &display_output);

        static OperatingSystem* GetOperatingSystem();
};

#endif /* OperatingSystem_H */
