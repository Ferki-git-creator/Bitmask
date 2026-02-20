# Bitmask

Minimal single-header C99 library for efficient bitmask manipulation. Provides inline functions for setting, clearing, checking individual bits, working with bit ranges, counting set bits (popcount), and iterating through set bits. Designed for embedded systems and performance-critical applications with zero memory allocations, platform independence.

## Safety notes

- Bit indices outside `0..63` are ignored by mutating functions and return `false` for checks.
- Range helpers now safely handle full-width ranges (e.g. `0..63`) and invalid ranges (`start > end`) without undefined shifts.
