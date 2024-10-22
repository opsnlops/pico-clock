
#pragma once

#include <time.h>

#include "hardware/rtc.h"

#include "utz/utz.h"
#include "utz/zones.h"

time_t convert_utc_to_local(time_t utc_time, uoffset_t offset);
udatetime_t convert_tm_to_udatetime(struct tm *timeinfo);

struct tm utc_tm_to_local_tm(struct tm utc_time, uzone_t timezone);