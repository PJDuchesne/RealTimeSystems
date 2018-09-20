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

const std::string months_str[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
                                     "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

const int month_numbers[12] = { 31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

typedef struct ti_time {
  union {
    struct {
      uint8_t tenth_sec_; // 0-9
      uint8_t sec_;       // 0-59
      uint8_t min_;       // 0-59
      uint8_t hour_;      // 0-23
      uint8_t day_;       // 0-31
      uint8_t month_;     // 0-11
      uint16_t year_;     // 0-9999
    };
    uint64_t raw_time_;
  };
} ti_time_t;

class TimeHandler {
    private:
        static TimeHandler* TimeHandlerInstance_;
        ti_time_t current_time_;
        bool leap_year_;
        // Array of alarms

    public:
        TimeHandler() : current_time_(ti_time_t()),
                             leap_year_(false) {}

        // Functions to handle commands
        void SetTime(uint8_t &sec, uint8_t &min, uint8_t &hour);
        void SetDate(uint8_t &day, uint8_t &month, uint16_t &year);
        void SetAlarm(ti_time_t &alarm_time);

        // Helper functions to be used to increment time
        void TickTenthSec();
        void TickSec();
        void TickMin();
        void TickHour();
        void TickDay();
        void TickMonth();
        void TickYear();

        void CheckLeapYear();

        void CheckAlarms();

        static TimeHandler* GetTimeHandler();
};

#endif /* TimeHandler_H */
