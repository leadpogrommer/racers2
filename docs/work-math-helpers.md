# Spline basis helpers

This batch covers three adjacent member functions in `mathhelpers.cpp`:

| VA | Bytes | C++ entry | Result |
| --- | ---: | --- | --- |
| `0x004d15e0` | `0x24` | `SplineBasis::SplineBasis(LegoU32, LegoFloat)` | 100% instruction match |
| `0x004d1610` | `0x1b5` | `SplineBasis::FUN_004d1610()` | Semantic reconstruction; retained as STUB (89.45% raw instruction match) |
| `0x004d17d0` | `0x2f` | `SplineBasis::FUN_004d17d0()` | 100% instruction match |

The layout is supported by the initializer and derivative accesses, plus a game
caller that allocates `0x88` bytes before invoking the constructor. The object
contains a 4x4 basis table at offset `0x00`, the corresponding 4x4 derivative
table at `0x40`, a mode at `0x80`, and a scalar parameter at `0x84`.
`FUN_004d17d0` writes each derivative row as `(0, b1, 2*b2, 3*b3)`. The
constructor stores its two arguments and invokes the initializer.

The initializer's branches for modes 1, 2, and 4 and its scalar-based default
case were recovered from the retail stores and floating-point operations. A
bounded pass raised the default compiler build from 87.39% to 89.45% by
interleaving the local reciprocal calculations with the corresponding matrix
stores. A scalar copy local lowered the score to 85.83%. A direct-member reuse
form reached 89.45%, but was not retained because rereading a rounded matrix
entry can lose the extended-precision intermediate that retail reuses. The
retained source keeps local values for the negative reciprocal, twice
reciprocal, and reciprocal and assigns the corresponding entries in the
observed order. Remaining differences are concentrated in the default case's
x87 stack handling and instruction scheduling; only reccmp was used for this
pass, with no behavioral tests.

The constructor is called both for an allocated object and for a global object,
and the initializer feeds the derivative table consumed by neighboring
path/spline routines. These call sites establish its role in this executable;
the original engine/library ownership is not established.

The exact source manifest row for integration is:

```text
LEGO Racers 2/src/mathhelpers.cpp
```

## Quaternion helpers

`Quaternion` stores four consecutive `LegoFloat` values at offsets 0, 4, 8,
and 0xc. The packed decoder at 0x004a5440 writes those components, and its
animation callers advance records by 16 bytes. This supports the 0x10 size
assertion. `Quaternion::Normalize` at 0x004a5310 returns the magnitude, leaves
an exactly zero quaternion unchanged, and otherwise scales all four components
by the reciprocal. Its direct four-square source remains STUB at 85.37%.
`Vector3::Normalize` at 0x004a4c80 uses the analogous direct expression and
matches exactly, but the four-term x87 stack schedule differs. Reordered and
staged square-sum source forms did not change the quaternion score.

`PackedQuaternion::Pack` at 0x004a5380 is a 177-byte custom codec helper. It
rounds three quaternion components after multiplying by 511, writes each into
a signed 10-bit field, and stores the fourth component's negative flag in bit
30. Its final mask clears both bits 30 and 31 before setting bit 30, so the
upper two bits encode a one-bit flag with bit 31 always zero. Callers at
0x0044a76e and 0x0044a804 pass the start of 16-byte animation records as ECX
and a pointer to a quaternion as the argument; only the first word of those
records is modified. `PackedQuaternion` therefore models that word only and
has no whole-record size assertion. The implementation remains STUB at
34.23% in the integrated build. The final source reads the w sign before
clearing the top two packed bits, preserving the retail read/write order.
The largest mismatch is MSVC's integer-conversion helper versus
retail's inline `fistp` sequence: retail conversion follows the current x87
rounding mode, while C++ float-to-integer casts truncate toward zero. Retail's
inline conversion and EBP frame leave open whether this TU used `/QIfist` or
an inline conversion helper; this is unresolved evidence, not a build-flag
change for this batch. No asm or per-function flags were introduced. The
bitfield codec and surrounding decoder are custom game data handling; no CRT
or third-party object match was found, and authorship of the surrounding
animation format remains unidentified.

`Quaternion::Decode` at 0x004a5440 spans 0xb7 bytes through its `ret 4` and
is also retained as STUB. It extracts three signed 10-bit fields, multiplies
them by the literal at 0x00535368 (approximately 1/511), then reconstructs
the magnitude of w from `r = 1 - (x*x + y*y + z*z)`: a positive r uses
`sqrt(r)`, while a nonpositive or unordered r sets w to zero. Either of the top
two packed bits makes w negative; this is why the decoder tests mask
0xc0000000 even though the current encoder clears bit 31. It normalizes the
result before returning `this`. The implementation matches 83.87% in both
integrated builds, using the complete 183-byte extent ending at 0x004a54f6.
The current natural source writes x/y/z before calculating
the remaining square, follows the zero and positive w paths separately, and
calls `Normalize` in each path. Remaining differences include x87 stack
scheduling, the zero store/branch shape, and operand scheduling in the
square calculation. No volatile access or instruction-level workaround was
used. The literal map includes 0x00535368 as a four-byte float (bits
0x3b004020).

Decoder callers at 0x0044a63c and 0x0044a680 use 16-byte quaternion
destinations/temporaries within animation processing. This establishes the
record stride and object extent, not an external library identity. The codec
is custom application data handling; no CRT or third-party decoder match was
identified.
