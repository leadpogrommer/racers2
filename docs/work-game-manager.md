# Game-manager registry and frame work

This batch reconstructs the concrete registry loops used by the startup frame
path. The global pointer list is at `0x5499b0`, its signed count at `0x578bf8`,
and its capacity at `0x5499b4`. The two temporary work lists live behind
`0x578c04` and `0x578c08`; their counts are `0x578c00` and `0x578bfc`.
`0x578c0c` records whether the derived work lists are valid. Rebuild sets it to 1; parent sorting invalidates it with 0. The retail initialized data starts with mutation/refresh flags equal to 1, capacity 32 and the registry pointer targeting a 32-pointer BSS array at `0x578b68`. The object count and derived worklist storage start at zero. Root integration corrected the initial source defaults against these PE bytes.

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

## Registry initialization pass

`GameManager::ResizeObjectRegistry` (`0x4a3170`, 274 bytes) preserves the
retail distinction between an initial mutation and later growth. With a
nonzero mutation-depth global and requested capacity zero, it obtains capacity
through application dispatch slot `+0x64`; otherwise a zero request returns
immediately when mutation depth is already zero. It allocates and copies the
object pointer array, frees the old object list only on the latter path, clears
mutation depth, replaces both derived work arrays, and clears the rebuild
flag. The three allocator tags are the retail strings `Main Object List`,
`Main physical List`, and `Main collidable List`.

`GameManager::SortObjectsByParent` (`0x4a33d0`, 198 bytes) compares each
object's parent at `+0x40` with the parent's and child's type words. If the
parent is eligible but appears later in the list, it moves that parent ahead
of the child and revisits the shifted position. The missing-parent path reports
the exact message `failed to find the parent object in the list!` and then
continues through the retail reorder writes. It clears the work-list rebuild
flag on both the empty and nonempty paths.

`GameManager::InitializeRegisteredObjects` (`0x4a3580`, 105 bytes) runs those
two helpers, applies the development-mode gate to global `0x549bb0`, then
walks flagged objects. The application callback at slot `+0x10` is cdecl with
`(Application*, object + 0x10)`; two pushes and caller stack cleanup establish
that ABI. The following object slot `+0x10` is a no-argument thiscall. Finally
the method sets manager word `+0` and runs two static-object passes.
`ResetGameObjectInitializationValue` (`0x4a6190`, 27 bytes) only writes 100 to
`0x549bb0` outside development mode. The PE's initialized DWORD at that
address is already 100.

The two static-object passes are documented in
[work-game-manager-static-passes.md](work-game-manager-static-passes.md). The
PE selector pointers lead to fixed static objects whose vtables are installed
by entries in the CRT initializer table. All 31 slot-`+8` targets in
`0x49a490` resolve to `0x00403910` (`xor eax,eax; ret`), with ignored return
values; the leading `0x00479030` target is bare `ret`. In `0x4e7080`, 21 of
25 slot-`+8` targets resolve to `0x00403910`, two to `0x0049a730` (return 2),
one to `0x00403920` (return 1), and one to `0x004ba110` (return 3). Their
returns are also discarded. Selector addresses have no other text references
or writes; each selector's object vptr is assigned by its CRT constructor and
has no later direct store. The source models these two helpers as effect-free
for that constructed table. They remain unannotated because their empty bodies
fold with the canonical `0x4a3f20` return helper; annotating either would steal
that folded mapping. The measured game-manager initializer score therefore
does not include the original aggregate call sequences.

The new source has not yet been integrated or measured. No game run or
behavioral test was performed. These registry helpers still do not construct
the menu objects; the next task is the actual startup object-registration and
scene-initializer path.
