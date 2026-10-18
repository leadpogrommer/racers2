# Collision spatial storage recovery

The linked support consists of slot storage (`spatialslots.cpp`), two-slot registry lists (`spatialregistry.cpp`), static-grid insertion/removal (`spatialgrid.cpp`), axis sorting (`spatialaxis.cpp`) and dynamic-owner construction (`spatialconstruction.cpp`). Original callback tables at 0x00537084, 0x00537528, 0x005375cc and 0x005375d4 each contain exactly two pointers. `tools/check_spatial_list_dispatch.py` checks all eight targets and rebuilt ABI bridges. Separate following float data is excluded.

The 0x10-byte allocator owns 0x0c-byte doubly linked nodes and a free chain. Removal retains the original null-input dereference behavior. Index conversion uses signed 32-bit subtraction/division. Registry insertion uses an optional cdecl comparator; the original update loop contains self-link writes, which remain in source under a STUB annotation.

Static-grid coordinates preserve the float store before x87 FISTP and the active rounding mode. Minimum indices clamp to zero, maximum indices clamp to size minus one, and the original loops exclude their upper bounds. The grid constructor shifts its origin by the supplied radius and derives the address shift through signed right shifts.

The dynamic index has two 0x14-byte axis prefixes and two 0x20-byte pair lists. Pair-list construction retains the fixed tail offset 0x7fff8, including when capacity is 512. It leaves the first unknown word untouched. Axis sorting swaps node values and corresponding proxy handles; the source preserves the observed XOR swap sequence rather than assuming aliases cannot occur. These factored sorting bodies have low raw similarity and remain STUB.

Fifteen new routines match exactly in both builds: registry removal; node unlink; slot reset, release, index conversion and lookup; list clear/removal/index/value forwarding; axis removal; dynamic construction; cell insert/removal; and grid update. Across the repository there are 647 measured rows: 269 exact, ten reviewed near and 368 STUB. Other routines retain their measured raw scores in the report.

Collision proxy/service ownership, broad/narrow queries, contact solving, concrete game objects and the actual entry/frame loop remain pending integration. No runtime probe or game launch was performed. Generic container provenance remains unresolved; see library identification.
