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

void ISRMsgHandler::SingletonGrab() {
    UART0DriverInstance_ = UART0Driver::GetUART0Driver();
}


ISRMsgHandler::ISRMsgHandler() {
    isr_queue_.reset(new RingBuffer<ISRMsg>(ISR_QUEUE_SIZE));
    output_data_buffer_.reset(new RingBuffer<char>(OUTPUT_DATA_BUFFER_SIZE));

    uart_output_idle_ = true;
}

void ISRMsgHandler::QueueMsg(MsgType_t type, char data) {
    // Create msg to pass into ISR queue
    ISRMsg msg = { .type = type, .data = data };
    isr_queue_->Add(msg);
}

void ISRMsgHandler::GetFromQueue(MsgType_t &type, char &data) {
    // Attempt to get message from queue
    ISRMsg msg = isr_queue_->Get();
    type = msg.type;
    data = msg.data;
}

bool ISRMsgHandler::CheckISRQueue() {
    return isr_queue_->Empty();
}

void ISRMsgHandler::QueueOutputMsg(std::string msg) {
    for (int i = 0; i < msg.length(); i++) {
        output_data_buffer_->Add(char(msg[i]));
    }

    // Jumpstart output if necessary
    if (uart_output_idle_) {
        char first_char = output_data_buffer_->Get();
        uart_output_idle_ = false;
        UART0DriverInstance_->JumpStartOutput(first_char);
    }
}

bool ISRMsgHandler::OutputBufferEmpty() {
    return output_data_buffer_->Empty();
}

char ISRMsgHandler::GetOutputChar() {
    return output_data_buffer_->Get();
}

ISRMsgHandler* ISRMsgHandler::GetISRMsgHandler() {
    if (!ISRMsgHandlerInstance_) ISRMsgHandlerInstance_ = new ISRMsgHandler;
    return ISRMsgHandlerInstance_;
}
