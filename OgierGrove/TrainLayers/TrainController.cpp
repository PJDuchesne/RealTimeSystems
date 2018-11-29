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
-> Name:  TrainController.cpp
-> Date: Nov 29, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainController.h"

// Singleton Instance
TrainController *TrainController::TrainControllerInstance_ = 0;

/*
    Function: TrainController
    Brief: 
*/
TrainController::TrainController() {
}

void TrainController::TrainControllerLoop() {
    if (!PBind(TRAIN_CONTROLLER_MB, BIG_LETTER)) { // Default mailbox size of 16, which is MAX_ALARMS*2
        std::cout << "TrainController::Initialize: WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainController::TrainControllerLoop: WARNING Mailbox bound!\n";

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[2]; // TODO: Magic number

    // Should just be receiving from UART and Data Link Layer
    while(1) {
        PRecv(src_q, TRAIN_CONTROLLER_MB, &msg_body, mailbox_msg_len);

        // TODO: Handle requests and start doing stuff
    }
}

/*
    Function: GetTrainController
    Output: CommandCenterInstance_: Pointer to the TrainController Singleton
    Brief: Get function for the TrainController singleton
*/
TrainController* TrainController::GetTrainController() {
    if (!TrainControllerInstance_) TrainControllerInstance_ = new TrainController;
    return TrainControllerInstance_;
}
