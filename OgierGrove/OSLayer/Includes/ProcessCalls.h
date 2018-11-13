#ifndef ProcessCalls_H
#define ProcessCalls_H

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
-> Name:  ProcessCalls.h
-> Date: Nov 4, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Kernel.h"
#include "OSLibrary.h"
#include "PostOffice.h"

uint32_t PGetID();
void PNice(priority_t priority);
void PTerminateProcess();
bool PSend(uint8_t src_q, uint8_t dst_q, void* msg_ptr, uint32_t msg_len);
bool PRecv(uint8_t& src_q, uint8_t dst_q, void* msg_ptr, uint32_t& msg_len, bool enable_sleep = true);
bool PBind(uint8_t req_q, letter_size_t size, uint8_t mailbox_size = MAX_LETTERS);

#endif /* ProcessCalls_H */
