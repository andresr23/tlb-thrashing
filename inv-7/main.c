#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define COUNT 10

void print_frame(void)
{
  printf("-----------------------------------------------\n");
}

uint32_t xor_7(uint32_t x)
{
  uint32_t hi = (x >> 7) & 0x7FU;
  uint32_t lo = (x >> 0) & 0x7FU;

  return (hi ^ lo);
}

uint32_t inv_7(uint32_t hi, uint32_t i)
{
  return ((hi ^ i) & 0x7FU);
}

int main()
{
  uint32_t x, i, hi, lo;

  srand(time(0));

  printf("VPN[13:00] | ( hi * 2^7) + ( lo) |   i | hi ^ i\n");
  print_frame();

  for (int c = 0; c < COUNT; c++) {
    // xor_7
    x = ((uint32_t) rand()) & 0x3FFFUL;
    i = xor_7(x);

    // inv_7
    hi = x >> 7;
    lo = inv_7(hi, i);
    printf("%10u | (%3u * 128) + (%3u) | %3u | %6u\n", x, hi, (x & 0x7FU), i, lo);
  }

  print_frame();
}
