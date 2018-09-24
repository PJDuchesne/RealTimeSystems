#ifndef TimeDefinitions_H
#define TimeDefinitions_H

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
-> Name:  TimeDefinitions.h
-> Date: Sept 20, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#define MAX_TENTH_SEC   9
#define MAX_SEC         59
#define MAX_MIN         59
#define MAX_HOUR        23
#define MAX_MONTH       11
#define MAX_YEAR        9999

const std::string months_str[MAX_MONTH + 1] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
                                               "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

const int8_t month_numbers[MAX_MONTH + 1] = { 31, -1, 31, 30, 31, 30, 
                                              31, 31, 30, 31, 30, 31 };

// smh -> second, min, hour
typedef struct smh {
  uint8_t sec;  // 0-59
  uint8_t min;  // 0-59
  uint8_t hour; // 0-23
  bool operator==(const smh &r) {
    return ((this->sec == r.sec) && (this->min == r.min) && (this->hour == r.hour));
  }
} smh_t;

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
           uint16_t year_; 
        };
      };

    };
    uint64_t raw_time_;
  };
} ti_time_t;

typedef struct alarm {
  smh_t alarm_time_; // TODO: Change ti_time_t to smh_t
  bool is_active_;
} alarm_t;

const dmy_t default_date { .day = 1, .month = 8, .year = 2018 };

#endif /* TimeDefinitions_H */
