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
    std::cout << "UARTDriver::UART0Init()\n";

    volatile int wait;

    /* Initialize UART0 */
    // Shared registers
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    // Disable UART0 to muck around with its registers
    UART0_CTL_R &= ~UART_CTL_UARTEN;
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556
    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    // Set up PORTA (For UART0)
    GPIO_PORTA_AFSEL_R = 0x3;                    // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);  // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_Px0 | EN_DIG_Px1;  // Enable Digital I/O on PA1-0

    // Enable UART0
    UART0_CTL_R = UART_CTL_UARTEN;
    wait = 0; // wait; give UART0 time to enable itself
}

void UART0Driver::UART1Init() {
    std::cout << "UARTDriver::UART1Init()\n";

    volatile int wait;

    /* Initialize UART1 */
    // Shared registers
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOB;   // Enable Clock Gating for UART1
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART1;   // Enable Clock Gating for PORTB
    wait = 0; // give time for the clocks to activate

    // Disable UART1 to muck around with its registers
    UART1_CTL_R &= ~UART_CTL_UARTEN;
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    // 9,600
    // UART1_IBRD_R = 104;   // IBRD = int(16,000,000 / (16 * 9,600)) = 104.1666666666667
    // UART1_FBRD_R = 11;  // FBRD = int(.1666666666667 * 64 + 0.5) = 11.16666666666667

    // 115,200
    UART1_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART1_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART1_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    // Set up PORTB (For UART1)
    GPIO_PORTB_AFSEL_R = 0x3;                    // Enable Receive and Transmit on PB1-0
    GPIO_PORTB_PCTL_R = (0x01) | ((0x01) << 4);  // Enable UART RX/TX pins on PB1-0
    GPIO_PORTB_DEN_R = EN_DIG_Px0 | EN_DIG_Px1;  // Enable Digital I/O on PB1-0

    // Enable UART1
    UART1_CTL_R = UART_CTL_UARTEN;
    wait = 0; // wait; give UART1 time to enable itself
}

/*
    Function: UART0Enable
    Input: flags: specified bits to set for interrupt
    Brief: Specifically enables the UART0 interrupt
*/
void UART0Driver::UART0Enable(unsigned long flags) {
    std::cout << "UARTDriver::UART0Enable()\n";
    UART0_IM_R |= flags;
}

/*
    Function: UART0Enable
    Input: flags: specified bits to set for interrupt
    Brief: Specifically enables the UART1 interrupt
*/
void UART0Driver::UART1Enable(unsigned long flags) {
    std::cout << "UARTDriver::UART1Enable()\n";
    UART1_IM_R |= flags;
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
    Brief: Constructor for the UART0Driver class, which initializes UART0 & UART1 on startup
*/
UART0Driver::UART0Driver() {
    // On startup, initialize UART0
    UART0Init();
    UART0Enable(UART_INT_RX | UART_INT_TX);

    // On startup, initialize UART1
    UART1Init();
    UART1Enable(UART_INT_RX | UART_INT_TX);
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
        // Queue the incoming msg
        uart_data = UART0_DR_R;
        KSend(&UARTArguments);

        /* Clear interrupt */
        UART0_ICR_R |= UART_INT_RX;
    }

    // Check if XMIT done
    if (UART0_MIS_R & UART_INT_TX)
    {
        /* Clear interrupt */
        UART0_ICR_R |= UART_INT_TX;

        // If there is more data in the buffer, output another char
        if (ISRMsgHandlerInstance_->OutputBufferEmpty(UART0)) {
            ISRMsgHandlerInstance_->uart0_output_idle_ = true;
        }
        else {
            char tmp = ISRMsgHandlerInstance_->GetOutputChar(UART0);
            UART0_DR_R = tmp;
        }
    }
}

void UART0Driver::UART1Handler() {
    // Set up arguments for future use
    static kcallargs_t UARTArguments;
    static bool first_time = true;
    static char uart_data;
    static char output_char;

    if (first_time) {
        UARTArguments.src_q = KERNEL_MB;
        UARTArguments.dst_q = UART_PHYSICAL_LAYER_MB;
        UARTArguments.msg_len = 1;
        UARTArguments.msg_ptr = &uart_data;
        first_time = false;
    }

    // Check if RECV Done
    if (UART1_MIS_R & UART_INT_RX)
    {
        // Queue the incoming msg
        uart_data = UART1_DR_R;

        KSend(&UARTArguments);

        /* Clear interrupt */
        UART1_ICR_R |= UART_INT_RX;
    }

    // Check if XMIT done
    if (UART1_MIS_R & UART_INT_TX)
    {
        /* Clear interrupt */
        UART1_ICR_R |= UART_INT_TX;

        // If the buffer is empty, set UART to idle
        if (ISRMsgHandlerInstance_->OutputBufferEmpty(UART1)) {
            ISRMsgHandlerInstance_->uart1_output_idle_ = true;
        }
        // Else, the buffer has more to print
        else {
            char tmp = ISRMsgHandlerInstance_->GetOutputChar(UART1);
            UART1_DR_R = tmp;
        }
    }
}

/*
    Function: JumpStartOutput
    Brief: Jumpstarts the transmission sequence mentioned in UART0Handler. Once started, the
           process cascades to empty out the buffer.
*/
void UART0Driver::JumpStartOutput0(char first_char) {
    UART0_DR_R = first_char;
}

/*
    Function: JumpStartOutput
    Brief: Jumpstarts the transmission sequence mentioned in UART0Handler. Once started, the
           process cascades to empty out the buffer.
*/
void UART0Driver::JumpStartOutput1(char first_char) {
    // std::cout << "UART0Driver::JumpStartOutput1() >>" << int(first_char) << "<<\n";
    UART1_DR_R = first_char;
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
