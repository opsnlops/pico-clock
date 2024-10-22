
#include <time.h>

#include "hardware/rtc.h"

#include "utz/utz.h"
#include "utz/zones.h"

#include "time_utils.h"

time_t convert_utc_to_local(time_t utc_time, uoffset_t offset) {
    // Convert hours and minutes into seconds
    int32_t offset_seconds = (offset.hours * 3600) + (offset.minutes * 60);

    // Adjust the UTC time by the offset
    time_t local_time = utc_time + offset_seconds;

    return local_time;
}

udatetime_t convert_tm_to_udatetime(struct tm *timeinfo) {
    udatetime_t udt;

    // Convert tm to udate_t
    udt.date.year = (uint8_t)(timeinfo->tm_year - 100);  // Offset by 100 to match the 2000 offset
    udt.date.month = (uint8_t)(timeinfo->tm_mon + 1);    // Months are 0-11, udate expects 1-12
    udt.date.dayofmonth = (uint8_t)(timeinfo->tm_mday);
    udt.date.dayofweek = (uint8_t)(timeinfo->tm_wday == 0 ? 7 : timeinfo->tm_wday); // tm_wday: Sunday = 0, udate: Sunday = 7

    // Convert tm to utime_t
    udt.time.hour = (uint8_t)(timeinfo->tm_hour);
    udt.time.minute = (uint8_t)(timeinfo->tm_min);
    udt.time.second = (uint8_t)(timeinfo->tm_sec);

    return udt;
}


struct tm utc_tm_to_local_tm(struct tm utc_time, uzone_t timezone) {

    // Convert the tm struct to an udatetime_t
    udatetime_t dt = convert_tm_to_udatetime(&utc_time);

    // Get the offset for the current time
    uoffset_t offset;
    get_current_offset(&timezone, &dt, &offset);

    // Convert the tm struct to a time_t
    time_t time = mktime(&utc_time);

    // Convert the UTC time to local time
    time_t local_time = convert_utc_to_local(time, offset);

    // Convert the local time to a tm struct
    struct tm local_time_tm;
    gmtime_r(&local_time, &local_time_tm);

    return local_time_tm;
}