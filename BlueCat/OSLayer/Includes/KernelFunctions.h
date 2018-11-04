#ifndef KernelFunctions_H
#define KernelFunctions_H

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
-> Name:  KernelFunctions.h
-> Date: Oct 21, 2018  (Created)
-> Author: Dr. Larry Hughes, Modified by Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "OSLibrary.h"
#include "OperatingSystem.h"
#include "PostOffice.h"

class OperatingSystem;
class PostOffice;

void SVCall();
extern "C" void SVCHandler(struct stack_frame *argptr);

void set_LR(volatile uint32_t);
void set_PSP(volatile uint32_t);
void set_MSP(volatile uint32_t);

uint32_t get_PSP();
uint32_t get_MSP();
uint32_t get_SP();

void volatile save_registers();
void volatile restore_registers();

void assignR7(volatile uint32_t data);

// KSpace Handler functions to handle process kernel calls

static OperatingSystem* OSInstance;
static PostOffice* PostOfficeInstance;

void KNice(priority_t new_priority);
void KTerminateProcess();

bool KSend(kcallargs_t *kcaptr);
bool KRecv(kcallargs_t *kcaptr);
bool KBind(kcallargs_t *kcaptr);

// PSpace calls used to pass to control kernel via SVC()
uint32_t PGetID();
void PNice(priority_t priority);
void PTerminateProcess();

bool PSend(uint8_t src_q, uint8_t dst_q, void* msg_ptr, uint32_t msg_len);
bool PRecv(uint8_t src_q, uint8_t dst_q, void* msg_ptr, uint32_t msg_len);
bool PBind(uint8_t req_q, letter_size_t size);

#endif /* KernelFunctions_H */
