# File façade boundary notes

The routines around `0x004a21b0` are not `CharBuffer24` methods. The
constructor at `0x004a21b0` writes six 32-bit fields at object offsets
`+0x00`, `+0x04`, `+0x08`, `+0x0c`, `+0x10`, and `+0x14`, including `-1`
initial values at `+0x04` and `+0x10`, then dispatches to `0x004a21f0` with
the path, mode, and optional output pointer. Callers such as `0x00403380`
reserve a `0x18`-byte local for this constructor. This establishes a minimum
object extent of `0x18`; it does not prove that arrays of these objects use
that stride.

`0x004a21f0` selects file operations by mode and depends on the file-manager
singleton (`0x004a2500`), existence/open helpers (`0x004a2a30`, `0x004a4570`),
and transfer helper `0x004a2810`. The forwarding routines `0x004a22b0` and
`0x004a22c0` obtain that singleton and call its `Close` and `GetSize` methods;
`0x004a22d0` is a tail thunk to `0x004a22b0`, and `0x004a22e0` forwards to
`FileManager::SeekCurrent`. The singleton accessor lazily initializes global
storage through `0x004a2560` and registers its cleanup thunk `0x004a2550` with
`atexit`.

These routines belong to the partial WinFile/FileManager model below. The
constructor/open dispatch still needs its remaining callees; neither class
has a proven complete layout.

## Recovered initialization and forwarding closure

The partial WinFile model now includes six 32-bit fields: handle at 0, cached
size at 4, archive offset at 8, logical position at 0xc, an unknown word at
0x10, and compressed-read state at 0x14. FileManager embeds this file at
+0x2c, followed by its archive position at +0x44 and flags at +0x48. The
constructor at 0x004a2560 (83 bytes) matches 100% with that member model; it
initializes fields through +0x48, leaves +0x10 untouched, calls the current
storage-mode getter at 0x0049ada0 (6 bytes, returns 3), and installs the paths
GameData.Dat, FileList.Inf and the pointer to Compress.Inf.

Manager-aware Close (0x004a22b0, 14 bytes), GetSize (0x004a22c0, 14 bytes),
the WinFile destructor (0x004a22d0, 5 bytes), relative-seek forwarding
(0x004a22e0, 21 bytes), flag updates (0x004a2d80, 40 bytes), and bounded native
seeking (0x004a4740, 69 bytes) also match 100%. The bounded seek clamps a
signed offset to [-size,size] and uses FILE_BEGIN for nonnegative offsets
and FILE_END for negative offsets, mapping INVALID_SET_FILE_POINTER to zero.
The 94-byte archive-aware relative seek at 0x004a2d20 remains STUB at 79.49%.
Retail returns the updated logical position in EAX. The caller at 0x004a29bb
stores the result of the forwarding wrapper at 0x004a22e0 and passes it to
SeekBounded at 0x004a29da, establishing that the result is consumed. Both
FileManager::SeekCurrent and WinFile::SeekManagedCurrent therefore return
DWORD; the latter directly forwards the value. The best natural source form
uses a scoped archive-position local for the pre-seek comparison and updates
the file position before returning it. Its remaining mismatch is compiler
register/evaluation order around the archive-position loads and post-seek
position update; the semantically correct return type is retained despite the
slightly lower score than the prior incorrect void form.

The 65-byte lazy accessor at 0x004a2500 remains STUB at 88.89%. Global pointer
0x00578b60 and path pointer 0x005498ec are recovered storage. Its real guard
byte at 0x00578b5c is mapped using the emitted PDB name in
compiler-globals.csv. The local manager at 0x00578b10 has a COFF symbol but
no PDB data symbol in VC6's output, so it has no source GLOBAL claim yet.
The pinned PDB importer cannot currently resolve its address operands.

The generated cleanup at 0x004a2550 is 10 bytes. Retail directly supplies the
embedded file address and jumps to the WinFile destructor; this build passes
the manager address through its generated destructor. Its PDB name $E2 is
now shared with generated screenshot initialization, so the ambiguous
SYNTHETIC name mapping was removed. The original size remains recorded for
research. Neither the cleanup nor the singleton accessor is claimed exact.
The singleton guard, path pointer and manager pointer pass data comparison;
the unresolved local static object is not included.

## Live mode-3 asset-read closure

The current implementation recovers the mode-3 path used by this executable:
`FileList.Inf` is opened as a native file, its 0x88-byte records are read into
the heap and their 128-byte names are uppercased in place. The archive member
then opens `GameData.Dat`; the retail path policy redirects that request once
to `GameData.gtc` and marks the stream compressed. The archive handle must stay
open after initialization. Its size is not stored by the live mode-3 branch;
`m_archiveSize` remains at its constructor value. `FindArchiveEntry` skips the
three manager-owned path pointers and flag bit 0, searches the table from the
cached index, and returns “handled” separately from its `p_found` output. A
miss in archive mode therefore does not fall through to the physical file
system.

`FileManager::Read` routes native files to `WinFile::ReadAt`; archive-backed
files use `archiveOffset + logicalPosition`, seek the shared GTC handle when
needed, read through the compressed reader, and advance the logical position
by the requested byte count. The standalone GTC block cache reads the 20-byte
records from `Compress.Inf`, retains a 0x20080-byte working buffer, and either
copies full 0x20000-byte blocks or invokes the block decoder for stored
compressed blocks. The installed tree contains `GAMEDATA.GTC`, `FILELIST.INF`
and `COMPRESS.INF`; the archive contains asset paths not present in the shallow
extracted tree, so the archive path is needed for ordinary game data.
The installed `FILELIST.INF` is 784,312 bytes (5,767 records of 0x88 bytes),
and `COMPRESS.INF` is 43,740 bytes (2,187 records of 0x14 bytes). The
compressed-record extents are contiguous and end at the 135,813,120-byte GTC
file size, which supports the recovered archive-offset interpretation.

The global cache's constructor can be the first caller of `GetFileManager` and
`GetMemoryHeap`. It opens `Compress.Inf`; the subsequent track-open callback
enters manager initialization, whose guard is set before opening the list and
archive. The archive open consumes the one-time `.Dat` to `.gtc` redirection
flag. Once manager initialization returns, the cache finishes reading its
compression table and allocates its buffer. The heap accessor constructs the
pool on demand, using the application reserve and aligned-allocation helpers.
A full startup run confirms application construction followed by complete
FILELIST/COMPRESS reads and an open GTC handle. A separate ignored asset probe
uses this file path to read selected real compressed assets, including a
multiblock texture and a late archive entry. It does not establish pixel
correctness or compare output against an independent retail decoder; see
[validation](validation.md).

`OpenPath` is still marked `STUB`. Its CreateFile arguments, retry-on-sharing
behavior and `.Dat` redirect are present, and retail calls the constructed
application accessor and its slot `+0x14`. The complete dispatch table now
provides that call path; the method remains unfinished at 80.31% raw similarity,
below the near-match threshold. Its body has not been independently validated. The
unique-file transfer logic in `TrackOpen` for storage mode 1 and the separate
mode-1/mode-2 initialization branches are also unfinished. The shipped
storage-mode helper returns 3, so those branches are not used by this build's
live asset path. The block decoder is separately implemented and reviewed by
the root worker; it remains annotated `STUB` because its instruction match is
low, not because this cache substitutes a fake success result.

### Function extents and matching

The isolated build used MSVC6 SP5 with the current heap, application-policy,
platform-allocation and decoder units. Raw reccmp results are recorded below;
all unresolved or low-match routines remain `STUB`. The WinFile constructor is
exact. `WinFile::Read` is a reviewed 95.24% forwarding match: both versions
obtain the manager and forward the same file, buffer, byte count and flags to
its read method; the remaining raw difference is compiler call setup.

| VA | Working name | Length | Raw | Annotation |
| --- | --- | ---: | ---: | --- |
| `0x004a21b0` | `WinFile::WinFile` | 53 | 100.00% | `FUNCTION` |
| `0x004a21f0` | `WinFile::Open` | 138 | 26.79% | `STUB` |
| `0x004a2290` | `WinFile::Read` | 29 | 95.24% | `FUNCTION` |
| `0x004a25c0` | `FileManager::Initialize` | 576 | 8.12% | `STUB` |
| `0x004a2810` | `FileManager::TrackOpen` | 544 | 3.45% | `STUB` |
| `0x004a2a30` | `FileManager::FindArchiveEntry` | 365 | 24.80% | `STUB` |
| `0x004a2ba0` | `FileManager::Read` | 121 | 87.38% | `STUB` |
| `0x004a4570` | `WinFile::OpenPath` | 175 | 89.43% | `STUB` |
| `0x004a4820` | GTC cache constructor | 180 | 63.79% | `STUB` |
| `0x004a48e0` | GTC cache destructor | 65 | 76.19% | `STUB` |
| `0x004a4930` | GTC cache block load | 185 | 36.51% | `STUB` |
| `0x004a49f0` | compressed block read | 152 | 46.85% | `STUB` |
| `0x004a4a90` | `WinFile::ReadAt` | 48 | 55.00% | `STUB` |

Lengths include all branch bodies through their retail return instruction.
The compressed-reader signature is a four-argument `__stdcall` helper
(`buffer, bytes, archive offset, flags`); the fourth argument is ignored by
retail. `ReadAt` is a four-argument `thiscall` WinFile method. `LoadBlock`
uses the cache object at `0x00578ca8`, the 20-byte compression records and the
embedded archive file in FileManager. The remaining assembly differences are
large enough that no raw-score claim is used as evidence of equivalence.

The literal strings used by this path are `GameData.Dat` at `0x00549914`,
`FileList.Inf` at `0x00549904`, `Compress.Inf` at `0x005498ec`,
`GameData.gtc` at `0x00549b5c`, `GameData.dat` at `0x00549b6c`,
`cFileCat: File entry list` at `0x00549938`, `Compressed data buffer` at
`0x00549b7c`, and `sCompressedChunkInfo array` at `0x00549b94`. These are
string-pool entries, not standalone pointer globals. The guard at
`0x00578b64`, redirect flag at `0x00549b58`, file-manager pointer at
`0x00578b60`, and cache object at `0x00578ca8` are storage objects. The
file-manager singleton's local object at `0x00578b10` remains unannotated
because the matching PDB importer does not resolve its storage operand.

## Configuration existence checks

The managed path-existence facade at 0x004a2300 (18 bytes) is exact. The
manager check at 0x004a2c60 (188 bytes) follows mode/flag selection, uppercases
a 128-byte path buffer and compares all 128 bytes against archive entries,
otherwise calling native PathExists. It measures 62.30% with a readable
memcmp implementation in place of the retail DWORD loop. Independent review
found no semantic deviation; it stays STUB below the promotion threshold.
Application config enables flag 1 while accessing per-user files, selecting
the native branch; see [display config recovery](work-application-config.md).
