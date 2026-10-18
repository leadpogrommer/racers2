# Contact impulse support

The 0x4c-byte contact record contains the first-body point at +0, normal at +0x0c, separation axis at +0x18, penetration at +0x24, lifecycle pointers at +0x28/+0x2c, relative point velocity at +0x30, second-body point at +0x3c and flags at +0x48. The participant swap at 0x004b96f0 shifts the point, reverses normal/separation vectors, exchanges body pointers and applies the optional position difference.

The impulse snapshot is 0x2c bytes: linear/angular momentum pointers, inverse mass, inverse inertia pointer, restitution and two output impulse vectors. Source now covers one-body and two-body normal impulses, the penetration correction scale, mass-weighted positional correction and fixed-body correction. The separation-weight helper at 0x004fc530 matches exactly; it returns equal weights when the mass sum is less than one or unordered. Larger factored calculations remain STUB, with floating-point evaluation/aliasing review outstanding. No behavioral-equivalence claim is inferred from their low instruction scores.

The call sites establish the physical-state fields more precisely: +0xb4 is angular momentum, +0x114 is the inverse-inertia diagonal, and +0xd8 is the world inverse-inertia tensor R * diagonal * R transpose. Their earlier geometric working names were corrected together with callers. The actual Vector3 cross-product callback at 0x004d5be0 also has source.

Frame-state cleanup at 0x004e8830 is independently linked. Its two scratch records, copied values and pending owner pointer have verified zero-initialized BSS backing. Record setter order and the second null check were reviewed from raw instructions. The descriptive audio working names remain inferred from surrounding callers.

A bounded signature scan of 0x004fc050, 0x004fc1f0, 0x004cd7c0 and 0x004cdb00 found no relocation-masked candidates among 1,466 pinned CRT archive functions. This does not establish game authorship; the generic constraint/cache provenance remains unresolved. Ignored evidence: data/contact-library-candidates.csv.

The full contact solver and cache construction/dispatch remain staged. No game, startup, screenshot or asset probe ran for this batch.
