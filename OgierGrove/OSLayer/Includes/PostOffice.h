#ifndef PostOffice_H
#define PostOffice_H

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
-> Name:  PostOffice.h
-> Date: Oct 31, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <OSLayer/Includes/RingBuffer.h>

#include "OSLibrary.h"

#define TOTAL_MAILBOXES 256
#define MAX_LETTERS 16

class PostOffice {
    private:
        static PostOffice* PostOfficeInstance_;
        mailbox_t *Mailboxes_;
    public:
        PostOffice() : Mailboxes_(new mailbox_t[TOTAL_MAILBOXES]) {}
        ~PostOffice() { delete[] Mailboxes_; };

        // BindMailbox
        bool BuyMailbox(uint8_t mailbox_no, 
                        letter_size_t letter_size,
                        pcb_t* current_pcb,
                        uint8_t mailbox_size = MAX_LETTERS);

        bool SellMailbox(uint8_t mailbox_no, pcb_t* current_pcb);

        mailbox_t* GetMailBox(uint8_t mailbox_no);

        static PostOffice* GetPostOffice();
};

#endif /* PostOffice_H */
