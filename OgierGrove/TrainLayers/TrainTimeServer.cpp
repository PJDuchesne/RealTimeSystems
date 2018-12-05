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
-> Name:  TrainTimeServer.cpp
-> Date: Nov 20, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainTimeServer.h"

// Singleton Instance
TrainTimeServer *TrainTimeServer::TrainTimeServerInstance_ = 0;

/*
    Function: TrainTimeServer
    Brief: Constructor for the TrainTimeServer class. Initializes the different alarms
        to all be disabled
*/
TrainTimeServer::TrainTimeServer() {
    currentDeciTime_ = 0;
    for (int i = 0; i < MAX_RESEND_ALARMS; i++) trainAlarms[i].is_active = false;
    for (int i = 0; i <= NUM_HALL_SENSORS; i++) hallSensorAlarms[i].is_active = false;
}

/*
    Function: TriggerResendAlarm
    Brief: Triggers a DLL resend alarm, sending a message to the DLL
*/
void TrainTimeServer::TriggerResendAlarm(uint8_t alarm_num) {
    trainAlarms[alarm_num].is_active = false;

    static char msg_body = alarm_num; 
    PSend(TRAIN_TIME_SERVER_MB, DATA_LINK_LAYER_MB, (void *)msg_body, 1);
}

/*
    Function: TriggerHallSensorAlarm
    Brief: Triggers a hall sensor reset alarm, sending a message to the monitor
        to visually lower the hall sensor status
*/
void TrainTimeServer::TriggerHallSensorAlarm(uint8_t alarm_num) {
    hallSensorAlarms[alarm_num].is_active = false;

    sensor_msg_t sensor_msg;
    sensor_msg.type = HALL_SENSOR;
    sensor_msg.state = false;
    sensor_msg.num = alarm_num;

    PSend(TRAIN_TIME_SERVER_MB, TRAIN_MONITOR_MB, &sensor_msg, 2);
}

/*
    Function: TickCentiSec
    Brief: Increments time, checking the alarms each time
*/
void TrainTimeServer::TickCentiSec() {
    currentDeciTime_++;
    CheckAlarms();
}

/*
    Function: CheckAlarms
    Brief: Checks alarms for both the DLL trigger and the hall sensor
        reset triggers
*/
void TrainTimeServer::CheckAlarms() {
    for(int i = 0; i < MAX_RESEND_ALARMS; i++) {
        if (trainAlarms[i].is_active && trainAlarms[i].alarm_time <= currentDeciTime_) {
            TriggerResendAlarm(i);
        }
    }

    for(int i = 0; i <= NUM_HALL_SENSORS; i++) {
        if (hallSensorAlarms[i].is_active && hallSensorAlarms[i].alarm_time <= currentDeciTime_) {
            TriggerHallSensorAlarm(i);
        }
    }
}

/*
    Function: TrainTimeServerLoop
    Brief: Central loop of the time server, which handles
        inbound requests and SYSTick triggers
*/
void TrainTimeServer::TrainTimeServerLoop() {
    if (!PBind(TRAIN_TIME_SERVER_MB, SMALL_LETTER)) { // Default mailbox size of 16, which is MAX_RESEND_ALARMS*2
        std::cout << "TrainTimeServer::Initialize: WARNING Mailbox failed to bind\n";
    }
    else std::cout << "TrainTimeServer::TrainTimeServerLoop: WARNING Mailbox bound!\n";

    static uint8_t src_q;
    static uint32_t mailbox_msg_len;
    static char msg_body[SMALL_LETTER];

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

                #if RESEND_TIMERS == 1
                SetResendAlarm((int)msg_body[0], (bool)msg_body[1]);
                #endif
                break;
            case TRAIN_APPLICATION_LAYER_MB:
                assert(mailbox_msg_len == 1);
                SetHallSensorAlarm((uint8_t)msg_body[0]);
                break;
            default:
                std::cout << "TrainTimeServer::TrainTimeServerLoop(): ERROR: MEssage from invalid source: " << (int)src_q << "\n!";
                while(1) {}
                break;
        }

    }
}

/*
    Function: SetResendAlarm
    Brief: Function to raise or lower an alarm for the re-send buffer of the
        Data Link Layer
    Input: alarm_num: Number of alarm to set
           set_flag: The state of the alarm to set (Raise or lower)
*/
void TrainTimeServer::SetResendAlarm(uint8_t alarm_num, bool set_flag) {
    if(set_flag) { // Set alarm
        trainAlarms[alarm_num].is_active = true;
        trainAlarms[alarm_num].alarm_time = currentDeciTime_ + HALL_TIMEOUT_TIME ;
    }
    else { // Cancel alarm
        trainAlarms[alarm_num].is_active = false;
    }
}

/*
    Function: SetHallSensorAlarm
    Brief: Function to set hall sensor alarm, which will tell the monitor
        to de-color a given hall sensor after a period of time
    Input: alarm_num: Number of alarm to set
*/
void TrainTimeServer::SetHallSensorAlarm(uint8_t alarm_num) {
    hallSensorAlarms[alarm_num].is_active = true;
    hallSensorAlarms[alarm_num].alarm_time = currentDeciTime_ + HALL_TIMEOUT_TIME;
}

/*
    Function: GetCurrentTime
    Brief: Getter function to access current system time since startup in deciseconds
    Output: currentDeciTime_
*/
uint32_t TrainTimeServer::GetCurrentTime() {
    return currentDeciTime_;
}

/*
    Function: GetTrainTimeServer
    Brief: Get function for the TrainTimeServer singleton
    Output: CommandCenterInstance_: Pointer to the TrainTimeServer Singleton
*/
TrainTimeServer* TrainTimeServer::GetTrainTimeServer() {
    if (!TrainTimeServerInstance_) TrainTimeServerInstance_ = new TrainTimeServer;
    return TrainTimeServerInstance_;
}
