#pragma once
#include <lib/types.h>

uint32_t memalign(uint32_t val, uint32_t align);
void*  memchr(const void* str, int c, size_t n);
int    memcmp(const void* str1, const void* str2, size_t n);
void*  memcpy(void* dest, const void* src, size_t n);
void*  memmove(void* dest, const void* src, size_t n);
void*  memset(void* str, int c, size_t n);
void*  memset16(void* str, uint16_t c, size_t n);
char*  strcat(char* dest, const char* src);
char*  strncat(char* dest, const char* src, size_t n);
char*  strchr(const char* str, int c);
int    strcmp(const char* str1, const char* str2);
int    strncmp(const char* str1, const char* str2, size_t n);
char*  strcpy(char* dest, const char* src);
char*  strncpy(char* dest, const char* src, size_t n);
size_t strcspn(const char* str1, const char* str2);
size_t strlen(const char* str);
char*  strpbrk(const char* str1, const char* str2);
char*  strrchr(const char* str, int c);
size_t strspn(const char* str1, const char* str2);
char*  strstr(const char* haystack, const char* needle);
char*  strtok(char* str, const char* delim);
size_t strxfrm(char* dest, const char* str, size_t n);
char*  stradd(char* str, char c);
char*  strdup(const char* str);
char** strsplit(char* str, char delim, int* count);
bool   strwhite(char* str);
char*  strupper(char* str);
char*  strlower(char* str);
char*  strback(char* str);
void   strswap(char* x, char* y);
char*  strrev(char* str);

