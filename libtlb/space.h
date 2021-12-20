#pragma once

#include <stdlib.h>

#include "vm_area.h"

/* API -----------------------------------------------------------------------*/

int tlb_space_map(vm_area_t *tlb_space);
void tlb_space_unmap(vm_area_t *tlb_space);

void *tlb_space_page(vm_area_t *tlb_space, size_t offset);
void *tlb_space_random_page(vm_area_t *tlb_space);
void *tlb_space_congruent_page(vm_area_t *tlb_space, size_t sector,
  void *target);

size_t tlb_space_random_offset_align(size_t align);
size_t tlb_space_random_offset_zbits(int zbits);
