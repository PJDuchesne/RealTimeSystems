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
-> Name:  TimeHandler.cpp
-> Date: Sept 19, 2018  (Created)
-> Author: Paul Duchesne (B00332119)
-> Contact: pl332718@dal.ca
*/

#include "Includes/TimeHandler.h"

// Singleton Instance
TimeHandler *TimeHandler::TimeHandlerInstance_ = new TimeHandler;

void TimeHandler::SetTime(uint8_t &sec, uint8_t &min, uint8_t &hour) {
  // TODO: Do this in one line by knowing structure size
  current_time_.tenth_sec_ = 0;
  current_time_.sec_       = sec;
  current_time_.min_       = min;
  current_time_.hour_      = hour;
}

void TimeHandler::SetDate(uint8_t &day, uint8_t &month, uint16_t &year) {
  current_time_.day_   = day;
  current_time_.month_ = month;
  current_time_.year_  = year;
}

void TimeHandler::SetAlarm(ti_time_t &alarm_time) {
  // TODO: Add alarm to alarm array (vector?)

  // Alarms should be ordered within the array
  // Find correct position by just checking integer value of entire array
}

#define MAX_TENTH_SEC 9
void TimeHandler::TickTenthSec() {
  if (++current_time_.tenth_sec_ > MAX_TENTH_SEC) TickSec();
  
  CheckAlarms();
}

#define MAX_SEC 59
void TimeHandler::TickSec() {
  current_time_.tenth_sec_ = 0;
  if (++current_time_.sec_ > MAX_SEC) TickMin();
}

#define MAX_MIN 59
void TimeHandler::TickMin() {
  current_time_.sec_ = 0;
  if (++current_time_.min_ > MAX_MIN) TickMin();
}

#define MAX_HOUR 23
void TimeHandler::TickHour() {
  current_time_.min_ = 0;
  if (++current_time_.hour_ > MAX_HOUR) TickDay();
}

void TimeHandler::TickDay() {
  current_time_.hour_ = 0;
  int8_t days_in_month = month_numbers[current_time_.month_];
  if (days_in_month == -1) days_in_month = (leap_year_ ? 29 : 28);
  if (++current_time_.day_ > days_in_month) TickMonth();
}

#define MAX_MONTH 11
void TimeHandler::TickMonth() {
  current_time_.day_ = 0;
  if (++current_time_.month_ > MAX_MONTH) TickYear();
}

#define MAX_YEAR 9999
void TimeHandler::TickYear() {
  current_time_.month_ = 0;
  if (++current_time_.year_ > MAX_YEAR) current_time_.year_ = 0;
}

void TimeHandler::CheckAlarms() {}

TimeHandler* TimeHandler::GetTimeHandler() {
    // if (!TimeHandlerInstance) TimeHandlerInstance = new TimeHandler;
    return TimeHandlerInstance_;
}
