/*
 * example.c - demonstration of bitmask library
 */

#define BITMASK_IMPLEMENTATION
#include "bitmask.h"
#include <stdio.h>

int main(void)
{
    uint64_t flags = 0;
    bitmask64_t perm_mask;
    uint8_t bit_idx;
    
    /* Initialize bitmask structure */
    bitmask64_init(&perm_mask);
    
    /* Set individual bits */
    bit_set(&flags, 0);  /* Bit 0 */
    bit_set(&flags, 3);  /* Bit 3 */
    bitmask64_set(&perm_mask, 5);
    
    /* Check bits */
    if (bit_check(flags, 0))
    {
        printf("Bit 0 is set\n");
    }
    
    /* Range operations */
    bits_set_range(&flags, 8, 15);
    uint64_t extracted = bits_get_range(flags, 8, 15);
    bits_write_range(&flags, 16, 23, 0xAB);
    
    /* Count set bits */
    uint8_t count = popcount(flags);
    printf("Number of set bits: %u\n", count);
    
    /* Iterate through set bits */
    printf("Set bits in flags: ");
    BITMASK_FOREACH_SET(flags, bit_idx)
    {
        printf("%u ", bit_idx);
    }
    BITMASK_FOREACH_END
    printf("\n");
    
    /* Advanced operations */
    uint64_t mask_a = 0x0F;
    uint64_t mask_b = 0x33;
    
    uint64_t merged = bitmask_merge(mask_a, mask_b);
    uint64_t common = bitmask_intersect(mask_a, mask_b);
    bool is_sub = bitmask_is_subset(0x03, mask_a);
    
    /* Find first set bit */
    uint8_t first_bit = bit_scan_forward(flags);
    if (first_bit != 64)
    {
        printf("First set bit at position: %u\n", first_bit);
    }
    
    return 0;
}