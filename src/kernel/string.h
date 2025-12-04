#pragma once
#include <stddef.h>

size_t strlen(const char* str);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
void* memset(void* ptr, int value, size_t num);
char* strcat(char* dest, const char* src);
