#pragma once
#include "stdint.h"

void* memcpy(void* dst, const void* src, uint16_t num);
// memset is in string.h
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);

void mem_init();
void* kmalloc(uint32_t size);
void kfree(void* ptr);
void* kalloc_raw(uint32_t size); // For raw allocation without header overhead if needed


