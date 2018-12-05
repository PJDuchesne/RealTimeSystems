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
-> Name:  TrainLibrary.cpp
-> Date: Nov 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TrainLibrary.h"

/*
    Function: MsgLengthFromCode
    Brief: Helper function to quickly get the message length from the command code
    Input: msg_code: Code to parse
    Output: uint8_t: Length of message
*/
uint8_t MsgLengthFromCode(uint8_t msg_code) {
    switch (msg_code) {
        case '\xA8':
            return 1;
        case '\xA2':
            return 2;
        case '\xC0':
        case '\xE0':
            return 3;
        default:
            std::cout << "'TrainLibrary'::MsgLengthFromCode(): ERROR, Invalid Message code\n";
            while(1) {}
    }
}

