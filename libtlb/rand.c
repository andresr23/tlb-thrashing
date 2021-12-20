#include "rand.h"

#include <stdlib.h>

/* Credit to Hallvard B. Furuseth for the INT_MAX_BITS macro. */
#define INT_MAX_BITS(m) ((m) / ((m) % 255 + 1) / 255 % 255 * 8 + 7 - 86 / ((m) % 255 + 12))

#define RAND_MAX_WIDTH INT_MAX_BITS(RAND_MAX)

_Static_assert((RAND_MAX & (RAND_MAX + 1U)) == 0, "RAND_MAX is not a Mersenne number.");


uint32_t rand_32(void) {
  uint32_t r = 0;

  for (int i = 0; i < 32; i += RAND_MAX_WIDTH) {
    r <<= RAND_MAX_WIDTH;
    r  ^= (unsigned) rand();
  }

  return r;
}

uint64_t rand_64(void) {
  uint64_t r = 0;

  for (int i = 0; i < 64; i += RAND_MAX_WIDTH) {
    r <<= RAND_MAX_WIDTH;
    r  ^= (unsigned) rand();
  }

  return r;
}
