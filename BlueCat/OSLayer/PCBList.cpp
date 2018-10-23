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
-> Name:  PCBList.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/PCBList.h"

PCBList::PCBList() {
	front_ = 0;
}

void PCBList::AddPCB(pcb_t* input_pcb) {
	// If list is empty
	if (front_ == 0) {
		// Initialize front_ to first PCB added
		front_ = input_pcb;
		// Add new PCB
		input_pcb->next = front_;
		input_pcb->prev = front_;
	}
	// If list is not empty
	else {
		// Attach new PCB
		input_pcb->next = front_->next;
		input_pcb->prev = front_;

		// Attach old PCB(s) to the new PCB
		front_->next->prev = input_pcb;
		front_->next = input_pcb;
	}
}

bool PCBList::IsEmpty() {
	return (front_ == 0);
}

// For debugging
void PCBList::DoALap() {
	if (IsEmpty()) {
		return;
	}

	pcb_t* runner = front_;

	do {
		std::cout << "[PCBList] DoALap Now: >>" << runner->name << "<<\n";
		runner = runner->next;
		std::cout << "[PCBList] DoALap Nxt: >>" << runner->name << "<<\n";
	} while (front_ != runner);

}

// Assumings 'IsEmpty' has been called first
pcb_t* PCBList::NextPCB() {
	// Move front to next and return it
	front_ = front_->next;
	return front_;
}




