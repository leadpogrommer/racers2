# Digital music controller recovery

The AudioService path at 0x004ef4ac allocates 0xb0 bytes and invokes the
constructor at 0x004f13e0. This supports the controller extent asserted in
`audiomusiccontroller.h`. The recovered methods use a DirectSound buffer at
offset 0x70, a worker state at 0x90, and playback-state words at 0x94 and
0x98. The projection deliberately leaves other bytes opaque.

The controller now includes the 0x004f19d0 update path. It claims the volatile
worker state with `InterlockedExchange`, queues mode-2 ticks when another thread
owns the lock, selects a full-buffer reset, play-cursor catch-up, or queued
chunk update, locks the actual `IDirectSoundBuffer`, fills both returned
regions, unlocks, and releases the worker state. It retains the original
zero-only HRESULT checks and the one-subtraction cursor wrap. Calls to
`CopyDecodedSamples` and the block decoder are connected to the recovered
stream ring.

`DigitalMusicPcmRing` is a projection of the first six words allocated by the
WAVE setup method: block count at +0, consumed-sample cursor at +4, file-sample
cursor at +8, decoded sample count at +0xc, compressed-block scratch at +0x10,
and PCM samples at +0x14. Constructor code places both buffers inside one
allocation, with the compressed scratch and PCM region overlapping. The
decoder currently uses a readable MS ADPCM implementation with the verified
retail adaptation/coefficient tables at 0x0054b6b0, 0x0054b6f0, and
0x0054b70c. Those values are the standard Microsoft ADPCM tables, but this
does not establish the provenance of the enclosing controller or decoder.

The stream code is intentionally still annotated `STUB`. In particular, the
retail decoder expands packed nibbles into the overlapping ring allocation
before decoding them in place. The current C++ decodes the same apparent
interleaved nibble stream into the PCM region directly, and the exact byte
consumption / channel ordering around partial final blocks still needs review
against raw instructions and callers. The matching run also remains to be
performed after root integrates this unit and the three read-only table rows.
These are known unresolved behavioral details, so the readable implementation
is not promoted to `FUNCTION`.

`ReadFileBytes` at 0x004f2070 updates the byte position only after a successful
full-length `ReadFile`; `SkipFileBytes` at 0x004f20c0 seeks relative to the
current position and advances the tracked byte position regardless of the API
result. `CopyDecodedSamples` preserves the retail loop, zero-fills after the
decoded sample limit when looping is disabled, and resets the stream when
looping is enabled.

Retail extents, including the final instruction and excluding alignment
padding, are 82 bytes at 0x004f1860, 54 at 0x004f18c0, 98 at 0x004f1900, 56 at
0x004f1970, 22 at 0x004f19b0, 481 at 0x004f19d0, 55 at 0x004f1bc0, 557 at
0x004f1d70, 196 at 0x004f1fa0, 72 at 0x004f2070, and 35 at 0x004f20c0.
