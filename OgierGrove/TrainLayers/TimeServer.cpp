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
-> Name:  TimeServer.cpp
-> Date: Nov 20, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TimeServer.h"

// Singleton Instance
TimeServer *TimeServer::TimeServerInstance_ = 0;

/*
    Function: TimeServer
    Brief: Constructor for the TimeServer class. Initailizes current time, alarm, and 
           leap_year_ to zero values. Also initializes date to default date and 
*/
TimeServer::TimeServer() {
    currentDeciTime_ = 0;
    for (int i = 0; i < MAX_ALARMS; i++) trainAlarms[i].is_active = false;
}

void TimeServer::TriggerAlarm(uint8_t alarm_num) {
    trainAlarms[alarm_num].is_active = false;

    static char msg_body = alarm_num; 
    PSend(TRAIN_TIME_SERVER_MB, DATA_LINK_LAYER_MB, (void *)msg_body, 1); // TODO: MAgic number
}

void TimeServer::TickCentiSec() {
    currentDeciTime_++;
    CheckAlarms();
}

void TimeServer::CheckAlarms() {
    for(int i = 0; i < MAX_ALARMS; i++) {
        if (trainAlarms[i].is_active && trainAlarms[i].alarm_time <= currentDeciTime_) {
            TriggerAlarm(i);
        }
    }
}

void TimeServer::TrainTimeServerLoop() {
    if (!PBind(TRAIN_TIME_SERVER_MB, BIG_LETTER)) { // Default mailbox size of 16, which is MAX_ALARMS*2
        std::cout << "TimeServer::Initialize: WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TimeServer::TrainTimeServerLoop: WARNING Mailbox bound!\n";

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[2]; // TODO: Magic number

    // Should just be receiving from UART and Data Link Layer
    while(1) {
        PRecv(src_q, TRAIN_TIME_SERVER_MB, &msg_body, mailbox_msg_len);

        switch (src_q) {
            case KERNEL_MB:
                // Handle systick
                assert(mailbox_msg_len == 0);

                TickCentiSec();
                break;
            case DATA_LINK_LAYER_MB:
                // Handle alarm request
                assert(mailbox_msg_len == 2);
                assert((int)msg_body[0] <= 8);
                assert((int)msg_body[1] <= 1);

                SetAlarm((int)msg_body[0], (bool)msg_body[1]);
                break;
            default:
                std::cout << "TimeServer::TrainTimeServerLoop(): ERROR: MEssage from invalid source\n!";
                while(1) {}
                break;
        }

    }
}

void TimeServer::SetAlarm(uint8_t alarm_num, bool set_flag) {
    if(set_flag) { // Set alarm
        // TODO: Remove debugging assert, this might not always be true?
        // This shouldnt be this layer's responsibility to check
        assert(trainAlarms[alarm_num].is_active == false);

        trainAlarms[alarm_num].is_active = true;
        trainAlarms[alarm_num].alarm_time = currentDeciTime_ + MSG_TIMEOUT_TIME;
    }
    else { // Cancel alarm
        // TODO: Remove debugging assert, this might not always be true?
        // This shouldnt be this layer's responsibility to check
        assert(trainAlarms[alarm_num].is_active == true);

        trainAlarms[alarm_num].is_active = false;

    }

}

/*
    Function: GetTimeServer
    Output: CommandCenterInstance_: Pointer to the TimeServer Singleton
    Brief: Get function for the TimeServer singleton
*/
TimeServer* TimeServer::GetTimeServer() {
    if (!TimeServerInstance_) TimeServerInstance_ = new TimeServer;
    return TimeServerInstance_;
}
