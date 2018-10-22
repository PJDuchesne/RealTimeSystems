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

void SVCall();
void SVCHandler(struct stack_frame *argptr);

void set_LR(volatile uint32_t);
void set_PSP(volatile uint32_t);
void set_MSP(volatile uint32_t);

uint32_t get_PSP();
uint32_t get_MSP();
uint32_t get_SP();

void volatile save_registers();
void volatile restore_registers();

void assignR7(volatile uint32_t data);

#endif /* KernelFunctions_H */
