#include "string.h"

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strcpy(char* dst, const char* src)
{
    char* ret = dst;
    while ((*dst++ = *src++));
    return ret;
}

char* strncpy(char* dst, const char* src, size_t n)
{
    char* ret = dst;
    while (n > 0 && *src)
    {
        *dst++ = *src++;
        n--;
    }
    while (n > 0)
    {
        *dst++ = '\0';
        n--;
    }
    return ret;
}

int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void* memset(void* ptr, int value, size_t num)
{
    unsigned char* p = ptr;
    while (num--) *p++ = (unsigned char)value;
    return ptr;
}

char* strcat(char* dest, const char* src)
{
    char* ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}
