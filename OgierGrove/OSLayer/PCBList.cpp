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

/*
    Function: AddPCB
    Input: input_pcb: PCB to add to the list
    Brief: Adds the given PCB to the list
*/
void PCBList:: AddPCB(pcb_t* input_pcb) {
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

/*
    Function: IsEmpty
    Output: <Return Value>: Boolean result
    Brief: Checks whether the list is empty or not
*/
bool PCBList::IsEmpty() {
	return (front_ == 0);
}

/*
    Function: NextPCB
    Output: <Return Value>: Pointer to the next PCB
    Brief: Fetches the next PCB and iterates the front
        Note: This assumes 'IsEmpty()' has been called first
*/
pcb_t* PCBList::NextPCB() {
	// Move front to next and return it
	front_ = front_->next;
	return front_;
}

/*
    Function: CurrentPCB
    Output: <Return Value>: Pointer to the current PCB
    Brief: Fetches the current PCB
        Note: This assumes 'IsEmpty()' has been called first
*/
pcb_t* PCBList::CurrentPCB() {
	return front_;
}

/*
    Function: DeleteCurrentPCB
    Brief: Deletes the current PCB
        Note: This assumes 'IsEmpty()' has been called first
*/
void PCBList::DeleteCurrentPCB() {
	// If last PCB in group: Special case
	if (front_->next == front_) front_ = 0;
	else {
		// Attach prev PCB's next to the current next PCB
		front_->prev->next = front_->next;
		// Attach next PCB's prev to the current prev PCB
		front_->next->prev = front_->prev;
		// Set front to prev (So on next "GetNextPCB" call does not skip a PCB) 
		front_ = front_->prev;
	}
}

/*
    Function: DiagnosticsDisplay
    Output: display_output: String for the output to be displayed with
    Brief: PCBList (Bottom) portion of debugging function performs PCBList query
*/
void PCBList::DiagnosticsDisplay(std::string &display_output) {
	if (IsEmpty()) {
		// std::cout << "\tThis list is empty\n\n";
		display_output += "This list is empty";
		return;
	}

	// Do a lap around
	pcb_t* runner = front_;
	std::stringstream tmp_ss;

	do {
		tmp_ss << ">>" << runner->name << " (" << runner->q_count << ")<<, ";
		runner = runner->next;
	} while (front_ != runner);

	display_output += tmp_ss.str();
}

