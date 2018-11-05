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

/*
    Function: PGetID
    Output: <Return Value>: The PID of the current process
    Brief: Process side call to fetch its own PID from the kernel layer via an SVC call.
*/
uint32_t PGetID() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = GETID;
    assignR7((uint32_t) &KernelArgs);
    SVC();
    return KernelArgs.rtnvalue;
}

/*
    Function: PNice
    Input: priority: The priority to change to
    Brief: Process side call to allow a process to change its own priority.
*/
void PNice(priority_t priority) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = NICE;
    KernelArgs.priority = priority;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

/*
    Function: PTerminateProcess
    Brief: Process side call that is set up to be called when every process terminates, in
           order to cleanly destroy the associated structures within the system.
*/
void PTerminateProcess() {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode = TERMINATE;
    assignR7((uint32_t) &KernelArgs);
    SVC();
}

/*
    Function: PSend
    Inputs:   srq_q: The mailbox number of the owner
              dst_q: The mailbox number of the destination
            msg_ptr: A pointer to the message itself
            msg_len: The length of bytes that the message contains
    Output: <Return Value>: True/False depending on success or failure
    Brief: Process side call to send a message to a specific mailbox
    // TODO: ^^ Make this true
*/
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

/*
    Function: PRecv
    Inputs: dst_q: The mailbox number to check (That the caller owns)
     enable_sleep: Boolean option to enable or disable blocking functionality (True by default)
    Output:   srq_q: The mailbox number of the process who sent the message
            msg_ptr: A pointer to the message itself
            msg_len: The length of bytes that the message contains
     <Return Value>: True/False depending on success or failure
    Brief: Process side call to check a mailbox for a message
    // TODO: ^^ Make this true
*/
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

/*
    Function: PBind
    Inputs: req_q: The mailbox number to bind
             size: The size of letters to create the mailbox with
    Output: <Return Value>: True/False depending on success or failure
    Brief: Process side call to bind a specific mailbox
*/
bool PBind(uint8_t req_q, letter_size_t size) {
    volatile kcallargs_t KernelArgs;
    KernelArgs.kcode  = BIND;
    KernelArgs.req_q  = req_q;
    KernelArgs.q_size = size;
    assignR7((uint32_t) &KernelArgs);
    SVC();

    return KernelArgs.rtnvalue;
    // TODO: ^^ Make this true
}
