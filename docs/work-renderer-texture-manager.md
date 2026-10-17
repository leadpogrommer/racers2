# Renderer texture-stage manager

The renderer embeds `RendererTextureStageManager` at `DisplayRenderer+0x25148`.
The embedding is visible at the call to `0x004e1960` in the renderer
initializer. Its extent is `0x35f8`: the next constructor in the same sequence
receives `this+0x35f8`, and its object begins at renderer offset `0x28740`.
The vertex-record array begins at `0x28768`, after that separate 0x28-byte
record. This supports the manager size assertion without inferring it from its
last initialized byte.

The manager begins with 32 `RendererSubrecordGroup` records at stride `0xc4`.
The renderer constructor then initializes 256 `RendererArrayMarker` records at
manager offset `0x1880`, each with a `0x14` stride. The manager's own initializer
sets the 0x800-byte byte map at `+0x2c80` to `0xff`; later record lookup code
uses signed bytes from this map. The two words at `+0x3480` and `+0x3484` are
zeroed by the enclosing renderer constructor, not by `Initialize`.

Ten route records at `+0x3490` contain an enabled word and a priority word.
Initialization sets all enabled words to zero, all priorities to one, then
assigns priorities 2 through 9 to route indices 5, 7, 9, 4, 1, 6, 2, and 3;
route zero is enabled by default. The rank values are established by the
selection helper at `0x004e1c30`, which chooses and clears the highest-ranked
eligible bit. That helper is outside this implementation batch.

Ten `0x1c` operation profiles begin at `+0x34e0`. The known color and alpha
operation fields at `+0` and `+0x0c` are D3DTOP values, as confirmed by the
capability check and profile contents. Other profile words are retained in the
typed record, with the last word unnamed. `AreTextureOperationsSupported`
checks both operation fields against the selected adapter's DX8
`D3DCAPS8::TextureOpCaps`. The old free helper at `0x004e1c80` read that value
through a global selected-adapter pointer; this reconstruction obtains the
enclosing `DisplayRenderer` using the proven embedding offset and then reads
its `m_selectedAdapter->m_caps`. The capability calculation itself maps D3DTOP
values 1 through 24 to bits 0 through 23 and returns false for all other
values.

When the selected adapter lacks `D3DDEVCAPS_SEPARATETEXTUREMEMORIES`,
`Initialize` populates profiles 1, 2, and 4 through 8 and enables routes based
on their operation support. Profile 1 first tests operation 11 and changes its
color operation to 7 on failure, then tests the fallback profile again. The
remaining capability-dependent profile fields and route flags are written in
retail order. Profile 9 is not initialized by this method and remains
untouched.

The isolated reccmp scores are 92.36% for `Initialize` and 37.74% for
`AreTextureOperationsSupported`. Both remain `STUB` because the raw matches do
not meet the repository's threshold for a `FUNCTION` claim. The capability
check is semantically reconstructed from the DX8 `TextureOpCaps` field; this
implementation obtains the selected adapter through the manager's enclosing
renderer, while retail calls a free helper that reads the selected adapter
through a global. This is DX8 renderer code, not CRT or third-party library
code.
