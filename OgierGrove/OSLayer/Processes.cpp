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
-> Name:  Processes.cpp
-> Date: Oct 21, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/Processes.h"

/*
    Function: MonitorProcessEntry 
    Brief: Provides an easy to access entry point for the Operating system to access the monitor
*/
void MonitorProcessEntry() {
    // Pass Control to Monitor
    Monitor::GetMonitor()->CentralLoop();
}

/*
    Function: EndlessProcess 
    Brief: Test function that simply loops
*/
void EndlessProcess() {
    while (1) {

    }
}

/*
    Function: ShortProcess 
    Brief: Test function that loops for a 'short' time before terminating. Also binds a mailbox
           and sends message updates to the monitor about its status
*/
void ShortProcess() {
    // Get ID so a unique mailbox can be bound
    uint32_t srq_q = PGetID();
    uint32_t counter;

    // Bind a mailbox
    PBind(srq_q, BIG_LETTER);

    // Send a message to indicate the process has started!
    char tmpArray[] = "[ShortProcess] Starting!";
    counter = 0;
    while (tmpArray[counter] != '\0') { counter++; }
    PSend(srq_q, MONITOR_MB, tmpArray, counter-1);

    uint32_t i = 0;
    while (i++ < SHORT_WAIT) {

    }

    char tmpArray2[] = "[ShortProcess] Terminating!";
    counter = 0;
    while (tmpArray2[counter] != '\0') { counter++; }
    PSend(srq_q, MONITOR_MB, tmpArray2, counter-1);
}

/*
    Function: LongProcess 
    Brief: Test function that loops for a 'long' time before terminating. Also binds a mailbox
           and sends message updates to the monitor about its status
*/
void LongProcess() {
    // Get ID so a unique mailbox can be bound
    uint32_t srq_q = PGetID();
    uint32_t counter;

    // Bind a mailbox
    PBind(srq_q, BIG_LETTER);

    // Send a message to indicate the process has started!
    char tmpArray[] = "[LongProcess] Starting!";
    counter = 0;
    while (tmpArray[counter] != '\0') { counter++; }
    PSend(srq_q, MONITOR_MB, tmpArray, counter-1);

    uint32_t i = 0;
    while (i++ < LONG_WAIT) {

    }

    char tmpArray2[] = "[LongProcess] Terminating!";
    counter = 0;
    while (tmpArray2[counter] != '\0') { counter++; }
    PSend(srq_q, MONITOR_MB, tmpArray2, counter-1);
}

/*
    Function: NiceTestProcess 
    Brief: Test function that utilizes the NICE command to change its own priority and
           at specifc intervals. After two jumps the process terminates. This also binds
           a mailbox and sends message updates to the monitor about its status
*/
void NiceTestProcess() {
    // Get ID so a unique mailbox can be bound
    uint32_t srq_q = PGetID();
    uint32_t counter;

    char tmpMsg1[] = "Attempting to NICE DummpyProcess2 (5 to 4)!";
    char tmpMsg2[] = "Attempting to NICE DummpyProcess2 (4 to 3)";
    char tmpMsg3[] = "Attempting to terminate DummpyProcess2!";

    // Bind a mailbox
    PBind(srq_q, BIG_LETTER);

    static int Iteration = 1;

    std::string DiagDisplay = "";
    bool end_flag = false;
    int i = 0;
    while (1) {
        i++;
        if (i >= LONG_WAIT) {
            i = 0;
            switch (Iteration) {
                case 1:
                    counter = 0;
                    while (tmpMsg1[counter] != '\0') { counter++; }
                    PSend(srq_q, MONITOR_MB, tmpMsg1, counter);
                    PNice(P_FOUR);
                    break;
                case 2:
                    counter = 0;
                    while (tmpMsg2[counter] != '\0') { counter++; }
                    PSend(srq_q, MONITOR_MB, tmpMsg2, counter);
                    PNice(P_THREE);
                    break;
                case 3:
                    counter = 0;
                    while (tmpMsg3[counter] != '\0') { counter++; }
                    PSend(srq_q, MONITOR_MB, tmpMsg3, counter);
                    end_flag = true;
                    break;
                default:
                    break;
            }
            if (end_flag) break;
            Iteration++;
        }
    }
}

/*
    Function: SendProcess 
    Brief: Test function that sends 3 messages in a row to a hardcoded mailbox
*/
void SendProcess() {
    // Mailboxes to test
    uint32_t my_mailbox = PGetID(); // 70
    uint8_t recv_mailbox = 80; // Hardcoded for the sake of testing

    // Buy a mailbox!
    PBind(my_mailbox, BIG_LETTER);

    // Add a noticable delay
    uint32_t counter;
    counter = 0;
    while (counter < 3*LONG_WAIT) counter++;

    char tmpArray[] = { "helloworld" };

    PSend(my_mailbox, recv_mailbox, &tmpArray, 10);      // helloworld
    PSend(my_mailbox, recv_mailbox, &tmpArray, 5);       // hello
    PSend(my_mailbox, recv_mailbox, &(tmpArray[5]), 5);  // world

    while (1) {

    }
}

/*
    Function: RecvProcess 
    Brief: Test function that receives 4 messages using the blocking option
*/
void RecvProcess() {
    uint32_t my_mailbox = PGetID(); // 80

    // Buy a mailbox!
    PBind(my_mailbox, BIG_LETTER);

    uint32_t counter = 0;
    while (counter < SHORT_WAIT) counter++;

    char tmpArray[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;

    // Get first message, send result to monitor!
    PRecv(srq_q, my_mailbox, tmpArray, msg_len);
    PSend(my_mailbox, MONITOR_MB, tmpArray, msg_len);

    // Get second message, send result to monitor!
    PRecv(srq_q, my_mailbox, tmpArray, msg_len);
    PSend(my_mailbox, MONITOR_MB, tmpArray, msg_len);

    // Get third message, send result to monitor!
    PRecv(srq_q, my_mailbox, tmpArray, msg_len);
    PSend(my_mailbox, MONITOR_MB, tmpArray, msg_len);

    // Attempt to get 4th message and go to sleep
    PRecv(srq_q, my_mailbox, tmpArray, msg_len);

    while (1) {

    }
}

/*
    Function: SendConstantRate 
    Brief: Test function that sends messages at a constant rate
*/
void SendConstantRate() {
    // Mailboxes to test
    uint32_t my_mailbox = PGetID(); // 25
    uint8_t recv_mailbox = 50; // Hardcoded for the sake of testing
    uint32_t counter;

    // Buy a mailbox!
    PBind(my_mailbox, BIG_LETTER);

    // All messages length 6 for simplicity
    char **msgArray = (char *[]){ "Foobar", "Barfoo", "Raboof", "Oofrab" };
    uint8_t arrayCnt = 0;

    while (1) {
        // Add a noticable delay
        counter = 0;
        while (counter < LONG_WAIT) counter++;

        // Print message
        PSend(my_mailbox, recv_mailbox, msgArray[arrayCnt++], 6);

        // Reset loop or go forever
        // if (arrayCnt > 3) arrayCnt = 0;
        if (arrayCnt > 3) break;
    }
}

/*
    Function: RecvNonBlockingProcess 
    Brief: Test function that receives messages using the non-blocking flag
*/
void RecvNonBlockingProcess() {
    uint32_t my_mailbox = PGetID(); // 50

    // Buy a mailbox!
    PBind(my_mailbox, BIG_LETTER);

    char tmpArray[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;

    while (1) {
        // Constantly poll for messages
        while (PRecv(srq_q, my_mailbox, tmpArray, msg_len, false) == false) {}

        // Pass any results on to the monitor
        PSend(my_mailbox, MONITOR_MB, tmpArray, msg_len);
    }
}

/*
    Function: ReverseString 
    Brief: Test function that reverses the string send to its mailbox and outputs to the monitor
*/
void ReverseString() {
    // Bind queue
    PBind(REVERSE_MSG_MB, BIG_LETTER);

    char tmpArray[256];
    char reversedMsg[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;

    int tmp = 0;
    while (tmp++ < 3*LONG_WAIT) {}

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
}

/*
    Function: IdleProcess 
    Brief: Default idle process that provides the user with a spinner
*/
void IdleProcess() {
    // Get ISRMsgHandler
    ISRMsgHandler *ISRMsgHandlerInstance_ = ISRMsgHandler::GetISRMsgHandler();

    // Clear screen
    ISRMsgHandlerInstance_->QueueOutputMsg(CLEAR_SCREEN, UART0);

    // Set a spinner going
    std::string spinner_parts[] = { "|", "/", "-", "\\" };

    uint32_t delay_cnt;
    uint8_t spinner_i = 0;
    while (1) {
        delay_cnt = 0;
        while (delay_cnt++ < LONG_WAIT) {}

        ISRMsgHandlerInstance_->QueueOutputMsg("\e[1;80H" + spinner_parts[spinner_i++], UART0);

        if (spinner_i > 3) spinner_i = 0;
    }
}
