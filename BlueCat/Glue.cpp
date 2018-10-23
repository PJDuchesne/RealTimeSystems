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
-> Name:  glue.cpp
-> Date: Sept 17, 2018	(Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <ISRLayer/Includes/SysTickDriver.h>
#include <ISRLayer/Includes/UART0Driver.h>
#include "OSLayer/Includes/KernelFunctions.h"

/*
    Function: UART0_ISR_C
    Brief: Dummy function to be called from C code to access its C++ equivalent for UART0
*/
extern "C" void UART0_ISR_C(void) {
	UART0Driver::GetUART0Driver()->UART0Handler();
}

/*
    Function: SysTick_ISR_C
    Brief: Dummy function to be called from C code to access its C++ equivalent for SysTick
*/
extern "C" void SysTick_ISR_C(void) {
	SysTickDriver::GetSysTickDriver()->SysTickHandler();
}

extern "C" void SVC_ISR_C(void) {
    std::cout << "[Glue] SVC_ISR_C\n";
    SVCall();
}
