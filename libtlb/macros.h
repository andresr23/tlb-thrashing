/*
 * Useful macros by Stephan van Schaik for the paper:
 *
 * Malicious Management Unit: Why Stopping Cache Attacks in Software is Harder
 * than You Think, by van Schaik et al.
 *
 * Credit to the authors...
 */
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define BIT(n) (1 << (n))

#define KIB ((size_t) 1024)
#define MIB (1024 * KIB)
#define GIB (1024 * MIB)

#define ALIGN_UP(ptr, align)                                       \
          ((((uintptr_t) (ptr)) & ((align) - 1))                 ? \
          (((uintptr_t) (ptr) + ((align) - 1)) & ~((align) - 1)) : \
          ((uintptr_t) (ptr)))

#define ALIGN_DOWN(ptr, align) (((uintptr_t) (ptr)) & ~((align) - 1))

#define IS_POWER_2(x) (((x) != 0) && (((x) & ((x) - 1)) == 0))

#define DEREFERENCE(ptr) (*(const char *) ptr)

#define PRINT_LOG(_s, ...)                                   \
  do {                                                       \
    fprintf(stderr, "["__FILE__"] " _s "\n", ##__VA_ARGS__); \
    fflush(stderr);                                          \
  } while(0)

// For 4-KB paging systems.
#define LINE_OFFSET 6
#define PAGE_OFFSET 12
