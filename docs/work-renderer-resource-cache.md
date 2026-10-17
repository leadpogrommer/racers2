# Renderer resource cache entries

The renderer allocates 32 cache entries at `0x004be0a0`, each with a `0x194`
byte stride. The constructor at `0x004be2f0` installs the abstract table
`0x0053570c` and sets the low four bits of the flags word at `+0x06` from its
type argument. The owner then replaces each entry's table with
`0x00536394`. Reading the original PE confirms that `0x0053570c` contains five
`_purecall` targets and that the concrete table is:

| Slot | Target | Observed operation |
| --- | --- | --- |
| `+0x00` | `0x004c0070` | Copy a path to `+0x08`, create a blank texture resource, store its signed ID at `+0x04`; return true. |
| `+0x04` | `0x004c00c0` | Parse and load an IFL animation; return whether at least one frame was loaded. |
| `+0x08` | `0x004c03c0` | Advance animation time and select the frame for the current tick. |
| `+0x0c` | `0x004c0450` | Release the base resource and each loaded frame reference. |
| `+0x10` | `0x00403920` | Shared unconditional-true callback (`ApplicationTruePolicy`). |

The callbacks establish a signed base resource ID at `+0x04`, 64 frame IDs at
`+0x88`, 64 cumulative frame-end times at `+0x108`, frame count at `+0x188`,
elapsed ticks at `+0x18c`, and total duration at `+0x190`. The fixed `0x194`
entry stride is independently shown by the renderer initializer's allocation
size `0x3280` (`32 * 0x194`) and the pointer increment in its loop. The blank
path passed by `SetPath` is the zero-filled data at `0x00553888`.

The callbacks obtain the active renderer through the existing global pointer
at `0x00549dfc`; they do not establish a safe renderer owner by walking back
from an arbitrary entry. `InitializeRecord` calls the recovered type-1 entry
constructor, clears only the three tail timing fields, then installs a concrete
five-slot callback table. Its abstract base table contains five real CRT
`_purecall` callbacks. The source-side callback table uses thin `__fastcall`
adapters to preserve the observed ECX receiver, unused EDX, and stack arguments;
the exact retail table address is not claimed as reproduced data.

`LoadAnimation` uses the shared `WinFile` and `MemoryHeap` abstractions. It
allocates its temporary buffer from the end of the game heap, reads the complete IFL file, uses signed-byte control-character comparisons to skip separators and semicolon
comments, uppercases each line, and accepts lines containing `.TGA`. It
preserves the directory prefix from the IFL path, loads each image through the
renderer factory, extracts at most nine decimal digits after the extension as
its duration, converts durations to cumulative 16-bit thresholds, and scales
them using the active simulation timing globals and the exact `0x5363cc` float
literal. The 64-element frame-ID and cumulative-duration arrays occupy separate
regions within the entry. Frame selection uses the renderer's virtual slot at
`+0x11c`.

`ReleaseAnimationReferences` calls the renderer's recovered release helper for
the base resource with mode zero and for each loaded frame resource with mode
one. No game or runtime execution is used for this recovery.

The function boundaries are `0x004be2f0` (37 bytes), `0x004c0070` (80 bytes),
`0x004c00c0` (766), `0x004c03c0` (129), and `0x004c0450` (76), each ending at
its final `ret` and excluding following alignment padding. Combined Python/CMake validation measures the constructor at 27.27%, the IFL
loader at 31.83%, and frame
advance at 20.69%; these remain STUB. The release method is an exact match.
SetPath is a reviewed 96.88% near match: its sole differing instruction is the
address of an empty path, whose retail bytes at 0x553888 were verified zero.
Both paths are read as the same empty C string by CreateTextureResource.
All 20 emitted texture/animation/abstract pointers pass the static dispatch
checker in both builders. No game execution was used.
