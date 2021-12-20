#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtlb/pte.h"
#include "libtlb/test.h"
#include "libtlb/space.h"
#include "libtlb/pte_set.h"

static vm_area_t *tlb_space_s = NULL;
static vm_area_t *tlb_space_t = NULL;

int main()
{
  void *target_pte;

  pte_set_t pte_set = {0};

  if (test_prepare(&tlb_space_s, &tlb_space_t))
    return -1;

  target_pte = tlb_space_random_page(tlb_space_t);

  fprintf(stdout, "Target PTE.\n");
  pte_info(target_pte, stdout);
  fprintf(stdout, "\n");

  pte_set_build_trel(tlb_space_s, &pte_set, target_pte);

  fprintf(stdout, "Target-relative PTE Set.\n");
  pte_set_info(&pte_set, stdout);
  fprintf(stdout, "\n");

  test_cleanup();
}
