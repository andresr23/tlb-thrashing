#pragma once

#include <stdio.h>

#include "vm_area.h"

/* API -----------------------------------------------------------------------*/

int test_prepare(vm_area_t **tlb_space_s, vm_area_t **tlb_space_t);
void test_cleanup(void);

// Timer.
void test_timer_reset(void);
void test_timer_print(void);
void test_timer_start(void);
void test_timer_end(void);

void test_print_align(size_t align, FILE *f);
