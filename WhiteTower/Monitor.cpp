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
-> Name:  Monitor.cpp
-> Date: Sept 17, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/Monitor.h"

// Singleton Instance
Monitor *Monitor::MonitorInstance_ = new Monitor;

Monitor::Monitor() {
  data_buffer_.reset(new RingBuffer<char>(DATA_BUFFER_SIZE));
  single_char = new char[2];
  single_char[1] = NULL;
}

void Monitor::CentralLoop() {
  MsgType_t type = NONE;
  char data = char();

  PrintMsg(clear_screen);
  PrintMsg(new_line);

  // Loop Forever
  while(1)
  {
    CheckMessageHandler(type, data);
  }
}

void Monitor::CheckMessageHandler(MsgType_t &type, char &data) {
  // If the queue is empty, return
  if (ISRMsgMaker::GetISRMsgMaker()->CheckISRQueue()) return;

  ISRMsgMaker::GetISRMsgMaker()->GetFromQueue(type, data);
  if (type == NONE) return;

  switch (type) {
    case UART:
      HandleUART(data);
      break;
    case SYSTICK:
      HandleSYSTICK();
      break;
    default:
      std::cout << "[CheckMessageHandler()] SWITCH TABLE ERROR: >>" << type << "<<\n";
      break;
  }

}

// TODO: Handle UART
void Monitor::HandleUART(char data) {
  char fetched_char;
  std::string command_string = "";
  single_char[0] = data;
  switch (data) {
    case 0x08: // Backspace
    case 0x7F: // Delete

      // Do not delete if there is nothing in the buffer
      if (!data_buffer_->Empty()) {
        single_char[0] = data;
        PrintMsg(single_char);
        data_buffer_->Get();
      }

      break;
    case 0x0D: // Enter (Carriage Return)
      // TODO: Write a more elegant "GetAll" function that avoids this loop
      while (1) {
        fetched_char = data_buffer_->Get();
        if (fetched_char) command_string += fetched_char;
        else {
          if (command_string != "") CommandCenter::GetCommandCenter()->HandleCommand(command_string);
          break;
        }
      }
      PrintMsg(new_line);
      data_buffer_->Reset();
      break;
    default:  // All other characters, add to buffer
      // Add data to buffer
      data_buffer_->Add(data);
      single_char[0] = data;
      PrintMsg(single_char);
      break;
  }
}

// TODO: HandleSYSTICK
void Monitor::HandleSYSTICK() {
}

void Monitor::PrintNewLine() {
  PrintMsg(single_char);
      data_buffer_->Reset();
}

void Monitor::PrintMsg(std::string msg) {
  ISRMsgMaker::GetISRMsgMaker()->QueueOutputMsg(msg);
}

Monitor* Monitor::GetMonitor() {
    // if (!MonitorInstance) MonitorInstance = new Monitor;
    return MonitorInstance_;
}
