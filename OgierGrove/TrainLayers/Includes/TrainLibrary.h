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

// For cleaning printing hex characters
#include <iomanip>
#define HEX(x) " " << std::setw(2) << std::setfill('0') << std::hex << int(x) << " " << std::dec

#define DELAY(x) i_DELAY = 0; while(i_DELAY++ < x) {}

#define UART_PHYSICAL_LAYER_MB_SIZE 100

#define MAX_PACKET_SIZE 5

#define ALL 255 // Used to acknowledge all sensor resets or throw all switches

// Macros to perform modulus 8 increments and decrements
#define MOD8PLUS1(x) (x = (x + 1) % 8)
#define MOD8MINUS1(x) (x = (--x >= 0 ? x : 7))

enum train_layer_mailboxes {
    UART_PHYSICAL_LAYER_MB     = 200,
    PACKET_PHYSICAL_LAYER_MB   = 201,
    DATA_LINK_LAYER_MB         = 202,
    TRAIN_APPLICATION_LAYER_MB = 203,
    TRAIN_TIME_SERVER_MB       = 204,
    TEST_PROCESS_MB = 210 // For future testing
};

typedef enum packet_type {
    DATA_PT   = 0,
    ACK_PT    = 1,
    NACK_PT   = 2,
    UNUSED_PT = 3 // Actually used internally as a NULL value of packets
} packet_type_t;

typedef struct control 
{
    union {
        struct {
            uint8_t nr : 3;
            uint8_t ns : 3;
            packet_type_t type : 2;
        };
        uint8_t all;
    };
} control_t;

typedef struct ack_control
{
    control_t control;
    uint8_t empty;
} ack_control_t;

typedef struct train_msg {
    packet_type_t code;
    uint8_t arg1;
    uint8_t arg2;
} train_msg_t; // 3 bytes

typedef struct packet {
    control_t control_block; // 1 byte
    uint8_t length; // 1 byte
    // TODO: Is this necessary?
    // Anonymous union for direct access
    union { // 3 bytes
        train_msg_t msg;
        struct 
        {
            packet_type_t code;
            uint8_t arg1;
            uint8_t arg2;
        };
    };
} packet_t; // 5 bytes

typedef struct train_request {

} train_request_t;

uint8_t MsgLengthFromCode(uint8_t msg_code);

typedef enum train_direction {
    CW = 0,
    CCW = 8 // 8 because that's the value within its nibble
} train_direction_t;

typedef enum switch_direction {
    STRAIGHT = 0,
    DIVERTED = 1
} switch_direction_t;

typedef struct train_settings
{
    uint8_t speed : 4;
    train_direction_t direction : 4;
} train_settings_t;

#endif /* TrainLibrary_H */
