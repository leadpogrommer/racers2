# Audio service recovery

`AudioService` is a game-owned sound manager, not a CRT object. The retail
image imports `DSOUND.dll` ordinal 11; the repository's DX8 check maps that
ordinal to `DirectSoundCreate8`. Its records also carry game-specific sound
and commentary names, while a separate path uses WinMM MCI and mixer APIs.
Some alternate-backend behavior is still being identified, so no broad
third-party library attribution is claimed.

The accessor at `0x004af2f0` creates the singleton at `0x0065312c` only when
its argument is true. It gets a provider/configuration value from `0x004ee6d0`,
allocates `0x2a624` bytes through the game's heap, calls the `__thiscall`
constructor at `0x004af380`, and registers teardown at exit. The constructor
accepts one stack argument (returned by `0x0043dcd0` at the current caller),
constructs 512 records at `this+0x3c` with stride `0xd8`, 256 at `this+0x1b03c`
with stride `0x98`, and eight at `this+0x249fc` with stride `0x34`. Those exact
strides and the exact total allocation justify the record extent assertions
in `audioservice.h`; their full internal layouts are not yet claimed.

Construction first installs the base dispatch at `0x00535a9c`, builds those
embedded record groups, switches to the operational dispatch at `0x005359f4`,
and calls `Initialize` (`0x004af4e0`). The constructor and record
initializers now have source implementations with reviewed field writes, but
remain STUB until rebuilt dispatch tables and every callback they invoke are
closed. The service destructor (`0x004af360`)
uses slot zero with argument 1 and then clears the singleton. The operational
table contains 42 slots. WinMain uses slot `+0x14` to set quality from
`/highqualitysound` and `/lowqualitysound`; a window-message path uses slot
`+0x5c` to play a requested CD track.

`Initialize` sets heap mode while constructing the selected provider. Quality
values 0 and the default use `0x004f0aa0` (mono DirectSound); 1 uses
`0x004f04e0` (stereo DirectSound); 2 uses `0x004ef930`; and 3 records failure
and releases the provider. The third constructor creates a DirectSound 3D
listener provider. The two PCM provider initialization bodies share the setup
captured by `InitializeDirectSoundOutput` in [work-audio-provider.md](work-audio-provider.md).
Their remaining dispatch methods and service callbacks are under recovery; the
service constructor and initializer are not ready to wire into startup until
their real dispatch targets are implemented.

The full object has substantial trailing storage beyond the three constructor
loops (`0x24b9c` through the allocator-proven end at `0x2a624`). Initializer,
streaming, and shutdown methods use that region. The header represents the
unresolved area as an explicit byte gap rather than assigning speculative
member names or introducing retail vtable addresses into rebuilt code. The
MCI path has a confirmed `MCI_OPEN_PARMSA` record at `+0x2484c`, a DWORD
remembered track at `+0x24860` (the low byte is sent to MCI), an MCI playback
flag at `+0x24864`, and the digital-audio selector at `+0x2486c`. The HWND
snapshot at `0x00653128` supplies the MCI callback value and DirectSound
cooperative-level window; it is copied from the view-window global at
`0x0067a4a0` by
`0x004ee6d0`.

The source-record projection names its playback object at `+0xc0`, observed in
provider cleanup `0x004f0ea0` and update callback `0x004f0fd0`. Its operational
table at `0x00535b4c` has 16 recovered entries; the abstract table at
`0x00535b8c` has the concrete deleting destructor followed by 15 CRT purecall
entries. The rebuilt tables use local callback addresses. Slot `+0x18` releases
a source playback buffer through the active provider; that provider method
stops the playback object through its own slot `+0x28`, reloads the source's
playback field, releases the interface, then clears the field. The reload
preserves the callback's ability to mutate the record. Channel-resource cleanup
also decrements its observed leading reference count and invokes the record
cleanup only when it reaches zero. The source constructor sets each record's
two empty 24-byte names, default ranges, playback pointer and observed scalar
defaults.

The initializer at `0x004af4e0` now expresses all four quality branches and
cleanup paths, including the retail status checks and heap mode toggling. It
calls mono/stereo/3D provider constructors and source/channel/device setup
helpers by their recovered roles. The provider table and the full service
dispatch tables remain open, so these bodies stay STUB and are not yet runtime
closure.

The recovered source extents are: `0x004af2f0` 108 bytes, `0x004af360` 27,
`0x004af380` 107, `0x004af4e0` 455, `0x004b02d0` 35, `0x004b0340` 42,
`0x004b03b0` 264, `0x004b07c0` 32, `0x004b0a20` 22, `0x004b0a40` 54,
`0x004b0d80` 32, `0x004b0fa0` 22, `0x004b0fc0` 64, `0x004f0aa0` 35,
and `0x004f12e0` 29.
The `0x0043dcd0` settings getter spans 22 bytes and is documented separately
in [work-audiosettings.md](work-audiosettings.md). These are full retail
instruction extents, including complete epilogues and excluding following
alignment bytes; implementations remain STUB until dispatch and provider
dependencies are linked and compared.

The update helpers at `0x004af860` and `0x004af8d0` are now expressed as well.
The first scans all 512 source records and asks the provider to stop eligible
buffers before setting a pending source flag. The second walks the linked
playback-source list, resets positions when requested, applies frequency and
mode changes, and reloads the next link after callbacks. Its scaled integer
conversion preserves the retail float spill and uses FISTP with the active x87
rounding mode. Both methods remain STUB until integrated and measured. Playback
objects are modeled as the actual `IDirectSoundBuffer` SDK interface, so COM
calls use their real stdcall slots rather than an application-defined vtable.

`DestroyAudioService` (`0x004af360`) calls the operational slot zero with
argument 1 and clears the singleton afterward. `AudioService::PlayCdTrack`
(`0x004eec50`) is the signed one-argument slot at `+0x5c`; callers pass `-1`
to replay the remembered track. Its CD path opens `cdaudio`, sets TMSF time
format, and issues MCI_PLAY with notify/from/to flags. The digital-audio branch
calls `0x004ef3b0`, whose custom file/cache and source-object dependencies are
not yet recovered, so this method remains STUB. The 42-slot table uses typed
fastcall bridge signatures where recovered: ECX carries the receiver, EDX is
an unused bridge parameter, and original stack arguments remain in order.

## Remaining closure

- Recover the operational service callbacks needed by initialization,
  shutdown, WinMain quality selection, active sound updates, and the digital
  track path, with a typed rebuilt table rather than retail pointer literals.
- Construct the full service and provider tables using rebuilt callback
  addresses, never retail pointers; source-record dispatch is implemented.
- Recover the stereo and 3D provider constructors and validate the
  application-backed frame-duration helper used by `Initialize`.
- Recover DirectSound provider creation/error paths and the 3D listener path.
- Identify the alternate backend reached through `0x004ef360` and its
  `0x004ef3b0`/`0x004ef520` calls; this path uses MCI and mixer APIs, but its
  surrounding provider provenance remains unresolved.
- The isolated VC6 build succeeds. `GetQualityMode` (12 bytes) measures 100%
  raw; `DestroyAudioService` (27 bytes) measures 73.68% because its required
  fastcall bridge clears EDX; `PlayCdTrack` measures 54.88% and remains STUB
  while the digital-file branch is unresolved. The latter retail body spans
  310 bytes: its last instruction is `ret 4` at `0x004eed83..0x004eed85`,
  followed by two alignment NOPs before the next function. No game run was
  performed.

The new source-record dispatch and cleanup bodies are `STUB` pending integration
and raw reccmp measurement. Their full retail extents are 4b0370/32,
4b0390/30, 4b04c0/22, 4b04e0/17, 4b0500/78, 4b0550/4, 4b0560/24,
4b0580/24, 4b05a0/24, 4b05c0/13, 4b05d0/23, 4b05f0/24, 4b0610/23,
4b0630/23, 4b0650/23, 4b0670/139, 4b0700/68, 4b0750/33,
4b0780/15, 4b0790/38, 4b0a90/55, and 4b0bd0/74 bytes. The sizes are
from the exported Ghidra function inventory and include each return epilogue.
