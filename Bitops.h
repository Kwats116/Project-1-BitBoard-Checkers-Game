#ifndef BITOPS_H
#define BITOPS_H

#include <stdint.h>

/* Basic bit ops for 32-bit values (Phase 1) */
uint32_t SetBit32(uint32_t value, int position);
uint32_t ClearBit32(uint32_t value, int position);
uint32_t ToggleBit32(uint32_t value, int position);
int GetBit32(uint32_t value, int position);
int CountBits32(uint32_t value);
uint32_t ShiftLeft32(uint32_t value, int positions);
uint32_t ShiftRight32(uint32_t value, int positions);
void PrintBinary32(uint32_t value);
void PrintHex32(uint32_t value);

#endif // BITOPS_H