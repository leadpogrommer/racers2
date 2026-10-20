# Physical lifecycle construction and motion

The canonical build now includes the physical-state constructor at 0x4a6070,
reset at 0x4a61d0, parent counter advance at 0x4a61b0, damping/gravity at
0x4a6620, sleeping-owner flag update at 0x4a6710 and motion integration at
0x4a67b0. The 0x38-byte parameter record names its position, quaternion,
friction, response, property index, mass and inertia. The state constructor
preserves existing flag bits, normalizes orientation, computes reciprocal mass
and inertia, and runs the original reset path. Reset retains its unused owner
stack argument (RET 4), large-mass callback, cached-parent counter sequence and
original byte-sized clears; no blanket object initialization was introduced.

The parent counter reuses the existing initialized global at 0x549bb0 rather
than introducing duplicate storage. Its advance wraps signed values above 499
back to 100 and matches exactly. The separate constructor ID counter starts at
one, verified directly from the PE. The complete twenty-slot physical table at
0x535398 is inventoried and checked in both builds, including the real indexed
property-data callback and base-transform deleting destructor reused from the
existing car-display recovery. This is an explicit ABI table, not a C++ vtable
annotation.

Damping/gravity preserves the parent early return and inactive-body force clears.
Motion integration preserves sleeping/disabled early returns, momentum updates,
quaternion velocity and acceleration composition, normalization, inertia rebuild
and velocity clamping. The sleep comparison uses the original 0.01 and 0.5
thresholds, x/y/z accumulation order, and carry-only x87 comparison semantics:
unordered values pass these two retail comparisons. The integration helpers
remain STUB, with actual measured scores visible. Their floating-point/register
code generation has not established a near-match equivalence claim.

Both pinned VC6 builders link 381 units and agree on all 1,835 measured scores
and statuses: 532 exact, twenty-six reviewed near matches and 1,277 STUB rows.
Both data comparisons report zero issues across 297 globals. All twelve dispatch
verifiers pass, including 521 scene pointers; seventeen complete folded aliases
and twenty-one tooling tests pass. All six new function extents were checked
against raw control flow. No game behavior tests, game runs or asset probes were
performed. Foyer model construction, preview owner tables and the remaining menu
startup graph still require integration; the menu has not launched.

Logs: build/msvc/physical-construction-final-build.log,
build/msvc/physical-construction-final-compare.log,
build/msvc/reports/physical-construction-datacmp.txt and corresponding CMake paths;
build/msvc/physical-construction-dispatch.log and
build/msvc/physical-construction-tooling-tests.log.
