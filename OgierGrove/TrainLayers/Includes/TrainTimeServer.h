#ifndef TrainTimeServer_H
#define TrainTimeServer_H

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
-> Name:  TrainTimeServer.h
-> Date: Nov 20, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

// Note: Works with DeciSecond Prevision

#define HALL_TIMEOUT_TIME  40 // In centisecond precision
#define MAX_RESEND_ALARMS  8 // TODO: Tie to MAX_DLL_WAITING_PACKETS from Data Link Layer

#include "TrainLibrary.h"
#include <OSLayer/Includes/OSLibrary.h>
#include <OSLayer/Includes/ProcessCalls.h>
#include <ISRLayer/Includes/GlobalMailboxes.h>

typedef struct trainMsgAlarm {
    bool is_active;
    uint32_t alarm_time;
} trainMsgAlarm_t;

typedef struct hallSensorAlarm {
    bool is_active;
    uint32_t alarm_time;
} hallSensorAlarm_t;

class TrainTimeServer {
    private:
        static TrainTimeServer* TrainTimeServerInstance_;

        // Called owhen MSG is received from SYSTICK
        void TickCentiSec();
        void CheckAlarms();
        void TriggerResendAlarm(uint8_t alarm_num);
        void TriggerHallSensorAlarm(uint8_t alarm_num);

        // Called when MSG is received from DLL
        void SetAlarm(uint8_t alarm_num, bool set_flag);
        void SetResendAlarm(uint8_t alarm_num, bool set_flag);
        void SetHallSensorAlarm(uint8_t alarm_num);

        trainMsgAlarm_t trainAlarms[MAX_RESEND_ALARMS]; // For resending timed out commands on the data link layer
        hallSensorAlarm_t hallSensorAlarms[NUM_HALL_SENSORS + 1]; // For temporarily displaying hall sensors
        // hallSensorAlarm_t trainPauseAlarms[NUM_HALL_SENSORS + 1]; // 

        uint32_t currentDeciTime_; // Will overflow after ~30000 hours

    public:
        TrainTimeServer();
        void TrainTimeServerLoop();

        uint32_t GetCurrentTime();

        static TrainTimeServer* GetTrainTimeServer();
};

#endif /* TrainTimeServer_H */
