# Renderer buffer storage and factories

The CPU/device buffer wrappers now have source models with retail layout
assertions: vertex records are `0x50` bytes, vertex storage pools `0x24`,
index records `0x30`, and index storage pools `0x14`. The renderer owns 5,000
records of each kind, 128 vertex pools and 64 index pools. Their offsets are
asserted in `renderer.h`. These types do not yet install their retail tables;
constructors and table integration wait for the remaining callbacks.

Vertex storage handles FVF-derived strides, CPU allocations, reservations,
references, and native Direct3D buffer locks. The vertex wrapper tracks lock
ranges, temporarily substitutes the native mapping when policy bit 1 is set,
and uploads modified CPU data on commit otherwise. If native and CPU strides
differ, upload copies one vertex at a time. Retail increments the generation
on a committed device upload even when mapping fails, and leaves pool lock
bits set on failed lock attempts. These observed behaviors are preserved.

Index wrappers retain the retail byte-offset convention: `Lock` passes the
start directly to Direct3D but adjusts the returned base by twice the start.
No speculative correction is made to that asymmetry. The committed written
extent uses the original signed shift when comparing the pointer distance.

The renderer factories search the actual embedded arrays. New pools use the
existing memory heap, application callback and display configuration. Native
GPU creation uses the owning renderer's real device and usage policy. Retail
helpers `0x004ea130` and `0x004e9ef0` instead read these same fields through the
static renderer; the source helpers explicitly accept the owner and have no
retail annotations. Likewise, the duplicated layout/reservation code from
`0x004e9560` is factored into unannotated source helpers, preserving layout
writes before the allocator's application callback. The factory is still
`STUB`, not an exact ABI/code-generation claim for those factored helpers.

The vertex factory returns NULL if all 5,000 records are occupied. The index
factory reserves CPU space before looking for a free record, as retail does.
The vertex pool allocator lacks an exhaustion guard in retail; its one-past
selected address is retained and remains an unresolved failure-path limit.

Leaf attribute accessors use the actual array layout and descriptive names.
The `0x004ab850` field-at-`+0x18` getter is shared by the vertex stride and
index device-buffer accessors. The field-at-`+0x10` color-offset getter also
folds with `Application::GetUnknownFlag` at `0x0049a6a0`; both are explicitly
marked FOLDED, without artificial writes to defeat ICF.

Initial integrated measurements leave the factories and larger uploads as
STUB. `TryReuseVertexStorage` is 67.89%, `CreateVertexBuffer` 42.28%,
`AllocateVertexStorage` 55.38%, `CreateIndexBuffer` 30.86%, vertex `Lock`
81.20%, vertex `Unlock` 42.28%, and pool `Initialize` 50.47%. Many leaf
accessors, index Lock/Release, native-buffer release, FVF helpers and pool
Unlock/Reserve match exactly; final counts are recorded after the integrated
FUNCTION guard passes. Similarity is diagnostic, not proof of complete runtime
behavior. No game runtime or source behavior tests were run for this batch.

At the combined checkpoint, both builders verify 204 exact and seven reviewed
near functions over 384 measured rows. Buffer wrappers and factories remain
unconnected to the full renderer table. The stream-resource initializer at
`0x004ec2c0` now builds its material descriptor through `InitializeMaterial`,
acquires the auxiliary resource, creates the streaming index record and calls
its actual policy callback. It remains STUB at 30% raw; no game was run.
