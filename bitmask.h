/*
 * bitmask.h - single header library for bitmask operations
 * License: Public Domain
 * Author: Ferki
 * Version: 1.0
 * Requirements: C99, GCC/Clang
 */

#ifndef BITMASK_H
#define BITMASK_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * CONFIGURATION MACROS
 * ============================================================================ */

#ifndef BITMASK_FORCE_INLINE
#ifdef __GNUC__
#define BITMASK_FORCE_INLINE __attribute__((always_inline)) static inline
#else
#define BITMASK_FORCE_INLINE static inline
#endif
#endif

#ifndef BITMASK_PACKED
#ifdef __GNUC__
#define BITMASK_PACKED __attribute__((packed))
#else
#define BITMASK_PACKED
#endif
#endif

/* ============================================================================
 * CONSTANTS AND MACROS
 * ============================================================================ */

#define BIT(n) ((uint64_t)1 << (n))
#define BITMASK64_ALL ((uint64_t)0xFFFFFFFFFFFFFFFF)
#define BITMASK32_ALL ((uint32_t)0xFFFFFFFF)

#define MASK_FROM_TO(start, end) (bitmask_make_mask((start), (end)))

#define BITMASK_INVALID_INDEX ((uint8_t)64)

/* ============================================================================
 * TYPE DEFINITIONS
 * ============================================================================ */

typedef struct
{
    uint64_t bits;
} bitmask64_t;

typedef struct
{
    uint32_t bits;
} bitmask32_t;

/* ============================================================================
 * FUNCTION DECLARATIONS (PROTOTYPES)
 * ============================================================================ */

/* Core bit operations */
BITMASK_FORCE_INLINE void bit_set(volatile uint64_t* mask, uint8_t bit);
BITMASK_FORCE_INLINE void bit_clear(volatile uint64_t* mask, uint8_t bit);
BITMASK_FORCE_INLINE void bit_toggle(volatile uint64_t* mask, uint8_t bit);
BITMASK_FORCE_INLINE bool bit_check(uint64_t mask, uint8_t bit);

/* Range operations */
BITMASK_FORCE_INLINE void bits_set_range(volatile uint64_t* mask, 
                                         uint8_t start, uint8_t end);
BITMASK_FORCE_INLINE void bits_clear_range(volatile uint64_t* mask, 
                                           uint8_t start, uint8_t end);
BITMASK_FORCE_INLINE uint64_t bits_get_range(uint64_t mask, 
                                             uint8_t start, uint8_t end);
BITMASK_FORCE_INLINE void bits_write_range(volatile uint64_t* mask, 
                                           uint8_t start, uint8_t end, 
                                           uint64_t value);

/* Bit counting and scanning */
BITMASK_FORCE_INLINE uint8_t bit_scan_forward(uint64_t mask);
BITMASK_FORCE_INLINE uint8_t bit_scan_reverse(uint64_t mask);
BITMASK_FORCE_INLINE uint8_t popcount(uint64_t mask);

/* Bitmask structure operations */
BITMASK_FORCE_INLINE void bitmask64_init(bitmask64_t* mask);
BITMASK_FORCE_INLINE void bitmask64_set(bitmask64_t* mask, uint8_t bit);
BITMASK_FORCE_INLINE bool bitmask64_check(bitmask64_t mask, uint8_t bit);

/* Utility helpers */
BITMASK_FORCE_INLINE uint64_t bitmask_make_mask(uint8_t start, uint8_t end);

/* ============================================================================
 * MACROS FOR ITERATION
 * ============================================================================ */

#define BITMASK_FOREACH_SET(mask, bit_var) \
{ \
    uint64_t __iter_mask = (mask); \
    uint8_t bit_var; \
    while (__iter_mask) \
    { \
        bit_var = bit_scan_forward(__iter_mask); \
        __iter_mask &= __iter_mask - 1;

#define BITMASK_FOREACH_END } }

#define BITMASK_FOREACH_SET_PTR(mask_ptr, bit_var) \
    BITMASK_FOREACH_SET((mask_ptr)->bits, bit_var)

/* ============================================================================
 * FUNCTION IMPLEMENTATIONS
 * ============================================================================ */

#ifdef BITMASK_IMPLEMENTATION

/* ----------------------------------------------------------------------------
 * Core bit operations
 * ---------------------------------------------------------------------------- */

BITMASK_FORCE_INLINE void bit_set(volatile uint64_t* mask, uint8_t bit)
{
    if (bit < 64)
    {
        *mask |= BIT(bit);
    }
}

BITMASK_FORCE_INLINE void bit_clear(volatile uint64_t* mask, uint8_t bit)
{
    if (bit < 64)
    {
        *mask &= ~BIT(bit);
    }
}

BITMASK_FORCE_INLINE void bit_toggle(volatile uint64_t* mask, uint8_t bit)
{
    if (bit < 64)
    {
        *mask ^= BIT(bit);
    }
}

BITMASK_FORCE_INLINE bool bit_check(uint64_t mask, uint8_t bit)
{
    if (bit >= 64)
    {
        return false;
    }

    return (mask >> bit) & 1;
}

BITMASK_FORCE_INLINE uint64_t bitmask_make_mask(uint8_t start, uint8_t end)
{
    uint8_t width;

    if ((start > end) || (start >= 64))
    {
        return 0;
    }

    if (end >= 63)
    {
        end = 63;
    }

    width = (uint8_t)(end - start + 1);

    if (width == 64)
    {
        return BITMASK64_ALL;
    }

    return ((UINT64_C(1) << width) - 1) << start;
}

/* ----------------------------------------------------------------------------
 * Range operations
 * ---------------------------------------------------------------------------- */

BITMASK_FORCE_INLINE void bits_set_range(volatile uint64_t* mask, 
                                         uint8_t start, uint8_t end)
{
    *mask |= bitmask_make_mask(start, end);
}

BITMASK_FORCE_INLINE void bits_clear_range(volatile uint64_t* mask, 
                                           uint8_t start, uint8_t end)
{
    *mask &= ~bitmask_make_mask(start, end);
}

BITMASK_FORCE_INLINE uint64_t bits_get_range(uint64_t mask, 
                                             uint8_t start, uint8_t end)
{
    uint64_t range_mask = bitmask_make_mask(start, end);
    return (mask & range_mask) >> start;
}

BITMASK_FORCE_INLINE void bits_write_range(volatile uint64_t* mask, 
                                           uint8_t start, uint8_t end, 
                                           uint64_t value)
{
    uint64_t range_mask = bitmask_make_mask(start, end);
    uint64_t clear_mask = ~range_mask;
    uint64_t write_mask = (value << start) & range_mask;
    *mask = (*mask & clear_mask) | write_mask;
}

/* ----------------------------------------------------------------------------
 * Bit counting and scanning
 * ---------------------------------------------------------------------------- */

BITMASK_FORCE_INLINE uint8_t bit_scan_forward(uint64_t mask)
{
    uint8_t idx = 0;
    
    if (mask == 0)
    {
        return 64;
    }
    
    /* Stepwise search for least significant set bit */
    if ((mask & 0xFFFFFFFF) == 0)
    {
        idx += 32;
        mask >>= 32;
    }
    
    if ((mask & 0xFFFF) == 0)
    {
        idx += 16;
        mask >>= 16;
    }
    
    if ((mask & 0xFF) == 0)
    {
        idx += 8;
        mask >>= 8;
    }
    
    if ((mask & 0xF) == 0)
    {
        idx += 4;
        mask >>= 4;
    }
    
    if ((mask & 0x3) == 0)
    {
        idx += 2;
        mask >>= 2;
    }
    
    if ((mask & 0x1) == 0)
    {
        idx += 1;
    }
    
    return idx;
}

BITMASK_FORCE_INLINE uint8_t bit_scan_reverse(uint64_t mask)
{
    uint8_t idx = 0;
    uint64_t tmp;
    
    if (mask == 0)
    {
        return 64;
    }
    
    /* Stepwise search for most significant set bit */
    tmp = mask >> 32;
    if (tmp)
    {
        idx += 32;
        mask = tmp;
    }
    
    tmp = mask >> 16;
    if (tmp)
    {
        idx += 16;
        mask = tmp;
    }
    
    tmp = mask >> 8;
    if (tmp)
    {
        idx += 8;
        mask = tmp;
    }
    
    tmp = mask >> 4;
    if (tmp)
    {
        idx += 4;
        mask = tmp;
    }
    
    tmp = mask >> 2;
    if (tmp)
    {
        idx += 2;
        mask = tmp;
    }
    
    tmp = mask >> 1;
    if (tmp)
    {
        idx += 1;
    }
    
    return idx;
}

BITMASK_FORCE_INLINE uint8_t popcount(uint64_t mask)
{
    /* Hamming weight algorithm for 64-bit integers */
    mask = mask - ((mask >> 1) & 0x5555555555555555);
    mask = (mask & 0x3333333333333333) + ((mask >> 2) & 0x3333333333333333);
    mask = (mask + (mask >> 4)) & 0x0F0F0F0F0F0F0F0F;
    mask = (mask * 0x0101010101010101) >> 56;
    
    return (uint8_t)mask;
}

/* ----------------------------------------------------------------------------
 * Bitmask structure operations
 * ---------------------------------------------------------------------------- */

BITMASK_FORCE_INLINE void bitmask64_init(bitmask64_t* mask)
{
    mask->bits = 0;
}

BITMASK_FORCE_INLINE void bitmask64_set(bitmask64_t* mask, uint8_t bit)
{
    bit_set(&mask->bits, bit);
}

BITMASK_FORCE_INLINE bool bitmask64_check(bitmask64_t mask, uint8_t bit)
{
    return bit_check(mask.bits, bit);
}

/* ----------------------------------------------------------------------------
 * Utility functions
 * ---------------------------------------------------------------------------- */

BITMASK_FORCE_INLINE uint64_t bitmask_merge(uint64_t mask1, uint64_t mask2)
{
    return mask1 | mask2;
}

BITMASK_FORCE_INLINE uint64_t bitmask_intersect(uint64_t mask1, uint64_t mask2)
{
    return mask1 & mask2;
}

BITMASK_FORCE_INLINE uint64_t bitmask_difference(uint64_t mask1, uint64_t mask2)
{
    return mask1 & ~mask2;
}

BITMASK_FORCE_INLINE bool bitmask_is_subset(uint64_t subset, uint64_t superset)
{
    return (subset & superset) == subset;
}

BITMASK_FORCE_INLINE bool bitmask_is_empty(uint64_t mask)
{
    return mask == 0;
}

BITMASK_FORCE_INLINE bool bitmask_is_full(uint64_t mask)
{
    return mask == BITMASK64_ALL;
}

#endif /* BITMASK_IMPLEMENTATION */

#endif /* BITMASK_H */
