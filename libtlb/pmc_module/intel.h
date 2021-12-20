#pragma once

/*
 * Performance Event Select Register MSRs.
 * ---------------------------------------
 * Architectural Performance Monitoring facilities provide a number of
 * MSRs to invididually configure PMCs.
 *
 * References:
 * -Intel® 64 and IA-32 architectures software developer's manual combined
 *  volumes 3A, 3B, 3C, and 3D: System programming guide,
 *  Architectural Performance Monitoring Facilities (Section 18.2.1.1).
 *
 * -Intel® 64 and IA-32 architectures software developer's manual volume 4:
 *  Model-specific registers.
 */
#define IA32_PERFEVTSEL0 0x186U
#define IA32_PERFEVTSEL1 0x187U
#define IA32_PERFEVTSEL2 0x188U
#define IA32_PERFEVTSEL3 0x189U

/* Events (https://perfmon-events.intel.com/) --------------------------------*/

/*
 * Performance Event: Memory Load Retired.
 * ---------------------------------------
 * 01H : Retired load instructions with L1 cache hits as data sources.
 * 02H : Retired load instructions with L2 cache hits as data sources.
 * 04H : Retired load instructions with L3 cache hits as data sources.
 * 08H : Retired load instructions missed L1 cache as data sources.
 * 10H : Retired load instructions missed L2 cache as data sources.
 * 20H : Retired load instructions missed L3 cache as data sources.
 * 40H : L1 Miss but FB hit.
 */
#define MEM_LOAD_RETIRED 0xD1UL
// U-Masks
#define L1_HIT  0x01UL
#define L2_HIT  0x02UL
#define L3_HIT  0x04UL
#define L1_MISS 0x08UL
#define L2_MISS 0x10UL
#define L3_MISS 0x20UL
#define FB_HIT  0x40UL

/* Performance Event: Data Table-lookaside Buffer (dTLB) Load Misses. */
#define DTLB_LOAD_MISSES 0x08UL

/* Performance Event: Data Table-Lookaside Buffer (dTLB) Store Misses. */
#define DTLB_STORE_MISSES 0x49UL

/*
 * Common U-Masks for 'DTLB_LOAD_MISSES' and 'DTLB_STORE_MISSES'.
 * --------------------------------------------------------------
 * 01H: Load/Store misses in all TLB levels that cause a page walk of any page size.
 * 0EH: Load/Store misses in all TLB levels causes a page walk that completes.
 * 10H: Counts 1 per cycle for each PMH that is busy with a page walk for a load/store.
 * 20H: Load/Store that misses the dTLB but hits the sTLB.
 */
#define MISS_CAUSES_A_WALK 0x01UL
#define WALK_COMPLETED     0x0EUL
#define WALK_PENDING       0x10UL
#define STLB_HIT           0x20UL
