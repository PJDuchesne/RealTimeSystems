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
Monitor *Monitor::MonitorInstance_ = 0;

void Monitor::CheckMessageHandler(MsgType_t &type, char &data) {
    ISRMsgHandlerInstance_->GetFromQueue(type, data);

    switch (type) {
        case NONE:
            // Meaning the queue is empty
            break;
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

void Monitor::HandleUART(char data) {
    static char* single_char = new char[1];
    char fetched_char;
    std::string command_string = "";

    static int8_t escape_mode = -1;

    // If in escape mode, skip next two characters
    if (escape_mode != -1) {
        // However, if one of the skipped chacters is a number, skip an extra one
        if (data >= 48 && data <= 57) return;
        if (++escape_mode == 2) escape_mode = -1;
        return;
    }

    switch (data) {
        case 0x08: // Backspace
        case 0x7F: // Delete
            // Do not delete if there is nothing in the buffer
            if (!data_buffer_->Empty()) {
                single_char[0] = static_cast<char>(0x7F);
                PrintMsg(single_char);
                data_buffer_->Pop();
            }

            break;
        case 0x0D: // Enter (Carriage Return)
            while (1) {
                fetched_char = data_buffer_->Get();
                if (fetched_char) command_string += fetched_char;
                else {
                    if (command_string != "") CommandCenterInstance_->HandleCommand(command_string);
                    break;
                }
            }
            PrintNewLine();
            data_buffer_->Reset();
            break;

        case 0x1B: // ESCAPE
            // Enter backslash mode, ignoring next two characters
            escape_mode = 0;
            break;

        default:  // All other characters, add to buffer
            data_buffer_->Add(data);
            single_char[0] = data;
            PrintMsg(single_char);
            break;
    }
}

void Monitor::HandleSYSTICK() {
    TimeHandlerInstance_->TickTenthSec();
}

void Monitor::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
    CommandCenterInstance_ = CommandCenter::GetCommandCenter();
    TimeHandlerInstance_ = TimeHandler::GetTimeHandler();
}

void Monitor::PrintNewLine() {
    PrintMsg(NEW_LINE);
}

Monitor::Monitor() {
    data_buffer_ = new RingBuffer<char>(DATA_BUFFER_SIZE);
}

Monitor::~Monitor() {
    delete data_buffer_;
}

void Monitor::CentralLoop() {
    MsgType_t type = NONE;
    char data = char();

    // Reset all VT-100 settings (background, foreground, etc.)
    PrintMsg("\e[0m");

    PrintMsg(CLEAR_SCREEN);
    PrintNewLine();

    // Loop Forever
    while(1)
    {
        CheckMessageHandler(type, data);
    }
}

void Monitor::RePrintOutputBuffer() {
    // Fetch contents of buffer
    std::string buffer_contents = "";
    while (!data_buffer_->Empty()) buffer_contents += data_buffer_->Get();

    // Print contents of buffer
    PrintMsg(buffer_contents);

    // Restore contents of buffer
    for (int i = 0; i < buffer_contents.length(); i++) data_buffer_->Add(buffer_contents[i]);

}

void Monitor::PrintMsg(std::string msg) {
    ISRMsgHandlerInstance_->QueueOutputMsg(msg);
}

void Monitor::PrintErrorMsg(std::string msg) {
    PrintMsg("\r\n\r ? >>" + msg + "<<");
}

Monitor* Monitor::GetMonitor() {
    if (!MonitorInstance_) MonitorInstance_ = new Monitor;
    return MonitorInstance_;
}
