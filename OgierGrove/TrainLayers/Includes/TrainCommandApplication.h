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

#include "TrainCommandCenter.h"

// Forward Declaration
class TrainCommandCenter;

class TrainCommandApplication {
    private:
        static TrainCommandApplication* TrainCommandApplicationInstance_;

        TrainCommandCenter* TrainCommandCenterInstance_;

        void HandleDLLMessage(char* request, uint8_t length);
        void HandleAppRequest(char* request, uint8_t length);
    public:
        void MailboxLoop();

        TrainCommandApplication();
        ~TrainCommandApplication();

        void SingletonGrab();
        void CentralLoop();
        static TrainCommandApplication* GetTrainCommandApplication();
};

#endif /* TrainCommandApplication_H */
