#ifndef LIB_HUFFMAN_DEFS_H
#define LIB_HUFFMAN_DEFS_H

#if defined(STM32)
#include <stdio.h>
#endif

#ifdef __linux__
#include <stdint.h>
#include <stdio.h>
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

// Encoder using extra ~3K SRAM vars
#define VHPCK_HDR 0x31484856
// Predefined variables types

#define u8 unsigned char
#define u16 unsigned short
#define u32 uint32_t
#define verr u32
#define verror(X) X
#define vok 0
#define cu8 const u8
#define cvoid const void

#endif // LIB_HUFFMAN_DEFS_H
