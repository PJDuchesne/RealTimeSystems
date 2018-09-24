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
TimeHandler *TimeHandler::TimeHandlerInstance_ = 0;

void TimeHandler::TickSec() {
    current_time_.tenth_sec_ = 0;
    if (++current_time_.sec_ > MAX_SEC) TickMin();
}

void TimeHandler::TickMin() {
    current_time_.sec_ = 0;
    if (++current_time_.min_ > MAX_MIN) TickHour();
}

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

void TimeHandler::TickMonth() {
    current_time_.day_ = 1;
    if (++current_time_.month_ > MAX_MONTH) TickYear();
}

void TimeHandler::TickYear() {
    current_time_.month_ = 0;
    if (++current_time_.year_ > MAX_YEAR) current_time_.year_ = 0;
    leap_year_ = CheckLeapYear(current_time_.year_);
}

bool TimeHandler::CheckLeapYear(uint16_t input_year) const {
    if (input_year % 4 == 0 && ((input_year % 100 != 0) || (input_year % 400 == 0))) return true;
    else return false;  
}

void TimeHandler::CheckAlarm() {
    if (alarm_.is_active_ && (current_time_.smh_ == alarm_.alarm_time_)) {
        // '\a' is the BEL (bell) character, which makes a noise if possible
        MonitorInstance_->PrintMsg("\n\r\a* ALARM * " + CreateSMHStr(alarm_.alarm_time_) + " *");
        MonitorInstance_->PrintNewLine();
        alarm_.is_active_ = false;
    }
}

// Always used in reference to current time
// Assumes that offset is a valid
smh_t TimeHandler::FutureTime(smh_t &offset) const {
    smh_t new_time = {.sec = 0, .min = 0, .hour = 0};

    // Sec
    new_time.sec = current_time_.sec_ + offset.sec;
    if (new_time.sec > MAX_SEC) {
        new_time.sec -= (MAX_SEC + 1);
        new_time.min++;
    }

    // Min
    new_time.min += current_time_.min_ + offset.min;
    if (new_time.min > MAX_MIN) {
        new_time.min -= (MAX_MIN + 1);
        new_time.hour++;
    }

    // Hour
    new_time.hour += current_time_.hour_ + offset.hour;
        if (new_time.hour > MAX_HOUR) {
        new_time.hour -= (MAX_HOUR + 1);
        // Not concerned about day rolling over
    }

    return new_time;
}

std::string TimeHandler::CreateSMHStr(smh_t &smh) const {
    std::string msg = LexicalIntToString(smh.hour, 2) + ":" 
                    + LexicalIntToString(smh.min,  2) + ":"
                    + LexicalIntToString(smh.sec,  2);

    return msg;
}

std::string TimeHandler::CreateDMYStr(dmy_t &dmy) const {
    std::string msg = LexicalIntToString(dmy.day, 2) + "-" 
                    + months_str[dmy.month] + "-"
                    + LexicalIntToString(dmy.year, 4);

    return msg;
}

std::string TimeHandler::LexicalIntToString(uint16_t input_int, uint8_t desired_len) const {
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

void TimeHandler::SingletonGrab() {
    MonitorInstance_ = Monitor::GetMonitor();
}

TimeHandler::TimeHandler() : current_time_(ti_time_t()),
                             alarm_(alarm_t()),
                             leap_year_(false) {
    current_time_.dmy_ = default_date;
    MonitorInstance_ = MonitorInstance_;
}

bool TimeHandler::SetTime(smh_t &new_smh) {
    if (!CheckValidTime(new_smh)) return false;
    current_time_.tenth_sec_ = 0;
    current_time_.smh_ = new_smh;
    return true;
}

bool TimeHandler::SetDate(dmy_t &new_dmy) {
    if (!CheckValidDate(new_dmy)) return false;
    current_time_.dmy_ = new_dmy;
    leap_year_ = CheckLeapYear(current_time_.year_);
    return true;
}

bool TimeHandler::SetAlarm(smh_t &new_alarm_time) {
    // Guaranteed by FutureTime() to be valid time, but checking anyway for consistency
    if (!CheckValidTime(new_alarm_time)) return false;
    alarm_.is_active_ = true;
    alarm_.alarm_time_ = FutureTime(new_alarm_time);
    PrintCurrentAlarm();
    return true;
}

void TimeHandler::ClearAlarm() {
    alarm_.is_active_ = false;
}

void TimeHandler::TickTenthSec() {
    if (++current_time_.tenth_sec_ > MAX_TENTH_SEC) TickSec();
    CheckAlarm();
}

bool TimeHandler::CheckValidTime(smh_t &input_smh) const {
    if ( (input_smh.sec > MAX_SEC) ||
         (input_smh.min > MAX_MIN) ||
         (input_smh.hour > MAX_HOUR) ) return false;

    return true;
}

bool TimeHandler::CheckValidDate(dmy_t &input_dmy) const {
    // Technically months can't be wrong at this stage given that they are always fed 
    // in as numbers and the string literals have already been checked for error
    if ( (input_dmy.year  > MAX_YEAR) ||
         (input_dmy.month > MAX_MONTH) ) return false;

    int8_t days_in_month = month_numbers[input_dmy.month];
    if (days_in_month == -1) days_in_month = (CheckLeapYear(input_dmy.year) ? 29 : 28);
    if ((input_dmy.day == 0) || (input_dmy.day > days_in_month)) return false;

    return true;
}

bool TimeHandler::CheckAlarmActive() const {
    return alarm_.is_active_;
}

void TimeHandler::PrintCurrentTime() {
    MonitorInstance_->PrintMsg("\n\r" + CreateSMHStr(current_time_.smh_));
}

void TimeHandler::PrintCurrentDate() {
    MonitorInstance_->PrintMsg("\n\r" + CreateDMYStr(current_time_.dmy_));
}

void TimeHandler::PrintCurrentAlarm() {
    if (alarm_.is_active_) MonitorInstance_->PrintMsg("\n\r" + CreateSMHStr(alarm_.alarm_time_));
    else MonitorInstance_->PrintErrorMsg("No Alarm Currently Set");
}

ti_time_t TimeHandler::GetCurrentTime() {
    return current_time_;
}

TimeHandler* TimeHandler::GetTimeHandler() {
    if (!TimeHandlerInstance_) TimeHandlerInstance_ = new TimeHandler;
    return TimeHandlerInstance_;
}
