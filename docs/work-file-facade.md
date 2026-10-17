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

The remaining open/initialization closure depends on 0x004a25c0,
0x004a2a30, 0x004a2810 and the application policy used by 0x004a4570.
Allocator research remains outside the main build. The working type names
and the storage getter's original source scope remain provisional.
