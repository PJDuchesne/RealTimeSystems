#ifndef KernelCalls_H
#define KernelCalls_H

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
-> Name:  KernelCalls.h
-> Date: Nov 4, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "OSLibrary.h"
#include "OperatingSystem.h"
#include "PostOffice.h"

// Forward declarations
class OperatingSystem;
class PostOffice;

static OperatingSystem* OSInstance;
static PostOffice* PostOfficeInstance;

void KSingletonGrab();

void KNice(priority_t new_priority);
void KTerminateProcess();
kernel_responses_t KSend(kcallargs_t *kcaptr);
kernel_responses_t KRecv(kcallargs_t *kcaptr);
kernel_responses_t KBind(kcallargs_t *kcaptr);

#endif /* KernelCalls_H */
