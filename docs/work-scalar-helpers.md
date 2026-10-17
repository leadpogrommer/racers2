# Scalar helper batch

The helper at `0x004a5dc0` returns the smallest signed power of two greater
than or equal to its argument, checking powers from `1` through `1 << 31` with
a signed comparison. Inputs at or below one return one; values above
`0x40000000` reach the `cl == 32` fallback and return zero. The shift value is
formed as unsigned before conversion so the source avoids signed-shift
undefined behavior while preserving the original signed comparison. Callers
use it to round integer dimensions, including geometry values at `0x004c3738`
and `0x004d9b2c`.

The helper at `0x004a5df0` counts trailing zero bits, with zero returning zero.
The original tests for zero and an already-set low bit once, then arithmetic
shifts until the low bit becomes set; this also produces the expected count
for negative bit patterns. Callers at `0x004d2d95` use it to derive aligned
dimensions and related counts.

Neither address appears in the pinned CRT candidate scan. Both reconstructed
bodies match retail instructions at 100% in the isolated VC6 build. Verified
lengths, including the final `ret`, are 34 bytes and 24 bytes respectively.

## Sine table generation

`BuildSineTable` at `0x004a5e10` fills `p_count` floats with one sine period.
Its complete 72-byte cdecl body ends at `0x004a5e57`. Retail first computes
`1.0f / p_count` even when the loop count is nonpositive, then converts each
integer index to floating point, multiplies by the single-precision pi at
`0x00535360` (bits `0x40490fdb`), multiplies by the cached reciprocal, doubles
the angle, and applies x87 FSIN. The reconstruction preserves this order;
a sequential local angle calculation matches 100% under the existing flags.
The first expression form was only an effective match because the compiler
commuted the two multiplications. Callers include `0x0043bf8f`,
`0x0048abfe`, `0x004cf8af`, and `0x005011a1`; these pass a float output array
and count. No original engine/library ownership is asserted.
