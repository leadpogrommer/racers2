# Renderer resource records

This unit models renderer resource records and recovers part of the texture
resource setup path. Cache and factory methods that depend on unreconstructed
resource callbacks remain `STUB`; small audited exact helpers are `FUNCTION`.

`RendererDefaultRecord` is initialized by `0x00479490`, a 52-byte thiscall
constructor at `this+0x28740`. It writes a 0x28-byte record: the first dword
is zero; bytes `+0x04..+0x07` become `00 00 00 01`; then four 8-byte slot
defaults are set to `{0xffffffff, 0xffff, 0x0f, 0x03}`. It follows the
0x35f8-byte texture-stage manager beginning at owner offset `+0x25148`.

`RendererArrayMarker` is initialized by `0x004abea0`, a 9-byte thiscall
constructor that writes `7` at offset zero and leaves the rest of its 0x14-byte
record untouched. The owner uses compiler array-constructor support at
`0x00401020` for 256 elements at stride 0x14; that runtime helper is not
reimplemented as game code.

`RendererTextureRecord` is a 0xa0-byte projection with signed resource ID at
`+0x04`, path at `+0x08`, resource type/index at `+0x8c/+0x8e`, flags at
`+0x90`, and texture pointer at `+0x98`. `GetFlag0x90` at `0x004abe80` is
exact; `MatchesResource` at `0x004bffd0` is exact and checks active/hash flags,
path equality, and resource type. `GetTexturePathHash` at `0x004bffb0` is also
exact; it sums signed path bytes modulo 256. The path setter at `0x004bfce0`
copies the path, updates flags/type, clears the index, and combines the path
hash into the flags word; it remains `STUB` at 80.46% raw. The record
constructor at `0x004abe50` and its ten-slot callback table at `0x005358e4`
remain unfinished, so this unit emits no replacement table.

`RendererResourceRef` is an 8-byte element in the renderer's reference array:
resource pointer at `+0`, reference count at `+4`, requested type at `+6`.
`AddTextureReference` at `0x004c0a60` finds/appends a reference, invokes the
resource availability callback for a new reference, conditionally adds it to
the active list, and returns the signed resource ID at resource `+4`. It
measures 89.66% raw and remains `STUB`. `FindTextureReference` at `0x004c0b30`
scans for the matching resource with nonzero count; it measures 51.16% and
remains `STUB`.

`FindOrCreateTextureRecord` at `0x004bfb50` (398 bytes) checks the path hash
when resource mode is 1, scans the 800-record table as needed, then initializes
a free record. It passes `(path, kind, resourceMode)` to `SetResourcePath`.
When loading is requested it calls the record's `+4` load callback, retaining
the hash entry only when that callback succeeds; on failure it clears the
active flag. This method remains `STUB` at 23.90% raw. Retail also constructs
and cleans up an empty temporary `BitmapImage` on return paths; its pointers
start null, so omitting that no-op cleanup does not alter data behavior.

`FindOrAllocateTextureResource` at `0x004c09a0` (188 bytes) searches 32 cache
entries by active flag and path, then initializes the first inactive entry and
calls its path/load slots. It measures 44.59% raw and remains `STUB` because
the cache entry's callback table and full record behavior are unresolved.

`CreateTextureResource` at `0x004c0b70` (840 bytes) takes path, kind, index,
create mode, and reference type. It handles `.TGA`, `.IFL`, and `.AVI`, the
renderer resource-kind path, high-resolution `.MIP` lookup with fallback, and
reference registration. The high-resolution path replaces the existing
extension with `.MIP`; if found, it changes that extension to `.tga` before
loading. It remains `STUB` at 13.55% raw while several downstream callbacks
remain unresolved.

`RendererAuxiliaryResourceRecord` is 0x5c bytes. It contains four float
vectors through `+0x3c`, power and mode factor at `+0x40/+0x44`, a `U32` mode
at `+0x48`, inline path at `+0x4c`, callback at `+0x54`, and usage/type/count
bytes at `+0x58..+0x5b`. `InitializeMaterial` at `0x004c11e0` (780 bytes)
unpacks high-nibble color channels, applies fixed color/power scaling, and
initializes path/callback fields. The mode is an integer even though the
decompiler inferred a float from its stack access. This implementation remains
`STUB` pending further matching/validation.

The lookup-state constructor `0x004abdb0` now matches exactly. It initializes
the 0x28-byte lookup state, clears five dwords, and installs the abstract table
at `0x005358dc`, whose two entries are the real CRT purecall. The derived
lookup tables and their drawing callbacks remain a separate closure. Vertex
and index record constructors at `0x004abde0` and `0x004abe30` still await
complete callback-table integration; no partial derived tables are emitted.

Function extents are from `data/retail-disasm.txt`: `0x00479490` 52,
`0x004abe50` 44, `0x004abe80` 12, `0x004abea0` 9, `0x004bfce0` 141,
`0x004bffb0` 31, `0x004bffd0` 124, `0x004bfb50` 398, `0x004c09a0` 188,
`0x004c0a60` 127, `0x004c0b30` 60, `0x004c0b70` 840, and `0x004c11e0` 780
bytes. Root owns shared function-size metadata and source manifests.
