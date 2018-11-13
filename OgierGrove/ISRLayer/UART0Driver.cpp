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

#include <ISRLayer/Includes/UART0Driver.h>

// Singleton Instance
UART0Driver *UART0Driver::UART0DriverInstance_ = 0;

/*
    Function: UART0Init
    Brief: Initializes almost everything for the UART to function: status control registers,
           clock gating registers, baud rate registers, and port A registers
*/
void UART0Driver::UART0Init() {
    volatile int wait;

    /* Initialize UART0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA_B;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0_1;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART0
    UART1_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART1
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    // UART0
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556
    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    // UART1
    UART1_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART1_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556
    UART1_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    // Set up PORTA (For UART0)
    GPIO_PORTA_AFSEL_R = 0x3;                    // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);  // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;  // Enable Digital I/O on PA1-0

    // Set up PORTB (For UART1)
    GPIO_PORTB_AFSEL_R = 0x3;                    // Enable Receive and Transmit on PA1-0
    GPIO_PORTB_PCTL_R = (0x01) | ((0x01) << 4);  // Enable UART RX/TX pins on PA1-0
    GPIO_PORTB_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;  // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN; // Enable the UART0
    UART1_CTL_R = UART_CTL_UARTEN; // Enable the UART1
    wait = 0; // wait; give UART time to enable itself
}

/*
    Function: UART0Enable
    Input: flags: specified bits to set for interrupt
    Brief: Specifically enables the UART0 interrupt
*/
void UART0Driver::UART0Enable(unsigned long flags) {
    UART0_IM_R |= flags;
    UART1_IM_R |= flags; // TODO: Decouple this, but fine for now
}

/*
    Function: SingletonGrab
    Brief: Setup function for the UART0Driver to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void UART0Driver::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
}

/*
    Function: UART0Driver
    Brief: Constructor for the UART0Driver class, which initializes UART on startup
*/
UART0Driver::UART0Driver() {
    UART0Init();
    UART0Enable(UART_INT_RX | UART_INT_TX);
}

/*
    Function: UART0Handler
    Brief: UART0 ISR, which has two possible triggers:
            1: Recieving Data: UART has recieved data that needs to be queued in the ISR Msg Queue
            2. Successful Transmission: UART has successfully transmitted data and is ready to transmit
                more from the UART output buffer
                    NOTE: This transmission process has to be jump started from the MsgHandler by manually
                          outputting the first character 
*/
void UART0Driver::UART0Handler() {

    // Set up arguments for future use
    static kcallargs_t UARTArguments;
    static bool first_time = true;
    static char uart_data;

    if (first_time) {
        UARTArguments.src_q = KERNEL_MB;
        UARTArguments.dst_q = ISR_MSG_HANDLER_MB;
        UARTArguments.msg_len = 1;
        UARTArguments.msg_ptr = &uart_data;
        first_time = false;
    }

    // Check if RECV Done
    if (UART0_MIS_R & UART_INT_RX)
    {
        // Queue the msg
        uart_data = UART0_DR_R;
        KSend(&UARTArguments);

        // KSendUARTFromKernel(UART0_DR_R);

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

void UART0Driver::UART1Handler() {
    std::cout << "UART1Handler()";
}

/*
    Function: JumpStartOutput
    Brief: Jumpstarts the transmission sequence mentioned in UART0Handler. Once started, the
           process cascades to empty out the buffer.
*/
void UART0Driver::JumpStartOutput(char first_char) {
    UART0_DR_R = first_char;
}

/*
    Function: GetUART0Driver
    Output: UART0DriverInstance_: Pointer to the UART0Driver Singleton
    Brief: Get function for the UART0Driver singleton
*/
UART0Driver* UART0Driver::GetUART0Driver() {
    if (!UART0DriverInstance_) UART0DriverInstance_ = new UART0Driver;
    return UART0DriverInstance_;
}
