#ifndef ISRMsgMaker_H
#define ISRMsgMaker_H

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
-> Name:  ISRMsgMaker.h
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "GlobalConfig.h"
#include "RingBuffer.h"
#include "UART0Driver.h"

#include <memory>

#define ISR_QUEUE_SIZE 100
#define OUTPUT_DATA_BUFFER_SIZE 100

struct ISRMsg {
  MsgType_t type;
  char data;
};

class ISRMsgMaker {
    private:
        static ISRMsgMaker* ISRMsgMakerInstance_;
        std::unique_ptr<RingBuffer<ISRMsg>> isr_queue_;
        std::unique_ptr<RingBuffer<char>> output_data_buffer_;
        
    public:
        ISRMsgMaker();
        void QueueMsg(MsgType_t type, char data);
        void GetFromQueue(MsgType_t &type, char &data);
        bool CheckISRQueue();
        void QueueOutputMsg(std::string msg);
        bool OutputBufferEmpty();
        char GetOutputChar();
        static ISRMsgMaker* GetISRMsgMaker();

        // This is public as it is shared with UART0Driver
        bool uart_output_idle_; // Used to jumpstart output
};

#endif /* ISRMsgMaker_H */
