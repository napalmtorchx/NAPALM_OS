#pragma once
#include <lib/types.h>

int atoi(const char* str);

uint32_t atol(const char* str);

uint32_t atox(const char* str);

float atof(const char* str);

char* itoa(int num, char* buff, int base);

char* ltoa(uint32_t num, char* buff, int base);

char* ftoa(float num, char* buff, int afterdot);

char* xtoa(uint32_t num, char* buff, uint8_t sz);

void* malloc(size_t sz);
void free(void* ptr);