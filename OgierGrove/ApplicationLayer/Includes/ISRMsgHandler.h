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

#include <ISRLayer/Includes/GlobalConfig.h>
#include <ISRLayer/Includes/SysTickDriver.h>
#include <ISRLayer/Includes/UART0Driver.h>
#include <OSLayer/Includes/RingBuffer.h>
#include <OSLayer/Includes/OperatingSystem.h>

// TODO: Delete
// #include <OSLayer/Includes/PostOffice.h/>

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
        RingBuffer<char> *uart0_output_data_buffer_;
        RingBuffer<char> *uart1_output_data_buffer_;

        UART0Driver *UART0DriverInstance_;
    public:
        ISRMsgHandler();
        ~ISRMsgHandler() ;
        void SingletonGrab();

        void GetFromISRQueue(MsgType_t &type, char &data);
        void QueueOutputMsg(std::string msg, uint8_t uart_num); // TODO: make uart_num an enum msg type
        void QueueOutputMsg(char* msg, uint8_t msg_len, uint8_t uart_num); // TODO: make uart_num an enum msg type
        void QueueOutputPacket(char* msg, uint16_t len);
        bool OutputBufferEmpty(uint8_t uart_num);
        char GetOutputChar(uint8_t uart_num);
        static ISRMsgHandler* GetISRMsgHandler();

        // This is public as it is shared with UART0Driver
        bool uart0_output_idle_; // Used to jumpstart output on uart0
        bool uart1_output_idle_; // Used to jumpstart output on uart1
};

#endif /* ISRMsgHandler_H */
