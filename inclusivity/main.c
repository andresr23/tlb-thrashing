#include <stdio.h>
#include <stdlib.h>

#include "libtlb/tlb.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/pte_set.h"

#define REPETITIONS 1000

#define NPTE_MAX 16

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float e_rates[TLB_L1_DTLB_SETS];

static void print_frame(void)
{
  printf("---------------------------------\n");
}

void print_report(void)
{
  FILE *f = fopen("rate.dat", "w");

  printf("L1-dTLB Set | Eviction Rate (PTW)\n");

  print_frame();

  for (int i = 0; i < TLB_L1_DTLB_SETS; i++)
    printf("%11d | %19.3f\n", i, e_rates[i]);

  print_frame();
  printf("(%d Repetitions)\n", REPETITIONS);

  fclose(f);
}

/*
 * Build a custom PTE set that is congruent with the 'target' PTE in the
 * L2-sTLB, but not in the L1-dTLB. If loading this PTE set into the TLB still
 * evicts the target PTE, then the TLB levels are inclusive. Otherwise, the
 * PTE levels are non-inclusive, or exclusive.
 */
void pte_set_build_custom(pte_set_t *pte_set, void *target)
{
  void *page;
  size_t sector;
  int l1_set = l1_dtlb_set(target);

  pte_set_init(pte_set, NPTE_MAX);

  while (pte_set->npte < NPTE_MAX) {
    sector = (size_t) (rand() % TLB_SECTOR_COUNT);
    page = tlb_space_congruent_page(tlb_space_s, sector, target);

    if (l1_dtlb_set(page) != l1_set)
      pte_set_push(pte_set, page);

    sector++;
  }
}

void experiment(void)
{
  int score;

  void *target_pte[TLB_L1_DTLB_SETS];

  pte_set_t pte_set;

  for (int i = 0; i < TLB_L1_DTLB_SETS; i++) {
    // Prepare target PTE.
    target_pte[i] = tlb_space_page(tlb_space_t, i * (4 * KIB));

    score = 0;
    for (int r = 0; r < REPETITIONS; r++) {
      // Prepare custom PTE Set.
      pte_set_build_custom(&pte_set, target_pte[i]);

      // Randomize the TLB state.
      tlb_randomize();

      // Load the target PTE.
      tlb_load(target_pte[i]);

      // Attempt to evict.
      tlb_load_pte_set(&pte_set);

      // Reload the target and monitor PTWs.
      if (tlb_query_pmc_ptw(target_pte[i]))
        score++;

      pte_set_destroy(&pte_set);
    }

    e_rates[i] = (float) score / REPETITIONS;
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
