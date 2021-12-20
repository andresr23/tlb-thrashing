#include "pte.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "tlb.h"
#include "shm.h"
#include "vm_area.h"

#define RANDOMIZE_NPTE 3200

/* Virtual Memory to randomize the TLB state. --------------------------------*/

static vm_area_t vm_area_randomize = {0};

int tlb_randomize_map(void)
{
  int fd;
  void *page;

  if (vm_area_randomize.mapped)
    vm_area_unmap(&vm_area_randomize);

  if (vm_area_map(&vm_area_randomize, RANDOMIZE_NPTE, 0)) {
    PRINT_LOG("Failed to map memory for tlb_randomize.");
    return -1;
  }

  if ((fd = shm_open_or_create_default()) < 0) {
    PRINT_LOG("Failed to open the shared-memory object.");
    return -1;
  }

  // Make redundant mappings to the shared-memory object.
  page = vm_area_randomize.area;
  for (size_t i = 0; i < RANDOMIZE_NPTE; i++) {
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
  vm_area_unmap(&vm_area_randomize);
}

void tlb_randomize_unmap(void)
{
  vm_area_unmap(&vm_area_randomize);
}

void tlb_randomize_info(void)
{
  fprintf(stderr,"\ntlb_randomize space.\n");
  vm_area_info(&vm_area_randomize);
}

/* PTE Set to randomize the TLB state. ---------------------------------------*/

int tlb_randomize_set_build(pte_set_t *pte_set)
{
  if (!vm_area_randomize.mapped)
    tlb_randomize_map();

  return pte_set_build_rand(&vm_area_randomize, pte_set, RANDOMIZE_NPTE);
}

void tlb_randomize_set_destroy(pte_set_t *pte_set)
{
  pte_set_destroy(pte_set);
}
