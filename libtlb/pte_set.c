#include "pte_set.h"

#include <stdlib.h>

#include "xor.h"
#include "tlb.h"

/* PTE set management. -------------------------------------------------------*/

/*
 * Allocate/Reallocate an amount of 'alloc' bytes, and save the corresponding
 * pointer in 'data'.
 *
 * @return:  0 on success.
 *          -1 if realloc() fails.
 */
static int pte_set_resize(pte_set_t *pte_set, size_t alloc)
{
  void *data;

  data = realloc(pte_set->data, alloc * sizeof(void *));

  if (!data)
    return -1;

  pte_set->data = data;
  pte_set->alloc = alloc;

  return 0;
}

/*
 * Initialize a 'pte_set' and allocate memory if necessary.
 *
 * @return:  0 on success.
 *          -1 if 'pte_set' is invalid or 'pte_set_resize' fails.
 */
int pte_set_init(pte_set_t *pte_set, size_t alloc)
{
  if (!pte_set)
    return -1;

  *pte_set = (const pte_set_t) {0};
  pte_set->alloc = alloc;

  if (alloc == 0)
    return 0;

  return pte_set_resize(pte_set, alloc);
}

/*
 * Allocate more memory for new PTE pointers, then push the provided 'page'
 * pointer into the PTE set.
 *
 * @return:  0 on success.
 *          -1 if 'pte_set' is invalid or it can't be resized.
 */
int pte_set_push(pte_set_t *pte_set, void *page)
{
  size_t alloc;

  if (!pte_set)
    return -1;

  if (pte_set->npte >= pte_set->alloc) {
    if (pte_set_resize(pte_set, pte_set->alloc + 64) < 0)
      return -1;
  }

  pte_set->data[pte_set->npte++] = page;

  return 0;
}

void pte_set_destroy(pte_set_t *pte_set)
{
  if (!pte_set)
    return;

  if (pte_set->data)
    free(pte_set->data);

  *pte_set = (const pte_set_t) {0};
}

static void print_frame(FILE *f)
{
  fprintf(f, "-------------------------------------------\n");
}

void pte_set_info(pte_set_t *pte_set, FILE *f)
{
  size_t l1_set;
  size_t l2_set;

  if (!pte_set)
    return;

  print_frame(f);

  fprintf(f, "[   #] PTE page address | L1-dTLB | L2-sTLB\n");
  print_frame(f);

  for (size_t i = 0; i < pte_set->npte; i++) {
    l1_set = l1_dtlb_set(pte_set->data[i]);
    l2_set = l2_stlb_set(pte_set->data[i]);

    if (pte_set->trel && (i == TREL_NPTE_L1E || i == (TREL_NPTE_L1E + TREL_NPTE_L2E)))
      print_frame(f);

    fprintf(f, "[%04lu] 0x%p | %7lu | %7lu\n", i, pte_set->data[i], l1_set, l2_set);
  }

  print_frame(f);
}

/* PTE Set build. ------------------------------------------------------------*/

int pte_set_build_strd(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte,
  size_t offset, size_t stride)
{
  void *page;
  uintptr_t area;

  size_t size;
  size_t npte_max;

  if (!vm_area || !pte_set)
    return -1;

  area = (uintptr_t) vm_area->area;
  size = vm_area->size_a;

  // Check 'stride'.
  if (stride < (4 * KIB) || !IS_POWER_2(stride))
    stride = (4 * KIB);

  npte_max = size / stride;

  if (npte > npte_max)
    npte = npte_max;

  pte_set_init(pte_set, npte);

  offset = (offset % size) & ~0xFFFUL;

  for (size_t i = 0; i < npte; i++) {
    page = (void *) (area + offset);

    pte_set_push(pte_set, page);

    offset = (offset + stride) % size;
  }

  pte_set->trel = 0;

  return 0;
}

int pte_set_build_sequ(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte,
  size_t offset)
{
  return pte_set_build_strd(vm_area, pte_set, npte, offset, 0);
}

int pte_set_build_rand(vm_area_t *vm_area, pte_set_t *pte_set, size_t npte)
{
  void *page;

  uintptr_t area;

  size_t size;
  size_t offset;
  size_t npte_max;

  if (!vm_area || !pte_set)
    return -1;

  area = (uintptr_t) vm_area->area;
  size = vm_area->size_a;

  npte_max = size / (4 * KIB);

  if (npte > npte_max)
    npte = npte_max;

  pte_set_init(pte_set, npte);

  for (size_t i = 0; i < npte; i++) {
    offset = (((size_t) rand()) % size) & ~0xFFFUL;
    page = (void *) (area + offset);

    pte_set_push(pte_set, page);
  }

  return 0;
}

/* Build target-relative (T-Rel). --------------------------------------------*/

#define TREL_MIN_SIZE (TLB_L2_STLB_WAYS * TLB_SECTOR_BYTES)

int pte_set_build_trel_core(vm_area_t *vm_area, pte_set_t *pte_set,
  size_t npte_l1e, size_t npte_l2e, size_t npte_l1f, void *target)
{
  void *page;

  uint32_t hi;
  uint32_t lo;

  uintptr_t area;

  size_t size;
  size_t sector;
  size_t offset;
  size_t l1_set;
  size_t l2_set;
  size_t npte_l2e_max;

  if (!vm_area || !pte_set || !target)
    return -1;

  area = (uintptr_t) vm_area->area;
  size = vm_area->size_a;

  // Make sure that 'area' has a suitable minimum size.
  if (size < TREL_MIN_SIZE)
    return -1;

  npte_l2e_max = size / TLB_SECTOR_BYTES;

  if (npte_l2e > npte_l2e_max)
    npte_l2e = npte_l2e_max;

  pte_set_init(pte_set, npte_l1e + npte_l2e + npte_l1f);

  l1_set = l1_dtlb_set(target);
  l2_set = l2_stlb_set(target);

  // 1. L1-dTLB set eviction.
  for (size_t i = 0; i < npte_l1e; i++) {
    offset = ((i * TLB_L1_DTLB_SETS) + l1_set) * (4 * KIB);
    page = (void *) (area + offset);

    pte_set_push(pte_set, page);
  }

  // 2. L2-sTLB set eviction.
  sector = 1;
  for (size_t i = 0; i < npte_l2e; i++) {
    hi = (uint32_t) sector;
    lo = inv_7(hi, (uint32_t) l2_set);

    offset = ((hi << TLB_L2_STLB_XOR_BITS) | lo) * (4 * KIB);
    page = (void *) (area + offset);

    pte_set_push(pte_set, page);

    sector = (sector + 1) % TLB_SECTOR_COUNT;
  }

  // 3. L1-dTLB flooding.
  offset = sector * TLB_SECTOR_BYTES;
  for (size_t i = 0; i < npte_l1f; i++) {
    page = (void *) (area + offset);

    pte_set_push(pte_set, page);

    offset += (4 * KIB);
  }

  // PTE set is target-relative.
  pte_set->trel = 1;

  return 0;
}

int pte_set_build_trel(vm_area_t *vm_area, pte_set_t *pte_set, void *target)
{
  return pte_set_build_trel_core(vm_area, pte_set, TREL_NPTE_L1E,
    TREL_NPTE_L2E, TREL_NPTE_L1F, target);
}
