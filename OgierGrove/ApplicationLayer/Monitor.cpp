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

/*
    Function: CheckMessageHandler
    Brief: Checks the ISR message queue and handles message if one is present,
       constantly polled from CentralLoop
*/
void Monitor::CheckMessageHandler() {
    static MsgType_t type = NONE;
    static char data = char();

    // Check the ISR message queue and handle the result
    // NOTE: This is a blocking process, but will trigger at ~10Hz
    ISRMsgHandlerInstance_->GetFromISRQueue(type, data);
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

    // Check mailbox from other processes for printing
    uint8_t src_q;
    uint32_t msg_len;
    static char msg_body[256];

    // If there is a message, print it
    if (PRecv(src_q, MONITOR_MB, &msg_body, msg_len, false)) {
        msg_body[msg_len] = '\0';
        PrintMsg(("\n[MSG FROM: " + std::to_string(src_q) + "] >>" + std::string(msg_body) + "<<"+NEW_LINE));

        // Then reprint output buffer!
        RePrintOutputBuffer();
    }
}

/*
    Function: HandleUART
    Input: char: Input character from UART message
    Brief: Handles a UART msg from the ISR queue. This function handles VT-100 codes, backspaces,
           enter to submit current data buffer, and generally filling the data buffer.
*/
void Monitor::HandleUART(char data) {
    static char single_char[1]; // Used to output a character
    static std::string command_string = ""; // Used to build command
    static int8_t escape_mode = -1;

    // VT-100 Code handling: If in escape mode, skip next two characters
    //      Escape mode is denoted by escape_mode equalling 0 or 1
    if (escape_mode != -1) {
        // However, if one of the skipped chacters is a number, skip an extra one
        if (data >= '0' && data <= '9') return;
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
            command_string = "";
            while (1) {
                single_char[0] = data_buffer_->Get();
                if (single_char[0]) command_string += single_char[0];
                else {
                    if (command_string != "") CommandCenterInstance_->HandleCommand(command_string);
                    break;
                }
            }
            PrintMsg(NEW_LINE);
            data_buffer_->Reset();
            break;

        case 0x1B: // Escape
            // Enter backslash mode, ignoring next two characters
            escape_mode = 0;
            break;

        default:  // All other characters, add to buffer if not already full
            if (!data_buffer_->Full()) {
                data_buffer_->Add((char *) &data);
                single_char[0] = data;
                PrintMsg(single_char);
            }
            break;
    }
}

/*
    Function: HandleSYSTICK
    Brief: Handles a SysTick ISR msg by simply incrementing the decisecond value in the TimeHandler
*/
void Monitor::HandleSYSTICK() {
    TimeHandlerInstance_->TickTenthSec();
}

/*
    Function: Monitor
    Brief: Constructor for the Monitor class, which initializes the data_buffer_ on startup
*/
Monitor::Monitor() {
    data_buffer_ = new RingBuffer<char>(DATA_BUFFER_SIZE);
}

/*
    Function: ~Monitor
    Brief: Destructor for the Monitor class, which deletes the data_buffer_ on shutdown
*/
Monitor::~Monitor() {
    delete data_buffer_;
}

/*
    Function: SingletonGrab
    Brief: Setup function for the Monitor to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void Monitor::SingletonGrab() {
    ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();
    CommandCenterInstance_ = CommandCenter::GetCommandCenter();
    TimeHandlerInstance_ = TimeHandler::GetTimeHandler();
}

/*
    Function: CentralLoop
    Brief: The central loop of the project. This function is handed control after main.cpp performs
           some setup functions. This clears the screen and then loops forever, polling the ISR message
           queue for more inputs to handle.
*/
void Monitor::CentralLoop() {
    // Bind monitor queue
    PBind(ISR_MSG_HANDLER_MB, ONE_CHAR, ISR_QUEUE_SIZE);
    PBind(MONITOR_MB, BIG_LETTER);

    // Reset all VT-100 settings (background, foreground, etc.)
    PrintMsg("\e[0m");

    // Clear the screen from previous sessions
    PrintMsg(CLEAR_SCREEN);
    PrintMsg(NEW_LINE);

    // Loop Forever
    while(1)
    {
        CheckMessageHandler();
    }
}

/*
    Function: RePrintOutputBuffer
    Brief: This function is used to reprint the current command buffer to screen after the user has been
           interrupted while typing by an alarm going off.
*/
void Monitor::RePrintOutputBuffer() {
    // Fetch contents of buffer
    std::string buffer_contents = "";
    while (!data_buffer_->Empty()) buffer_contents += data_buffer_->Get();

    // Print contents of buffer
    PrintMsg(buffer_contents);

    // Restore contents of buffer
    for (int i = 0; i < buffer_contents.length(); i++) data_buffer_->Add((char *) &buffer_contents[i]);

}

/*
    Function: PrintMsg
    Input:  msg: Message to print out through the UART transmit buffer
    Brief: Queues the message into the UART transmit buffer
*/
void Monitor::PrintMsg(std::string msg) {
    // ISRMsgHandlerInstance_->QueueOutputMsg(msg, UART0);

    if (msg.length() <= UART0_OUTPUT_DATA_BUFFER_SIZE / 2) {
        ISRMsgHandlerInstance_->QueueOutputMsg(msg, UART0);
        return;
    }

    bool end_flag = false;
    int16_t next_endline;
    std::string substr;
    while (1) {
        next_endline = msg.find_first_of("\n");
        if (next_endline == std::string::npos) {
            // On last line
            end_flag = true;
            substr = msg;
        }
        else {
            // Get substring
            substr = msg.substr(0, next_endline + 1); // +1 to include '\n'

            // Slice off that line
            msg = msg.substr(next_endline + 1); // +1 to exclude '\n'
        }
        ISRMsgHandlerInstance_->QueueOutputMsg(substr + "\r", UART0);
        if (end_flag) break;
    }
}

/*
    Function: PrintErrorMsg
    Input:  msg: Error Message to print out
    Brief: Prints out an error message in a standarized format for code readability elsewhere.
*/
void Monitor::PrintErrorMsg(std::string msg) {
    PrintMsg("\r\n\r ? >>" + msg + "<<");
}

/*
    Function: GetMonitor
    Output: MonitorInstance_: Pointer to the Monitor Singleton
    Brief: Get function for the Monitor singleton
*/
Monitor* Monitor::GetMonitor() {
    if (!MonitorInstance_) MonitorInstance_ = new Monitor;
    return MonitorInstance_;
}
