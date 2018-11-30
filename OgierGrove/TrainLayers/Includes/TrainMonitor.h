#ifndef TrainMonitor_H
#define TrainMonitor_H

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
-> Name:  TrainMonitor.h
-> Date: Nov 26, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"
#include "../../OSLayer/Includes/RingBuffer.h"
#include <ISRLayer/Includes/GlobalMailboxes.h>
#include <ApplicationLayer/Includes/ISRMsgHandler.h>

#define TRAIN_DATA_BUFFER_SIZE 61
#define NEW_LINE         "\n\r > "

#define CUP_STRUCT_SIZE 9

// Forward Declarations
class ISRMsgHandler;

class TrainMonitor {
    private:
        static TrainMonitor* TrainMonitorInstance_;

        ISRMsgHandler *ISRMsgHandlerInstance_;

        bool CupReset_;

        // Poll the ISR Msg Queue
        void CheckMessageHandler();

        void InputUARTChar(char input_char);
        void ResetCommandLine();
        void PrintDefaultScreen();
        void PrintCup(int row, int col);

    public:
        TrainMonitor();
        ~TrainMonitor();
        void SingletonGrab();
        void CentralLoop();
        void InitializeScreen();
        void VisuallySetHallSensor(uint8_t sensor_num, bool status);
        void VisuallySetSwitch(uint8_t switch_num, switch_direction_t dir);

        void UpdateCommandStatus(color_t color);

        static TrainMonitor* GetTrainMonitor();
};

#endif /* TrainMonitor_H */
