#ifndef DataLinkLayer_H
#define DataLinkLayer_H

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
-> Name:  DataLinkLayer.h
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "TrainLibrary.h"

#include <OSLayer/Includes/RingBuffer.h>
#include <ApplicationLayer/Includes/ISRMsgHandler.h>

#define MAX_DLL_WAITING_PACKETS 8
#define MOD_SIZE 8
#define PACKET_BUFFER_SIZE 32 // Incredibly arbitrary

#define WINDOW_SIZE 2 // "A window size of 2 is quite common"

class DataLinkLayer {
    private:
        static DataLinkLayer* DataLinkLayerInstance_;

        // Ring buffer for any packets that are waiting to be sent
        RingBuffer<packet_t> *packet_buffer_;

        // Array for packets that have been sent, but not yet acknowledged
        packet_t outgoing_messages_[MAX_DLL_WAITING_PACKETS];
        // Number of packets that have been sent but not yet acknowledged,
        // uint8_t num_packets_in_limbo_; // At all times, should be <= WINDOW_SIZE

        // NS and NR to use
        uint8_t tiva_ns_;
        uint8_t tiva_nr_;

        void SendMessageUp(packet_t* packet); // Up to application layer
        void SendPacketDown(packet_t* packet); // Down to physical layer
        void HandleACK(uint8_t train_nr);
        void HandleNACK(uint8_t train_nr);
        void SendACK();
        void SendNACK();

        void InternallyACK(uint8_t buffer_idx);
        void SetPacketAlarm(uint8_t alarm_num, bool set_flag = true);

        bool CheckACKRange(uint8_t incoming_nr);

        void MakePacket(packet_t &packet, train_msg_t *msg);

        uint8_t NumPacketsInLimbo();

    public:
        void MailboxLoop();

        DataLinkLayer();
        ~DataLinkLayer();
        void CentralLoop();
        static DataLinkLayer* GetDataLinkLayer();

};

#endif /* DataLinkLayer_H */
