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
-> Name:  TrainCommandApplication.cpp
-> Date: Nov 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainCommandApplication.h"

#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>

// Singleton Instance
TrainCommandApplication *TrainCommandApplication::TrainCommandApplicationInstance_ = 0;

/*
    Function: UARTMailboxLoop
    Brief: <>
*/
void TrainCommandApplication::MailboxLoop() {
    // Bind TrainCommandApplication queue
    if (!PBind(TRAIN_APPLICATION_LAYER_MB, SMALL_LETTER)) { // Default mailbox size of 16
        std::cout << "TrainCommandApplication::MailboxLoop(): WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainCommandApplication::MailboxLoop(): Mailbox bound\n";

    uint8_t src_q;
    uint32_t mailbox_msg_len = 7;
    char msg_body[SMALL_LETTER];

    while (1) {
        // Blocking message request
        PRecv(src_q, TRAIN_APPLICATION_LAYER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < SMALL_LETTER);

        // Handle mailbox depending on SRC
        if (src_q == DATA_LINK_LAYER_MB) {
            // Handle message coming from DLL
            HandleDLLMessage(msg_body, mailbox_msg_len);
        }
        else {
            // Handle message coming from another application
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::MailboxLoop(): Application Layer Msg, Sending Down to DLL\n";
            #endif

            HandleAppRequest(msg_body, mailbox_msg_len);
        }
    }
}

// Pass DLL message on to interested parties (Train scheduler? Train Display? Etc.) 
void TrainCommandApplication::HandleDLLMessage(char* request, uint8_t length) {
    // For now, just print message contents to screen for debugging
    assert(length >= 1 && length <= 3);

    switch (request[0]) {
        case '\xA0':
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::HandleDLLMessage(): Hall Sensor Triggered Msg >> Sensor: " << int(request[1]) << "<<\n";
            #endif

            // Send a reply to reset the sensor
            SendSensorAcknowledge(int(request[1]), TRAIN_APPLICATION_LAYER_MB);

            // TODO: Tell interested parties (GUI and TrainController)

            break;
        case '\xAA':
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::HandleDLLMessage(): Hall Sensor Queue Reset Acknowledgement Msg\n";
            #endif
            break;
        case '\xC2':
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::HandleDLLMessage(): Train Speed/Dir Acknowledgement Msg\n";
            #endif
            break;
        case '\xE2':
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::HandleDLLMessage(): Throw-Switch Acknowledgement Msg\n";
            #endif
            break;
        default:
            std::cout << "TrainCommandApplication::HandleDLLMessage(): ERROR: Invalid COMMAND >>" << HEX(request[0]) << "<<\n";
            while (1) {}
            break;
    }

    #if DEBUGGING_TRAIN == 1
    std::cout << "TrainCommandApplication::HandleDLLMessage(): Msg >> ";
    for(int i = 0; i < length; i++) {
        std::cout << HEX(request[i]);
    }
    std::cout << "<<\n";
    #endif

}

// Pass Message down to DLL layer
// This function expects fully formed messages, but does perform error
//    checking to ensure that is the case
void TrainCommandApplication::HandleAppRequest(char* request, uint8_t length) {
    // Check that length is correct
    assert(MsgLengthFromCode(request[0]) == length);

    // Send message down
    PSend(TRAIN_APPLICATION_LAYER_MB, DATA_LINK_LAYER_MB, (void *)request, length);
}

// Construct a message to acknowledge sensors and send to TrainCommand mailbox
void TrainCommandApplication::SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q) {
    static char msg_body[2];

    msg_body[0] = '\xA2';
    msg_body[1] = sensor_number;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 2)) {
        std::cout << "TrainCommandApplication::SendSensorAcknowledge(): Warning! PSend Failed\n";
    }
}

// Construct a message to reset sensor queue and send to TrainCommand mailbox
void TrainCommandApplication::SendSensorQueueReset(uint8_t src_q) {
    static char msg_body[1];

    msg_body[0] = '\xA8';

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 1)) {
        std::cout << "TrainCommandApplication::SendSensorQueueReset(): Warning! PSend Failed\n";
    }
}

// Construct a message to command a train and send to TrainCommand mailbox
void TrainCommandApplication::SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q) {
    static char msg_body[3];

    msg_body[0] = '\xC0';
    msg_body[1] = train_num;
    ((train_settings_t *)(&msg_body[2]))->speed = speed;
    ((train_settings_t *)(&msg_body[2]))->direction = direction;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandApplication::SendTrainCommand(): Warning! PSend Failed\n";
    }
}

// Construct a message to throw switch(es) and send to TrainCommand mailbox
void TrainCommandApplication::SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q) {
    static char msg_body[3];

    msg_body[0] = '\xE0';
    msg_body[1] = switch_num;
    msg_body[2] = direction;

    if (!PSend(src_q, TRAIN_APPLICATION_LAYER_MB, msg_body, 3)) {
        std::cout << "TrainCommandApplication::SendSwitchCommand(): Warning! PSend Failed\n";
    }
}

/*
    Function: TrainCommandApplication
    Brief: Constructor for the TrainCommandApplication class
*/
TrainCommandApplication::TrainCommandApplication() {
}

/*
    Function: ~TrainCommandApplication
    Brief: Destructor for the TrainCommandApplication class
*/
TrainCommandApplication::~TrainCommandApplication() {
}

void TrainCommandApplication::SingletonGrab() {
}

/*
    Function: GetTrainCommandApplication
    Output: TrainCommandApplicationInstance_: Pointer to the TrainCommandApplication Singleton
    Brief: Get function for the TrainCommandApplication singleton
*/
TrainCommandApplication* TrainCommandApplication::GetTrainCommandApplication() {
    if (!TrainCommandApplicationInstance_) TrainCommandApplicationInstance_ = new TrainCommandApplication;
    return TrainCommandApplicationInstance_;
}
