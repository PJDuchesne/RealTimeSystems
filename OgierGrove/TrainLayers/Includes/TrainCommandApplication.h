#ifndef TrainCommandApplication_H
#define TrainCommandApplication_H

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
-> Name:  TrainCommandApplication.h
-> Date: Nov 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"
#include <ApplicationLayer/Includes/ISRMsgHandler.h>

class TrainCommandApplication {
    private:
        static TrainCommandApplication* TrainCommandApplicationInstance_;

        void HandleDLLMessage(char* request, uint8_t length);
        void HandleAppRequest(char* request, uint8_t length);
    public:
        void MailboxLoop();

        // Public API for other processes to create and send 
        // messages to the TrainCommand Mailbox
        void SendSensorAcknowledge(uint8_t sensor_number, uint8_t src_q);
        void SendSensorQueueReset(uint8_t src_q);
        void SendTrainCommand(uint8_t train_num, uint8_t speed, train_direction_t direction, uint8_t src_q);
        void SendSwitchCommand(uint8_t switch_num, switch_direction_t direction, uint8_t src_q);

        TrainCommandApplication();
        ~TrainCommandApplication();

        void SingletonGrab();
        void CentralLoop();
        static TrainCommandApplication* GetTrainCommandApplication();
};

#endif /* TrainCommandApplication_H */
