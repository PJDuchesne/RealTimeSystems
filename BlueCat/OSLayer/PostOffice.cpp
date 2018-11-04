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

// TODO: PCB should not be an input value, that should simply 
//   be a "GET Current PCB from 'running' value
bool PostOffice::BuyMailbox(uint8_t mailbox_no, letter_size_t letter_size, pcb_t* TMP_input_pcb) {
    // Return false if the mailbox is already in use
    mailbox_t* relevant_mailbox = &Mailboxes_[mailbox_no];

    if (Mailboxes_[mailbox_no].currently_owned == true) return false;

    // Else, buy it!
    relevant_mailbox->currently_owned = true;
    relevant_mailbox->owner_pcb = TMP_input_pcb;
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
