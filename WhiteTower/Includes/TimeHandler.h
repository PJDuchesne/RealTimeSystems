#ifndef TimeHandler_H
#define TimeHandler_H

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
-> Name:  TimeHandler.h
-> Date: Sept 19, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "GlobalConfig.h"
#include "Monitor.h"
#include "TimeDefinitions.h"

// Forward Declaration
class Monitor;

class TimeHandler {
    private:
        static TimeHandler* TimeHandlerInstance_;
        ti_time_t current_time_;
        alarm_t alarm_;
        bool leap_year_;

        Monitor *MonitorInstance_;

        // Tick functions to increment time
        void TickSec();
        void TickMin();
        void TickHour();
        void TickDay();
        void TickMonth();
        void TickYear();

        // Internal Member Functions
        bool CheckLeapYear(uint16_t input_year) const;
        void CheckAlarm();

        smh_t FutureTime(smh_t &offset) const;

        std::string CreateSMHStr(smh_t &smh) const;
        std::string CreateDMYStr(dmy_t &dmy) const;
        std::string LexicalIntToString(uint16_t input_int, uint8_t desired_len) const;

    public:
        TimeHandler();
        void SingletonGrab();

        // Functions to handle commands
        bool SetTime(smh_t &new_smh);
        bool SetDate(dmy_t &new_dmy);
        bool SetAlarm(smh_t &alarm_time);
        void ClearAlarm();
        void TickTenthSec();

        // Internal Member functions
        bool CheckValidTime(smh_t &input_smh) const;
        bool CheckValidDate(dmy_t &input_dmy) const;
        bool CheckAlarmActive() const;

        void PrintCurrentTime();
        void PrintCurrentDate();
        void PrintCurrentAlarm();

        ti_time_t GetCurrentTime();

        static TimeHandler* GetTimeHandler();
};

#endif /* TimeHandler_H */
