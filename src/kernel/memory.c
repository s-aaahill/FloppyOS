#include "memory.h"
#include <mm/memstats.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

extern uint8_t __entry_start;
extern uint8_t __end;

MemStats memstats;

static uint8_t* g_NextAlloc;

void mem_init()
{
    // Hardcoded for QEMU default (usually 128MB or similar, but prompt says explicitly set based on config e.g. 32MB)
    // Let's assume 32MB as per prompt suggestion to be safe and demonstrate usage percentage.
    memstats.total_ram_bytes = 32 * 1024 * 1024;
    
    // Calculate static kernel size
    // Note: __entry_start is where we loaded.
    uint32_t kernel_start = (uint32_t)&__entry_start;
    uint32_t kernel_end = (uint32_t)&__end;
    
    memstats.kernel_static_bytes = kernel_end - kernel_start;
    
    memstats.used_ram_bytes = memstats.kernel_static_bytes;
    memstats.free_ram_bytes = memstats.total_ram_bytes - memstats.used_ram_bytes;
    memstats.framebuffer_bytes = 0;
    memstats.backbuffer_bytes = 0;
    memstats.heap_bytes = 0;
    
    g_NextAlloc = &__end;
    
    // Align to 4 bytes
    if ((uint32_t)g_NextAlloc % 4 != 0)
    {
        g_NextAlloc += (4 - ((uint32_t)g_NextAlloc % 4));
    }
}

void* memcpy(void* dst, const void* src, uint16_t num)
{
    uint8_t* u8Dst = (uint8_t *)dst;
    const uint8_t* u8Src = (const uint8_t *)src;

    for (uint16_t i = 0; i < num; i++)
        u8Dst[i] = u8Src[i];

    return dst;
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* u8Ptr1 = (const uint8_t *)ptr1;
    const uint8_t* u8Ptr2 = (const uint8_t *)ptr2;

    for (uint16_t i = 0; i < num; i++)
        if (u8Ptr1[i] != u8Ptr2[i])
            return 1;

    return 0;
}

// Simple Bump Allocator
void* kalloc_raw(uint32_t size)
{
    // Align size
    if (size % 4 != 0) size += (4 - (size % 4));
    
    // Check OOM (Simple check)
    if (memstats.used_ram_bytes + size > memstats.total_ram_bytes)
    {
        return NULL; // OOM
    }
    
    void* ptr = g_NextAlloc;
    g_NextAlloc += size;
    return ptr;
}

void* kmalloc(uint32_t size)
{
    void* ptr = kalloc_raw(size);
    if (ptr)
    {
        memstats.heap_bytes += size;
        memstats.used_ram_bytes += size;
        memstats.free_ram_bytes -= size;
    }
    return ptr;
}

void kfree(void* ptr)
{
    // Bump allocator cannot free.
    // For this task, we will just NOT decrement used_ram if we can't free.
    // However, the instructions say: "If kfree size is unknown, track heap allocations only".
    // Since we don't store size metadata in kalloc_raw, we can't accurately track free here without overhead.
    // But for Backbuffers (which is the main dynamic thing), we handle them separately in WM.
    // So for generic kmalloc/kfree, we might just leak in stats if we don't track size.
    
    // Actually, prompt says: "On kfree... If kfree size is unknown, track heap allocations only"
    // So we will do nothing here for now.
}
