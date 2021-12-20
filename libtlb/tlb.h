#pragma once

#include <stdint.h>

#include "xor.h"
#include "macros.h"
#include "pte_set.h"

/* Skylake, Kaby-Lake --------------------------------------------------------*/

// L1-dTLB.
#define TLB_L1_DTLB_SETS 16
#define TLB_L1_DTLB_WAYS 4

#define TLB_L1_DTLB_IDX_BITS 4  // log_2(TLB_L1_DTLB_SETS)

// L2-sTLB.
#define TLB_L2_STLB_SETS 128
#define TLB_L2_STLB_WAYS 12

#define TLB_L2_STLB_IDX_BITS 14 // log_2(TLB_L2_STLB_SETS) * 2
#define TLB_L2_STLB_XOR_BITS 7  // xor_7

// L1 dTLB and L2 sTLB are exclusive.
#define TLB_IS_EXCLUSIVE 1

// Virtual Address bits used for TLB indexing.
#define TLB_L1_DTLB_IDX_VA_MASK 0x00000000F000UL
#define TLB_L2_STLB_IDX_VA_MASK 0x000003FFF000UL

/* General Macros ------------------------------------------------------------*/

// Working on a 64-bit system that uses a 48-bit Virtual Address space.
#define TLB_VA_VPNM 0x7FFFFFFFF000UL

#define TLB_L1_DTLB_ENTRIES (TLB_L1_DTLB_SETS * TLB_L1_DTLB_WAYS)
#define TLB_L2_STLB_ENTRIES (TLB_L2_STLB_SETS * TLB_L2_STLB_WAYS)

#if TLB_IS_EXCLUSIVE
  #define TLB_ENTRIES (TLB_L1_DTLB_ENTRIES + TLB_L2_STLB_ENTRIES)
#else
  #define TLB_ENTRIES TLB_L2_STLB_ENTRIES
#endif /* TLB_IS_EXCLUSIVE? */

#define TLB_SPACE_PAGES (1 << TLB_L2_STLB_IDX_BITS)
#define TLB_SPACE_BYTES (TLB_SPACE_PAGES * 4 * KIB)

#define TLB_SECTOR_PAGES (1 << TLB_L2_STLB_XOR_BITS)
#define TLB_SECTOR_BYTES (TLB_SECTOR_PAGES * 4 * KIB)

#define TLB_SECTOR_COUNT (TLB_SPACE_PAGES / TLB_SECTOR_PAGES)

// See init_pmc_func() in pmc_module.c
#define TLB_PMC_EVENT_PTW 0U
#define TLB_PMC_EVENT_L1M 1U
#define TLB_PMC_EVENT_L2H 2U

/* API -----------------------------------------------------------------------*/

size_t tlb_zbits_to_align(int zbits);

void tlb_load(void *page);

uint64_t tlb_load_pte_set(pte_set_t *pte_set);
uint64_t tlb_load_pte_set_pass(pte_set_t *pte_set, size_t pass);

// Examine, PMCs must be configured from the kernel-space (pmc_module).
uint64_t tlb_query_tsc(void *page);
uint64_t tlb_query_pmc(void *page, uint32_t event);
uint64_t tlb_query_pmc_ptw(void *page);
uint64_t tlb_query_pmc_l1m(void *page);
uint64_t tlb_query_pmc_l2h(void *page);

// Initialize.
void tlb_randomize(void);

// Target-relative eviction.
void tlb_trel_eviction_core(pte_set_t *pte_set, size_t npte_l1e, size_t npte_l2e);
void tlb_trel_eviction(pte_set_t *pte_set);

// Get set index.
int l1_dtlb_set(void *page);
int l2_stlb_set(void *page);
