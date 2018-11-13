#ifndef Monitor_H
#define Monitor_H

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
-> Name:  Monitor.h
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "../../OSLayer/Includes/RingBuffer.h"
#include "ISRMsgHandler.h"
#include "CommandCenter.h"
#include "TimeHandler.h"

#define DATA_BUFFER_SIZE 81
#define NEW_LINE         "\n\r > "
#define CLEAR_SCREEN     "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"

// Forward Declarations
class ISRMsgHandler;
class CommandCenter;
class TimeHandler;

class Monitor {
    private:
        static Monitor* MonitorInstance_;
        RingBuffer<char> *data_buffer_;

        ISRMsgHandler *ISRMsgHandlerInstance_;
        CommandCenter *CommandCenterInstance_;
        TimeHandler *TimeHandlerInstance_;

        // Poll the ISR Msg Queue
        void CheckMessageHandler();

        // Handle incoming msgs
        void HandleUART(char data);
        void HandleSYSTICK();

    public:
        Monitor();
        ~Monitor();
        void SingletonGrab();
        void CentralLoop();
        void RePrintOutputBuffer();
        void PrintMsg(std::string msg);
        void PrintErrorMsg(std::string msg);
        static Monitor* GetMonitor();
};

#endif /* Monitor_H */
