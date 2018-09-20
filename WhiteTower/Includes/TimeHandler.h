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

const int8_t month_numbers[12] = { 31, -1, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// smh -> second, min, hour
typedef struct smh {
  uint8_t sec;  // 0-59
  uint8_t min;  // 0-59
  uint8_t hour; // 0-23
  bool operator==(const smh &r) {
    return ((this->sec == r.sec) && (this->min == r.min) && (this->hour == r.hour));
  }
} smh_t;

// bool operator==(const smh &l, const smh &r) {
//     return ((l.sec == r.sec) && (l.min == r.min) && (l.hour == r.hour));
// }

// dmy -> day, month, year
typedef struct dmy {
  uint8_t day;   // 0-59
  uint8_t month; // 0-59
  uint16_t year; // 0-23
} dmy_t;

// Trailing underscore names because this is only used a member variable
typedef struct ti_time {
  union {
    struct {
      uint8_t tenth_sec_; // 0-9

      // Anonymous unions to allow direct access to lower levels
      union {
        smh_t smh_;
        struct {
           uint8_t sec_;
           uint8_t min_;
           uint8_t hour_; 
        };
      };

      union {
        dmy_t dmy_;
        struct {
           uint8_t day_;
           uint8_t month_;
           uint8_t year_; 
        };
      };

    };
    uint64_t raw_time_;
  };
} ti_time_t;

typedef struct alarm {
  ti_time_t alarm_time_;
  bool is_active_;
} alarm_t;

class TimeHandler {
    private:
        static TimeHandler* TimeHandlerInstance_;
        ti_time_t current_time_;
        alarm_t alarm_;
        bool leap_year_;

    public:
        TimeHandler() : current_time_(ti_time_t()),
                        alarm_(alarm_t()),
                        leap_year_(false) {}

        // Functions to handle commands
        void SetTime(smh_t &new_smh);
        void SetDate(dmy_t &new_dmy);
        void SetAlarm(smh_t &alarm_time);

        // Helper functions to be used to increment time
        void TickTenthSec();
        void TickSec();
        void TickMin();
        void TickHour();
        void TickDay();
        void TickMonth();
        void TickYear();

        void CheckLeapYear();
        void CheckAlarm();

        std::string CreateSMHStr(smh_t &smh);
        std::string CreateDMYStr(dmy_t &dmy);

        std::string LexicalIntToString(uint16_t input_int, uint8_t desired_len);

        static TimeHandler* GetTimeHandler();
};

#endif /* TimeHandler_H */
