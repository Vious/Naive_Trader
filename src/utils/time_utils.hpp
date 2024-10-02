#pragma once

#include <string>

// seconds
#define DAY_SECONDS 86400
#define MINUTE_SECONDS 60
#define HOUR_SECONDS 3600

// milliseconds
#define SECOND_MILLISECONDS 1000
#define MINUTE_MILLISECONDS 60000
#define HOUR_MILLISECONDS 3600000
#define DAY_MILLISECONDS 86400000

namespace naiveTrader
{

namespace TimeUtils
{

inline std::string dataTimeToString(time_t timestamp, const char* format = "%Y-%m-%d %H:%M:%S") {
    char buffer[64] = { 0 };
    struct tm* tmInfo = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), format, tmInfo);
    return std::string(buffer);
}

inline time_t makeDate(int year, int month, int day) {
    tm aTm;
    aTm.tm_year = year - 1900;
    aTm.tm_mon = month - 1;
    aTm.tm_mday = day;
    aTm.tm_hour = 0;
    aTm.tm_min = 0;
    aTm.tm_sec = 0;
    aTm.tm_isdst = 0;  /* Daylight saving time (DST), AKA summer time */
    return mktime(&aTm);
}

inline time_t makeDate(uint32_t date) {
    int year, month, day;
    year = date / 10000;
    month = date % 10000 / 100;
    day = date % 100;
    return makeDate(year, month, day);
}

inline time_t makeTime(uint32_t utime) {
    int hour, min, sec;
    hour = utime / 10000;
    min = utime % 10000 / 100;
    sec = utime % 100;
    return hour * HOUR_SECONDS + min * MINUTE_SECONDS + sec;
}

// inline time_t makeTime(const char *ctime) {
//     int timeValue[3] = {0};
//     char tmp[3] = {'\0'};

// }

} // namespace TimeUtils



} // namespace naiveTrader
