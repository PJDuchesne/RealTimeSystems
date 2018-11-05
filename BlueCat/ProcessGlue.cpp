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

#include <ISRLayer/Includes/GlobalConfig.h>
#include <OSLayer/Includes/OperatingSystem.h>
#include <OSLayer/Includes/PostOffice.h> // TODO: Remove

void MonitorProcess() {
    std::cout << "[ProcessesGlue] Entering Monitor!\n";

    // Bind queues for the monitor's message handler
    PBind(MONITOR_MB, BIG_LETTER);
    PBind(ISR_MSG_HANDLER_MB, ONE_CHAR, ISR_QUEUE_SIZE);

    // Pass Control to Monitor
    Monitor::GetMonitor()->CentralLoop();
}

// Currently configured as an overarching test for NICE, GETID, and TERMINATE
void DummpyProcess2() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess2!\n";

    while(1) {
        
    }

    // static int Iteration = 1;

    // std::string DiagDisplay = "";
    // bool TermFlag = false;
    // int i = 0;
    // int fetched_pid = -1;
    // while (1) {
    //     i++;
    //     if (i >= 1000000) {
    //         i = 0;
    //         std::cout << "[DummpyProcess2] PRE STATE\n";
    //         OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
    //         std::cout << DiagDisplay;
    //         switch (Iteration) {
    //             case 1:
    //                 std::cout << "\nAttempting to NICE DummpyProcess2 (5 to 4)\n";
    //                 PNice(P_FOUR);
    //                 break;
    //             case 2:
    //                 std::cout << "\nAttempting to NICE DummpyProcess2 (4 to 3)\n";
    //                 PNice(P_THREE);
    //                 break;
    //             case 3:
    //                 std::cout << "\nAttemtping to GETID DummyProcess2" << PGetID() << "\n";
    //                 break;
    //             case 4:
    //                 std::cout << "\nAttempting to terminate DummpyProcess2\n";
    //                 TermFlag = true;
    //                 break;
    //             default:
    //                 // Redtext immediately
    //                 assert (1 == 0);
    //                 break;
    //         }
    //         if (TermFlag) break;
    //         Iteration++;
    //         std::cout << "[DummpyProcess2] Post STATE\n";
    //         OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
    //         std::cout << DiagDisplay;
    //     }
    // }
}

void DummpyProcess3() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess3!\n";

    // Buy a mailbox!
    PBind(3, BIG_LETTER);

    // Add a noticable delay
    uint32_t counter = 0;
    while (counter < 1000000) counter++;
    std::cout << "[DummpyProcess3] FIRST CHECK\n";
    counter = 0;
    while (counter < 1000000) counter++;
    std::cout << "[DummpyProcess3] SECOND CHECK\n";

    char tmpArray[] = { 'h','e','l','l','o','w','o','r','l','d' };

    PSend(3, 4, &tmpArray, 10);      // helloworld
    PSend(3, 4, &tmpArray, 5);       // hello
    PSend(3, 4, &(tmpArray[5]), 5);  // world

    while (1) {

    }
}

void DummpyProcess4() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess4!\n";

    // Buy a mailbox!
    PBind(4, BIG_LETTER);

    uint32_t counter = 0;
    while (counter < 1000000) counter++;
    std::cout << "[DummpyProcess4] FIRST CHECK\n";

    char* tmpArray = new char[256];
    uint8_t srq_q = 0;
    uint32_t msg_len = 0;
    uint32_t return_value = 100;

    return_value = PRecv(srq_q, (uint8_t) 4, tmpArray, msg_len);
    assert(msg_len == 10);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";
    PRecv(srq_q, (uint8_t) 4, tmpArray, msg_len);
    assert(msg_len == 5);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";
    msg_len = 0;
    PRecv(srq_q, (uint8_t) 4, tmpArray, msg_len);
    assert(msg_len == 5);
    std::cout << "[DummpyProcess4] >>" << msg_len << "<<\n";

    delete[] tmpArray;

    while (1) {

    }
}
