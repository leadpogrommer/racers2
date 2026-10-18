# Game-manager registry and frame work

This batch reconstructs the concrete registry loops used by the startup frame
path. The global pointer list is at `0x5499b0`, its signed count at `0x578bf8`,
and its capacity at `0x5499b4`. The two temporary work lists live behind
`0x578c04` and `0x578c08`; their counts are `0x578c00` and `0x578bfc`.
`0x578c0c` records whether those derived lists need rebuilding. The retail initialized data starts with mutation/refresh flags equal to 1, capacity 32 and the registry pointer targeting a 32-pointer BSS array at `0x578b68`. The object count and derived worklist storage start at zero. Root integration corrected the initial source defaults against these PE bytes.

`GameManager::RebuildObjectWorkLists` (`0x4a34a0`, 209 bytes) resets both
counts, then visits each registered object. If its field at `+0x54` is set,
the function calls that subobject's first dispatch slot with argument zero;
when the predicate returns nonzero and the subobject's `+8` word is nonzero,
the owner is appended to the active-component list. The owner is also appended
to the other work list when that predicate succeeds or its signed type word
shifted right by eight is 3, 4, 5, 6, or 14. The routine sets the rebuild flag
even when the registry is empty. The names describe observed use, not recovered
original symbols or complete class hierarchies.

`GameManager::UpdateObjects` (`0x4a36c0`, 75 bytes) visits the same registry
in order. It skips objects if either flag bit `0x4` or bit `0x10` is set;
otherwise it calls slot `+0x18` with the frame parameter. It then calls the
renderer resource sweep at `0x4c0910`. That sweep first invokes slot `+8` on
each resource reference in `m_resources0x98`, then makes two ordered passes
over `m_resources0xa0`: callbacks at `+0x54` receive mode 3 on the first pass
and mode 1 on the second. The raw pushes and caller stack cleanup identify
these callbacks as cdecl `(record, mode)` functions.

`GetGameManager` (`0x4a2dd0`, 53 bytes) returns the eight-byte singleton at
`0x578be8`. Its byte guard is `0x578bf4`; on first use it sets bit zero,
clears the two singleton words and registers the observed empty destructor
callback with the verified CRT `atexit` implementation. `ShutdownGameManager`
(`0x4a3ca0`, 12 bytes) obtains that singleton and enters the object-removal
loop at `0x4a3710` (104 bytes). The loop calls slot `+0x1c` unless flag
`0x00400000` is set, and accounts for callbacks that remove entries from the
registry by searching backward for the current object after the count changes.

The integrated Python and CMake builds measure the singleton accessor at 93.33%, classification at 35.38%, update at 47.27%, removal at 23.16% and renderer resource sweep at 84.11%. These remain STUB pending further semantic review. The shutdown wrapper is verified at 100% and uses FUNCTION. Both builds verify the annotated registry data. Full startup is still
unfinished: registry allocation and insertion (`0x4a3170` and neighbors),
concrete menu-object construction, and the large scene/render-mode initializer
(`0x4aa8e0`) are not part of this batch. Consequently these methods do not yet
make an empty rebuilt application reach the menu.
