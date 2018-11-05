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
-> Name:  KernelCalls.cpp
-> Date: Nov 4, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/KernelCalls.h"
#include <ISRLayer/Includes/GlobalConfig.h>

#include <iostream>
#include <string>

// Forward declaration
class OperatingSystem;
class PostOffice;

/*
    Function: KSingletonGrab
    Brief: Fetches and stores the singletons used within KSpace calls to prevent duplicate
           calls to access them later.
*/
void KSingletonGrab() {
    OSInstance = OperatingSystem::GetOperatingSystem();
    PostOfficeInstance = PostOffice::GetPostOffice();
}

/*
    Function: KNice
    Input: new_priority: Priority to update the current PCB to
    Brief: Kernel side call to allow processes to change their own priority and update their
           position in the queue. Also iterates to the next process.
*/
void KNice(priority_t new_priority) {
    pcb_t* CurrentPCB = OSInstance->GetCurrentPCB();

    // Deattach PCB
    OSInstance->DeleteCurrentPCB();

    // Update priority
    assert(new_priority <= P_FIVE); // For debugging
    CurrentPCB->priority = new_priority;

    // Store current PSP
    CurrentPCB->stack_ptr = get_PSP();

    // Move PCB
    OSInstance->QueuePCB(CurrentPCB);

    // Set PCB to next process
    set_PSP(OSInstance->GetNextPCB()->stack_ptr);
}

/*
    Function: KTerminateProcess
    Brief: Kernel side call to terminate processes by deleting all relevent data structures
           and setting the next process running.
*/
void KTerminateProcess() {
    pcb_t* CurrentPCB = OSInstance->GetCurrentPCB();

    // Deattach PCB
    OSInstance->DeleteCurrentPCB();

    // Delete any mailboxes that the PCB owns
    for (uint8_t i = 0; i < 3; i++) {
        if (CurrentPCB->mailbox_numbers[i] == 0) break;
        else PostOfficeInstance->SellMailbox(CurrentPCB->mailbox_numbers[i], CurrentPCB);
    }

    // Deallocate Process Stack
    // Deallocate PCB
    delete[] CurrentPCB->stack_start;
    delete CurrentPCB;

    // Set PCB to next process
    set_PSP(OSInstance->GetNextPCB()->stack_ptr);
}

/*
    Function: KSend
    Input: kcaptr: Structure passed through the stack that allows for inputs and outputs
    Output: <Return Value>: Code to indicate failure/success
    Brief: Kernel side call to send a message to a specific mailbox. This takes into account
           the possible RingBuffer sizes and casts appropriately. This also handles blocked
           processes by waking them up and directly transfering the message to the process.
*/
kernel_responses_t KSend(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == false) return FAILURE_KR;

    // Check if the current PCB owns owns the "SRC_Q" mailbox (For replies) and is in use
    // TODO: Let this be optional! So processes (or the kernel itself) can send anonymous messages
    mailbox_t* owners_mailbox = PostOfficeInstance->GetMailBox(kcaptr->src_q);
    if (owners_mailbox->currently_owned == false) return FAILURE_KR;
    if (OSInstance->GetCurrentPCB() != owners_mailbox->owner_pcb) return FAILURE_KR;

    // Add message to mailbox!
    uint16_t actual_msg_len = MIN(kcaptr->msg_len, (uint16_t)(requested_mailbox->letter_size));

    // Define a structure of the largest size and use it
    big_letter_msg_t input_msg;
    input_msg.msg_size = actual_msg_len;
    input_msg.msg_src = kcaptr->src_q;

    // If the receiver is currently blocked, then wake it up and directly pass messages
    if (requested_mailbox->currently_blocked) {
        // Wake up PCB by adding to correct queue
        OSInstance->QueuePCB(requested_mailbox->owner_pcb);

        // Update mailbox flag
        requested_mailbox->currently_blocked = false;

        // Manually copy message to destination!
        requested_mailbox->kcaptr->msg_len = actual_msg_len;
        requested_mailbox->kcaptr->src_q = kcaptr->src_q;

        switch(requested_mailbox->letter_size) {
            case ZERO_CHAR: // Not forgotten, just empty. This will (Probably) be optimized away
                break;
            case ONE_CHAR:
                ((char *)(requested_mailbox->kcaptr->msg_ptr))[0] = ((char *)(kcaptr->msg_ptr))[0];
                break;
            case BIG_LETTER:
                memcpy(requested_mailbox->kcaptr->msg_ptr, kcaptr->msg_ptr, actual_msg_len);
                break;
            default:
                // TODO: Some sort of error msg here
                break;
        }
    }
    // Else, pass message into the correct buffer
    else {
        switch (requested_mailbox->letter_size) {
            case ZERO_CHAR:
                if (((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return FAILURE_KR;
                ((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            case ONE_CHAR:
                if (((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return FAILURE_KR;
                (input_msg.msg)[0] = ((char*)(kcaptr->msg_ptr))[0];
                ((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            case BIG_LETTER:
                if (((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Full()) return FAILURE_KR;
                // TODO: Fix the fact that this is copying twice by adding a new "Add" function that directly copies it into the
                memcpy(input_msg.msg, kcaptr->msg_ptr, actual_msg_len);
                ((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Add(&input_msg);
                break;
            default:
                // TODO: Some sort of error msg here
                break;
        }
    }
    return SUCCESS_KR;
}

/*
    Function: KRecv
    Input: kcaptr: Structure passed through the stack that allows for inputs and outputs
    Output: <Return Value>: Code to indicate failure/success
    Brief: Kernel side call to check a specific mailbox for a message. If no message is found,
           the process will be put to sleep if that option is enabled (Which it is by default).
           This function also takes into account the possible RingBuffer sizes and casts
           appropriately.
*/
kernel_responses_t KRecv(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == false) return FAILURE_KR;

    // Check if this is your mailbox!
    if (OSInstance->GetCurrentPCB() != requested_mailbox->owner_pcb) return FAILURE_KR;

    // Fetch mail! (Using different structures)
    empty_msg_t recv_msg0;
    one_char_msg_t recv_msg1;
    big_letter_msg_t recv_msg256;

    bool put_to_sleep = false;
    switch (requested_mailbox->letter_size) {
    case ZERO_CHAR:
        if (((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) {
            if (kcaptr->enable_sleep) put_to_sleep = true;
            else kcaptr->rtnvalue = NO_MSG_KR;
        } 
        else {
            recv_msg0 = ((RingBuffer<empty_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg0.msg_size;
            kcaptr->src_q = recv_msg0.msg_src;
        }
        break;
    case ONE_CHAR:
        if (((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) {
            if (kcaptr->enable_sleep) put_to_sleep = true;
            else kcaptr->rtnvalue = NO_MSG_KR;
        } 
        else {
            recv_msg1 = ((RingBuffer<one_char_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg1.msg_size;
            kcaptr->src_q = recv_msg1.msg_src;
            ((char *)kcaptr->msg_ptr)[0] = recv_msg1.msg[0];
        }
        break;
    case BIG_LETTER:
        if (((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Empty()) {
            if (kcaptr->enable_sleep) put_to_sleep = true;
            else kcaptr->rtnvalue = NO_MSG_KR;
        } 
        else {
            recv_msg256 = ((RingBuffer<big_letter_msg_t>*)(requested_mailbox->mailbox_ptr))->Get();
            kcaptr->msg_len = recv_msg256.msg_size;
            kcaptr->src_q = recv_msg256.msg_src;
            memcpy(kcaptr->msg_ptr, recv_msg256.msg, recv_msg256.msg_size);
        }
        break;
    default:
        // TODO: Some sort of error msg here
        break;
    }

    // If required, put message to sleep
    if (put_to_sleep) {
        // Update mailbox state
        requested_mailbox->currently_blocked = true;
        requested_mailbox->kcaptr = kcaptr;

        // Deattach PCB
        OSInstance->DeleteCurrentPCB();

        // Store current PSP
        requested_mailbox->owner_pcb->stack_ptr = get_PSP();

        // Set PSP to next process
        set_PSP(OSInstance->GetNextPCB()->stack_ptr);
    }

    return SUCCESS_KR;
}

/*
    Function: KBind
    Input: kcaptr: Structure passed through the stack that allows for inputs and outputs
    Output: <Return Value>: Code to indicate failure/success
    Brief: Kernel side call to bind a specific mailbox.
*/
kernel_responses_t KBind(kcallargs_t *kcaptr) {
    // Get requested mailbox
    mailbox_t* requested_mailbox = PostOfficeInstance->GetMailBox(kcaptr->dst_q);

    // Check if it is in use
    if (requested_mailbox->currently_owned == true) return FAILURE_KR;

    // Buy a mailbox!
    return (kernel_responses_t) PostOfficeInstance->BuyMailbox(kcaptr->req_q, kcaptr->q_size, OSInstance->GetCurrentPCB());
}
