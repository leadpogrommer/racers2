# Library identification and allocator provenance

## Pinned MSVC CRT matches

The retail image at `/home/ilya/work/lr2_decomp/LEGO Racers 2/LEGO Racers 2.exe`
was compared with object members from the pinned
`.tools/msvc6-sp5/VC98/Lib/LIBCMT.LIB`. `_strlwr` and `_stricmp` are present as
`build\intel\mt_obj\strlwr.obj` and `build\intel\mt_obj\stricmp.obj`.
Their object symbol names are `__strlwr`, and `__stricmp`/`__strcmpi`
respectively. The latter pair are aliases at the same `.text` offset.

| Retail VA | Object member / symbol | Body | Relocation bytes masked | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x0052e00a` | `strlwr.obj` / `__strlwr` | 308 | 68 | 240 | 59 |
| `0x00524eb0` | `stricmp.obj` / `__stricmp`, `__strcmpi` | 208 | 36 | 172 | 31 |

Every non-relocation byte matches exactly, with longest unchanged runs of 58
and 68 bytes. The COFF file string in `strlwr.obj` names `strlwr.c`; the one in
`stricmp.obj` names `intel\stricmp.asm`. The supplied `VC98/CRT/SRC` subset does
not contain either implementation source (it does contain, for example,
`STRFTIME.C`, which calls `_stricmp`). Thus the archive objects provide the
direct body evidence; the source tree alone does not.

Other clear, raw-byte-identical runtime matches found in the same archive are:

| Retail VA | Object member / symbol | Body bytes |
| --- | --- | ---: |
| `0x0051a000` | `strncmp.obj` / `_strncmp` | 56 |
| `0x0051a0a0` | `strrchr.obj` / `_strrchr` | 39 |
| `0x0051aa70` | `strncpy.obj` / `_strncpy` | 254 |
| `0x0051e470` | `strlen.obj` / `_strlen` | 123 |
| `0x00521230` | `memset.obj` / `_memset` | 88 |
| `0x00524400` | `strcspn.obj` / `_strcspn` | 62 |
| `0x00524440` | `strpbrk.obj` / `_strpbrk` | 58 |

These seven matches have no relocation bytes and zero raw differing bytes.
They are stronger than a name or call-pattern guess because each full body
matches its named member from the pinned archive. The `strrchr` and `strncpy`
identifications were already recorded in `library_msvc.h`.

Additional confirmed CRT/compiler support routines:

| Retail VA | Object member / symbol | Body bytes | Relocation bytes masked | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x00519f5e` | `onexit.obj` / `_atexit` | 18 | 4 | 14 | 4 |
| `0x0051a038` | `ftol.obj` / `__ftol` | 39 | 0 | 39 | 0 |
| `0x0051a060` | `chkstk.obj` / `__alloca_probe`, `__chkstk` | 47 | 0 | 47 | 0 |
| `0x0051baa0` | `memchr.obj` / `_memchr` | 165 | 0 | 165 | 0 |

`_atexit` is shorter than the scanner's minimum unmasked-byte threshold, so it
was checked directly using the complete archive symbol extent. Its sole
relocation occupies bytes 4–7; all other 14 bytes match, including a 9-byte
contiguous run. Its body calls the adjacent `__onexit` implementation, then
normalizes that result to the `atexit` success/failure convention. The other
three bodies are raw-byte-identical. `__alloca_probe` and `__chkstk` are two
COFF names for the same body and retail address. These mappings identify CRT
startup/compiler helpers as well as string/memory routines; they do not imply
that adjacent, unmatched code belongs to the CRT.

Two additional parser callees were checked against their full archive bodies:
`_strstr` at `0x0051adc0` matches `strstr.obj` over 128 bytes (four relocation
bytes masked), and `_atol` at `0x0051a19b` matches `atox.obj` over 139 bytes
(24 relocation bytes masked). All remaining bytes match exactly. The latter
is **not** `_atoi`: that archive member also contains a distinct 11-byte
`_atoi` wrapper. The reconstructed numeric switch helper must call `atol`
to preserve the observed library boundary.

## Candidate scanner and limits

`tools/identify_libraries.py` parses COFF archive members and i386 relocations,
then checks function bodies listed in
`data/ghidra-export/discovered-functions.csv` against retail `.text`. It masks
the operand bytes at supported COFF relocation sites and reports a candidate
only when at least 32 unmasked bytes and an 8-byte contiguous unmasked run
remain, with every compared byte equal. By default it scans the pinned
`LIBCMT.LIB` and writes `data/library-candidates.csv`; `--targets` accepts a
comma-separated list of retail VAs for a focused check.

The CSV distinguishes `masked_bytes`, `unmasked_bytes`, `longest_unmasked_run`
and `raw_differences`. Relocation-masked identity is not raw-byte identity and
is not the same metric as reccmp's instruction comparison. The current full
scan produced 191 candidate rows for 180 retail addresses; aliases and
different archive objects can share bodies, so these rows require manual
symbol, caller, and provenance review. The scanner never modifies annotations,
and a miss does not establish that an address is game code or belongs to some
other library.

Discovery sizes can include disjoint ranges or tail-call thunks. For example,
Ghidra assigns 133 bytes to `0x0051adc0`, while the matching `_strstr` object
body is 128 bytes. The size-indexed scan therefore misses that body with the
default inventory; it was checked separately over the complete object extent.
Use a reviewed size CSV with `--sizes` when investigating such a miss.

## DirectShow family evidence

The bodies at `0x004a2050` (`DeleteMediaType`) and `0x004a20d0`
(`FreeMediaType`) match the behavior and field use in Microsoft's DirectShow
base-class [`mtype.cpp`](https://raw.githubusercontent.com/microsoft/Windows-classic-samples/main/Samples/Win7Samples/multimedia/directshow/baseclasses/mtype.cpp).
Together with callers and the x86 offsets, they support an `AM_MEDIA_TYPE`
layout of 72 bytes: `pUnk` at `+0x3c`, `cbFormat` at `+0x40`, and `pbFormat` at
`+0x44`. This is evidence for a DirectShow base-class family, not proof of an
exact SDK release or link provenance.

The related `0x004a2070` copy helper differs from that later public source on
allocation failure: the retail code clears `cbFormat` and still calls
`AddRef`, without returning `E_OUTOFMEMORY`. Preserve that target behavior; do
not replace it with the newer sample implementation. No DirectShow
`LIBRARY` annotations have been added because the exact SDK version and
linkage remain unresolved.

## Allocator provenance

The standard CRT allocator and the separate pool allocator must not be
conflated. The following retail routines were checked against the object
sections in the pinned SP5 `VC98/Lib/LIBCMT.LIB`:

| Retail VA | Object symbol | Body bytes |
| --- | --- | ---: |
| `0x0051ab6e` | `_malloc` | 18 |
| `0x0051ab80` | `__nh_malloc` | 44 |
| `0x0051abac` | `__heap_alloc` | 252 |
| `0x0051aca8` | `_free` | 233 |

All four bodies have zero differing bytes after masking the four-byte operands
at the COFF object's relocation sites. The object symbols, complete body sizes,
branch layouts, and runtime calls establish these identifications. They are
mapped as `LIBRARY` in `library_msvc.h`, with their implementation supplied by
the toolchain rather than reconstructed source.

The code around `0x004a3ed0`–`0x004a452e` is a separate pool allocator. Its
allocation/free wrappers at `0x004a4050` and `0x004a4150` can fall back to the
CRT functions above, while its normal path manages a doubly linked free list
with 16-byte headers and units. Initialization queries an application object's
virtual method at `0x0049a760`, then reserves memory through `0x004e7b90`, a
wrapper over `GlobalAlloc` with alignment rounding.

Its diagnostics include `Thread overlap, make this wait till its ok` and
`Exiting function without entering???`; neither was found in the supplied CRT
sources or sample repositories. Searches of those strings did not identify a
public source. This is evidence against treating the pool as MSVC's allocator;
it does **not** prove who authored it or exclude a third-party engine library.
Its application configuration and logging dependencies suggest an engine-level
allocator, but that attribution remains provisional.

Allocator reconstruction is deferred. The exploratory source is retained
under `research/allocator/`, outside the game source root,
build manifest, and game-function progress claims. A named library attribution
requires stronger evidence, such as matching source/object code or symbols.
