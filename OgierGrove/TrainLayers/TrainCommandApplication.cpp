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
    Function: MailboxLoop
    Brief: Central loop of this layer, checking the mailbox for messages coming up
        from the DLL, as well as messages to pass down to the DLL from other processes
*/
void TrainCommandApplication::MailboxLoop() {
    // Bind TrainCommandApplication queue
    if (!PBind(TRAIN_APPLICATION_LAYER_MB, SMALL_LETTER)) { // Default mailbox size of 16
        std::cout << "TrainCommandApplication::MailboxLoop(): WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainCommandApplication::MailboxLoop(): Mailbox bound\n";

    SingletonGrab();

    uint8_t src_q;
    uint32_t mailbox_msg_len = 7;
    char msg_body[SMALL_LETTER];

    static uint32_t last_time = 0;

    while (1) {
        // Blocking message request
        PRecv(src_q, TRAIN_APPLICATION_LAYER_MB, &msg_body, mailbox_msg_len);
        
        // Error state checking for testing
        assert(mailbox_msg_len < SMALL_LETTER);

        // Handle mailbox depending on SRC
        if (src_q == DATA_LINK_LAYER_MB) {
            while(TrainTimeServer::GetTrainTimeServer()->GetCurrentTime() - last_time < TIME_GAP) {}

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

/*
    Function: MailboxLoop
    Brief: Passes a DLL message on to interested parties (Monitor, controller, etc.)
    Inputs: request: Pointer to message
            length: Length of message
*/
void TrainCommandApplication::HandleDLLMessage(char* request, uint8_t length) {
    assert(length >= 1 && length <= 3);

    uint8_t tmp_num;
    uint32_t tmp_time;

    switch (request[0]) {
        case '\xA0': // Hall sensor (#) triggered
            #if DEBUGGING_TRAIN == 1
            std::cout << "TrainCommandApplication::HandleDLLMessage(): Hall Sensor Triggered Msg >> Sensor: " << int(request[1]) << "<<\n";
            #endif

            tmp_num = (uint8_t)request[1];
            assert(tmp_num != 0 && tmp_num <= NUM_HALL_SENSORS); // Check this is a real hall sensor

            // Send a reply to reset the sensor
            TrainCommandCenterInstance_->SendSensorAcknowledge(tmp_num, TRAIN_APPLICATION_LAYER_MB);

            // Tell the controller and the monitor if enough time has passed since last trigger
            tmp_time = TrainTimeServer::GetTrainTimeServer()->GetCurrentTime();
            if (tmp_time - HallTriggertimes_[tmp_num] > BOUNCE_TIME) {
                assert(tmp_num != 0 && tmp_num <= NUM_HALL_SENSORS); 
                
                // Update time for debouncing
                HallTriggertimes_[tmp_num] = tmp_time;

                // Tell TrainMonitor so it can update the screen
                SendHallMsg(tmp_num);

                // Tell TrainController so it can keep track of the train
                PSend(TRAIN_APPLICATION_LAYER_MB, TRAIN_CONTROLLER_MB, request, length);
            }
            break;
        case '\xAA': // Not currently used
            break;
        case '\xC2': // Train ACK
            // Tell TrainController so it can keep track of the train
            PSend(TRAIN_APPLICATION_LAYER_MB, TRAIN_CONTROLLER_MB, request, length);

            break;
        case '\xE2': // Switch ACK
            // Tell TrainController so it can keep track of the train
            PSend(TRAIN_APPLICATION_LAYER_MB, TRAIN_CONTROLLER_MB, request, length);
            break;
        default:
            std::cout << "TrainCommandApplication::HandleDLLMessage(): ERROR: Invalid COMMAND >>" << HEX(request[0]) << "<<\n";
            while (1) {}
            break;
    }
}

/*
    Function: HandleAppRequest
    Brief: Passes a message down to the DLL. This expectes a fully formed message
    Inputs: request: Pointer to message
            length: Length of message
*/
void TrainCommandApplication::HandleAppRequest(char* request, uint8_t length) {
    // Check that length is correct
    assert(MsgLengthFromCode(request[0]) == length);

    // Send message down
    PSend(TRAIN_APPLICATION_LAYER_MB, DATA_LINK_LAYER_MB, (void *)request, length);
}

/*
    Function: SendHallMsg
    Brief: Sends a message to the monitor to visually trigger the message on the
        UI. Also sets a timer on the train time server to reset that visual
        trigger after a certain time period
    Inputs: sensor_num: Number of the hall sensor to trigger
*/
void TrainCommandApplication::SendHallMsg(uint8_t sensor_num) {
    // Tell the monitor
    sensor_msg_t sensor_msg;
    sensor_msg.type = HALL_SENSOR;
    sensor_msg.state = true;
    sensor_msg.num = sensor_num;
    PSend(TRAIN_APPLICATION_LAYER_MB, TRAIN_MONITOR_MB, &sensor_msg, 2);

    // Send the time server so it can be reset
    PSend(TRAIN_APPLICATION_LAYER_MB, TRAIN_TIME_SERVER_MB, &sensor_num, 1);
}

/*
    Function: TrainCommandApplication
    Brief: Constructor for the TrainCommandApplication class
*/
TrainCommandApplication::TrainCommandApplication() {
    // Clear hall sensor trigger times
    for (uint8_t i = 0; i <= NUM_HALL_SENSORS; i++) HallTriggertimes_[i] = 0;
}

/*
    Function: ~TrainCommandApplication
    Brief: Destructor for the TrainCommandApplication class
*/
TrainCommandApplication::~TrainCommandApplication() {
}

/*
    Function: SingletonGrab
    Brief: Grabs instance of train command center to avoid unnecessary calls
*/
void TrainCommandApplication::SingletonGrab() {
    TrainCommandCenterInstance_ = TrainCommandCenter::GetTrainCommandCenter();
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
