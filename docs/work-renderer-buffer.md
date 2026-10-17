# Renderer lookup-table state

The renderer constructor calls `0x004abdb0` at offsets `+0x4` and `+0x2c`.
That helper clears selected fields and installs vptr `0x005358dc`. The first
pointer-sized table entry at that address is `0x004bd870`; the next entry is
`0x0051a08f`, which enters the runtime's purecall handler. `0x004bd880` is an
instruction inside the first method, not a table target. The object code
therefore does not justify a one-slot reconstructed vtable or a literal retail
vptr in the rebuilt program.

`0x004bd870` (65 bytes, ending at `0x004bd8b0`) traverses bucket-head pointers
backward from the last bucket. When the item count is positive, it follows
each linked chain and calls `0x004bda20` for every node. This method receives
the global renderer context at `0x00549dfc`; `0x004bda20` is a 1,215-byte
renderer-dependent operation. Its behavior and the global owner's interface
are not yet recovered, so the vtable method and `0x004abdb0` constructor remain
unimplemented.

The adjacent state methods do not need that global context:

- `0x004bd6d0` (49 bytes) frees the bucket-head array and tail array through
  `GetMemoryHeap()->Free`, then frees the 16-byte-entry storage through
  `FreeHeapMemory`. The rebuilt `RendererLookupTableState::ReleaseStorage`
  expresses that sequence. Its measured raw score is 66.67%; the remaining
  differences are generated call/register scheduling, while the calls and
  ownership order agree with retail.
- `0x004bd710` (90 bytes) does nothing for a nonpositive item count. Otherwise
  it zeroes `bucketCount * 4` bytes at the bucket-head array, resets the item
  and tail counts, and clears the first tail pointer. The rebuilt
  `RendererLookupTableState::Clear` expresses those visible effects. Its raw
  score is 28.07%; VC6 emitted a different inline clear loop and register
  schedule. The branch condition, extent, and stores match the disassembly.

Both methods remain `STUB` because their scores are below the repository's
near-match threshold; the readable bodies and actual measured scores are
retained. `RendererLookupTableState` is a descriptive working name based on
the bucket/chain operations. The field names for `+0x04` bucket heads,
`+0x08` tail pointers, `+0x0c` entry storage, `+0x1c` bucket count, and
`+0x20` item count follow the allocations, indexed writes, and clear path.
Offset assertions cover the observed prefix through `+0x24`; no complete
object size is claimed.

## Storage initialization and insertion

`0x004bd5f0` is the table's explicit initialization method, not its C++
constructor: it does not install the vptr, and the base renderer calls it on
three already initialized subobjects at `+0x4`, `+0x2c`, and `+0x54`. The
fourth argument is stored unchanged at `+0x10`; its meaning is not established.
The other arguments are bucket count, entry capacity, and signed hash shift,
stored at `+0x1c`, `+0x18`, and `+0x14` respectively.

Initialization preserves retail allocation and write order. It first asks
`AllocateHeapMemory` for `entryCapacity << 4` bytes of 16-byte entries. It then
allocates `(bucketCount << 2) + 4` bytes for the order/tail array using the
engine diagnostic tag `"Render group cache order table"`, followed by an array
of the same size tagged `"Render group cache lookup table"`. Both arrays are
zeroed in that order. It sets the item count to one and calls `Clear`, which
resets the item and tail counts and first tail pointer. Retail has no null
checks between these allocations and the zeroing; the reconstruction preserves
that failure behavior. The strings and direct caller identify this as an
engine renderer cache structure, not a generic CRT container.

`0x004bd770` appends a 16-byte entry to the entry storage, increments the item
count, shifts a signed hash key arithmetically by the stored shift, and
prepends the entry to that bucket's linked chain. Its four stack arguments
populate the key, two opaque payload dwords, and hash key; the two payload
fields are kept unnamed. This function has no virtual call or external helper.
The complete 16-byte `Entry` stride follows both its indexed writes and the
initializer allocation size. The other insertion variants at `0x004bd7c0` and
`0x004bd830` use additional tail-list behavior and are not included here.

The isolated build measured `0x004bd5f0` at 51.97% raw accuracy and
`0x004bd770` at 60.00%. Their readable implementations remain `STUB` under the
95% near-match threshold. The initial measurements for `0x004bd6d0` and
`0x004bd710` remain 66.67% and 28.07%, respectively; no further matching
variants were tried.
