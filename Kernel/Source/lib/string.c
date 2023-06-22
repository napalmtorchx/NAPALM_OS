#include <lib/string.h>
#include <core/kernel.h>

uint32_t memalign(uint32_t val, uint32_t align)
{
    uint32_t out = val;
    out &= (0xFFFFFFFF - (align - 1));
    if (out < val) { out += align; }
    return out;
}

void* memchr(const void* str, int c, size_t n)
{
    uint8_t* p    = (uint8_t*)str;
    uint8_t* find = NULL;
    while((str != NULL) && (n--))
    {
        if( *p != (uint8_t)c) { p++; }
        else { find = p; break; }
    }
    return find;
}

int memcmp(const void* str1, const void* str2, size_t size)
{
    int r = 0;
    uint8_t* x = (uint8_t*)str1;
    uint8_t* y = (uint8_t*)str2;

    while (size--) 
    {
        r = *x - *y;
        if (r) { return r; }
        x++; y++;
    }
    return 0;
}

void* memcpy(void* dest, const void* src, size_t size)
{
    uint32_t  num_dwords = size / 4;
    uint32_t  num_bytes  = size % 4;
    uint32_t* dest32     = (uint32_t*)dest;
    uint32_t* src32      = (uint32_t*)src;
    uint8_t*  dest8      = ((uint8_t*)dest) + num_dwords * 4;
    uint8_t*  src8       = ((uint8_t*)src) + num_dwords * 4;
    inline_asm("cld;rep movsl" : "+D"(dest32), "+S"(src32), "+c"(num_dwords) : : "memory");
    inline_asm("cld;rep movsb" : "+D"(dest8),  "+S"(src8),  "+c"(num_bytes)  : : "memory");
    return dest;
}

void* memset(void* dest, int data, size_t size)
{
    uint32_t  num_dwords = size / 4;
    uint32_t  num_bytes  = size % 4;
    uint32_t* dest32     = (uint32_t*)dest;
    uint8_t*  dest8      = ((uint8_t*)dest) + num_dwords * 4;
    inline_asm("cld;rep stosl" : "+D"(dest32), "+c"(num_dwords) : "a"(data) : "memory");
    inline_asm("cld;rep stosb" : "+D"(dest8),  "+c"(num_bytes)  : "a"(data) : "memory");
    return dest;
}

void* memset16(void* str, uint16_t c, size_t n)
{
    return (uint16_t*)memset(str, (c << 16) | c, n);
}

void* memmove(void* dest, const void* src, size_t n)
{
    return NULL;
}

char* strcat(char* dest, const char* src)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return dest; }
    uint64_t src_len = strlen(src);
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < src_len; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strncat(char* dest, const char* src, size_t n)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return dest; }
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < n; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strchr(const char* str, int c)
{
    do 
    {
        if (*str == c) { return (char*)str; }
    } while (*str++);
    return NULL;
}

int strcmp(const char* str1, const char* str2)
{
    if (str1 == NULL && str1 != str2) { return 1; }
    while (*str1)
    {
        if (*str1 != *str2) { break; }
        str1++; str2++;
    }
    return *(char*)str1 - *(char*)str2;
}

int strncmp(const char* str1, const char* str2, size_t n)
{
    if (str1 == NULL || str2 == NULL) { return 1; }
    while (n && *str1 && (*str1 == *str2))
    {
        ++str1;
        ++str2;
        --n;
    }
    if (n == 0) { return 0; }
    else { return (*(char*)str1 = *(char*)str2); }
    return 0;
}

char* strcpy(char* dest, const char* src)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return NULL; }

    int i = 0;
    while (src[i] != 0) { dest[i] = src[i]; i++; }
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    for (uint32_t i = 0; i < n; i++) { dest[i] = src[i]; }
    return dest;
}

size_t strcspn(const char* str1, const char* str2)
{
    return 0;
}

size_t strlen(const char* str)
{
    if (str == NULL) { return 0; }
    uint32_t len = 0;
    while (str[len] != 0) { len++; }
    return len;
}

char* strpbrk(const char* str1, const char* str2)
{
    return NULL;
}

char* strrchr(const char* str, int c)
{
    return NULL;
}

size_t strspn(const char* str1, const char* str2)
{
    return 0;
}

char* strstr(const char* haystack, const char* needle)
{
    return NULL;
}

char* strtok(char* str, const char* delim)
{
    return NULL;
}

size_t strxfrm(char* dest, const char* str, size_t n)
{
    return 0;
}

char* stradd(char* str, char c)
{
    if (str == NULL) { return NULL; }
    uint64_t len = strlen(str);
    str[len] = (char)c;
    str[len + 1] = 0;
    return str;
}

char* strdup(const char* str)
{
    if (str == NULL) { return NULL; }
    char* dup = (char*)malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

char** strsplit(char* str, char delim, int* count)
{
    if (str == NULL) { return NULL; }
    if (strlen(str) == 0) { return NULL; }

    int len = strlen(str);
    uint32_t num_delimeters = 0;

    for(int i = 0; i < len - 1; i++) { if(str[i] == delim) { num_delimeters++; } }

    uint32_t arr_size = sizeof(char*) * (num_delimeters + 1);
    char** str_array = (char**)malloc(arr_size);
    int str_offset = 0;

    int start = 0;
    int end = 0;
    while(end < len)
    {
        while(str[end] != delim && end < len) { end++; }

        char* substr = (char*)malloc(end - start + 1);
        memcpy(substr, str + start, end - start);
        start = end + 1;
        end++;
        str_array[str_offset] = substr;
        str_offset++;
    }
    *count = str_offset;
    return str_array;
}

bool strwhite(char* str)
{
    if (str == NULL) { return false; }
    if (strlen(str) == 0) { return false; }
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isspace(str[i])) { return false; }
    }
    return true;
}

char* strupper(char* str)
{
    for (int i = 0; i < strlen(str); i++) { str[i] = toupper(str[i]); }
    return str;
}

char* strlower(char* str)
{
    for (int i = 0; i < strlen(str); i++) { str[i] = tolower(str[i]); }
    return str;
}

char* strback(char* str)
{
    if (str == NULL) { return NULL; }
    str[strlen(str) - 1] = 0;
    return str;
}

void strswap(char* x, char* y)
{
    char t = *x; 
    *x  = *y; 
    *y = t;
}

char* strrev(char* str)
{
    if (str == NULL) { return NULL; }
    size_t sz = strlen(str);

    int i, j, temp;
    for (i = 0, j = sz - 1; i < j; i++, j--) 
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}