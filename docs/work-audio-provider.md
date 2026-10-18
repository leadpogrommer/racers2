# DirectSound provider setup

`InitializeDirectSoundOutput` is an extracted helper, not an annotation for a
single retail function. It captures the common initialization sequence in
retail `0x004f0550` and `0x004f0b10`: create an `IDirectSound8`, set priority
cooperative level for the current window, create a primary buffer, set its
format, and release the temporary primary-buffer interface.

The format values are copied from the instructions. Both use PCM at 22,100 Hz
and 16 bits per sample. The stereo provider uses two channels, 4-byte block
alignment, and `0x15950` bytes per second. The mono provider uses one channel,
2-byte alignment, and `0xaca8` bytes per second. The primary `DSBUFFERDESC`
size is `0x24` and its flags value is `DSBCAPS_PRIMARYBUFFER`.

The mono provider now has a typed 16-byte state projection. Constructor
`0x004f0aa0` writes the provider dispatch, clears its two DirectSound pointers
and status, then calls the initialization body with the constructor's HWND.
Its fields are the
dispatch pointer, `IDirectSound8*`, a temporary primary-buffer pointer, and
initialization flag. `0x004f0690` releases the primary buffer before the
device, clearing each field after its release. `0x004f0b10` only sets the
initialized flag after the primary format succeeds; its error paths release
the interfaces but deliberately retain the stored pointer values.

Three related playback-object callbacks are also represented with a typed
vtable prefix: `0x004f0f20` reads status through slot `+0x24` and tests bit 0;
`0x004efe40` invokes slot `+0x48` only when that bit is set; and `0x004f0e30`
stops then releases the object stored through its pointer argument. The
`0x004f0f20` retail routine aliases its status output with its incoming stack
argument. The typed reconstruction uses a local because the routine does not
read the pointer again after the call.

`0x004f0f50` clamps a requested playback frequency to 100 through 100,000,
queries slot `+0x20`, and calls slot `+0x44` when the query fails or differs.
`0x004f0fb0` sets playback position to zero through slot `+0x34` and reports
success only for a zero HRESULT. Both functions use the same observed vtable
prefix and preserve the retail callee-cleaned stack arguments.

These methods remain `STUB` until the complete 15-entry provider table is
reconstructed and their measured output has been reviewed.

The mono-table resource callbacks at `0x004f0c50` and `0x004efdd0` invoke
`0x00502fc0` to parse sample data. That parser's library provenance and full
record contract remain unresolved, so the callbacks and table are not yet
claimed as a complete usable provider.

The provider table at `0x0053700c` contains 15 entries. Several entries share
buffer-management helpers with the stereo provider, and the final callback
contains the main sound-buffer playback path. The mono constructor cannot yet
publish a rebuilt table safely; no retail vtable address is used as a runtime
pointer. Thus the helper and method bodies do not constitute a usable provider
object yet.
