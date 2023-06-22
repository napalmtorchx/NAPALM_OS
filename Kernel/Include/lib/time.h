#pragma once
#include <lib/types.h>

typedef enum
{
    TIMEFORMAT_STANDARD,
    TIMEFORMAT_MILITARY,
} TIMEFORMAT;

typedef struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t mday;
    uint8_t month;
    uint8_t year;
    uint8_t wday;
    uint8_t yday;
} time_t;

time_t   time_create(int sec, int min, int hr, int mday, int mn, int yr, int wday, int yday);
time_t   time(time_t* tm);
uint32_t time_millis(uint32_t* millis);
float    time_secs(float* secs);
char*    timestr(time_t* tm, char* buff, TIMEFORMAT fmt, bool secs);