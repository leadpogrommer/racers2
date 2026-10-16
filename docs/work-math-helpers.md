# Spline basis helpers

This batch covers three adjacent member functions in `mathhelpers.cpp`:

| VA | Bytes | C++ entry | Result |
| --- | ---: | --- | --- |
| `0x004d15e0` | `0x24` | `SplineBasis::SplineBasis(LegoU32, LegoFloat)` | 100% instruction match |
| `0x004d1610` | `0x1b5` | `SplineBasis::FUN_004d1610()` | Semantic reconstruction; retained as STUB (87.39% raw instruction match) |
| `0x004d17d0` | `0x2f` | `SplineBasis::FUN_004d17d0()` | 100% instruction match |

The layout is supported by the initializer and derivative accesses, plus a game
caller that allocates `0x88` bytes before invoking the constructor. The object
contains a 4x4 basis table at offset `0x00`, the corresponding 4x4 derivative
table at `0x40`, a mode at `0x80`, and a scalar parameter at `0x84`.
`FUN_004d17d0` writes each derivative row as `(0, b1, 2*b2, 3*b3)`. The
constructor stores its two arguments and invokes the initializer.

The initializer's branches for modes 1, 2, and 4 and its scalar-based default
case were recovered from the retail stores and floating-point operations. It
remains STUB because its current code compiles to 87.39% instruction accuracy.
The remaining differences are concentrated in the default case's x87 stack
handling and instruction scheduling; the source computes the three reciprocal
values in retail order and stores the recovered basis entries in retail order.
The functions are game-specific methods: the constructor is called both for an
allocated object and for a global object, and the initializer feeds the
derivative table consumed by neighboring path/spline routines.

The exact source manifest row for integration is:

```text
LEGO Racers 2/src/mathhelpers.cpp
```
