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

void MonitorProcess() {
    std::cout << "[ProcessesGlue] Entering Monitor!\n";
    // Pass Control to Monitor
    Monitor::GetMonitor()->CentralLoop();
}

void DummpyProcess2() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess2!\n";

    static int Iteration = 1;

    std::string DiagDisplay = "";
    bool TermFlag = false;
    int i = 0;
    int fetched_pid = -1;
    while (1) {
        i++;
        if (i >= 1000000) {
            i = 0;
            std::cout << "[DummpyProcess2] PRE STATE\n";
            OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
            std::cout << DiagDisplay;
            switch (Iteration) {
                case 1:
                    std::cout << "\nAttempting to NICE DummpyProcess2 (5 to 4)\n";
                    PNice(P_FOUR);
                    Iteration++;
                    break;
                case 2:
                    std::cout << "\nAttempting to NICE DummpyProcess2 (4 to 3)\n";
                    PNice(P_THREE);
                    Iteration++;
                    break;
                case 3:
                    std::cout << "\nAttempting to terminate DummpyProcess2\n";
                    TermFlag = true;
                    break;
                default:
                    // Redtext immediately
                    assert (1 == 0);
                    break;
            }
            if (TermFlag) break;
            std::cout << "[DummpyProcess2] Post STATE\n";
            OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
            std::cout << DiagDisplay;
        }
        /* Test Terminate */
        /* Test Nice */
        // if (i >= 2500000) {
        //     i = 0;
        //     // std::cout << "Attempting to NICE()\n";
        //     PNice(P_ONE);
        //     // OperatingSystem::GetOperatingSystem()->DiagnosticsDisplay(DiagDisplay);
        //     // std::cout << DiagDisplay;
        // }
        /* Test PGetID */
        // if (i >= 2500000) {
        //     i = 0;
        //     fetched_pid = PGetID();
        //     std::cout << "[DummpyProcess2 - 456] >>" << fetched_pid << "<<\n";
        // }
    }
}

void DummpyProcess3() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess3!\n";
    while (1) {

    }
}
