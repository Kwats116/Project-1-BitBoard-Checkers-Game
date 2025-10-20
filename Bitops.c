#include "bitops.h"
#include <stdio.h>

/* Defensive: check position bounds 0..31.
   If invalid pos, return value unchanged (or 0 for getters). */

uint32_t SetBit32(uint32_t value, int position) {
    if (position < 0 || position >= 32) return value;
    return value | (1u << position);
}

uint32_t ClearBit32(uint32_t value, int position) {
    if (position < 0 || position >= 32) return value;
    return value & ~(1u << position);
}

uint32_t ToggleBit32(uint32_t value, int position) {
    if (position < 0 || position >= 32) return value;
    return value ^ (1u << position);
}

int GetBit32(uint32_t value, int position) {
    if (position < 0 || position >= 32) return 0;
    return (value >> position) & 1u;
}

/* Kernighan's algorithm */
int CountBits32(uint32_t value) {
    int count = 0;
    while (value) {
        value &= (value - 1);
        ++count;
    }
    return count;
}

uint32_t ShiftLeft32(uint32_t value, int positions) {
    if (positions < 0) return ShiftRight32(value, -positions);
    if (positions >= 32) return 0;
    return value << positions;
}

uint32_t ShiftRight32(uint32_t value, int positions) {
    if (positions < 0) return ShiftLeft32(value, -positions);
    if (positions >= 32) return 0;
    return value >> positions;
}

void PrintBinary32(uint32_t value) {
    for (int i = 31; i >= 0; --i) {
        putchar(((value >> i) & 1U) ? '1' : '0');
        if (i % 8 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}

void PrintHex32(uint32_t value) {
    printf("0x%08X\n", value);
}