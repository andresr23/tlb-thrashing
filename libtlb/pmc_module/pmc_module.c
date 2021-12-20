#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/msr.h>

#include "intel.h"

MODULE_LICENSE("GPL");

/* To enable the 'rdpmc' by setting the 9th bit of the CR4 register. */
static void enable_rdpmc(void)
{
  register uint64_t cr4;

  asm volatile(
    "mov %%cr4, %0"
    : "=r" (cr4)
    :
    :
  );

  cr4 |= 0x100UL;

  asm volatile(
    "mov %0, %%cr4"
    :
    : "r" (cr4)
    :
  );
}

/* Reset the 9th bit of the CR4 register to disable the 'rdpmc' instruction. */
static void disable_rdpmc(void)
{
  register uint64_t cr4;

  asm volatile(
    "mov %%cr4, %0"
    : "=r" (cr4)
    :
    :
  );

  cr4 &= ~0x100UL;

  asm volatile(
    "mov %0, %%cr4"
    :
    : "r" (cr4)
  );
}

/*
 * Configure a PMC counter by writing into the required MSR.
 *
 * Configuration layout for Intel's IA32 Architectural MSRs in accordance
 * to Table 2-2 in:
 * Intel 64 and IA-32 architectures software developer's
 * manual volume 4: Model-specific registers, Section 2.1 ARCHITECTURAL MSRS,
 * Page 2-3
 *
 * [32:24] - CounterMask = 0x00 -> Detect multiple events on each clock cycle
 * [23]    - CounterMask Invert
 * [22]    - Enable
 * [21]    - Reserved
 * [20]    - APIC interrupt Enable = 0
 * [19]    - Pin Control = 0
 * [18]    - Edge detection = 0
 * [17]    - OS Mode (Ring 0) = 1
 * [16]    - User Mode (1,2,3)= 1
 * [15:08] - Event Mask (UMask)
 * [07:00] - Event Select
 */
static void config_pmc(u32 pmc, uint64_t event, uint64_t mask)
{
  uint64_t _v0 = (event & 0xFFUL);      // Event.
  uint64_t _m0 = (mask  & 0xFFUL) << 8; // U-Mask.
  uint64_t _ou = 0x3 << 16;             // Count OS/User events - 11b.
  uint64_t _e0 = 0x1 << 22;             // Enable.
  _e0 |= (_v0 | _m0 | _ou);
  wrmsrl(pmc, _e0);
}

/*
 * Discard any PMC configuration.
 */
static void reset_pmc(u32 pmc)
{
  wrmsrl(pmc, 0x0UL);
}

/*
 * Configure PMCs as required. The 'pmc_init' function calls this function on
 * each logical core.
 */
static void init_pmc_func(void *info)
{
  // Count page-table walks.
  config_pmc(IA32_PERFEVTSEL0, DTLB_LOAD_MISSES, MISS_CAUSES_A_WALK);
  // Count L1-dTLB misses.
  config_pmc(IA32_PERFEVTSEL1, DTLB_LOAD_MISSES, MISS_CAUSES_A_WALK | STLB_HIT);
  // Count L1-dTLB misses / L2-sTLB hits.
  config_pmc(IA32_PERFEVTSEL2, DTLB_LOAD_MISSES, STLB_HIT);

  enable_rdpmc();
}

/*
 * Revert all the configuration made by 'init_pmc_func'. The 'pmc_exit'
 * function calls this function on each logical core.
 */
static void exit_pmc_func(void *info)
{
  reset_pmc(IA32_PERFEVTSEL0);
  disable_rdpmc();
}

/*
 * Module entry point. On installation the module writes to the corresponding
 * MSRs to configure the PMCs required by the user. The function additionally
 * enables the 'rdpmc' instruction to the userspace.
 */
static int __init pmc_init(void)
{
  on_each_cpu(init_pmc_func, NULL, 1);

  printk(KERN_INFO "[pmc] Configured PMCs in all execution cores.\n");

  return 0;
}

/*
 * Module exit point. On removal the module discards all the changes made
 * by the 'pmc_init' function.
 */
static void __exit pmc_exit(void)
{
  on_each_cpu(exit_pmc_func, NULL, 1);

  printk(KERN_INFO "[pmc] Discarded any PMC configuration in all execution cores.\n");
}

module_init(pmc_init);
module_exit(pmc_exit);
