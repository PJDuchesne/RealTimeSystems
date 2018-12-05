#ifndef PhysicalLayer_H
#define PhysicalLayer_H

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
-> Name:  PhysicalLayer.h
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"
#include <ApplicationLayer/Includes/ISRMsgHandler.h>

#define CHAR_BUFFER_SIZE 100
#define MAX_FRAME_SIZE 16

#define ONE_BYTE_MAX 255

class ISRMsgHandler;

class PhysicalLayer {
    private:
        static PhysicalLayer* PhysicalLayerInstance_;

        ISRMsgHandler *ISRMsgHandlerInstance_;

        void PassFrame(unsigned char* frame_ptr, uint8_t frame_len);

    public:
        void UARTMailboxLoop();
        void PacketMailboxLoop();

        void FakePacket(); // For debugging

        PhysicalLayer() {};
        ~PhysicalLayer() {};
        void CentralLoop();
        static PhysicalLayer* GetPhysicalLayer();
};

#endif /* PhysicalLayer_H */
