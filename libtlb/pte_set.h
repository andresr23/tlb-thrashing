#pragma once

#include <stdio.h>

#include "vm_area.h"

typedef struct {
  int trel;
  void **data;
  size_t npte;
  size_t alloc;
} pte_set_t;

#define PTE_SET_LOAD_ALL(pte_set)                                         \
  for (register size_t ___i___ = 0; ___i___ < (pte_set)->npte; ___i___++) \
    *(volatile char *) ((pte_set)->data[___i___])

#define PTE_SET_LOAD_ALL_REVERSE(pte_set)                                                   \
  for (register size_t ___i___ = (pte_set)->npte - 1; ___i___ < (pte_set)->npte; ___i___--) \
    *(volatile char *) ((pte_set)->data[___i___])

// Minimum PTE set alignment (fixed to zero bits).
#define PTE_SET_FTZB_ALIGNMENT(ftzb) ((4 * KIB) << (ftzb))
#define PTE_SET_FTZB_OFFSET_MASK(offset, ftzb) ((offset) & (0xFFFFFFFFFFFFF000UL << (ftzb)))

// Target-relative PTE set dimensions.
#define TREL_NPTE_L1E 6  // ../l1-dtlb/set-eviction
#define TREL_NPTE_L2E 16 // ../l2-stlb/set-eviction
#define TREL_NPTE_L1F 64 // ../l1-dtlb/flooding

/* API -----------------------------------------------------------------------*/

// Management.
int pte_set_init(pte_set_t *pte_set, size_t npte);
int pte_set_push(pte_set_t *pte_set, void *page);
void pte_set_destroy(pte_set_t *pte_set);
void pte_set_info(pte_set_t *pte_set, FILE *f);

// Build.
int pte_set_build_strd(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte,
  size_t offset, size_t stride);
int pte_set_build_sequ(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte,
  size_t offset);
int pte_set_build_rand(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte);

// Build target-relative (T-Rel).
int pte_set_build_trel_core(vm_area_t *vm_area, pte_set_t *pte_set,
  size_t npte_l1e, size_t npte_l2e, size_t npte_l1f, void *target);
int pte_set_build_trel(vm_area_t *vm_area, pte_set_t *pte_set, void *target);
