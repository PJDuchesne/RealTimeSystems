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
    DATA_LINK_LAYER_MB       = 202,
    APPLICATION_LAYER_MB     = 203
};

typedef enum packet_type {
    DATA_PT = 0,
    ACK_PT  = 1,
    NACK_PT = 2,
    UNUSED_PT = 3 // Actually used internally as a NULL value of packets
} packet_type_t;

typedef struct control 
{
    // TODO: Constructor for useful initilizations
    // control(uint8_t nr, uint8_t ns, packet_type_t type) : nr(nr),
    //                                                   ns(ns),
    //                                                   type(type) {}
    union {
        struct {
            uint8_t nr : 3;
            uint8_t ns : 3;
            packet_type_t type : 2;
        };
        uint8_t all;
    };
    // TODO: Add constructor
} control_t;

typedef struct train_msg {
    packet_type_t code;
    uint8_t arg1;
    uint8_t arg2;
} train_msg_t;

typedef struct packet {
    control_t control_block;
    uint8_t length;
    // TODO: Is this necessary?
    // Anonymous union for direct access
    union {
        train_msg_t msg;
        struct 
        {
            packet_type_t code;
            uint8_t arg1;
            uint8_t arg2;
        };
    };
} packet_t;

#endif /* TrainLibrary_H */
