# Spline evaluation helpers

This source unit contains eight recovered spline and path helpers:

| VA | Bytes | C++ entry | Result |
| --- | ---: | --- | --- |
| `0x004d0f60` | 607 (`0x25f`) | `Vector3* SplinePath::FUN_004d0f60(Vector3*, LegoS32, LegoFloat)` | STUB, 48.34% raw instruction match |
| `0x004d11c0` | 461 (`0x1cd`) | `Vector3* SplinePath::FUN_004d11c0(Vector3*, LegoS32, LegoFloat)` | STUB, 47.13% raw instruction match |
| `0x004d1390` | 588 (`0x24c`) | `Vector3* SplinePath::FUN_004d1390(Vector3*, LegoS32, LegoFloat)` | STUB, 47.00% raw instruction match |
| `0x004d1800` | 188 (`0xbc`) | `FUN_004d1800(const Vector3*, Vector3*)` | STUB, 65.28% raw instruction match |
| `0x004d1a20` | 77 (`0x4d`) | `SplinePath::FUN_004d1a20()` | 100% instruction match |
| `0x004d1a70` | 39 (`0x27`) | `SplinePath::FUN_004d1a70()` | 100% instruction match |
| `0x004d1aa0` | 747 (`0x2eb`) | `SplinePath::FUN_004d1aa0(LegoS32, LegoFloat, LegoFloat, LegoS32)` | STUB, 39.73% raw instruction match |
| `0x004d1d90` | 474 (`0x1da`) | `SplinePath::FUN_004d1d90(Vector3*, LegoFloat, LegoS32*)` | STUB, 47.71% raw instruction match |

`FUN_004d1800` takes a reference vector and a mutable vector. The disassembly
backs up the mutable vector, computes three components from both vectors,
updates the mutable vector, and checks its squared length against the positive
zero literal at `0x0052f2f0`. It normalizes the new vector when nonzero and
restores the saved vector when zero. The helper is called from `FUN_004d1980`
while constructing spline-frame data; normalization reuses the already
recovered `Vector3::Normalize` method. The implementation follows the observed
formulas and restore path, but remains STUB because its raw match is 65.28%.

`SplinePath::FUN_004d1a70` sums the cached segment lengths and stores and returns
the total. Its caller at `0x0045dc2e` discards the floating-point return, which
supports the `LegoFloat` return type. `FUN_004d1aa0` accesses the same segment
array at a `0x20` stride, reads each length at offset `0x18`, and reads flags at
`0x1c`. The related path fields are supported by `FUN_004d1a20`,
`FUN_004d1aa0`, and callers: segment-array pointer at `0x58`, closed-path flag
at `0x5c`, segment count at `0x60`, and total length at `0x68`. The complete
`SplinePath` extent beyond the recovered cache and basis pointer remains
unknown, so the header models only this prefix and has no size assertion.

The three evaluation helpers take an output `Vector3*`, a segment index, and a
parameter `t`. Callers supply these three arguments using the x86 thiscall convention,
and each callee returns with `ret 0xc`. All three choose adjacent
segments using the count at path offset `0x60` and closed-path flag at `0x5c`.
The flag at segment offset `0x1c`, bit zero, selects a linear special case.
Otherwise, `0x4d0f60` and `0x4d1390` evaluate the basis coefficients at the
`SplineBasis*` stored at path offset `0x174`; `0x4d11c0` evaluates its derivative
coefficients. The functions use the two 12-byte vectors at segment offsets
`0x00` and `0x0c`. Their field names stay offset-based because their higher-level
meaning is not established by these functions alone.

Each of the three helpers leaves the output pointer in EAX on every return path;
their x87 stack is consumed by stores and does not carry a return value. Their
callers discard EAX, but declaring `Vector3*` matches this observed ABI and
improves the reccmp scores. Basis coefficients are indexed as the four rows of
the recovered 4-by-4 `SplineBasis` arrays.

`FUN_004d1aa0` calculates a partial or full segment length. It handles zero
parameters, the linear segment flag, or smooth curves sampled at `1/16`
increments. Its cache state is now represented from the observed offsets:
last segment and sample indices at `0x6c` and `0x70`, 16 cached vectors at
`0x74`, and 16 cached distances at `0x134`. The constructor `FUN_004d0c80`
initializes both indices to `-1` and stores the `SplineBasis*` at `0x174`.
The sample step and its one-time initialization flag map to zero-initialized
data at `0x0067b79c` and `0x0067b6f8`; the PE `.data` virtual extent covers both
addresses, and the function sets the step before reading it.

`SplinePath::FUN_004d1a20` calls the evaluator for each segment with `t=1.0`,
start parameter `0.0`, and the length-store flag set, then stores and returns
the accumulated path length. Its 77-byte body matches raw 100%. The three
vector evaluators return their output pointer in EAX on every path; callers
discard it. Their x87 stack is consumed by stores, so the pointer return is
supported by both return-register state and the non-floating return behavior.

`FUN_004d1d90` maps a path distance back to a segment and normalized segment
parameter. The caller at `0x0045db36` passes a `Vector3*` output, the distance,
and a `LegoS32*` for the selected segment index; the callee returns the
normalized parameter in ST0. It wraps/clamps the distance according to the
closed-path flag, walks cached segment lengths, then refines the initial
fraction with at most six evaluations of `FUN_004d1aa0`. The constants used by
that refinement were read from the original image: `1.0` at `0x0052f2d0`,
positive zero at `0x0052f2f0`, `0.01` at `0x0052f3bc`, and `0.5` at
`0x0052f408`.

The refinement loop spells the retail rejection condition directly as
`upperBound < evaluatedLength || evaluatedLength < lowerBound`. This preserves
the disassembly's behavior for unordered floating-point values; the previous
inside-the-bounds break condition treated NaN differently. The explicit form
does not change the isolated raw score (47.71%). In `FUN_004d1aa0`, changing
the final cached-length write from the already computed segment pointer to an
indexed expression reduced the isolated score from 39.73% to 32.16%, so the
pointer-based access remains.

The evaluation helpers now initialize the output by copying `p0`, then scale
its components in place before adding the remaining weighted points. This
matches the retail copy-then-scale sequence and preserves its observable
behavior when the output aliases one of the segment vectors. That source form
raised the isolated raw scores from 42.72%, 42.37%, and 43.19% to 48.34%,
47.13%, and 47.00%, respectively. Reordering coefficient accumulation to the
retail polynomial order did not change the score; writing the scale as an
explicit multiplication and writing weighted additions as compound
assignments also generated the same object code. The matches remain incomplete:
the compiler emits substantially different register and x87 schedules. A
bounded matching pass on `FUN_004d1800` retained the aggregate
snapshot and original branch layout at 65.28% raw. Replacing the aggregate copy
with three scalar snapshots or inverting the zero test into an early return
reduced the score to 57.75% and 58.33%, respectively. Reversing the source
operand order in the cross products and spelling each subtraction as a
compound assignment produced the same compiler output as the retained form.
The helper is cdecl: its parameter order is reference vector then adjustable
vector, and callers push the adjustable vector first and the reference second.
The caller at `0x004d1980` confirms both pointers are stack arguments; there is
no missing `this` or hidden structure-return parameter.

The helper at `0x004d1980` is a frame-sampling loop called by
`0x004d2670`: it takes the current vertex-index pointer and segment index,
samples parameters `i * 0x0052fa60` for `i` from 1 through 8, evaluates the
position and two orientation vectors, orthogonalizes one vector with
`FUN_004d1800`, and forwards the four outputs to `0x004d1fc0`. The latter
consumes the spline object's additional field at `0x178` and walks renderer and
vertex-buffer interfaces; that object layout and its rendering dependencies
are not established here, so neither frame function is implemented yet.

The adjacent `0x004d18c0` function is setup code called during the same path
initialization, not a spline calculation. Its caller passes pointers to the
16-bit globals at `0x0067b798` and `0x0067b7c4`; the callee writes the results of
`0x004be050` and `0x004c0b70` respectively. It also invokes `0x004a2360` and
uses the string at `0x0054ab50`. The helper bodies and their engine-object
interfaces are not in this recovered source closure, so I have not added an
implementation based on guessed declarations. This pass used per-function
reccmp only; no behavioral tests were run.

The shared source manifest includes:

```text
LEGO Racers 2/src/spline.cpp
```

The size CSV records `004d0f60,607`, `004d11c0,461`, `004d1390,588`,
`004d1800,188`, `004d1a20,77`, `004d1a70,39`, `004d1aa0,747`, and
`004d1d90,474` bytes respectively. `FUN_004d1a20` and `FUN_004d1a70` are
annotated `FUNCTION`; the other six remain `STUB` pending 100% raw matches.
