#pragma once

#include <stdint.h>

#define XOR_7_VA_MASK 0x000003FFF000UL // 14 LSBs of a VPN for XOR-7.
#define XOR_8_VA_MASK 0x00000FFFF000UL // 16 LSBs of a VPN for XOR-8.

/* API -----------------------------------------------------------------------*/

uint32_t xor_7(uint32_t v);
uint32_t xor_8(uint32_t v);

uint32_t xor_7_fast(uint32_t v);
uint32_t xor_8_fast(uint32_t v);

uint32_t xor_7_virt(void *virt);
uint32_t xor_8_virt(void *virt);

uint32_t inv_7(uint32_t hi, uint32_t i);
uint32_t inv_8(uint32_t hi, uint32_t i);
