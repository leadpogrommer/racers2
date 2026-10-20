# GameObject base-class recovery

The base constructor at `0x004a2e20` is 170 bytes. It initializes type,
flags, both inline 24-byte strings, parent/template/child pointers, the
`0x4f000000` scalar, and lifecycle pointer. With a non-null template it first
clears template flag `0x10` outside development mode, then copies the template
flags and both strings. It forces flag bit 0 on and registers the object with
`GetGameManager()`. The constructor and the template normalizer are present in
`src/game/objects/gameobject.cpp`, currently annotated STUB pending a linkable base dispatch
table and reccmp review.

The deleting destructor at `0x004534e0` is 30 bytes, and the base destructor at
`0x004a2ed0` is 58 bytes. The latter installs the base vptr, skips an audio
callback when heap flag `+0x0c` is set, otherwise calls AudioService slot
`+0x40` with this object, then unregisters it. These bodies are also present
but remain STUB until measured. They use the currently observed AudioService
slot and do not claim its original semantic name.

The base table has 20 function-pointer entries (size `0x50`) at
`0x00535240`; each entry was read from the retail PE rather than inferred from
source order. The float at `+0x50` is separate adjacent data, not a table
entry. In particular, the active
targets are `0x004a2f10`, `0x004a2fe0`, `0x004a3010`, `0x004aa790`, and
`0x004a30e0`. The slot at `+0x30` is `0x004aa790`, not `+0x38`.

The child transform callback at `0x004a3010` (33 bytes) and its callees
`0x004a9d90` (604 bytes)/`0x004a9ff0` (616 bytes) are now represented. The child prefix used by those
bodies has a resource pointer at `+0x08`, quaternion at `+0x8c`, position at
`+0x9c`, and transformed-vertex pointer at `+0xd0`. The resource supplies the
source vertex pointer at `+0x14`; the code transforms eight vectors with the
quaternion, adds position, updates the midpoint and bounds, copies the source
scalar at `+0x70`, then applies the original X/Z aspect-ratio flag test. The
intermediate structure names in `gameobject.cpp` are deliberately descriptive
of the verified offsets, not a claim about the original class name.

The base `Initialize`, `Remove`, `SetParent`, and `ClearParent` callbacks are
now represented in `gameobject.cpp`; lifecycle service functions remain STUB
dependencies until the collision service and parent-transform helper closure
is integrated. These bodies preserve the observed call and state-update order.

The active callback paths now have source bodies, with these remaining review
and integration points:

* `Initialize` at `0x004a2f10` updates lifecycle flags, calls lifecycle slot
  `+0x40`, notifies the state service through `0x004b9810` and `0x004b9850`,
  calls `0x004e6a30`, and may find/attach a parent through `0x004a37f0` and
  `0x004a3040`.
* `Remove` at `0x004a2fe0` conditionally passes a copied lifecycle state to
  `0x004b9870`.
* Slot `+0x30` targets `0x004aa790`, which calls `0x004aa540` before
  dispatching to the child at `+0x48`; the child callback itself remains a
  runtime vtable call recovered only by its observed slot and ABI.
* Slot `+0x48` targets `0x004a30e0` and has one stack argument (`ret 4`);
  its helper `0x004d40a0` uses a typed minimum region-record view but still
  needs instruction-level comparison.

The full 20-slot base table is now emitted in `gameobject.cpp` with typed
fastcall bridges for every slot; `gameobjectdispatch.h` exposes its initializer
so the statically separate GlobalPause table can share the same observed
targets. Empty/value bridges preserve each target's stack cleanup, including
the one- and two-argument leaves. The no-op/value target inventory is
`+0x08/+0x0c -> 0x4038d0`, `+0x14 -> 0x403920`, `+0x18 -> 0x403900`,
`+0x20/+0x24/+0x38/+0x40 -> 0x403910`, `+0x28/+0x2c -> 0x4038d0`,
`+0x34 -> 0x403930`, `+0x3c -> 0x4038e0`, and `+0x44 -> 0x4038f0`.
The latter three targets pop one or two stack arguments; their bridges must
preserve that ABI even though they do no work.

The active draw chain is represented through its dependencies: `DrawChild`
calls visibility predicate `0x004aa540` and then the actual child object's
slot `+0x04`; the predicate uses projected depth (`0x004ba370`), camera-plane
tests (`0x004aa730`), optional shadow-state lookup, and the transformed child
volume. The region callback `0x004a30e0` stores the mesh radius and passes the
child position/radius and four-byte key to `0x004d40a0`. The key begins with
level `0xfe`, followed by cell and signed index. These bodies remain STUB
pending root measurement and review; the child, camera, and region overlays are
limited to offsets read by these functions.

Retail extents including the complete `ret` instructions are: `0x004a30e0`
53 bytes, `0x004aa540` 488, `0x004aa730` 93, `0x004aa790` 29, `0x004ba370`
58, and `0x004d40a0` 563. The 4d40a0 region lookup has not been instruction-
matched; its grid record field names are descriptive offset labels only.
