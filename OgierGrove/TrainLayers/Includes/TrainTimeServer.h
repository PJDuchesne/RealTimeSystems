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

#define MSG_TIMEOUT_TIME 100 // In centisecond precision
#define MAX_ALARMS 8 // TODO: Tie to MAX_DLL_WAITING_PACKETS from Data Link Layer

#include "TrainLibrary.h"
#include <OSLayer/Includes/OSLibrary.h>
#include <OSLayer/Includes/ProcessCalls.h>
#include <ISRLayer/Includes/GlobalMailboxes.h>

typedef struct trainMsgAlarm {
    bool is_active;
    uint32_t alarm_time;
} trainMsgAlarm_t;

class TrainTimeServer {
    private:
        static TrainTimeServer* TrainTimeServerInstance_;

        // Called owhen MSG is received from SYSTICK
        void TickCentiSec();
        void CheckAlarms();
        void TriggerAlarm(uint8_t alarm_num);

        // Called when MSG is received from DLL
        void SetAlarm(uint8_t alarm_num, bool set_flag);

        uint32_t currentDeciTime_; // Will overflow after ~30000 hours
        trainMsgAlarm_t trainAlarms[MAX_ALARMS];

    public:
        TrainTimeServer();
        void TrainTimeServerLoop();

        uint32_t GetCurrentTime();

        static TrainTimeServer* GetTrainTimeServer();
};

#endif /* TrainTimeServer_H */
