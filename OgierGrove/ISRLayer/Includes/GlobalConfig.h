#ifndef GlobalConfig_H
#define GlobalConfig_H

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
-> Name: GlobalConfig.h
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <iostream>
#include <cstring>
#include <string>
#include <cstdint>

#include <ISRLayer/Includes/SysTickDriver.h>
#include <ISRLayer/Includes/UART0Driver.h>
#include <ApplicationLayer/Includes/ISRMsgHandler.h>
#include <ApplicationLayer/Includes/Monitor.h>
#include <ApplicationLayer/Includes/CommandCenter.h>
#include <ApplicationLayer/Includes/TimeHandler.h>
#include <TrainLayers/Includes/TrainMonitor.h>

#include "GlobalMailboxes.h"

#define NVIC_EN0_R (*((volatile unsigned long *)0xE000E100)) // Interrupt  0-31 Set Enable Register
#define NVIC_EN1_R (*((volatile unsigned long *)0xE000E104)) // Interrupt 32-54 Set Enable Register

#define INT_VEC_UART0   5  // UART0 Rx and Tx interrupt index (decimal)
#define INT_VEC_UART1   6  // UART1 Rx and Tx interrupt index (decimal)

#define ISR_QUEUE_SIZE 100
#define UART0_OUTPUT_DATA_BUFFER_SIZE 1000
#define UART1_OUTPUT_DATA_BUFFER_SIZE 100

void InterruptMasterEnable();
void InterruptEnable(unsigned long InterruptIndex);

void SingletonSetup();

void CustomFaultISR();

#endif /* GlobalConfig_H */
