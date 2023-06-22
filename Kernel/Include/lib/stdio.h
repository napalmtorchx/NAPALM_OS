#pragma once
#include <lib/types.h>

void vsprintf(char* buff, const char* fmt, va_list args);
void sprintf(char* buff, const char* fmt, ...);

void vprintf(const char* fmt, va_list args);
void printf(const char* fmt, ...);