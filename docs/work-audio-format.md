# Audio sample-format helpers

The output-provider path uses one parser for RIFF/WAVE and AIFF data, followed
by a sample conversion helper. The recovered contracts are represented in
`audioformat.h` and `audioformat.cpp`.

`ParseWaveOrAiffAudioData` selects the parser from the first four file bytes.
The WAVE parser returns the first `fmt ` payload (requiring at least fourteen
bytes) and the first `data` payload. It follows the retail RIFF-size fallback
of `0x20` when the stored size is zero, and advances over odd-sized chunks with
one byte of padding. It does not validate the complete WAVE format structure.
The AIFF parser requires `FORM`/`AIFF`, `COMM` and `SSND` chunks, accepts only
mono 8-bit or 16-bit samples, and creates a PCM `WAVEFORMATEX` in the retail
scratch record at `0x006fc190`. Its rate conversion reproduces the observed
mantissa/shift arithmetic; it is not a general AIFF extended-float decoder.
The returned sample pointer is the start of the `SSND` payload, including the
eight-byte offset/block-size header as in retail.

`ConvertBigEndianPcm` flips bit 7 for 8-bit PCM samples, swaps each adjacent byte for 16-bit samples, and copies other
bit depths forward in DWORD groups followed by trailing bytes. The 16-bit loop intentionally retains retail's behavior
for odd byte counts. These leaf implementations remain `STUB` until isolated
reccmp measurement and review.

The global at `0x006fc190` lies in the virtual, zero-initialized part of `.data`
after the section's file-backed range. It is modeled as a writable
`WAVEFORMATEX`, not as file data. A bounded scan of the pinned MSVC CRT archive
found no relocation-masked match for the endian, conversion, or parser helpers.
That result does not establish application authorship; provenance remains
unresolved.
