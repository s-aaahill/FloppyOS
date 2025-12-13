#pragma once
#include <stdint.h>

typedef struct {
    uint32_t total_ram_bytes;
    uint32_t used_ram_bytes;
    uint32_t free_ram_bytes;
    
    uint32_t kernel_static_bytes;
    uint32_t framebuffer_bytes;
    uint32_t backbuffer_bytes;
    uint32_t heap_bytes;
} MemStats;

extern MemStats memstats;
