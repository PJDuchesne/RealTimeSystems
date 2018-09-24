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
-> Name:  UART0Driver.cpp
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/UART0Driver.h"

// Singleton Instance
UART0Driver *UART0Driver::UART0DriverInstance_ = 0;

void UART0Driver::UART0Init() {
    volatile int wait;

    /* Initialize UART0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;        // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself
}

void UART0Driver::UART0Enable(unsigned long flags) {
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
}

void UART0Driver::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
}

// Constructor, which initializes UART on startup
UART0Driver::UART0Driver() {
    UART0Init();
    UART0Enable(UART_INT_RX | UART_INT_TX);
}

void UART0Driver::UART0Handler() {
    // Check if RECV Done
    if (UART0_MIS_R & UART_INT_RX)
    {
        // Queue the msg
        ISRMsgHandlerInstance_->QueueMsg(UART, UART0_DR_R);

        /* Clear interrupt */
        UART0_ICR_R |= UART_INT_RX;
    }

    // Check if XMIT done
    if (UART0_MIS_R & UART_INT_TX)
    {
        /* Clear interrupt */
        UART0_ICR_R |= UART_INT_TX;

        // If there is more data in the buffer, output another char
        if (ISRMsgHandlerInstance_->OutputBufferEmpty()) {
            ISRMsgHandlerInstance_->uart_output_idle_ = true;
        }
        else {
            char tmp = ISRMsgHandlerInstance_->GetOutputChar();
            UART0_DR_R = tmp;
        }
    }
}

void UART0Driver::JumpStartOutput(char first_char) {
    UART0_DR_R = first_char;
}

UART0Driver* UART0Driver::GetUART0Driver() {
    if (!UART0DriverInstance_) UART0DriverInstance_ = new UART0Driver;
    return UART0DriverInstance_;
}
