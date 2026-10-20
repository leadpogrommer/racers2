# Skeleton transforms and the shared car catalogue

Two canonical units recover quaternion right multiplication and the skeleton
record bind/transform path required by Foyer model templates. Each 0x48-byte
record retains signed source/parent indices, a parent pointer, local position
and orientation, and world position and orientation. Binding reads the complete
0x20-byte definition and preserves the relative-parent pointer arithmetic.

World-transform rebuilding applies the root offset, reuses the preceding parent
when its index matches and reloads other parents by index. It rotates each local
position by the parent quaternion, translates it and composes orientations in
the retail order. Quaternion multiplication retains the component products,
intermediate float values and mutation sequence; instruction and floating-point
code-generation differences remain under review. All new bodies remain STUB.
The bare record constructor folds with an existing implementation and has no
separate measured report row. Four complete extents were checked against raw
control flow and Ghidra.

The catalogue at 0x53ed7c contains 46 car paths, rather than just the eight used
by player-default initialization. Its complete backing storage is now shared
through gamecontrollerplayers.h; the first eight entries and their callers are
preserved. All 46 string targets and contents were read from the original PE.
The remaining 38 literal extents are included in strings.csv. This supplies the
SecondaryManager's full catalogue-loading loop without duplicate storage.

Both pinned VC6 builds link 378 units and agree on all 1,806 measured raw scores
and annotation statuses: 517 exact, twenty-four reviewed near matches and 1,265
STUB rows. All twelve dispatch verifiers and twelve complete folded aliases
pass. Both data comparisons report zero issues across 287 globals, including
the complete catalogue. Tooling tests are unchanged and the twenty-one tests
passed at the preceding scene-feedback checkpoint. No game behavior tests,
runtime runs or asset probes were added or performed.

Logs: build/msvc/skeleton-catalogue-build.log,
build/msvc/skeleton-catalogue-final-compare.log,
build/msvc/reports/skeleton-catalogue-datacmp.txt and corresponding CMake paths;
build/msvc/skeleton-catalogue-dispatch.log. Foyer model/render integration, actual
character preview resources and SecondaryManager startup remain staged. The
bootstrap still does not create a window or render the menu.
