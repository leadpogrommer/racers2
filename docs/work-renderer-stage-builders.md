# Renderer texture-stage record builders

`RendererTextureStageManager` keeps a 0x14-byte stage-record array at offset
0x1880 and a 0x800-byte key-to-group-index cache at 0x2c80. The manager
initializer clears the cache to `0xff`. A `RendererSubrecordGroup` contains
four 0x30-byte subrecords; the constructor initializes the eight words at
0x00..0x1c to `0xffffffff`, clears 0x20 and 0x28, writes 1 at 0x2c, and leaves
0x24 untouched.

The helper at 0x004e4e80 initializes a node's first four words: type 1 followed
by three caller-provided values. It leaves the fifth word unchanged. Its
measured instruction accuracy is 100%.

The helper at 0x004e4ea0 stores `value` in `table[index]` and raises the table's
highest-written-index field at +0x28 when needed. The profile builder uses two
such tables at a 0x30-byte stride; the local view models only the accessed
prefix and does not claim a larger owner type. The raw branch compares indices
as signed integers, so the implementation uses `LegoS32`; the observed call
sites supply nonnegative slot indices. It also matches at 100%.

The helper at 0x004e5e70 performs node initialization with its observed
argument ordering, then links the entry into the receiver subrecord. It
measures 15.38%; this low score is likely driven by the inlined field stores
and call setup, so it remains `STUB` while its argument mapping is documented.

The helper at 0x004e5ea0 links a stage-record node into one subrecord. It stores
the previous head from subrecord+0x20 into entry+0x10, then updates the head.
The node is 0x14 bytes: its first four words are descriptor data and its last
word is the next pointer. This refines the observed meaning of subrecord+0x20
as a linked-list head; the shared layout still uses an integer field there
because no root-header edit was authorized in this batch.

The helper at 0x004e5eb0 takes a group-record base, a group index, a source
array, and a signed count. For each positive-count source item, it copies four
words into the next 0x14-byte manager entry, increments the manager entry
count at 0x3484, and links that entry to `groupBase[groupIndex]`. Non-positive
counts perform no writes. The raw body uses the source records at a 0x10-byte
stride and entries at a 0x14-byte stride. Retail reloads the manager count
after copying the four words; the implementation preserves that order in case
the input aliases manager storage. Its measured instruction accuracy is
42.86%, so the function remains `STUB` despite the reviewed data-flow.

The linker at 0x004e5ea0 matches at 100%. The 0x004e5eb0 copy-and-link helper
measures 42.86%; it remains `STUB` because its score is low even though the
copy stride, list link, positive-count loop, and counter reread were checked
against the raw body.

The 0x004e5f30 wrapper is implemented as an adapter from the embedded manager
to its actual `DisplayRenderer` owner. The manager is at renderer+0x25148, so
the rebuilt code derives the owner from that proven embedding rather than
retaining the retail singleton address. It preserves the original three-stack-
argument cleanup ABI and restores counters at +0x3480/+0x3484 and invalidates
the packet's cache key at +0x14 when the renderer-side operation fails. Its
isolated score is 94.55%; the remaining instruction difference is the required
owner derivation (`lea ecx,[manager-0x25148]`) in place of retail's absolute
singleton load. It remains annotated `STUB` pending combined review.

The owner-explicit implementation of 0x004e5f90 keeps the caller-supplied group
argument so its `ret 0x0c` ABI is retained. The mode-1 path returns success
immediately. The other path resets default render states, installs the
temporary material, allocates temporary vertex/index buffers, configures the
device stream and indices, ensures requested textures exist, applies each
cached subrecord and retries device validation after resetting texture
coordinate indices. It releases textures created for this attempt and both
temporary buffers, restores the cached shader/indices, and returns its
success flag. Raw validation failure after the retry sets the flag to false
and restores default render states. Rollback traverses the caller-supplied
group, as confirmed by the first stack argument at `4e61ca`. The source uses the proven vertex
buffer, texture record, stage manager and Direct3D 8 types; it does not emulate
the retail static renderer pointer. The earlier 40.15% measurement predates corrections to descriptor offsets,
material values, call ordering and validation return behavior; current raw
measurement is pending and the function remains `STUB`.

The profile builder at 0x004e4fd0 now has compiled source; the default builder
at 0x004e2850 remains open work. Their raw
bodies initialize shared profile tables and generate stage records before
calling the helpers above; they are substantially larger than the closed
record operations in this batch.
