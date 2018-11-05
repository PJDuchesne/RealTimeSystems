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
-> Name:  PostOffice.cpp
-> Date: Oct 31, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/PostOffice.h"

// Singleton Instance
PostOffice *PostOffice::PostOfficeInstance_ = 0;

/*
    Function: BuyMailbox
    Input:  mailbox_no: Mailbox number to buy
           letter_size: Letter size (Structure) to create the RingBuffer with
           current_pcb: The PCB to attach to the mailbox
    Output: <Return Value>: Boolean value to indicate success or failure
    Brief: Used to instantiate a mailbox within the Mailboxes pointer list
*/
bool PostOffice::BuyMailbox(uint8_t mailbox_no, letter_size_t letter_size, pcb_t* current_pcb) {
    // Return false if the mailbox is already in use
    mailbox_t* relevant_mailbox = &Mailboxes_[mailbox_no];

    if (Mailboxes_[mailbox_no].currently_owned == true) return false;

    // Check if the process is allowed to buy more mailboxes
    bool full_flag = true;
    for (uint8_t i = 0; i < 3; i++) {
        if (current_pcb->mailbox_numbers[i] == 0) {
            current_pcb->mailbox_numbers[i] = mailbox_no;
            full_flag = false;
        }
    }
    if (full_flag) return false;

    // Else, buy it!
    relevant_mailbox->currently_owned = true;
    relevant_mailbox->owner_pcb = current_pcb;
    relevant_mailbox->letter_size = letter_size;

    void* tmp;
    switch(letter_size) {
        case ZERO_CHAR:
            tmp = (void *)(new RingBuffer<empty_msg_t>(MAX_LETTERS));
            break;
        case ONE_CHAR:
            tmp = (void *)(new RingBuffer<one_char_msg_t>(MAX_LETTERS));
            break;
        case BIG_LETTER:
            tmp = (void *)(new RingBuffer<big_letter_msg>(MAX_LETTERS));
            break;
        default:
            // TODO: ERROR STATE HERE
            break;
    }
    Mailboxes_[mailbox_no].mailbox_ptr = tmp;
    return true;
}

/*
    Function: SellMailbox
    Input:  mailbox_no: Mailbox number to sell
           current_pcb: The that should be attached to the mailbox
    Output: <Return Value>: Boolean value to indicate success or failure
    Brief: Used to de-allocate a mailbox after a process is finished using it
*/
bool PostOffice::SellMailbox(uint8_t mailbox_no, pcb_t* current_pcb) {
    // Return false if the mailbox is not currently in use
    mailbox_t* relevant_mailbox = &Mailboxes_[mailbox_no];
    if (Mailboxes_[mailbox_no].currently_owned == false) return false;    

    // Else, sell it!
    relevant_mailbox->currently_owned = false;
    switch(relevant_mailbox->letter_size) {
        case ZERO_CHAR:
            delete (RingBuffer<empty_msg_t> *)(relevant_mailbox->mailbox_ptr);
            break;
        case ONE_CHAR:
            delete (RingBuffer<one_char_msg_t> *)(relevant_mailbox->mailbox_ptr);
            break;
        case BIG_LETTER:
            delete (RingBuffer<big_letter_msg> *)(relevant_mailbox->mailbox_ptr);
            break;
        default:
            // TODO: ERROR STATE HERE
            break;
    }
    return true;
}

/*
    Function: GetMailBox
    Input:  mailbox_no: Mailbox number to get
    Output: <Return Value>: Pointer to the desired mailbox slot
    Brief: Used to fetch the mailbox structure from the table
*/
mailbox_t* PostOffice::GetMailBox(uint8_t mailbox_no) {
    return &Mailboxes_[mailbox_no];
}

/*
    Function: GetPostOffice
    Output: PostOfficeInstance_: Pointer to the PostOffice Singleton
    Brief: Get function for the PostOffice singleton
*/
PostOffice* PostOffice::GetPostOffice() {
    if (!PostOfficeInstance_) PostOfficeInstance_ = new PostOffice;
    return PostOfficeInstance_;
}
