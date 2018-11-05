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
-> Name:  ProcessCalls.cpp
-> Date: Nov 4, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/ProcessCalls.h"

uint32_t PGetID() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = GETID;
    assignR7((uint32_t) &KernelArgs);
    SVC();
    return KernelArgs.rtnvalue;
}

void PNice(priority_t priority) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = NICE;
    KernelArgs.priority = priority;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

void PTerminateProcess() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = TERMINATE;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

bool PSend(uint8_t src_q, uint8_t dst_q, void* msg_ptr, uint32_t msg_len) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = SEND;
    KernelArgs.src_q = src_q;
    KernelArgs.dst_q = dst_q;
    KernelArgs.msg_ptr = msg_ptr;
    KernelArgs.msg_len = msg_len;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    return KernelArgs.rtnvalue;
}

bool PRecv(uint8_t& src_q, uint8_t dst_q, void* msg_ptr, uint32_t& msg_len, bool enable_sleep) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = RECV;
    KernelArgs.dst_q = dst_q;
    KernelArgs.msg_ptr = msg_ptr;
    KernelArgs.enable_sleep = enable_sleep;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    if (KernelArgs.rtnvalue != 0) {
        src_q = KernelArgs.src_q;
        msg_len = KernelArgs.msg_len;
    }

    return KernelArgs.rtnvalue;
}

bool PBind(uint8_t req_q, letter_size_t size) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode  = BIND;
    KernelArgs.req_q  = req_q;
    KernelArgs.q_size = size;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    return KernelArgs.rtnvalue;
}
