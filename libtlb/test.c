#include "test.h"

#include <time.h>
#include <stdlib.h>

#include "tlb.h"
#include "space.h"
#include "macros.h"
#include "randomize.h"

static vm_area_t test_tlb_space_s = {0}; // PTE sets.
static vm_area_t test_tlb_space_t = {0}; // Target PTEs.

int test_prepare(vm_area_t **tlb_space_s, vm_area_t **tlb_space_t)
{
  srand(time(NULL));

  if (tlb_space_map(&test_tlb_space_s)) {
    PRINT_LOG("Failed to spawn a TLB space to create PTE sets.");
    return -1;
  }

  fprintf(stderr, "\nTLB space (PTE sets).\n");
  vm_area_info(&test_tlb_space_s);

  if (tlb_space_map(&test_tlb_space_t)) {
    PRINT_LOG("Failed to spawn a TLB space to create target PTEs.");
    return -1;
  }

  fprintf(stderr, "\nTLB space (target PTEs).\n");
  vm_area_info(&test_tlb_space_t);

  fprintf(stderr, "\n");

  if (tlb_randomize_map()) {
    PRINT_LOG("Failed to map a VM area to randomize for tlb_randomize.");
    return -1;
  }

  *tlb_space_s = &test_tlb_space_s;
  *tlb_space_t = &test_tlb_space_t;

  return 0;
}

void test_cleanup(void)
{
  tlb_space_unmap(&test_tlb_space_s);
  tlb_space_unmap(&test_tlb_space_t);
}

void test_print_align(size_t align, FILE *f)
{
  if (align >= GIB)
    fprintf(f, "%3lu-GB", align >> 30);
  else if (align >= MIB)
    fprintf(f, "%3lu-MB", align >> 20);
  else
    fprintf(f, "%3lu-KB", align >> 10);
}

/* Timer. --------------------------------------------------------------------*/

static struct timespec timer_start, timer_end;
static int timer_s = 0, timer_e = 0;

void test_timer_reset(void)
{
  timer_s = timer_e = 0;
}

void test_timer_print(void)
{
  double elapsed;

  if (timer_s && timer_e) {
    elapsed  = timer_end.tv_sec   + (timer_end.tv_nsec   * 0.000000001);
    elapsed -= timer_start.tv_sec + (timer_start.tv_nsec * 0.000000001);

    printf("Elapsed time: %0.3f sec\n\n", elapsed);
  } else {
    printf("Timer was not properly set.\n\n");
  }
}

void test_timer_start(void)
{
  clock_gettime(CLOCK_REALTIME, &timer_start);
  timer_s = 1;
  timer_e = 0;
}

void test_timer_end(void)
{
  clock_gettime(CLOCK_REALTIME, &timer_end);
  timer_e = 1;

  test_timer_print();
  test_timer_reset();
}
