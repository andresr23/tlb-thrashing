#include "vm_area.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "shm.h"

/*
 * Map a virtual memory region that starts at an address that may, or may not,
 * be aligned. The 'area' region has a size 'size_a', and the complete mapping
 * that starts from 'base' has a size 'size_b'.
 *
 *        |                  |
 *        --------------------
 *   ^    |                  |
 *   |    |                  |
 *   |    |       ....       |  'size_a' = ('npages' * (4 * KIB)) bytes.
 *   |    |                  |
 *   |    |                  |
 *   |    -------------------- <- 'area' (aligned if 'align' != 0)
 *        |                  |
 *        |       ....       |
 *        |                  |
 *        -------------------- <- 'base' (not aligned)
 *        |                  |
 *
 * @return:  0 on success.
 *          -1 on failure.
 */
int vm_area_map(vm_area_t *vm_area, size_t npages, size_t align)
{
  int fd;

  void *area;
  void *base;

  void *r;
  void *page;

  size_t size_a;
  size_t size_b;

  if (!vm_area)
    return -1;

  if (align <= (4 * KIB) || !IS_POWER_2(align))
    align = 0;

  // Check if there is nothing to do.
  if (vm_area->mapped) {
    if (vm_area->npages == npages && vm_area->align == align)
      return 0;
    else
      vm_area_unmap(vm_area);
  }

  size_a = npages * (4 * KIB);

  if (align >= size_a) {
    size_b = align * 2;
  } else {
    if (align != 0)
      size_b = size_a + (align - (4 * KIB));
    else
      size_b = size_a;
  }

  // Initial anonymous mapping.
  base = mmap(NULL, size_b, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (base == MAP_FAILED) {
    PRINT_LOG("Initial anonymous mapping failed: %s.", strerror(errno));
    return -1;
  }

  vm_area->base = base;

  if (align != 0)
    vm_area->area = area = (void *) ALIGN_UP(base, align);
  else
    vm_area->area = area = base;

  vm_area->npages = npages;
  vm_area->size_a = size_a;
  vm_area->size_b = size_b;
  vm_area->align = align;

  vm_area->mapped = 1;
  return 0;

err_close:
  close(fd);

err_unmap:
  munmap(base, size_b);

  vm_area->mapped = 0;
  return -1;
}

void vm_area_unmap(vm_area_t *vm_area)
{
  if (vm_area->mapped)
    munmap(vm_area->base, vm_area->size_b);

  *vm_area = (const vm_area_t) {0};
}

void *vm_area_ptr(vm_area_t *vm_area, size_t offset)
{
  void *ptr;

  if (!vm_area)
    return NULL;

  offset %= vm_area->size_a;
  ptr = (void *) ((uintptr_t) vm_area->area + offset);

  return ptr;
}

static void print_info_frame(void)
{
  fprintf(stderr, "---------------------------------------------------------------\n");
}

void vm_area_info(vm_area_t *vm_area)
{
  print_info_frame();

  if (vm_area->mapped) {
    fprintf(stderr, "area @ 0x%p - 0x%p (%3lu-MB) | ", vm_area->area,
      vm_area->area + vm_area->size_a, vm_area->size_a >> 20);

    if (vm_area->align != 0) {
      fprintf(stderr, "Aligned\n");
      fprintf(stderr, "base @ 0x%p - 0x%p (%3lu-MB) | \n", vm_area->base,
        vm_area->base + vm_area->size_b, vm_area->size_b >> 20);
    } else {
      fprintf(stderr, "Unaligned\n");
    }
  } else {
    fprintf(stderr, "Virtual memory area not mapped.\n");
  }

  print_info_frame();

  fflush(stderr);
}
