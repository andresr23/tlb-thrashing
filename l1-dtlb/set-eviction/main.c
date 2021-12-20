#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtlb/tlb.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/macros.h"
#include "libtlb/pte_set.h"

#define REPETITIONS 1000

#define NPTE_MIN 1
#define NPTE_MAX 10
#define NPTE_INC 1

#define STRIDE_COUNT 8

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float e_rate[STRIDE_COUNT][NPTE_MAX / NPTE_INC];

void print_frame(void)
{
  printf("----------");

  for (size_t npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC)
    printf("--------");

  printf("--------------\n");
}

void print_header(void)
{
  print_frame();

  printf("Stride\\Count ");

  for (size_t npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC)
    printf("| %5lu ", npte);

  printf("\n");

  print_frame();
}

void print_report(void)
{
  float rate;
  size_t align;

  FILE *f_0, *f_1;
  f_0 = fopen("rate.dat", "w");
  f_1 = fopen("latex.dat", "w");

  // Header.
  printf("/* Randomized TLB. */\n");
  print_frame();
  printf("          | | PTE Set | (L1-dTLB Set Eviction).                                           |\n");
  print_frame();

  printf("Alignment ");
  fprintf(f_1, "Alignment & ");
  for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
    printf("| %5d ", npte);
    fprintf(f_1, " %d &", npte);
  }

  printf("| L1-dTLB Sets\n");
  print_frame();
  fprintf(f_1, " L1-dTLB Sets \\\\\n");

  // Body.
  align = 4 * KIB;
  for (int i = 0; i < STRIDE_COUNT; i++) {
    printf("    ");
    test_print_align(align, stdout);

    test_print_align(align, f_1);
    fprintf(f_1, " & ");

    for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
      rate = e_rate[i][(npte / NPTE_INC) - 1];
      printf("| %5.3f ", rate);
      fprintf(f_0, "%.6f ", rate);
      fprintf(f_1, "%5.3f & ", rate);
    }

    printf("| %12d\n", 1 << i);
    fprintf(f_0, "\n");
    fprintf(f_1, "%d \\\\\n", 1 << i);

    align *= 2;
  }

  print_frame();
  printf("(%d repetitions)\n", REPETITIONS);

  fclose(f_0);
  fclose(f_1);
}

void experiment(void)
{
  int score;
  size_t align;
  size_t offset;

  void *target_pte;
  pte_set_t pte_set;

  align = 4 * KIB;
  for (int i = 0; i < STRIDE_COUNT; i++) {

    for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
      score = 0;
      for (int r = 0; r < REPETITIONS; r++) {
        // Random target PTE.
        target_pte = tlb_space_random_page(tlb_space_t);

        // Build a strided PTE set.
        offset = ((uintptr_t) target_pte) & TLB_L1_DTLB_IDX_VA_MASK;
        pte_set_build_strd(tlb_space_s, &pte_set, npte, offset, align);

        // Randomize the TLB.
        tlb_randomize();

        // Load the target PTE that we are trying to evict.
        tlb_load(target_pte);

        // L1-dTLB eviction.
        tlb_load_pte_set(&pte_set);

        // Reload the target PTE and check if L1-dTLB eviction was effective.
        if (tlb_query_pmc_l1m(target_pte))
          score++;

        pte_set_destroy(&pte_set);
      }

      e_rate[i][(npte / NPTE_INC) - 1] = (float) score / REPETITIONS;
    }

    align *= 2;
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
