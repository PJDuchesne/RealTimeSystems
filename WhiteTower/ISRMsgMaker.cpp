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
-> Name:  ISRMsgMaker.cpp
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/ISRMsgMaker.h"

// Singleton Instance
ISRMsgMaker *ISRMsgMaker::ISRMsgMakerInstance_ = new ISRMsgMaker;

ISRMsgMaker::ISRMsgMaker() {
  isr_queue_.reset(new RingBuffer<ISRMsg>(ISR_QUEUE_SIZE));
  output_data_buffer_.reset(new RingBuffer<char>(OUTPUT_DATA_BUFFER_SIZE));

  uart_output_idle_ = true;
}

void ISRMsgMaker::QueueMsg(MsgType_t type, char data) {
  // Create msg to pass into ISR queue
  ISRMsg msg = { .type = type, .data = data };
  // std::cout << "Queueing [ISR]\n";
  isr_queue_->Add(msg);
}

void ISRMsgMaker::GetFromQueue(MsgType_t &type, char &data) {
  // Attempt to get message from queue
  ISRMsg msg = isr_queue_->Get();
  type = msg.type;
  data = msg.data;

  // std::cout << "------------------------------------\n";
  // std::cout << "TYPE: >>" << type << "<<\n";
  // std::cout << "DATA: >>" << data << "<<\n";
  // std::cout << "------------------------------------\n";
}

bool ISRMsgMaker::CheckISRQueue() {
  return isr_queue_->Empty();
}

void ISRMsgMaker::QueueOutputMsg(std::string msg) {
  for (int i = 0; i < msg.length(); i++) {
    // std::cout << "Queueing [Output Chars] >>" << i << "<<\n";
    output_data_buffer_->Add(char(msg[i]));
  }

  // Jumpstart output if necessary
  if (uart_output_idle_) {
      char first_char = output_data_buffer_->Get();
      // std::cout << "FIRST CHAR: >>" << first_char << "<<\n";
      uart_output_idle_ = false;
      UART0Driver::GetUART0Driver()->JumpStartOutput(first_char);
  }
}

bool ISRMsgMaker::OutputBufferEmpty() {
  return output_data_buffer_->Empty();
}

char ISRMsgMaker::GetOutputChar() {
  return output_data_buffer_->Get();
}

ISRMsgMaker* ISRMsgMaker::GetISRMsgMaker() {
  // if (!ISRMsgMakerInstance) ISRMsgMakerInstance = new ISRMsgMaker;
  return ISRMsgMakerInstance_;
}
