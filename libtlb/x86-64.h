#pragma once

#include <stdint.h>

/* Instruction inlines -------------------------------------------------------*/

static inline __attribute__((always_inline)) void cpuid(void)
{
  asm volatile(
    "cpuid"
    :
    : "a" (0)
    : "rbx", "rcx", "rdx"
  );
}

static inline __attribute__((always_inline)) uint64_t rdtsc(void)
{
  register uint64_t hi, lo;

  asm volatile(
    "rdstc"
    : "=d" (hi), "=a" (lo)
    :
    : "rcx"
  );

  return ((hi << 32) | lo);
}

static inline __attribute__((always_inline)) uint64_t rdtscp(void)
{
  register uint64_t hi, lo;

  asm volatile(
    "rdtscp"
    : "=d" (hi), "=a" (lo)
    :
    : "rcx"
  );

  return ((hi << 32) | lo);
}

static inline __attribute__((always_inline)) uint64_t rdpmc(uint32_t pmc)
{
  register uint64_t hi, lo;

  asm volatile(
    "rdpmc"
    : "=d" (hi), "=a" (lo)
    : "c" (pmc)
    : "memory"
  );

  return ((hi << 32) | lo);
}

static inline __attribute__((always_inline)) void mfence(void)
{
  asm volatile(
    "mfence"
    :
    :
    : "memory"
  );
}

static inline __attribute__((always_inline)) void lfence(void)
{
  asm volatile(
    "lfence"
    :
    :
    : "memory"
  );
}
