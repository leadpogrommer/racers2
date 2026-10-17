# Renderer texture format selection

The working names describe observed behavior; original symbols are unavailable.
`DisplayRenderer::GetTextureQuality` (`0x004ac110`, seven bytes) returns the
signed setting at `+0x7ec`. The renderer table at `0x00535554` exposes it at
slot `0x184`.

`AddSupportedTextureFormat` (`0x004d8800`, 56 bytes) appends a 40-byte format
record while the signed count at `+0x2395c` is below 64. Each record contains
the D3D format, a selection flag and a 32-byte pixel description. The flag is
cleared before initializing the description. The original has no negative-count
guard; the reconstruction preserves that contract.

`InitializeTextureFormats` (`0x004d8840`, 261 bytes) queries the real
`IDirect3D8::CheckDeviceFormat` for HAL texture support. Its ordered list is
R8G8B8, A8R8G8B8, X8R8G8B8, R5G6B5, X1R5G5B5, A1R5G5B5, A4R4G4B4,
X4R4G4B4, then DXT1 through DXT5. Only exact `D3D_OK` results append a record;
these calls do not update the renderer HRESULT globals. The method does not
reset the count or reject an empty result and returns true unconditionally,
as observed in retail. It fills eight compressed-preference pointers at
`+0x844` and eight ordinary pointers at `+0x824`. The first selector call has
arguments `(flags, 1, 0)`; the second has `(flags, 0, compressedSelectionFlag)`.

`SelectTextureFormat` (`0x004da240`, 571 bytes) chooses an index rather than a
format enum. The low two flag bits request 16, 16, 24 or 32 bits. Bit 2 requests
an alpha-first search. The compressed search prefers DXT3, DXT1, DXT5, DXT2,
DXT4 for ordinary flags, and DXT3, DXT5, DXT2, DXT4, DXT4 for alpha flags.
The repeated DXT4 is present in the original stack initialization. A compressed
match sets the record flag to one. No match falls through to the ordinary
selection logic.

An ordinary forced-32-bit request bypasses quality fallback. Otherwise, when
the force argument is zero, virtual texture quality of at least five or the
signed field at `+0x4d8` of at least six lowers requests above 16 bits to 16.
The meaning of `+0x4d8` is still unknown. The alpha search increases its bit
budget by eight until a candidate is found, selecting the largest normalized
alpha mask no larger than the green mask. The fallback chooses the largest
normalized green mask within the bit budget, requires a nonzero alpha mask,
and accepts either an eight-bit green channel or fewer than five alpha bits.
The comparison helper accepts only exact low contiguous masks. If all searches
fail, index zero is retained; no new failure sentinel is introduced.

These helpers operate on an existing renderer. They do not instantiate its
incomplete owner or emit a partial renderer dispatch table. The full
constructor, initializer and texture-state dispatcher remain prerequisites for
connecting real display startup. Validation uses compilation and reccmp only;
no game execution or behavioral tests were added.

The initial combined VC6 build passes. Raw instruction comparisons are 100%
for `GetTextureQuality` and `InitializeTextureFormats`, 50% for
`AddSupportedTextureFormat`, and 40% for `SelectTextureFormat`. Only the two
exact bodies use `FUNCTION`; the other bodies retain `STUB` and their actual
scores. The selector uses readable loops and structured field accesses;
matching register allocation is deferred in favor of closing startup
dependencies.
