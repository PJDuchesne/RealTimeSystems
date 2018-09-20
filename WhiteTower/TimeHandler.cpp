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

void TimeHandler::SetTime(smh_t &new_smh) {
  current_time_.tenth_sec_ = 0;
  current_time_.smh_ = new_smh;

  // TODO: Alarm handling? Check with Hughes
}

void TimeHandler::SetDate(dmy_t &new_dmy) {
  current_time_.dmy_ = new_dmy;
}

void TimeHandler::SetAlarm(smh_t &alarm_time) {
  // TODO: Calculate new alarm time in the future, including rollover
}

#define MAX_TENTH_SEC 9
void TimeHandler::TickTenthSec() {
  if (++current_time_.tenth_sec_ > MAX_TENTH_SEC) TickSec();
  CheckAlarm();
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

void TimeHandler::CheckLeapYear() {
  uint16_t year = current_time_.year_;

  /* Gregorian calendar and the concept of Feb 29 was in 1582
  if (year <= 1582) {
    leap_year_ = false;
    return
  } 
  */

  if (year % 4 == 0 && ((year % 100 != 0) || (year % 400 == 0))) leap_year_ = true;
  else leap_year_ = false;  
}

// TODO: Ask Hughes about the interaction with the user changing the date while an alarm is active
void TimeHandler::CheckAlarm() {
  if (true) {
  // if (int(current_time_.smh_) == int(alarm_.alarm_time_.smh_)) {
    // '\a' is the BEL (bell) character, which makes a noise if possible
    Monitor::GetMonitor()->PrintMsg("\a* ALARM * " + CreateSMHStr(alarm_.alarm_time_.smh_) + " *");
    alarm_.is_active_ = false;
  }
}

std::string TimeHandler::CreateSMHStr(smh_t &smh) {
  std::string msg = LexicalIntToString(smh.sec, 2) + ":" 
                  + LexicalIntToString(smh.min, 2) + ":"
                  + LexicalIntToString(smh.hour, 2);

  return msg;
}

std::string TimeHandler::CreateDMYStr(dmy_t &dmy) {
  std::string msg = LexicalIntToString(dmy.day, 2) + "-" 
                  + months_str[dmy.month] + "-"
                  + LexicalIntToString(dmy.year, 4);

  return msg;
}

std::string TimeHandler::LexicalIntToString(uint16_t input_int, uint8_t desired_len) {
  std::string return_val = std::to_string(input_int);
  uint8_t str_len = return_val.length();

  if (str_len > desired_len) {
    std::cout << "[LexicalIntToString] ERROR: NUMBER TO LARGE FOR LENGTH\n";
    std::cout << "\t>>" << input_int << "<<  >>" << return_val << "<<\n";
    return "";
  }

  // Else it is either too small or the correct size
  for (int i = 0; i < (desired_len - str_len); i++) return_val = "0" + return_val;

  return return_val;
}

TimeHandler* TimeHandler::GetTimeHandler() {
    // if (!TimeHandlerInstance) TimeHandlerInstance = new TimeHandler;
    return TimeHandlerInstance_;
}
