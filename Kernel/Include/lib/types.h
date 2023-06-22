#pragma once
#include <stdint-gcc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdalign.h>
#include <stdnoreturn.h>

#define attr_pack        __attribute__((packed))
#define attr_align(x)    __attribute__((aligned(x)))
#define attr_optimize(x) __attribute__((optimize(x)))

#define inline_asm asm volatile

#define NULLSTR "(null)"

#define NEWLINE '\n'

#define LOW_16(x)  ((uint16_t)(x))
#define HIGH_16(x) ((uint16_t)((x) >> 16))
#define BIT(x)     (1 << (x))

#define KILOBYTE 1024L
#define MEGABYTE (KILOBYTE * 1024L)
#define GIGABYTE (MEGABYTE * 1024L)