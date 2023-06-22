#include <lib/time.h>
#include <core/kernel.h>

static rtc_device_t* _rtc;

time_t time_create(int sec, int min, int hr, int mday, int mn, int yr, int wday, int yday)
{
    time_t time = (time_t){ sec, min, hr, mday, mn, yr, wday, yday };
    return time;
}

time_t time(time_t* tm)
{
    if (_rtc == NULL) 
    { 
        _rtc = (rtc_device_t*)devmgr_from_name("rtc_timer"); 
        if (_rtc == NULL) { return time_create(0, 0, 0, 0, 0, 0, 0, 0); }
    }

    time_t now = _rtc->tm;
    if (tm != NULL) { *tm = now; }
    return now;
}

uint32_t time_millis(uint32_t* millis)
{
    if (_rtc == NULL) 
    { 
        _rtc = (rtc_device_t*)devmgr_from_name("rtc_timer"); 
        if (_rtc == NULL) { return 0; }
    }

    if (millis != NULL) { *millis = _rtc->millis_t; }
    return _rtc->millis_t;
}

float time_secs(float* secs)
{
    if (_rtc == NULL) 
    { 
        _rtc = (rtc_device_t*)devmgr_from_name("rtc_timer"); 
        if (_rtc == NULL) { return 0; }
    }

    if (secs != NULL) { *secs = _rtc->seconds; }
    return _rtc->seconds;
}

char* timestr(time_t* tm, char* buff, TIMEFORMAT fmt, bool secs)
{
    // temp vars
    char temp[64];
    memset(temp, 0, sizeof(temp));

    // 24-hour
    if (fmt == TIMEFORMAT_MILITARY)
    {
        if (tm->hour < 10) { strcat(buff, "0"); } 
        strcat(buff, ltoa(tm->hour, temp, 10));
    }
    // 12-hour
    else
    {
        int hour = tm->hour;
        if (hour > 12) { hour -= 12; }
        if (hour == 0) { hour = 12; }
        if (hour < 10) { strcat(buff, "0"); }
        strcat(buff, ltoa(hour, temp, 10));
    }
    
    // clear temp and add colon
    memset(temp, 0, sizeof(temp));
    strcat(buff, ":");

    // minute
    if (tm->minute < 10) { strcat(buff, "0"); }
    strcat(buff, ltoa(tm->minute, temp, 10));

    // clear temp
    memset(temp, 0, sizeof(temp));

    // seconds
    if (secs)
    {
        strcat(buff, ":");
        if (tm->second < 10) { strcat(buff, "0"); }
        strcat(buff, ltoa(tm->second, temp, 10));
    }

    // am/pm
    if (fmt == TIMEFORMAT_STANDARD) { if (tm->hour >= 12) { strcat(buff, " PM"); } else { strcat(buff, " AM"); } }

    // return output string
    return buff;
}