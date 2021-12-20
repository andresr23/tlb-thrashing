#include "space.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "tlb.h"
#include "shm.h"
#include "macros.h"

int tlb_space_map(vm_area_t *tlb_space)
{
  int fd;
  void *page;

  if (!tlb_space)
    return -1;

  if (vm_area_map(tlb_space, TLB_SPACE_PAGES, TLB_SPACE_BYTES)) {
    PRINT_LOG("Failed to map a virtual-memory area to spawn a TLB space.");
    return -1;
  }

  if ((fd = shm_open_or_create_default()) < 0) {
    PRINT_LOG("Failed to open the shared-memory object.");
    return -1;
  }

  // Make redundant mappings to the shared-memory object.
  page = tlb_space->area;
  for (size_t i = 0; i < TLB_SPACE_PAGES; i++) {
    if (shm_remap(page, 4 * KIB, fd) == MAP_FAILED) {
      PRINT_LOG("Failed to make a redundant mapping (i = %lu): %s.", i,
        strerror(errno));
      goto err_close;
    }

    page += (4 * KIB);
  }

  return 0;

err_close:
  close(fd);

err_unmap:
  vm_area_unmap(tlb_space);

  return -1;
}

void tlb_space_unmap(vm_area_t *tlb_space)
{
  vm_area_unmap(tlb_space);
}

void *tlb_space_page(vm_area_t *tlb_space, size_t offset)
{
  void *page;

  if (!tlb_space)
    return NULL;

  offset %= tlb_space->size_a;
  offset = ALIGN_DOWN(offset, 4 * KIB);

  page = (void *) ((uintptr_t) tlb_space->area + offset);
  return page;
}

void *tlb_space_random_page(vm_area_t *tlb_space)
{
  return tlb_space_page(tlb_space, (size_t) rand());
}

void *tlb_space_congruent_page(vm_area_t *tlb_space, size_t sector,
  void *target)
{
  void *page;

  uint32_t hi, lo;
  uint32_t l2_set;

  size_t offset;

  if (!tlb_space)
    return NULL;

  l2_set = (uint32_t) l2_stlb_set(target);

  hi = sector;
  lo = inv_7(hi, l2_set);

  offset = ((hi << TLB_L2_STLB_XOR_BITS) | lo) << PAGE_OFFSET;
  offset %= tlb_space->size_a;

  page = (void *) ((uintptr_t) tlb_space->area + offset);

  return page;
}

size_t tlb_space_random_offset_align(size_t align)
{
  size_t offset = ((size_t) rand()) % TLB_SPACE_BYTES;

  if (align < (4 * KIB) || !IS_POWER_2(align))
    align = 4 * KIB;

  offset = ALIGN_DOWN(offset, align);
  return offset;
}

size_t tlb_space_random_offset_zbits(int zbits)
{
  size_t align = tlb_zbits_to_align(zbits);
  return tlb_space_random_offset_align(align);
}
