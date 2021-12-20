#include "pte.h"

#include "tlb.h"

static void print_frame(FILE *f)
{
  fprintf(f, "-------------------------------------------------------------\n");
}

void pte_info(void *page, FILE *f)
{
  page = (void *) (((uintptr_t) page) & TLB_VA_VPNM);

  print_frame(f);
  fprintf(f, "VPN = 0x%p | L1-dTLB set = %2d | L2-sTLB set = %3d\n",
    page, l1_dtlb_set(page), l2_stlb_set(page));
  print_frame(f);
}
