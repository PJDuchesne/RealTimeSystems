#ifndef UART0Driver_H
#define UART0Driver_H

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
-> Name:  UART0Driver.h
-> Date: Sept 13, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include <ApplicationLayer/Includes/ISRMsgHandler.h>
#include "../../OSLayer/Includes/RingBuffer.h"
#include <TrainLayers/Includes/TrainLibrary.h> // For nice hex output

#include <iomanip>

// PORTA & UART0 Registers
#define GPIO_PORTA_AFSEL_R (*((volatile unsigned long *)0x40058420)) // GPIOA Alternate Function Select Reg
#define GPIO_PORTA_PCTL_R  (*((volatile unsigned long *)0x4005852C)) // GPIOA Port Control Reg
#define GPIO_PORTA_DEN_R   (*((volatile unsigned long *)0x4005851C)) // GPIOA Digital Enable Reg

#define UART0_DR_R         (*((volatile unsigned long *)0x4000C000)) // UART0 Data Reg
#define UART0_FR_R         (*((volatile unsigned long *)0x4000C018)) // UART0 Flag Reg
#define UART0_IBRD_R       (*((volatile unsigned long *)0x4000C024)) // UART0 Integer Baud-Rate Divisor Reg
#define UART0_FBRD_R       (*((volatile unsigned long *)0x4000C028)) // UART0 Fractional Baud-Rate Divisor Reg
#define UART0_LCRH_R       (*((volatile unsigned long *)0x4000C02C)) // UART0 Line Control Reg
#define UART0_CTL_R        (*((volatile unsigned long *)0x4000C030)) // UART0 Control Reg
#define UART0_IFLS_R       (*((volatile unsigned long *)0x4000C034)) // UART0 Interrupt FIFO Level Select Reg
#define UART0_IM_R         (*((volatile unsigned long *)0x4000C038)) // UART0 Interrupt Mask Reg
#define UART0_MIS_R        (*((volatile unsigned long *)0x4000C040)) // UART0 Masked Interrupt Status Reg
#define UART0_ICR_R        (*((volatile unsigned long *)0x4000C044)) // UART0 Interrupt Clear Reg
#define UART0_CC_R         (*((volatile unsigned long *)0x4000CFC8)) // UART0 Clock Control Reg

// PORTB & UART1 Registers
#define GPIO_PORTB_AFSEL_R (*((volatile unsigned long *)0x40059420)) // GPIOB Alternate Function Select Reg
#define GPIO_PORTB_PCTL_R  (*((volatile unsigned long *)0x4005952C)) // GPIOB Port Control Reg
#define GPIO_PORTB_DEN_R   (*((volatile unsigned long *)0x4005951C)) // GPIOB Digital Enable Reg

#define UART1_DR_R         (*((volatile unsigned long *)0x4000D000)) // UART1 Data Reg
#define UART1_FR_R         (*((volatile unsigned long *)0x4000D018)) // UART1 Flag Reg
#define UART1_IBRD_R       (*((volatile unsigned long *)0x4000D024)) // UART1 Integer Baud-Rate Divisor Reg
#define UART1_FBRD_R       (*((volatile unsigned long *)0x4000D028)) // UART1 Fractional Baud-Rate Divisor Reg
#define UART1_LCRH_R       (*((volatile unsigned long *)0x4000D02C)) // UART1 Line Control Reg
#define UART1_CTL_R        (*((volatile unsigned long *)0x4000D030)) // UART1 Control Reg
#define UART1_IFLS_R       (*((volatile unsigned long *)0x4000D034)) // UART1 Interrupt FIFO Level Select Reg
#define UART1_IM_R         (*((volatile unsigned long *)0x4000D038)) // UART1 Interrupt Mask Reg
#define UART1_MIS_R        (*((volatile unsigned long *)0x4000D040)) // UART1 Masked Interrupt Status Reg
#define UART1_ICR_R        (*((volatile unsigned long *)0x4000D044)) // UART1 Interrupt Clear Reg
#define UART1_CC_R         (*((volatile unsigned long *)0x4000DFC8)) // UART1 Clock Control Reg

// Common UART bits
#define UART_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART_RX_FIFO_ONE_EIGHT  0x00000038  // UART Receive FIFO Interrupt Level at >= 1/8
#define UART_TX_FIFO_SVN_EIGHT  0x00000007  // UART Transmit FIFO Interrupt Level at <= 7/8
#define UART_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART_LCRH_FEN           0x00000010  // UART Enable FIFOs 
#define UART_CTL_UARTEN         0x00000301  // UART RX/TX Enable
#define UART_INT_TX             0x020       // Transmit Interrupt Mask
#define UART_INT_RX             0x010       // Receive Interrupt Mask
#define UART_INT_RT             0x040       // Receive Timeout Interrupt Mask
#define UART_CTL_EOT            0x00000010  // UART End of Transmission Enable
#define EN_RX_PA0               0x00000001  // Enable Receive Function on PA0
#define EN_TX_PA1               0x00000002  // Enable Transmit Function on PA1
#define EN_DIG_Px0              0x00000001  // Enable Digital I/O on Px0
#define EN_DIG_Px1              0x00000002  // Enable Digital I/O on Px1

// Clock Gating Registers (Shared by UART0 and UART1)
#define SYSCTL_RCGCGPIO_R      (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCUART_R      (*((volatile unsigned long *)0x400FE618))

#define SYSCTL_RCGCGPIO_UART0      0x00000001  // UART0 Clock Gating Control
#define SYSCTL_RCGCUART_GPIOA      0x00000001  // Port A Clock Gating Control

#define SYSCTL_RCGCGPIO_UART1      0x00000002  // UART1 Clock Gating Control
#define SYSCTL_RCGCUART_GPIOB      0x00000002  // Port B Clock Gating Control

#define SYSCTL_RCGCGPIO_UART0_1    0x00000003  // UART0 & UART1 Clock Gating Control
#define SYSCTL_RCGCUART_GPIOA_B    0x00000003  // PortA & PORTB Clock Gating Control

// Clock Configuration Register
#define SYSCTRL_RCC_R          (*((volatile unsigned long *)0x400FE0B0)) //

#define CLEAR_USRSYSDIV     0xF83FFFFF  // Clear USRSYSDIV Bits //
#define SET_BYPASS          0x00000800  // Set BYPASS Bit //

typedef enum uarts {
    UART0,
    UART1
} uarts_t;

// Forward Declaration
class ISRMsgHandler;

class UART0Driver {
    private:
        static UART0Driver* UART0DriverInstance_;
        ISRMsgHandler *ISRMsgHandlerInstance_;
        void UART0Init();
        void UART1Init();
        void UART0Enable(unsigned long flags);
        void UART1Enable(unsigned long flags);

    public:
        UART0Driver();
        void SingletonGrab();
        void UART0Handler();
        void UART1Handler();
        void JumpStartOutput0(char first_char);
        void JumpStartOutput1(char first_char);
        static UART0Driver* GetUART0Driver();
};

#endif /* UART0Driver_H */
