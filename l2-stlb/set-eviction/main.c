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
#define NPTE_MAX 32
#define NPTE_INC 1

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float e_rates[NPTE_MAX / NPTE_INC];

void print_frame(void)
{
  printf("--------------------------------------\n");
}

void print_report(void)
{
  float e_rate;

  FILE *f_0;

  f_0 = fopen("rate.dat", "w");

  // Header.
  print_frame();
  printf("| PTE Set |            |\n");
  printf("(L2-sTLB Set Eviction) | Eviction rate\n");
  print_frame();

  // Body.
  for (int npte_l2e = NPTE_MIN; npte_l2e <= NPTE_MAX; npte_l2e += NPTE_INC) {
    e_rate = e_rates[(npte_l2e / NPTE_INC) - 1];

    printf("%22d | %13.3f\n", npte_l2e, e_rate);
    fprintf(f_0, "%.6f\n", e_rate);
  }

  print_frame();

  fclose(f_0);
}

int experiment(void)
{
  int score;

  void *target_pte;
  pte_set_t pte_set;

  for (int npte_l2e = NPTE_MIN; npte_l2e <= NPTE_MAX; npte_l2e += NPTE_INC) {

    score = 0;
    for (int r = 0; r < REPETITIONS; r++) {
      // Random target PTE.
      target_pte = tlb_space_random_page(tlb_space_t);

      // Build a target-relative PTE set.
      pte_set_build_trel_core(tlb_space_s, &pte_set, TREL_NPTE_L1E, npte_l2e,
        TREL_NPTE_L1F, target_pte);

      // Randomize the TLB.
      tlb_randomize();

      // Load the target PTE that we are trying to evict.
      tlb_load(target_pte);

      // Target-relative TLB eviction.
      tlb_trel_eviction_core(&pte_set, TREL_NPTE_L1E, npte_l2e);

      // Reload the target PTE and check if T-Rel. eviction was effective.
      if (tlb_query_pmc_ptw(target_pte))
        score++;

      pte_set_destroy(&pte_set);
    }

    e_rates[(npte_l2e / NPTE_INC) - 1] = (float) score / REPETITIONS;
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
