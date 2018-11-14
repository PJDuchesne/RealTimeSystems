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
-> Name:  ISRMsgHandler.cpp
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/ISRMsgHandler.h"

// Singleton Instance
ISRMsgHandler *ISRMsgHandler::ISRMsgHandlerInstance_ = 0;

/*
    Function: SingletonGrab
    Brief: Setup function for the UART0Driver to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void ISRMsgHandler::SingletonGrab() {
    UART0DriverInstance_ = UART0Driver::GetUART0Driver();
}

/*
    Function: ISRMsgHandler
    Brief: Constructor for ISRMsgHandler. This sets up both the ISR_queue and the output data queue.
           This function also initializes the uart_output_idle_ to false.
*/
ISRMsgHandler::ISRMsgHandler() {
    uart0_output_data_buffer_ = new RingBuffer<char>(UART0_OUTPUT_DATA_BUFFER_SIZE);
    uart1_output_data_buffer_ = new RingBuffer<char>(UART1_OUTPUT_DATA_BUFFER_SIZE);

    uart0_output_idle_ = true;
    uart1_output_idle_ = true;
}

/*
    Function: ~ISRMsgHandler
    Brief: Destructor for ISRMsgHandler. This deletes the two queues when the program ends.
*/
ISRMsgHandler::~ISRMsgHandler() {
    delete uart0_output_data_buffer_;
    delete uart1_output_data_buffer_;
}

/*
    Function: GetFromISRQueue
    Output: type: Enumeration of message type from queue (UART or SYSTICK)
            data: Character data for UART message (or empty for SYSTick)
    Brief: Getter to get a message from the ISR_MSG_HANDLER mailbox
*/
void ISRMsgHandler::GetFromISRQueue(MsgType_t &type, char &data) {
    uint8_t src_q;
    uint32_t msg_len;
    // if (PRecv(src_q, ISR_MSG_HANDLER_MB, &data, msg_len, false)) {
    if (PRecv(src_q, ISR_MSG_HANDLER_MB, &data, msg_len)) {
        type = ((msg_len == 1) ? UART : SYSTICK);
    }
    else type = NONE;
}

/*
    Function: QueueOutputMsg
    Input:  msg: Message to queue into the output UART character queue
    Brief: API to queue a message into the output UART character queue
*/
void ISRMsgHandler::QueueOutputMsg(std::string msg, uint8_t uart_num) {
    char tmpChar, first_char;

    // TODO: Make this table driven
    switch (uart_num) {
        case 0:
            for (int i = 0; i < msg.length(); i++) {
                tmpChar = msg[i];
                uart0_output_data_buffer_->Add(tmpChar);
            }

            // Jumpstart output if necessary
            if (uart0_output_idle_) {
                first_char = uart0_output_data_buffer_->Get();
                uart0_output_idle_ = false;
                UART0DriverInstance_->JumpStartOutput0(first_char);
            }
        case 1:
            for (int i = 0; i < msg.length(); i++) {
                tmpChar = msg[i];
                uart1_output_data_buffer_->Add(tmpChar);
            }

            // Jumpstart output if necessary
            if (uart1_output_idle_) {
                first_char = uart1_output_data_buffer_->Get();
                uart1_output_idle_ = false;
                UART0DriverInstance_->JumpStartOutput1(first_char);
            }
        default:
            // TODO: Error Msg here
    }
}

/*
    Function: QueueOutputMsg
    Input:  msg: Message to queue into the output UART character queue
    Brief: API to queue a message into the output UART character queue
*/
void ISRMsgHandler::QueueOutputMsg(char* msg, uint16_t len, uint8_t uart_num) {
    char first_char;

    // TODO: Make this table driven
    switch (uart_num) {
        case 0:
            for (int i = 0; i < len; i++) uart0_output_data_buffer_->Add(msg[i]);

            // Jumpstart output if necessary
            if (uart0_output_idle_) {
                first_char = uart0_output_data_buffer_->Get();
                uart0_output_idle_ = false;
                UART0DriverInstance_->JumpStartOutput0(first_char);
            }
        case 1:
            std::cout << "ISRMsgHandler::QueueOutputMsg(): Outputting on UART1\n";

            uart1_output_data_buffer_->Add('\x02');
            for (int i = 0; i < len; i++) uart1_output_data_buffer_->Add(msg[i]);
            uart1_output_data_buffer_->Add('\x03');

            // Jumpstart output if necessary
            if (uart1_output_idle_) {
                first_char = uart1_output_data_buffer_->Get();
                uart1_output_idle_ = false;
                UART0DriverInstance_->JumpStartOutput1(first_char);
            }
        default:
            // TODO: Error Msg here
    }
}

/*
    Function: OutputBufferEmpty
    Output:  bool: Boolean that indicates whether the uart0_output_data_buffer_ is empty not.
    Brief: Returns the bool described above
*/
bool ISRMsgHandler::OutputBufferEmpty(uint8_t uart_num) {
    switch (uart_num) {
        case UART0:
            return uart0_output_data_buffer_->Empty();
        case UART1:
            return uart1_output_data_buffer_->Empty();
        default:
            // TODO: Error case
    }
}

/*
    Function: GetOutputChar
    Output:  char: Character value from the uart0_output_data_buffer_
    Brief: Returns the char described above
*/
char ISRMsgHandler::GetOutputChar(uint8_t uart_num) {
    switch (uart_num) {
        case UART0:
            return uart0_output_data_buffer_->Get();
        case UART1:
            return uart1_output_data_buffer_->Get();
        default:
            // TODO: Error case
    }
}

/*
    Function: GetISRMsgHandler
    Output: ISRMsgHandlerInstance_: Pointer to the ISRMsgHandler Singleton
    Brief: Get function for the ISRMsgHandler singleton
*/
ISRMsgHandler* ISRMsgHandler::GetISRMsgHandler() {
    if (!ISRMsgHandlerInstance_) ISRMsgHandlerInstance_ = new ISRMsgHandler;
    return ISRMsgHandlerInstance_;
}