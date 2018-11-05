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
-> Name:  ProcessesGlue.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/Processes.h"

void MonitorProcessEntry() {
    std::cout << "[MonitorProcessEntry] Starting!\n";

    // Bind queues for the monitor's message handler
    PBind(MONITOR_MB, BIG_LETTER);
    PBind(ISR_MSG_HANDLER_MB, ONE_CHAR, ISR_QUEUE_SIZE);

    // Pass Control to Monitor
    Monitor::GetMonitor()->CentralLoop();
}

void EndlessProcess() {
    std::cout << "[EndlessProcess] Starting!\n";

    while (1) {

    }

    std::cout << "[EndlessProcess] Terminating!\n"; // Should never trigger
}

void ShortProcess() {
    std::cout << "[ShortProcess] Starting!\n";

    uint32_t i = 0;
    while (i++ < SHORT_WAIT) {

    }

    std::cout << "[ShortProcess] Terminating!\n";
}

void LongProcess() {
    std::cout << "[LongProcess] Starting!\n";

    uint32_t i = 0;
    while (i++ < LONG_WAIT) {

    }

    std::cout << "[LongProcess] Terminating!\n";
}

// Currently configured as an overarching test for NICE, GETID, and TERMINATE
void NiceTestProcess() {
    std::cout << "[NiceTestProcess] Starting\n";

    static int Iteration = 1;

    std::string DiagDisplay = "";
    bool TermFlag = false;
    int i = 0;
    int fetched_pid = -1;
    while (1) {
        i++;
        if (i >= SHORT_WAIT) {
            i = 0;
            std::cout << "[DummpyProcess2] PRE STATE\n";
            OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
            std::cout << DiagDisplay;
            switch (Iteration) {
                case 1:
                    std::cout << "\nAttempting to NICE DummpyProcess2 (5 to 4)\n";
                    PNice(P_FOUR);
                    break;
                case 2:
                    std::cout << "\nAttempting to NICE DummpyProcess2 (4 to 3)\n";
                    PNice(P_THREE);
                    break;
                case 3:
                    std::cout << "\nAttemtping to GETID DummyProcess2" << PGetID() << "\n";
                    break;
                case 4:
                    std::cout << "\nAttempting to terminate DummpyProcess2\n";
                    TermFlag = true;
                    break;
                default:
                    // Redtext immediately
                    assert (1 == 0);
                    break;
            }
            if (TermFlag) break;
            Iteration++;
            std::cout << "[DummpyProcess2] Post STATE\n";
            OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
            std::cout << DiagDisplay;
        }
    }
}

void DummpyProcess3() {
    std::cout << "[DummpyProcess3] Starting!\n";

    // Mailboxes to test
    uint8_t my_mailbox = 4;
    uint8_t recv_mailbox = 5;

    // Buy a mailbox!
    PBind(my_mailbox, BIG_LETTER);

    // Add a noticable delay
    uint32_t counter = 0;
    while (counter < SHORT_WAIT) counter++;
    std::cout << "[DummpyProcess3] FIRST CHECK\n";
    counter = 0;
    while (counter < SHORT_WAIT) counter++;
    std::cout << "[DummpyProcess3] SECOND CHECK\n";

    char tmpArray[] = { 'h','e','l','l','o','w','o','r','l','d' };

    PSend(my_mailbox, recv_mailbox, &tmpArray, 10);      // helloworld
    PSend(my_mailbox, recv_mailbox, &tmpArray, 5);       // hello
    PSend(my_mailbox, recv_mailbox, &(tmpArray[5]), 5);  // world

    PSend(my_mailbox, MONITOR_MB, &tmpArray, 10);      // helloworld
    PSend(my_mailbox, MONITOR_MB, &tmpArray, 5);       // hello
    PSend(my_mailbox, MONITOR_MB, &(tmpArray[5]), 5);  // world

    while (1) {

    }
}

void DummpyProcess4() {
    std::cout << "[DummpyProcess4] Starting!\n";

    uint8_t recv_mailbox = 5;

    // Buy a mailbox!
    PBind(recv_mailbox, BIG_LETTER);

    uint32_t counter = 0;
    while (counter < SHORT_WAIT) counter++;
    std::cout << "[DummpyProcess4] FIRST CHECK\n";

    char* tmpArray = new char[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;
    uint32_t return_value = 100;

    return_value = PRecv(srq_q, recv_mailbox, tmpArray, msg_len);
    assert(msg_len == 10);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";
    PRecv(srq_q, recv_mailbox, tmpArray, msg_len);
    assert(msg_len == 5);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";
    msg_len = 0;
    PRecv(srq_q, recv_mailbox, tmpArray, msg_len);
    assert(msg_len == 5);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";

    delete[] tmpArray;

    while (1) {

    }
}

void ReverseString() {
    std::cout << "[ReverseString] Starting!\n";

    // Bind queue
    PBind(REVERSE_MSG_MB, BIG_LETTER);

    char* tmpArray = new char[256];
    char* reversedMsg = new char[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;
    uint32_t return_value = 100;

    int tmp = 0;
    while (tmp++ < 3*LONG_WAIT) {}
    std::cout << "[ReverseString] Stalling!!\n";

    while (1) {
        // Get string to reverse
        PRecv(srq_q, REVERSE_MSG_MB, tmpArray, msg_len);

        // Reverse it
        for (int i = 0; i < msg_len; i++) {
            reversedMsg[i] = tmpArray[msg_len - 1 - i];
        }

        // Send it to the monitor for printing
        PSend(REVERSE_MSG_MB, MONITOR_MB, reversedMsg, msg_len);
    }

    delete[] tmpArray;
    delete[] reversedMsg;
}

void IdleProcess() {
    std::cout << "[IdleProcess] Starting!\n";

    // Get ISRMsgHandler
    ISRMsgHandler *ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();

    // Clear screen
    ISRMsgHandlerInstance_->QueueOutputMsg(CLEAR_SCREEN);

    // Set a spinner going
    std::string spinner_parts[] = { "|", "/", "-", "\\" };

    uint32_t delay_cnt;
    uint8_t spinner_i = 0;
    while (1) {
        delay_cnt = 0;
        while (delay_cnt++ < LONG_WAIT) {}

        ISRMsgHandlerInstance_->QueueOutputMsg("\e[1;80H" + spinner_parts[spinner_i++]);

        if (spinner_i >= 3) spinner_i = 0;
    }
}
