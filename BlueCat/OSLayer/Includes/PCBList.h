#ifndef PCBList_H
#define PCBList_H

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
-> Name:  PCBList.h
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "OSLibrary.h"

class PCBList {
    private:
        pcb_t* front_;

    public:
        PCBList() { front_ = 0; };
        void AddPCB(pcb_t* input_pcb);
        bool IsEmpty();
        pcb_t* NextPCB();
        pcb_t* CurrentPCB();
        void DeleteCurrentPCB();

        // Diagnostics
        void DiagnosticsDisplay(std::string &display_output);
};

#endif /* PCBList_H */
