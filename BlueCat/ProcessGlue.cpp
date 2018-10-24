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
    std::string DiagDisplay = "";
    int i = 0;
    int fetched_pid = -1;
    while (1) {
        i++;

        /* Test Terminate */
        if (i >= 2500000) {
            i = 0;
            std::cout << "Attempting to Terminate() DummpyProcess2\n";
            break;
        }
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
