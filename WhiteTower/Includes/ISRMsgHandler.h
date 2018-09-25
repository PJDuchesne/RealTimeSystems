#ifndef ISRMsgHandler_H
#define ISRMsgHandler_H

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
-> Name:  ISRMsgHandler.h
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <string>

#include "RingBuffer.h"
#include "UART0Driver.h"
#include "SysTickDriver.h"

#define ISR_QUEUE_SIZE 100
#define OUTPUT_DATA_BUFFER_SIZE 1250

typedef enum MsgType {
  NONE,
  UART,
  SYSTICK
} MsgType_t;

typedef struct ISRMsg {
  MsgType_t type;
  char data;
} ISRMsg_t;

// Forward Declaration
class UART0Driver;

class ISRMsgHandler {
    private:
        static ISRMsgHandler* ISRMsgHandlerInstance_;
        RingBuffer<ISRMsg_t> *isr_queue_;
        RingBuffer<char> *output_data_buffer_;

        UART0Driver *UART0DriverInstance_;
    public:
        ISRMsgHandler();
        ~ISRMsgHandler() ;
        void SingletonGrab();

        void QueueISRMsg(MsgType_t type, char data);
        void GetFromISRQueue(MsgType_t &type, char &data);
        void QueueOutputMsg(std::string msg);
        bool OutputBufferEmpty();
        char GetOutputChar();
        static ISRMsgHandler* GetISRMsgHandler();

        // This is public as it is shared with UART0Driver
        bool uart_output_idle_; // Used to jumpstart output
};

#endif /* ISRMsgHandler_H */
