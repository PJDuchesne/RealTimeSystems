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
-> Name: GlobalConfig.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/GlobalConfig.h"

void InterruptMasterEnable() {
    __asm("    cpsie   i");
}

void InterruptEnable(unsigned long InterruptIndex) {
    /* Indicate to CPU which device is to interrupt */
    if(InterruptIndex < 32) {
        // Enable the interrupt in the EN0 Register
        NVIC_EN0_R = 1 << InterruptIndex;      
    }
    else {
        // Enable the interrupt in the EN1 Register
        NVIC_EN1_R = 1 << (InterruptIndex - 32);
    }
}
