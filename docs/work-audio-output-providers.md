# Audio output providers

This batch adds the stereo DirectSound provider and the 3D DirectSound provider
used by `AudioService::Initialize`. Their constructor calls and class extents
are grounded by the service's allocation sizes: stereo uses 0x10 bytes, while
the 3D provider uses 0x24 bytes. The stereo record has the same four-word
prefix as the mono provider. The 3D record has the device at +0x04, primary
buffer at +0x08, initialization status at +0x0c, a zeroed word at +0x10, and a
queried `IDirectSound3DListener*` at +0x14. Three additional COM-pointer-sized
fields at +0x18, +0x1c, and +0x20 are preserved as unknown fields because the
constructor only zeroes them and the complete 3D callback set has not yet been
recovered.

Retail constructor and initializer extents are 0x004f04e0 (36 bytes) and
0x004f0550 (316 bytes) for stereo, and 0x004ef930 (51 bytes),
0x004ef9d0 (224 bytes), and 0x004efab0 (100 bytes) for the 3D provider. The
stereo initializer uses 22,100 Hz, 16-bit stereo PCM and the shared output
setup sequence. The 3D initializer requests a primary DirectSound buffer with
`DSBCAPS_CTRL3D`, queries the listener interface, calls `SetDistanceFactor`
with 0.75 and immediate flag 1, then sets the initialization status. Its
failure paths retain the retail release and pointer-clearing behavior.

These bodies remain `STUB` until the full provider dispatch tables and their
callback closure are reconstructed. In particular, the 3D callback table
contains source-buffer and playback operations that are not implemented here.
The diagnostic callback used by the retail listener-query failure path is also
not yet identified exactly; the current source calls the existing diagnostic
wrapper. No DirectSound or other library provenance claim is made for these
game-specific provider wrappers.
