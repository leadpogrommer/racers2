# Collision-query recovery notes

The query state at `0x00679e30` is allocated with the observed size `0x4c70`.
It contains 256 `CollisionContact` records, a signed current count at `+0x4c00`,
the two active proxies at `+0x4c04/+0x4c08`, and two `CollisionModelTransform`
values beginning at `+0x4c0c`. The count comparisons in the pair/record helpers
are signed branches, even though normal operation keeps the count in `[0,255]`.

`TryRecordCollisionPair` (`0x508a10`) stores its proxy arguments before testing
the `-3` filter case. That path writes only the contact's point, normal,
separation axis, penetration and participant pointers; it leaves the trailing
velocity, offset and flag words alone. The normal path orders proxies by their
signed complexity, builds the second proxy's transform relative to the first,
tests either the geometry records or the high-bit special box record, then
rotates each newly emitted contact's three vectors into the first proxy's
orientation. The special high-bit branch uses the box-record pointer at model
offset `+0x10` and does not set the four temporary geometry-query globals.

The current source includes the sphere and oriented-box record paths and the
15-axis box overlap classifier (`0x505ae0`/`0x506580`). The projection helpers
`0x505370`/`0x5054d0` retain the retail ±0.05 support-selection deadband. The
box-sphere helper writes into the current contact slot and preserves its side
argument, including normal reversal and point adjustment. `0x508ca0` transforms
the center and three box axes by the relative transform, translates only the
center, and delegates contact construction.

That delegated builder is `0x4fa2b0`. Its sphere-record branch initializes the
contact distance to one box half-axis and tests every sphere record with the
segment/sphere helper `0x509760`. The current implementation follows that
branch. The builder's polygon-data and auxiliary-model branches still depend
on `0x509d00`, `0x4fb3e0`, `0x4fb460`, and the 0x50a* triangle helpers, so this
does not yet close the geometry contact path.

The box-box contact path (`0x5066f0`) now uses distinct face support paths and a
closest-points solve for the selected edge pair. The reconstruction remains
`STUB`: its cross-axis endpoint selection and final contact point still need a
raw-level review against the retail edge branch before it can be treated as
equivalent.
The geometry record loops in `0x4f97f0`/`0x4f9d50` also do not yet cover their
encoded polygon data. Exports `0x4faea0`, `0x509a90`, `0x509c40`, and `0x509d00`
show that this path enters a global mesh iterator and additional 0x50a* triangle
helpers; those are still required for full geometry coverage.

`0x5069e0` is a closed record helper used by `0x4faea0`: it copies the box
center and writes the square root of the three half-extent squares as the
bounding-sphere radius, leaving the final word in the destination record
untouched. The source now exposes that behavior as `BuildBoxBoundingSphere`.
The triangle-list and contiguous-array walkers (`0x4fb3e0`, `0x4fb460`) now
set the triangle-base scratch pointer, traverse their respective records, and
delegate ray tests to `IntersectRayWithCollisionTriangle` (`0x50bfd0`). That
helper performs the one-sided plane test, distance cap, all three oriented edge
tests, and writes the triangle record index into the contact's trailing word.
The mesh walkers remain `STUB` because the surrounding encoded-model branches
and their broad-phase dependencies are not yet closed. All marked query
functions remain ordered by original VA so repository-wide matching can run
while the larger triangle closure is still in progress.

The encoded mesh query remains an explicit open dependency. `0x509a90` and
`0x509c40` build oriented-box and sphere AABBs, then call `0x509d00` before
dispatching to the indexed/contiguous triangle walkers. `0x509d00` first rejects
queries outside model bounds (`+0x9c..+0xa4`), then selects among seven encoded
geometry modes: linked polygon/index traversal (`0x50a280`, `0x50a2e0`,
`0x50a300`, `0x50abf0`), clipped indexed and box/polygon traversal
(`0x50a110`, `0x50a470`), or the special mode-7 path (`0x50a8b0`). These
branches use shared queue/scratch storage at `0x702580`, `0x701580`,
`0x700560`, `0x700540`, `0x6fdfd8`, and `0x6fdfe4..0x6fdfe8`; their complete
record layouts, reset behavior, and helper closure have not yet been recovered.
The swept-box path also needs the full `0x50b4a0` mesh test and the polygon
path needs `0x50c410`; both perform substantial geometry work. They remain
unfinished rather than being represented as successful no-op queries.

The linked segment-query path now includes dynamic axis candidate collection,
static-grid traversal, proxy-space transformation, sphere intersections,
encoded-tree candidate collection and terrain cell intersections. The temporary
query bounds retain the retail radius-constructor typo (Z minimum overwrites
X minimum), and execute the ordinary detach destructor. Candidate capacity is
passed in element zero of the caller's output buffer; returned elements are
lifecycle tags, not proxy addresses or filter flags. Both broad-phase pair
lists use signed packed-key ordering and the observed six-by-six filter table.

`CollisionQueryResult` now exposes the complete 0x4c contact record. The ray
path writes tags at +0x28/+0x2c and flags at +0x4a; the former 0x28-byte
projection was insufficient. The sphere helper at 0x509760 now interprets +0x10
as floating-point radius squared and updates +0x4a, rather than numerically
converting its bits or changing +0x48. Static-grid scratch storage models the
128 visited proxies followed by the delta pointer, so its initialized end
pointer refers to the actual adjacent boundary.

The tree collector retains the 500-candidate sorted list, duplicate rejection,
256-entry ring queue, child ordering and queue-capacity early exit. Its partial
half-extent writes, floating-point evaluation/NaN behavior and aliasing still
need review. It stays STUB, as do the unfinished box/encoded-model contact
branches above. The retail sweep predicate 0x4047a0 is an actual constant-false
function; the original sweep's problematic loop is retained, not represented
as a successful clearance query. No runtime collision or game test was run.
