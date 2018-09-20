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

#include "RingBuffer.h"
#include "ISRMsgMaker.h"
#include "CommandCenter.h"
#include "GlobalConfig.h"
#include "TimeHandler.h"

#define DATA_BUFFER_SIZE 80

const std::string clear_screen = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
const std::string new_line = "\n\rTIVA > ";

class Monitor {
    private:
        static Monitor* MonitorInstance_;
        std::unique_ptr<RingBuffer<char>> data_buffer_;
        char* single_char;

    public:
        Monitor();
        void CentralLoop();
        void CheckMessageHandler(MsgType_t &type, char &data);
        void HandleUART(char data);
        void HandleSYSTICK();
        void PrintNewLine();
        void PrintMsg(std::string msg);
        static Monitor* GetMonitor();
};

#endif /* Monitor_H */
