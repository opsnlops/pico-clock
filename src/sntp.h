
#pragma once

#include "types.h"

void set_rtc(u32 time, u32 us);
bool get_date_now(struct tm *t);