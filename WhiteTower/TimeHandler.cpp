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

/*
    Function: TickSec
    Brief: Ticks the internal time second value, rolling over if necessary
*/
void TimeHandler::TickSec() {
    current_time_.tenth_sec_ = 0;
    if (++current_time_.sec_ > MAX_SEC) TickMin();
}

/*
    Function: TickMin
    Brief: Ticks the internal time minute value, rolling over if necessary
*/
void TimeHandler::TickMin() {
    current_time_.sec_ = 0;
    if (++current_time_.min_ > MAX_MIN) TickHour();
}

/*
    Function: TickHour
    Brief: Ticks the internal time hour value, rolling over if necessary
*/
void TimeHandler::TickHour() {
    current_time_.min_ = 0;
    if (++current_time_.hour_ > MAX_HOUR) TickDay();
}

/*
    Function: TickDay
    Brief: Ticks the internal time day value, rolling over if necessary
*/
void TimeHandler::TickDay() {
    current_time_.hour_ = 0;
    int8_t days_in_month = month_numbers[current_time_.month_];
    if (days_in_month == -1) days_in_month = (leap_year_ ? FEB_LEAP_YEAR_LEN : FEB_NORM_YEAR_LEN);
    if (++current_time_.day_ > days_in_month) TickMonth();
}

/*
    Function: TickMonth
    Brief: Ticks the internal time month value, rolling over if necessary
*/
void TimeHandler::TickMonth() {
    current_time_.day_ = 1;
    if (++current_time_.month_ > MAX_MONTH) TickYear();
}

/*
    Function: TickYear
    Brief: Ticks the internal time year value, rolling over to 0 if necessary
*/
void TimeHandler::TickYear() {
    current_time_.month_ = 0;
    if (++current_time_.year_ > MAX_YEAR) current_time_.year_ = 0;
    leap_year_ = CheckLeapYear(current_time_.year_);
}

/*
    Function: TickYear
    Input: input_year: Year value to check as leap year or not
    Output: boolean: True if the year is a leap year, false if not
    Brief: Checks if the year is a leap year (Ignoring the fact that leap years were invented in 1582)
*/
bool TimeHandler::CheckLeapYear(uint16_t input_year) const {
    if (input_year % 4 == 0 && ((input_year % 100 != 0) || (input_year % 400 == 0))) return true;
    else return false;  
}

/*
    Function: CheckAlarm
    Brief: Checks if there is an alarm active and if so checks if it has triggered.
           If it has, the proper messages are printed and the alarm is reset. If data
           was currently in the input command character buffer, it is reprinted to the
           screen afterwards.
*/
void TimeHandler::CheckAlarm() {
    if (alarm_.is_active && (current_time_.smh_ == alarm_.alarm_time)) {
        // '\a' is the BEL (bell) character, which makes a noise if possible
        MonitorInstance_->PrintMsg("\n\r\a* ALARM * " + CreateSMHStr(alarm_.alarm_time) + " *" + NEW_LINE);
        MonitorInstance_->RePrintOutputBuffer();
        alarm_ = zero_alarm;
    }
}

/*
    Function: FutureTime
    Input: offset: Time structure that is added to current time to generate new time
    Brief: Used by the alarm command to calculate the relative time of the alarm.
           Note: The offset time is assumed to be valid
*/
void TimeHandler::FutureTime(smh_t &offset) {
    // Sec
    offset.sec += current_time_.sec_;
    if (offset.sec > MAX_SEC) {
        offset.sec -= (MAX_SEC + 1);
        offset.min++;
    }

    // Min
    offset.min += current_time_.min_;
    if (offset.min > MAX_MIN) {
        offset.min -= (MAX_MIN + 1);
        offset.hour++;
    }

    // Hour
    offset.hour += current_time_.hour_;
        if (offset.hour > MAX_HOUR) {
        offset.hour -= (MAX_HOUR + 1);
        // Not concerned about day rolling over
    }
}

/*
    Function: CreateSMHStr
    Input: smh: Seconds-Minutes-Hours structure used to create string
    Output: msg: String created based on the input string
    Brief: Helper function to convert a SMH structure into its lexical equivalent string for output
*/
std::string TimeHandler::CreateSMHStr(smh_t &smh) const {
    std::string msg = LexicalIntToString(smh.hour, 2) + ":" // 2 is the max width of seconds
                    + LexicalIntToString(smh.min,  2) + ":" // 2 is the max width of minutes
                    + LexicalIntToString(smh.sec,  2);      // 2 is the max width of hours

    return msg;
}

/*
    Function: CreateDMYStr
    Input: dmy: Day-Month-Year structure used to create string
    Output: msg: String created based on the input string
    Brief: Helper function to convert a dmy structure into its lexical equivalent string for output
*/
std::string TimeHandler::CreateDMYStr(dmy_t &dmy) const {
    std::string msg = LexicalIntToString(dmy.day, 2) + "-" // 2 is the max width of day
                    + months_str[dmy.month] + "-"
                    + LexicalIntToString(dmy.year, 4);     // 4 is the max width of year

    return msg;
}

/*
    Function: LexicalIntToString
    Input: input_int: Input number to convert to integer
           desired_len: Length of input 
    Output: return_val: String created from the input integer
    Brief: Helper function to convert an interger into its string equivalent, with zero
           padding to a desired length.
*/
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

/*
    Function: SingletonGrab
    Brief: Setup function for the Monitor to grab its required singleton pointers.
           Called from main.cpp at startup.
*/
void TimeHandler::SingletonGrab() {
    MonitorInstance_ = Monitor::GetMonitor();
}

/*
    Function: TimeHandler
    Brief: Constructor for the TimeHandler class. Initailizes current time, alarm, and 
           leap_year_ to zero values. Also initializes date to default date and 
*/
TimeHandler::TimeHandler() : current_time_(ti_time_t()),
                             alarm_(zero_alarm),
                             leap_year_(false) {
    current_time_.dmy_ = default_date;
}

/*
    Function: SetTime
    Input: new_smh: Time structure to set the time to
    Output: Boolean: Failure indicator -> true if no error and false if there is an error
    Brief: Set function for setting new time, also performs error checking
*/
bool TimeHandler::SetTime(smh_t &new_smh) {
    if (!CheckValidTime(new_smh)) return false;
    current_time_.tenth_sec_ = 0;
    current_time_.smh_ = new_smh;
    return true;
}

/*
    Function: SetDate
    Input: new_dmy: Date structure to set the date to
    Output: Boolean: Failure indicator -> true if no error and false if there is an error
    Brief: Set function for setting new date, also performs error checking
*/
bool TimeHandler::SetDate(dmy_t &new_dmy) {
    if (!CheckValidDate(new_dmy)) return false;
    current_time_.dmy_ = new_dmy;
    leap_year_ = CheckLeapYear(current_time_.year_);
    return true;
}

/*
    Function: SetAlarm
    Input: new_alarm: Alarm structure to set the alarm to
    Output: Boolean: Failure indicator -> true if no error and false if there is an error
    Brief: Set function for setting new date, also performs error checking
           Note: the alarm structure is assumed to contain the offset time, not the actual alarm time
*/
void TimeHandler::SetAlarm(alarm_t &new_alarm) {
    FutureTime(new_alarm.alarm_time);
    alarm_ = new_alarm;
    if (alarm_.is_active) PrintCurrentAlarm();
    else MonitorInstance_->PrintMsg("\r\nAlarm cleared");
}

/*
    Function: TickTenthSec
    Brief: Ticks the internal time tength second value, rolling over if necessary
*/
void TimeHandler::TickTenthSec() {
    if (++current_time_.tenth_sec_ > MAX_TENTH_SEC) TickSec();
    CheckAlarm();
}

/*
    Function: CheckValidTime
    Input: input_smh: Time structure to check validity of
    Output: Boolean: Failure indicator -> true if no error and false if there is an error
    Brief: Checks the validity of a time structure by performing bounds checks on the values
*/
bool TimeHandler::CheckValidTime(smh_t &input_smh) const {
    if ( (input_smh.sec > MAX_SEC) ||
         (input_smh.min > MAX_MIN) ||
         (input_smh.hour > MAX_HOUR) ) return false;

    return true;
}

/*
    Function: CheckValidDate
    Input: input_dmy: Date structure to check validity of
    Output: Boolean: Failure indicator -> true if no error and false if there is an error
    Brief: Checks the validity of a date structure by performing bounds checks on the values,
           checking leap years as well
*/
bool TimeHandler::CheckValidDate(dmy_t &input_dmy) const {
    // Technically months can't be wrong at this stage given that they are always fed 
    // in as numbers and the string literals have already been checked for error
    if ( (input_dmy.year  > MAX_YEAR) ||
         (input_dmy.month > MAX_MONTH) ) return false;

    int8_t days_in_month = month_numbers[input_dmy.month];
    if (days_in_month == -1) {
        days_in_month = (CheckLeapYear(input_dmy.year) ? FEB_LEAP_YEAR_LEN : FEB_NORM_YEAR_LEN);
    }
    if ((input_dmy.day == 0) || (input_dmy.day > days_in_month)) return false;

    return true;
}

/*
    Function: CheckAlarmActive
    Output: Boolean: True if alarm is active and false if not
    Brief: Checks if there is currently an alarm active
*/
bool TimeHandler::CheckAlarmActive() const {
    return alarm_.is_active;
}

/*
    Function: PrintCurrentTime
    Brief: Helper function to print the current date
*/
void TimeHandler::PrintCurrentTime() {
    MonitorInstance_->PrintMsg("\n\r" + CreateSMHStr(current_time_.smh_));
}

/*
    Function: PrintCurrentDate
    Brief: Helper function to print the current date
*/
void TimeHandler::PrintCurrentDate() {
    MonitorInstance_->PrintMsg("\n\r" + CreateDMYStr(current_time_.dmy_));
}

/*
    Function: PrintCurrentAlarm
    Brief: Helper function to print the current date
*/
void TimeHandler::PrintCurrentAlarm() {
    if (alarm_.is_active) MonitorInstance_->PrintMsg("\n\r" + CreateSMHStr(alarm_.alarm_time));
    else MonitorInstance_->PrintErrorMsg("No Alarm Currently Set");
}

/*
    Function: PrintCurrentAlarm
    Output: current_time: Current time stored in the TimeHandler
    Brief: Get function to access the current time
*/
ti_time_t TimeHandler::GetCurrentTime() {
    return current_time_;
}

/*
    Function: GetTimeHandler
    Output: CommandCenterInstance_: Pointer to the TimeHandler Singleton
    Brief: Get function for the TimeHandler singleton
*/
TimeHandler* TimeHandler::GetTimeHandler() {
    if (!TimeHandlerInstance_) TimeHandlerInstance_ = new TimeHandler;
    return TimeHandlerInstance_;
}
