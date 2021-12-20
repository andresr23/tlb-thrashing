#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int flag = 0;

uint32_t order; // 2^n
uint32_t bmask; // 2^n - 1

FILE *f;

void print_frame(void)
{
  printf("---------------------------------------------\n");
}

void print_operation(uint32_t hi, uint32_t lo)
{
  uint32_t xor = (hi ^ lo) & bmask;

  uint32_t op1;
  uint32_t op2;

  int mul;
  int tmp = hi;

  printf("%3u | %3u | %5u | ", hi, lo, xor);

  while (tmp >= 0) {
    op1 = (lo + order + tmp) & bmask;
    op2 = (lo + order - tmp) & bmask;

    if (!flag) {
      if (xor == op1) {
        printf("(%3d + %3d + %3d) mod %3d", lo, order, tmp, order);
        mul =  1;
        flag = 1;
        break;
      } else if (xor == op2) {
        printf("(%3d + %3d - %3d) mod %3d", lo, order, tmp, order);
        mul = -1;
        flag = 1;
        break;
      }
    } else {
      if (xor == op2) {
        printf("(%3d + %3d - %3d) mod %3d", lo, order, tmp, order);
        mul = -1;
        flag = 0;
        break;
      } else if (xor == op1) {
        printf("(%3d + %3d + %3d) mod %3d", lo, order, tmp, order);
        mul =  1;
        flag = 0;
        break;
      }
    }

    tmp--;
  }

  printf("\n");

  fprintf(f, "%u %d\n", xor, tmp * mul);

  return;
}

uint32_t main(uint32_t argc, char *argv[])
{
  uint32_t n = 4;

  if (argc > 1)
    n = (uint32_t) atoi(argv[1]);

  order = (1 << n);
  bmask = order - 1;

  f = fopen("xor.dat", "w");

  printf("hi  | lo  | xor_n | Solution\n");
  print_frame();

  for (uint32_t hi = 0; hi < order; hi++) {
    flag = 0;

    for (uint32_t lo = 0; lo < order; lo++)
      print_operation(hi, lo);

    print_frame();
  }

  fclose(f);
}
