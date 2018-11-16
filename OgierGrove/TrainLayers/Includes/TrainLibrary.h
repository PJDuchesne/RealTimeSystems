#ifndef TrainLibrary_H
#define TrainLibrary_H

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
-> Name:  TrainLibrary.h
-> Date: Nov 16, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

// TODO: Remove any unncessary includes
#include <cstdint>
#include <iostream>
#include <string>
#include <sstream>

#include <OSLayer/Includes/ProcessCalls.h>

#define UART_PHYSICAL_LAYER_MB_SIZE 100

enum train_layer_mailboxes {
    UART_PHYSICAL_LAYER_MB   = 200,
    PACKET_PHYSICAL_LAYER_MB = 201,
    DATA_LINK_LAYER_MB       = 202
};

typedef enum PacketType {
    DATA_PT = 0,
    ACK_PT  = 1,
    NACK_PT = 2
} PacketType_t;

typedef struct control 
{
    // control(uint8_t nr, uint8_t ns, PacketType_t type) : nr(nr),
    //                                                   ns(ns),
    //                                                   type(type) {}
    union {
        struct {
            uint8_t nr : 3;
            uint8_t ns : 3;
            PacketType_t type : 2;
        };
        uint8_t all;
    };
    // TODO: Add constructor
} control_t;

#endif /* TrainLibrary_H */
