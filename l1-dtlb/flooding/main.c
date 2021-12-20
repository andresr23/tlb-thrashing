#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtlb/pte.h"
#include "libtlb/tlb.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/macros.h"

#define REPETITIONS 1000

#define NPTE_MIN 16
#define NPTE_MAX 128
#define NPTE_INC 16

#define PASS_MIN 1
#define PASS_MAX 4
#define PASS_INC 1

#define TARGET_COUNT 16

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

static float f_rates[NPTE_MAX / NPTE_INC][PASS_MAX / PASS_INC];
static float e_rates[NPTE_MAX / NPTE_INC][PASS_MAX / PASS_INC];

void print_frame(void)
{
  printf("------------");
  for (int pass = PASS_MIN; pass <= PASS_MAX; pass += PASS_INC)
    printf("--------");
  printf("\n");
}

void print_report(void)
{
  float f_rate;
  float e_rate;

  FILE *f_0, *f_1;

  f_0 = fopen("rate.dat", "w");
  f_1 = fopen("sets.dat", "w");

  printf("/* Randomized TLB. */\n");

  // Header.
  printf("             | Number of accesses.\n");
  print_frame();
  printf("PTE set size ");
  for (int pass = PASS_MIN; pass <= PASS_MAX; pass += PASS_INC)
    printf("| %5d ", pass);

  printf("\n");
  print_frame();

  // Body.
  for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
    printf("%12d ", npte);

    for (int pass = PASS_MIN; pass <= PASS_MAX; pass += PASS_INC) {
      f_rate = f_rates[(npte / NPTE_INC) - 1][(pass / PASS_INC) - 1];
      e_rate = e_rates[(npte / NPTE_INC) - 1][(pass / PASS_INC) - 1];
      printf("| %5.3f ", f_rate);

      fprintf(f_0, "%.6f ", f_rate);
      fprintf(f_1, "%.6f ", e_rate);
    }

    printf("\n");

    fprintf(f_0, "\n");
    fprintf(f_1, "\n");
  }

  print_frame();

  fclose(f_0);
  fclose(f_1);
}

void experiment(void)
{
  int idx_0, idx_1;

  int score;
  int e_count;
  int e_total;

  void *target_pte[TARGET_COUNT];
  pte_set_t pte_set;

  // Prepare target PTEs.
  for (int i = 0; i < TARGET_COUNT; i++)
    target_pte[i] = sttlb_space_page(tlb_space_t, i * (4 * KIB));

  for (int npte = NPTE_MIN; npte <= NPTE_MAX; npte += NPTE_INC) {
    pte_set_build_sequ(tlb_space_s, &pte_set, npte, 0);

    for (int pass = PASS_MIN; pass <= PASS_MAX; pass += PASS_INC) {

      score = 0;
      e_total = 0;
      for (int r = 0; r < REPETITIONS; r++) {
        // Randomize the TLB.
        tlb_randomize();

        // Load the target PTEs that we are trying to evict.
        for (int i = 0; i < TARGET_COUNT; i++)
          tlb_load(target_pte[i]);

        // L1-dTLB flooding.
        tlb_load_pte_set_pass(&pte_set, pass);

        // Reload the target PTEs and check if L1-dTLB flooding was effective.
        e_count = 0;
        for (int i = 0; i < TARGET_COUNT; i++) {
          if (tlb_query_pmc_l1m(target_pte[i]))
            e_count++;
        }

        if (e_count >= TARGET_COUNT)
          score++;

        e_total += e_count;
      }

      idx_0 = (pass / PASS_INC) - 1;
      idx_1 = (npte / NPTE_INC) - 1;

      f_rates[idx_1][idx_0] = (float) score / REPETITIONS;
      e_rates[idx_1][idx_0] = (float) e_total / REPETITIONS;
    }

    pte_set_destroy(&pte_set);
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
