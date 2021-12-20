#include "tlb.h"

#include "x86-64.h"
#include "randomize.h"

size_t tlb_zbits_to_align(int zbits)
{
  zbits %= (TLB_L2_STLB_IDX_BITS + 1);
  return (4 * KIB) * (1 << zbits);
}

void tlb_load(register void *page)
{
  mfence();
  lfence();
  *(volatile char *) page;
  mfence();
  lfence();
}

uint64_t tlb_load_pte_set(pte_set_t *pte_set)
{
  register uint64_t tsc_s, tsc_e;

  tsc_s = rdtscp();
  mfence();
  lfence();
  PTE_SET_LOAD_ALL(pte_set);
  mfence();
  lfence();
  tsc_e = rdtscp();

  return (tsc_e - tsc_s);
}

uint64_t tlb_load_pte_set_pass(pte_set_t *pte_set, size_t pass)
{
  register uint64_t tsc_s, tsc_e;

  pass = pass > 0 ? pass : 1;

  tsc_s = rdtscp();
  mfence();
  lfence();

  for (register size_t i = 0; i < pass; i++)
    PTE_SET_LOAD_ALL(pte_set);

  mfence();
  lfence();
  tsc_e = rdtscp();

  return (tsc_e - tsc_s);
}

uint64_t tlb_query_tsc(register void *page)
{
  register uint64_t tsc_s, tsc_e;

  mfence();
  lfence();
  tsc_s = rdtscp();
  *(volatile char *) page;
  mfence();
  lfence();
  tsc_e = rdtscp();

  return (tsc_e - tsc_s);
}

uint64_t tlb_query_pmc(register void *page, register uint32_t event)
{
  register uint64_t pmc_s, pmc_e;

  mfence();
  lfence();
  pmc_s = rdpmc(event);
  *(volatile char *) page;
  mfence();
  lfence();
  pmc_e = rdpmc(event);

  return (pmc_e - pmc_s);
}

uint64_t tlb_query_pmc_ptw(register void *page)
{
  return tlb_query_pmc(page, TLB_PMC_EVENT_PTW);
}

uint64_t tlb_query_pmc_l1m(register void *page)
{
  return tlb_query_pmc(page, TLB_PMC_EVENT_L1M);
}

uint64_t tlb_query_pmc_l2h(register void *page)
{
  return tlb_query_pmc(page, TLB_PMC_EVENT_L2H);
}

/*
 * Randomize the TLB content with random PTEs that are picked from a designated
 * virtual memory area managed in 'randomize.c'.
 */
void tlb_randomize(void)
{
  size_t count;
  pte_set_t pte_set;

  tlb_randomize_set_build(&pte_set);

  mfence();
  lfence();

  for (size_t i = 0; i < pte_set.npte; i++) {
    // Load each PTE a random number of times.
    count = (size_t) (rand() % TLB_L2_STLB_WAYS);

    for (size_t j = 0; j < count; j++)
      *(volatile char *) pte_set.data[i];
  }

  mfence();
  lfence();

  tlb_randomize_set_destroy(&pte_set);
}

void tlb_trel_eviction_core(pte_set_t *pte_set, size_t npte_l1e,
  size_t npte_l2e)
{
  register size_t i;

  register size_t l1_e = npte_l1e;
  register size_t l2_e = npte_l1e + npte_l2e;

  if (pte_set->trel) {
    mfence();
    lfence();

    // T-Rel.1) L1-dTLB Set Eviction.
    for (i = 0; i < l1_e; i++)
      *(volatile char *) pte_set->data[i];

    mfence();
    lfence();

    // T-Rel.2) L2-sTLB Set Eviction.
    for (i = l1_e; i < l2_e; i++)
      *(volatile char *) pte_set->data[i];

    mfence();
    lfence();

    // T-Rel.3) L1-dTLB Flooding.
    for (i = l2_e; i < pte_set->npte; i++)
      *(volatile char *) pte_set->data[i];
    for (i = pte_set->npte - 1; i >= l2_e; i--)
      *(volatile char *) pte_set->data[i];

    mfence();
    lfence();
  } else {
    tlb_load_pte_set(pte_set);
  }
}

void tlb_trel_eviction(pte_set_t *pte_set)
{
  tlb_trel_eviction_core(pte_set, TREL_NPTE_L1E, TREL_NPTE_L2E);
}

/* Get set index. ------------------------------------------------------------*/

int l1_dtlb_set(void *page)
{
  uintptr_t i = (uintptr_t) page;

  i = (i & TLB_L1_DTLB_IDX_VA_MASK) >> PAGE_OFFSET;

  return ((int) i);
}

int l2_stlb_set(void *page)
{
  return ((int) xor_7_virt(page));
}
