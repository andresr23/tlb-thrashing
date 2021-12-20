#pragma once

#include <stdlib.h>

typedef struct {
  int mapped;
  void *area;
  void *base;
  size_t npages;
  size_t size_a;
  size_t size_b;
  size_t align;
} vm_area_t;

/* API -----------------------------------------------------------------------*/

// Map/unmap an entire virtual memory area (anonymous mapping).
int vm_area_map(vm_area_t *vm_area, size_t npages, size_t align);
void vm_area_unmap(vm_area_t *vm_area);

// Get a pointer within a virtual memory area.
void *vm_area_ptr(vm_area_t *vm_area, size_t offset);

// Print information.
void vm_area_info(vm_area_t *vm_area);
