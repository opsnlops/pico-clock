
#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "hardware/rtc.h"

#include "logging/logging.h"

#include "types.h"

void set_rtc(u32 t, u32 us) {

    info("setting RTC to %d %d", t, us);

    time_t seconds_since_1970 = t;
    struct tm *datetime = gmtime(&seconds_since_1970);
    datetime_t dt;

    dt.year = datetime->tm_year + 1900;
    dt.month = datetime->tm_mon + 1;
    dt.day = datetime->tm_mday;
    dt.dotw = datetime->tm_wday;
    dt.hour = datetime->tm_hour;
    dt.min = datetime->tm_min;
    dt.sec = datetime->tm_sec;

    rtc_set_datetime(&dt);
}

bool get_date_now(struct tm *t) {

    datetime_t rtc;
    bool state = rtc_get_datetime(&rtc);

    if(state) {

        t->tm_sec = rtc.sec;
        t->tm_min = rtc.min;
        t->tm_hour = rtc.hour;
        t->tm_mday = rtc.day;
        t->tm_mon = rtc.month - 1;
        t->tm_year = rtc.year - 1900;
        t->tm_wday = rtc.dotw;
        t->tm_yday = 0;
        t->tm_isdst = -1;
    }

    return state;
}