#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtlb/tlb.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/pte_set.h"

#define REPETITIONS 1000

#define ZBITS_MAX TLB_L2_STLB_IDX_BITS

#define NPTE_INC 64
#define NPTE_MIN 64
#define NPTE_MAX 3072

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float e_rates[ZBITS_MAX + 1][NPTE_MAX / NPTE_INC];

void print_frame(void)
{
  printf("------------");
  for (int zbits = 0; zbits <= ZBITS_MAX; zbits++)
    printf("---------");
  printf("\n");
}

void print_report(void)
{
  float e_rate;
  size_t align;

  FILE *f = fopen("rate.dat", "w");

  printf("/* Randomized TLB. */\n");

  // Header.
  printf("              (Non-aligned) <------------------------------------------- PTE set sequence. ---------------------------------------------> (Aligned)\n");
  printf("PTE set size");
  for (int zbits = 0; zbits <= ZBITS_MAX; zbits++) {
    align = tlb_zbits_to_align(zbits);
    printf(" | ");
    test_print_align(align, stdout);
  }
  printf("\n");

  print_frame();

  for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
    printf("%12d", npte);

    for (int zbits = 0; zbits <= ZBITS_MAX; zbits++) {
      e_rate = e_rates[zbits][(npte / NPTE_INC) - 1];
      printf(" | %6.3f", e_rate);
      fprintf(f, "%8.6f ", e_rate);
    }
    printf("\n");
    fprintf(f, "\n");

    print_frame();
  }

  printf("(%d repetitions)\n\n", REPETITIONS);

  fclose(f);
}

int experiment(void)
{
  int score;
  float rate;
  size_t offset;

  void *target_pte;
  pte_set_t pte_set;

  for (int zbits = 0; zbits <= ZBITS_MAX; zbits++) {

    for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {

      score = 0;
      for (int r = 0; r < REPETITIONS; r++) {
        // Random PTE target.
        target_pte = tlb_space_random_page(tlb_space_t);

        // Build a sequential PTE set that is partially aligned.
        offset = tlb_space_random_offset_zbits(zbits);
        pte_set_build_sequ(tlb_space_s, &pte_set, npte, offset);

        // Randomize the TLB.
        tlb_randomize();

        // Load the target PTE.
        tlb_load(target_pte);

        // Basic TLB eviction.
        tlb_load_pte_set(&pte_set);

        // Reload the target PTEs and check basic TLB eviction was effective.
        if (tlb_query_pmc_ptw(target_pte))
          score++;

        pte_set_destroy(&pte_set);
      }

      e_rates[zbits][(npte / NPTE_INC) - 1] = (float) score / REPETITIONS;
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
