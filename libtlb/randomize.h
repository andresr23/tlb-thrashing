#pragma once

#include "pte_set.h"

/* API -----------------------------------------------------------------------*/

int tlb_randomize_map(void);
void tlb_randomize_unmap(void);

int tlb_randomize_set_build(pte_set_t *pte_set);
void tlb_randomize_set_destroy(pte_set_t *pte_set);
