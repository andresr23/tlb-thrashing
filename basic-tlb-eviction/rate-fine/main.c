#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtlb/tlb.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/pte_set.h"

#define REPETITIONS 10000

#define NPTE_MIN 1344
#define NPTE_MAX 1664
#define NPTE_INC 8

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float e_rates[2][NPTE_MAX / NPTE_INC];

void print_frame(void)
{
  printf("------------------------\n");
}

void print_report(void)
{
  float e_rate_n;
  float e_rate_a;

  FILE *f = fopen("rate.dat", "w");

  printf("        |    Alignment  \n");
  print_frame();
  printf("PTE Set |  4-KB | 512-KB\n");

  print_frame();

  for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
    e_rate_n = e_rates[0][(npte / NPTE_INC) - 1];
    e_rate_a = e_rates[1][(npte / NPTE_INC) - 1];

    printf("%7d | %4.3f | %6.3f\n", npte, e_rate_n, e_rate_a);
    fprintf(f, "%8.6f %8.6f\n", e_rate_n, e_rate_a);
  }

  print_frame();
  printf("(%d Repetitions)\n", REPETITIONS);

  fclose(f);
}

int experiment(void)
{
  int score;
  float rate;
  size_t offset;

  void *target_pte;
  pte_set_t pte_set;

  for (int i = 0; i < 2; i++) {
    for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {

      score = 0;
      for (int r = 0; r < REPETITIONS; r++) {
        // Random target PTE.
        target_pte = tlb_space_random_page(tlb_space_t);

        // Build a sequential PTE set.
        if (i == 0)
          offset = tlb_space_random_offset_zbits(0); // 4-KB
        else
          offset = tlb_space_random_offset_zbits(7); // 512-KB

        pte_set_build_sequ(tlb_space_s, &pte_set, npte, offset);

        // Randomize the TLB.
        tlb_randomize();

        // Load the target PTE.
        tlb_load(target_pte);

        // Basic TLB eviction.
        tlb_load_pte_set(&pte_set);

        if (tlb_query_pmc_ptw(target_pte))
          score++;

        pte_set_destroy(&pte_set);
      }

      e_rates[i][(npte / NPTE_INC) - 1] = (float) score / REPETITIONS;
    }
  }
}

int main()
{
  if (test_prepare(&tlb_space_s, &tlb_space_t))
    return -1;

  test_timer_start();

  experiment();

  test_timer_end();

  test_cleanup();

  print_report();
}
