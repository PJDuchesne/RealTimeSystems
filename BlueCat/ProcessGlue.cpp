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

void MonitorProcess() {
    std::cout << "[ProcessesGlue] Entering Monitor!\n";
    // Pass Control to Monitor
    Monitor::GetMonitor()->CentralLoop();
}

void DummpyProcess2() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess2!\n";
    int i = 0;
    while (1) {
      i++;
    }
}

void DummpyProcess3() {
    std::cout << "[ProcessesGlue] Entering DummpyProcess3!\n";
    while (1) {
      
    }
}
