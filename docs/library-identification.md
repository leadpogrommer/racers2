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

## Additional confirmed candidates

I added the following mappings after reviewing the complete candidate body
against its named LIBCMT member. In each row, every non-relocation byte is
identical; the remaining differences are limited to bytes covered by the
member's COFF relocation records. These are relocation-masked body identities,
not raw reccmp percentages.

| Retail VA | Archive member / symbol | Body | Masked bytes | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x00519ee0` | `onexit.obj` / `__onexit` | 126 | 56 | 70 | 37 |
| `0x00519f9f` | `sprintf.obj` / `_sprintf` | 82 | 8 | 74 | 4 |
| `0x0051a231` | `vsprintf.obj` / `_vsprintf` | 81 | 8 | 73 | 4 |
| `0x0051a2c2` | `atof.obj` / `_atof` | 87 | 20 | 67 | 12 |
| `0x0051ad91` | `wcscat.obj` / `_wcscpy` | 37 | 0 | 37 | 0 |
| `0x0051b929` | `toupper.obj` / `_toupper` | 111 | 32 | 79 | 20 |
| `0x0051bb45` | `ldexp.obj` / `_ldexp` | 486 | 56 | 430 | 30 |
| `0x0051bd2b` | `floor.obj` / `_floor` | 207 | 36 | 171 | 19 |
| `0x0051c045` | `frexp.obj` / `_frexp` | 176 | 32 | 144 | 17 |
| `0x0051c16e` | `cvt.obj` / `__forcdecpt` | 90 | 20 | 70 | 13 |
| `0x0051c22e` | `cvt.obj` / `__fassign` | 62 | 8 | 54 | 4 |
| `0x0051c26c` | `cvt.obj` / `__cftoe` | 97 | 12 | 85 | 5 |
| `0x0051c2cd` | `cvt.obj` / `__cftoe2` | 194 | 16 | 178 | 10 |
| `0x0051c38f` | `cvt.obj` / `__cftof` | 85 | 12 | 73 | 5 |
| `0x0051c3e4` | `cvt.obj` / `__cftof2` | 167 | 20 | 147 | 10 |
| `0x0051c48b` | `cvt.obj` / `__cftog` | 147 | 16 | 131 | 12 |
| `0x0051c51e` | `cvt.obj` / `__cfltcvt` | 81 | 12 | 69 | 12 |
| `0x00521097` | `powhlp.obj` / `__powhlp` | 304 | 56 | 248 | 36 |
| `0x005217e9` | `a_cmp.obj` / `_strncnt` | 43 | 0 | 43 | 0 |
| `0x00522115` | `ieeemisc.obj` / `__copysign` | 33 | 0 | 33 | 0 |
| `0x00522b2f` | `tolower.obj` / `_tolower` | 111 | 32 | 79 | 24 |
| `0x00523055` | `_fptostr.obj` / `__fptostr` | 119 | 8 | 111 | 8 |
| `0x005231f0` | `strcat.obj` / `_strcat` | 224 | 0 | 224 | 0 |
| `0x00524017` | `strtol.obj` / `_strtoxl` | 517 | 44 | 473 | 38 |
| `0x00524e20` | `xtoa.obj` / `__itoa` | 45 | 4 | 41 | 1 |
| `0x00524e4d` | `xtoa.obj` / `_xtoa` | 92 | 0 | 92 | 0 |
| `0x00524f80` | `strnicmp.obj` / `__strnicmp` | 257 | 36 | 221 | 31 |
| `0x0052ded6` | `strupr.obj` / `__strupr` | 308 | 68 | 240 | 58 |

`_strncnt` is emitted twice in the archive (`a_cmp.obj` and `a_map.obj`) with
the same 43-byte body; I recorded the first member as the source of the
candidate. The `build-files` link map emits `__onexit`, `__itoa`, and `_xtoa`
under the same names as their archive symbols. The main `build` map also
confirms the `cvt.obj` conversion-helper symbols. Where a selected routine
was not pulled into either map, its name is supported directly by its unique
COFF function symbol. The selected names are kept to one symbol per address;
the scanner's other aliases are not added as combined annotation labels.

The mappings include CRT string case/search/number helpers and standard math
entry points. They identify the exact linked CRT bodies; they do not assert
that the retail executable used the same source revision as the pinned SP5
archive.

I additionally confirmed five compiler/runtime routines from complete
LIBCMT object bodies. The first four have no relocations and match every byte;
`_qsort` has relocation operands, so its identity is based on all bytes outside
those relocation sites:

| Retail VA | Archive member / symbol | Body bytes | Relocation bytes masked | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x0051a870` | `lldiv.obj` / `__alldiv` | 170 | 0 | 170 | 0 |
| `0x0051a940` | `ullrem.obj` / `__aullrem` | 117 | 0 | 117 | 0 |
| `0x0051a9c0` | `ulldiv.obj` / `__aulldiv` | 104 | 0 | 104 | 0 |
| `0x0051aa30` | `llmul.obj` / `__allmul` | 52 | 0 | 52 | 0 |
| `0x0051be77` | `qsort.obj` / `_qsort` | 340 | 16 | 324 | 5 |

The four arithmetic helpers' complete bodies are raw-byte-identical. `_qsort`
ends with its `ret` at `0x0051bfca`; its next function, `_shortsort`, begins at
`0x0051bfcb`, which confirms the 340-byte extent. The object also contains
`_swap` at `0x0051c019`, called by `_qsort`. A focused VC6 link-map check that
references these routines emits the exact names above from `LIBCMT` members
`lldiv.obj`, `ullrem.obj`, `ulldiv.obj`, `llmul.obj`, and `qsort.obj`.

The qsort member and several other CRT bodies also have unique complete-body
matches. The listed link maps emit each symbol from the corresponding member.
All non-relocation bytes match; only `_shortsort` has any relocation bytes:

| Retail VA | Archive member / symbol | Body bytes | Relocation bytes masked | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x0051bfcb` | `qsort.obj` / `_shortsort` | 78 | 4 | 74 | 1 |
| `0x0051c019` | `qsort.obj` / `_swap` | 44 | 0 | 44 | 0 |
| `0x00522a14` | `ieee87.obj` / `__abstract_cw` | 146 | 0 | 146 | 0 |
| `0x00522aa6` | `ieee87.obj` / `__hw_cw` | 137 | 0 | 137 | 0 |
| `0x00522c69` | `intrncvt.obj` / `__ZeroTail` | 73 | 0 | 73 | 0 |
| `0x00522dd6` | `intrncvt.obj` / `__ShrMan` | 141 | 0 | 141 | 0 |
| `0x00523128` | `cfout.obj` / `___dtold` | 182 | 0 | 182 | 0 |
| `0x00523e03` | `mbctype.obj` / `_CPtoLCID` | 51 | 0 | 51 | 0 |

The qsort link-map probe emits `_shortsort` and `_swap` from `qsort.obj`; both
are called inside the 340-byte `_qsort` body. Existing VC6 build maps emit
`__abstract_cw` and `__hw_cw` from `ieee87.obj`, `__ZeroTail` and `__ShrMan`
from `intrncvt.obj`, `___dtold` from `cfout.obj`, and `_CPtoLCID` from
`mbctype.obj`. The annotations use one symbol name per address, matching the
linker map entries.

## Startup and command-line runtime routines

The current `WinMainCRTStartup` body enters several CRT startup helpers in the
retail image. These five unambiguous matches have substantial complete bodies
and exact named entries in the current VC6 link map:

| Retail VA | Archive member / symbol | Body bytes | Relocation bytes masked | Other bytes | Raw differing bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x0051e731` | `wincmdln.obj` / `__wincmdln` | 88 | 16 | 72 | 10 |
| `0x0051e842` | `stdargv.obj` / `__setargv` | 153 | 48 | 105 | 33 |
| `0x0051e8db` | `stdargv.obj` / `_parse_cmdline` | 436 | 16 | 420 | 12 |
| `0x0051ea8f` | `a_env.obj` / `___crtGetEnvironmentStringsA` | 306 | 52 | 254 | 41 |
| `0x0051ebc1` | `ioinit.obj` / `__ioinit` | 444 | 76 | 368 | 60 |

Each body's non-relocation bytes match exactly. The `build` map emits these
same five names from their respective LIBCMT archive members. They are kept as
individual CRT mappings; no alias candidates are folded into these labels.
Retail `WinMainCRTStartup` at `0x0051a33b` directly calls `__ioinit`,
`___crtGetEnvironmentStringsA`, `__setargv`, and `__wincmdln`; `__setargv`
calls `_parse_cmdline` at `0x0051e8db`.

## Full-section fingerprints beyond the discovery sizes

I also searched complete `.text` sections from the archive object members
across the retail `.text`, without using Ghidra's per-function byte counts.
This exposed four standard routines whose discovery sizes did not describe
their complete COFF symbols:

| Retail symbol VA | Archive member / symbol | Compared extent | Relocations | Result |
| --- | --- | ---: | ---: | --- |
| `0x00520dd0` | `strchr.obj` / `_strchr` | 204-byte object section | 0 | Exact section match at `0x00520dc0`; `_strchr` begins at section offset `+0x10` |
| `0x005231e0` | `strcat.obj` / `_strcpy` | 240-byte object section | 0 | Exact section match; `_strcpy` is the first 16-byte symbol |
| `0x005236f9` | `strgtold.obj` / `___strgtold12` | 1233-byte object section | 34 | 1097 unmasked bytes identical; 1185-byte function followed by a 48-byte jump table |
| `0x00524360` | `strcmp.obj` / `_strcmp` | 132-byte symbol | 0 | Exact full-symbol match |

The `strchr.obj` section contains a 16-byte prefix, then `_strchr` at offset
`0x10`, followed by the separately named 182-byte
`___from_strstr_to_strchr` helper at offset `0x16`. The retail code at
`0x00520dd0` has the matching 188-byte symbol-and-helper span; callers enter
the public `_strchr` at that address. The `strcat.obj` section similarly
contains `_strcpy` at its start and `_strcat` at offset `0x10`. The retail
callers at `0x0051c332` and `0x0051e80f` call the `_strcpy` entry. For `_strcmp`,
the retail body runs through its final `ret` at `0x005243e3`; callers such as
`0x0051f4da` use it to compare strings. The size-indexed scan missed these
because the discovery records count only 188 bytes at `_strchr`, 7 bytes at
`_strcpy`, and 129 bytes at `_strcmp`.

The first section in `strgtold.obj` matches at `0x005236f9` after masking its
34 COFF relocations. The function itself ends at `0x00523b99`; the following
48 bytes are its internal jump table. Calls at `0x0051e407`, `0x00523012`, and
`0x0052303f` enter the function, and the `build` link map emits
`___strgtold12` from that object under the same symbol name.

The `memcpy.obj` and `memmove.obj` sections are each 821 bytes and have
identical bytes and relocation layouts. Each section has a full relocation-
masked match at both `0x0051ae40` and `0x00521290` (637 unmasked bytes and a
98-byte unchanged run). Both retail blocks include the complete up/down copy
helpers through the final `ret`, rather than only the 112-byte public entry
symbol. The routines implement overlap-safe backward copying; game callers
enter `0x0051ae40`, while CRT reallocation code at `0x0051b5fa` calls
`0x00521290`. Since the two archive objects are byte-identical and both retail
blocks match both objects, this evidence identifies the CRT memory-copy
implementation family but does not distinguish which retail address is
specifically `_memcpy` versus `_memmove`. I therefore added no individual
symbol mapping for either address. The complete-body search found no exact
`_memcmp` or `_strncat` body in retail `.text`.

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
`+0x44`.

The local VC98 `STRMBASE.LIB` provides older-family evidence. Its
`obj\i386\mtype.obj` names the source as
`h:\nt\private\quartz\sdk\classes\base\mtype.cpp`; the archive member is
dated September 29, 1997. The nearby `STRMIF.H` declaration was generated by
MIDL 3.02.88 on November 13, 1997. The 26-byte `?DeleteMediaType@@...` function
from that object matches retail `0x004a2050` at every non-relocation byte
(18 compared bytes, 8 relocation bytes masked, no remaining differences),
supporting the `LIBRARY` mapping in `library_msvc.h`.

The local `?CopyMediaType@@...` body has the same 91-byte extent as retail
`0x004a2070` and the same behavior: it copies the 72-byte structure, allocates
and copies the format block, clears `cbFormat` if allocation fails, and still
AddRefs `pUnk`. The register allocation and instruction sequence differ, so I
have not mapped it as a byte-identical library body. The local
`?FreeMediaType@@...` body is 49 bytes versus 60 retail bytes; it has the same
format-buffer free/field clearing and `pUnk` release/field clearing behavior,
but its code sequence also differs.

This local archive identifies a 1997 Quartz/DirectShow base-class lineage and
an exact body for `DeleteMediaType`. It does not prove that the retail game
linked this precise archive or identify an exact Platform SDK release. The
provided files contain headers and libraries, but no `mtype.cpp` source or
SDK release manifest.

The related `0x004a2070` copy helper differs from that later public source on
allocation failure: the retail code clears `cbFormat` and still calls
`AddRef`, without returning `E_OUTOFMEMORY`. Preserve that target behavior; do
not replace it with the newer sample implementation. The local `STRMBASE.LIB`
has the same failure behavior even though its instruction sequence differs.

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

The pool is a proven startup and archive-loading dependency. Its recovered
implementation is now in the game build; exploratory history remains under
`research/allocator/`. See [memory-heap recovery](work-memoryheap.md) for raw
scores, layout and unresolved application lifecycle effects. Known CRT code
continues to come from the pinned library. A named attribution for the pool
requires stronger evidence, such as matching source/object code or symbols.
