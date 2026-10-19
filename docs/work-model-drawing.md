# Model draw queues and frustum integration

The model draw dispatcher at `0x004a7250`, shadow visibility queue at
`0x004a9680` and two depth-sort helpers now link in the canonical build. They
connect recovered geometry to the renderer's real bucketed, ordered and
deferred lookup tables. Car-preview objects and controller/foyer startup are
still being connected; the bootstrap remains active.

## Draw submission

The dispatcher updates binding flag bit 3 from the camera frustum, obtains a
point/depth sort key, invokes the owning GameObject callback and selects the
model group. The original depth threshold at `0x00549bd8` is `0.2f`; mode one
forces the first group for model radii above 25. Blended primitives use the
bucketed queue, optionally with per-primitive depth offsets. Opaque primitives
select their queue by material mode, using the first kind-zero texture slot as
the ordering key. The observed unguarded group/material contracts are retained.

Shadow visibility tests the camera planes with radius plus 50, then queues
the binding through the existing deferred table. This is visibility/submission
support; it does not reconstruct the remaining shadow-generation function.

The depth key preserves the negative sign-bit rejection, fourth-power mapping
around 8192, and FISTP conversion under the current FPU rounding mode. Arithmetic
and calling-convention differences remain under review; no new depth helper is
promoted solely from a plausible formula or compilation success.

## Corrections to existing renderer code

The frustum helper's loop must compare the current normal pointer against the
normal-array end. The earlier loop compared an address displaced by `0x50`,
which could skip the planes. Plane points remain at normal plus `0x48`.
After correction this helper matches exactly and uses `FUNCTION`.

The immediate lookup path now reads its GameObject owner from binding `+0x04`,
rather than interpreting the model's primitive flag as a pointer. Its mode-one
LOD override also uses the original greater-than comparison. The large
immediate renderer path remains `STUB` and requires further review.

The existing camera depth helper is exposed through its shared header and
reused by sort-key construction. Shared lookup globals retain their existing
storage/layout. The proven `0xe4`-byte binding view supplies the draw inputs;
its construction and custom object lifecycle remain separate worker tasks.

## Validation

Both SP5 builders compile and link 350 canonical units. All 1,595 measured
scores and statuses agree: 482 exact functions, nineteen reviewed near matches
and 1,094 STUB rows. Twelve complete folded aliases remain verified. All twelve
dispatch inventories pass in both builds. Annotated data has zero issues across
265 globals. The existing tooling tests passed in the preceding batch; no
tooling logic changed here.

No game, asset or menu probe was run. The frontend widget factory compiles as
an isolated unit, but remains outside the canonical manifest until its actual
car-preview dependencies close. Full shadow generation and the remaining
foyer/controller connections are still required.
