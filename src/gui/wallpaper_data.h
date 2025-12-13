#pragma once
#include <stdint.h>

extern uint8_t _binary_courtyard_fixed_bmp_start[];
extern uint8_t _binary_courtyard_fixed_bmp_end[];
extern uint8_t _binary_courtyard_fixed_bmp_size[];

#pragma pack(push, 1)
typedef struct {
    uint16_t type;              // Magic identifier: 0x4d42
    uint32_t size;              // File size in bytes
    uint16_t reserved1;         // Not used
    uint16_t reserved2;         // Not used
    uint32_t offset;            // Offset to image data in bytes from beginning of file
} BMPHeader;

typedef struct {
    uint32_t size;              // Header size in bytes
    int32_t  width;             // Width of the image
    int32_t  height;            // Height of the image
    uint16_t planes;            // Number of color planes
    uint16_t bits;              // Bits per pixel
    uint32_t compression;       // Compression type
    uint32_t imagesize;         // Image size in bytes
    int32_t  xresolution;       // Pixels per meter
    int32_t  yresolution;       // Pixels per meter
    uint32_t ncolours;          // Number of colors
    uint32_t importantcolours;  // Important colors
} BMPInfoHeader;
#pragma pack(pop)
