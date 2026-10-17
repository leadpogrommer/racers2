# Renderer transform-query work

This batch implements the closed transform-cache, point-transform and scene-end
bodies in `renderertransformqueries.cpp` as `DisplayRenderer` members. The
renderer remains an evidenced prefix; these declarations do not establish its
full class size or vtable.

`0x004abf00` checks the cache flag at `DisplayRenderer+0x788`. When clear, it
multiplies world (`+0x57c`) by view (`+0x5bc`) into `+0x67c`, then multiplies
that result by projection (`+0x5fc`) into `+0x6bc`, then by viewport (`+0x63c`)
into `+0x6fc`, and sets the flag. All three destination matrices are distinct
from their inputs. The existing `MultiplyMatrix4` helper expresses the same
row-major product and preserves retail's four-term accumulation order.

`0x004ddc00` transforms a point through the cached `+0x6bc` matrix and
`0x004ddce0` does the same through `+0x6fc`. Both take two stack pointers and
return with `ret 8`; the source and result are written/read in X, Y, Z order.
Each output first receives its three matrix products, then the translation
component. The homogeneous W uses the last column and is computed after those
stores; output is divided only when W is ordered-positive. Zero, negative and
unordered values skip division. The sequential stores intentionally retain
retail's behavior when source and destination alias. The implementation uses
ordinary float locals, so x87 extended intermediate precision and reciprocal
retention may still differ; these entries remain STUB until measured and
reviewed.

`0x004ddb30` clamps depth to `m_nearClip` when the x87 comparison is less-than
or unordered, negates `m_projectionTransform.m[0][0]` only when
`m_clockwiseCulling == 1`, then computes `(projectionScale / depth) * scale`.
Its two float arguments are on the stack (`ret 8`) and the result is returned
in x87 ST0. `0x004dd230` is an ECX-only wrapper around
`IDirect3DDevice8::EndScene` (COM slot `+0x8c`); it ignores the HRESULT.

The wrapper at `0x004ac080` sets ECX to renderer+`0x245e8` and passes the sole
stack argument through to `0x004dd240`, which returns with `ret 4`. The callee
maintains a six-float process-global metrics result and samples a millisecond
timestamp. It updates only after a strictly greater than 1000 ms interval and
copies current counters to snapshot regions then. The visible calculation
writes metrics 0, 1, 2, 3, and 5; metric 4 is retained from its previous
global value. Metric 0 is the counter-1 delta, metrics 1 and 2 both begin as
the counter-0 delta, and only metric 2 is divided by metric 0 when that count
is greater than one. Metric 3 is the sum of eight paired-counter deltas,
divided by metric 0 only above one. Metric 5 is the unsigned delta of the
first tail counter, with the same divide condition. The second tail counter
is copied into the snapshot but not read by the metric calculation. It reads/writes the embedded
record through offset `+0xb5f`; the next known embedded manager starts at
renderer+`0x25148`, exactly `0xb60` bytes after the state start. The owned
`RendererTimingStatisticsState` maps the counter-history, paired-counter,
previous-value and snapshot regions and asserts this observed extent. The
DisplayRenderer wrapper calls its `GetMetrics` member.

Other reviewed targets remain unimplemented in this unit because their
closures include the renderer's larger frame and device paths:

* `0x004dd130` is a 247-byte render-scene/device-lost state handler. It checks
  application activity and Direct3D cooperative level, attempts reset, updates
  a state field, and enters the device scene. Its complete startup/reset call
  closure is not part of this batch.
* `0x004dd460` and `0x004dd900` are draw-packet paths with texture-stage and
  device state behavior, not point-transform callbacks.
* `0x004dd9f0` lazily updates the transform cache, projects a point through
  renderer dispatch slot `+0x64`, and checks the result against the global
  display configuration's width and height. Slot `+0x64` returns a float; the
  method rejects a nonpositive or unordered result before checking the X/Y
  viewport bounds.
* `0x004ddb70` is a stdcall helper applying a data-resident 4x4 transform at
  `0x0058b7bc` to a 3-float source, with output order X/Z/Y. Its matrix's
  ownership and semantic role need caller/global initialization evidence before
  giving it a named renderer field.

Function annotations remain STUB pending an isolated build and raw score.
Measured retail extents from the current function inventory are: `0x004abf00`
290 bytes, `0x004ac080` 19, `0x004dd130` 247, `0x004dd230` 16, `0x004dd240` 541,
`0x004dd460` 1125, `0x004dd900` 226, `0x004dd9f0` 212, `0x004ddb30` 57,
`0x004ddb70` 141, `0x004ddc00` 220, and `0x004ddce0` 220.

Combined Python/CMake verification matches GetTimingMetrics (0x4ac080),
EndScene (0x4dd230), and ScaleProjectionCoordinate (0x4ddb30) exactly. These
three use FUNCTION; larger statistics, cache and point-transform bodies remain
STUB. The static world/view helper uses RET 8 and is therefore declared
__stdcall; the root review corrected its original cdecl declaration.
