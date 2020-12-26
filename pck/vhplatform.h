#ifndef VHPLATFORM_H
#define VHPLATFORM_H

#ifdef __linux__

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Predefined variables types
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define verr u32
#define verror(X) X
#define vok 0
#define cu8 const u8
#define cvoid const void

#elif _WIN32

#endif


#endif // VHPLATFORM_H
