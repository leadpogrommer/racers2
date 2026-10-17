# Decompilation progress

The primary milestone is a playable rebuild. Instruction accuracy remains
visible, but does not take precedence over startup and gameplay dependencies.
Reviewed semantically identical near matches (initially at least 95% raw) are
implemented functions, counted separately from exact matches. The current
checkpoint has no reviewed near-match promotions; its counts below are unchanged.

Latest combined Python/SP5/Wine and CMake/NMake verification: 2026-10-17.
The strict match guard verified **79 functions at 100% instruction accuracy**.
19 additional reconstructions remain `STUB` because they have unresolved
behavior or are below the near-match threshold. Both build paths agree on all 98 measured scores and annotation statuses.
Data comparison verifies 14 globals with zero issues.

This is the reconstructed subset, not whole-game coverage. The automatic
Ghidra inventory has **4,228 discovered function starts**, including **21 thunks**
and linked runtime/library code. It is incomplete and is not a verified count
of game-authored functions; see [inventory notes](research.md#automatic-function-inventory).
Identified libraries are excluded from these matches with `--nolib`.
Allocator research remains outside the main build and counts, but is now a
required startup dependency in the [runtime roadmap](runtime-roadmap.md);
see [library identification](library-identification.md).

| Original VA | Function | Instruction accuracy | Status |
| --- | --- | ---: | --- |
| `0x0045dcb0` | `FUN_0045dcb0` | 100.00% | Verified |
| `0x0045dda0` | `IsTokenCharacter` | 100.00% | Verified |
| `0x0045de00` | `IsHorizontalSpace` | 100.00% | Verified |
| `0x0045de20` | `FUN_0045de20` | 57.94% | Work in progress |
| `0x0045df40` | `FUN_0045df40` | 100.00% | Verified |
| `0x0045dfd0` | `FUN_0045dfd0` | 100.00% | Verified |
| `0x0049ada0` | `GetFileStorageMode` | 100.00% | Verified |
| `0x004a2130` | `CharBuffer24::CharBuffer24(char const *)` | 100.00% | Verified |
| `0x004a2170` | `CharBuffer24::CharBuffer24(void)` | 100.00% | Verified |
| `0x004a22b0` | `WinFile::CloseManaged` | 100.00% | Verified |
| `0x004a22c0` | `WinFile::GetManagedSize` | 100.00% | Verified |
| `0x004a22d0` | `WinFile::~WinFile` | 100.00% | Verified |
| `0x004a22e0` | `WinFile::SeekManagedCurrent` | 100.00% | Verified |
| `0x004a2320` | `CharBuffer128::CharBuffer128(char const *)` | 100.00% | Verified |
| `0x004a2360` | `CharBuffer128::CharBuffer128(void)` | 100.00% | Verified |
| `0x004a2380` | `CharBuffer128::AppendTilde` | 100.00% | Verified |
| `0x004a23c0` | `CharBuffer128::FindAfterDot` | 100.00% | Verified |
| `0x004a23e0` | `CharBuffer128::SetText` | 100.00% | Verified |
| `0x004a2420` | `CharBuffer128::CopyBasenameTo` | 100.00% | Verified |
| `0x004a2460` | `CharBuffer128::Uppercase` | 100.00% | Verified |
| `0x004a2480` | `CharBuffer128::CopyPathWithSeparatorTo` | 100.00% | Verified |
| `0x004a2500` | `GetFileManager` | 83.33% | Work in progress |
| `0x004a2560` | `FileManager::FileManager` | 100.00% | Verified |
| `0x004a2c20` | `FileManager::Close` | 100.00% | Verified |
| `0x004a2c40` | `FileManager::GetSize` | 100.00% | Verified |
| `0x004a2d20` | `FileManager::SeekCurrent` | 79.49% | Work in progress |
| `0x004a2d80` | `FileManager::SetFlags` | 100.00% | Verified |
| `0x004a4620` | `WinFile::CreateAlways` | 100.00% | Verified |
| `0x004a46a0` | `WinFile::OpenOrCreate` | 100.00% | Verified |
| `0x004a4720` | `WinFile::SeekEnd` | 100.00% | Verified |
| `0x004a4740` | `WinFile::SeekBounded` | 100.00% | Verified |
| `0x004a4790` | `WinFile::SeekCurrent` | 100.00% | Verified |
| `0x004a47b0` | `WinFile::OpenAppend` | 100.00% | Verified |
| `0x004a4ac0` | `WinFile::ReadExact` | 100.00% | Verified |
| `0x004a4b00` | `WinFile::WriteExact` | 100.00% | Verified |
| `0x004a4b40` | `WinFile::GetSize` | 100.00% | Verified |
| `0x004a4b60` | `WinFile::Close` | 100.00% | Verified |
| `0x004a4b80` | `PathExists` | 100.00% | Verified |
| `0x004a4bc0` | `GetPathDriveKind` | 35.71% | Work in progress |
| `0x004a4c80` | `Vector3::Normalize` | 100.00% | Verified |
| `0x004a4ce0` | `Vector3::FUN_004a4ce0` | 100.00% | Verified |
| `0x004a4d30` | `Matrix3::FUN_004a4d30` | 25.71% | Work in progress |
| `0x004a4e00` | `Vector3::FUN_004a4e00` | 100.00% | Verified |
| `0x004a4e50` | `Vector3::FUN_004a4e50` | 100.00% | Verified |
| `0x004a4ea0` | `Matrix3::FUN_004a4ea0` | 100.00% | Verified |
| `0x004a4f00` | `Matrix3::FUN_004a4f00` | 100.00% | Verified |
| `0x004a4f60` | `Matrix3::FUN_004a4f60` | 100.00% | Verified |
| `0x004a5040` | `Matrix3::FUN_004a5040` | 100.00% | Verified |
| `0x004a5080` | `Matrix3::FUN_004a5080` | 42.22% | Work in progress |
| `0x004a5110` | `Matrix3::FUN_004a5110` | 100.00% | Verified |
| `0x004a5150` | `Matrix3::FUN_004a5150` | 77.22% | Work in progress |
| `0x004a5250` | `Matrix3::FUN_004a5250` | 61.90% | Work in progress |
| `0x004a52c0` | `Matrix3::FUN_004a52c0` | 100.00% | Verified |
| `0x004a5310` | `Quaternion::Normalize` | 85.37% | Work in progress |
| `0x004a5380` | `PackedQuaternion::Pack` | 34.23% | Work in progress |
| `0x004a5440` | `Quaternion::Decode` | 83.87% | Work in progress |
| `0x004a5dc0` | `RoundUpToPowerOfTwo` | 100.00% | Verified |
| `0x004a5df0` | `CountTrailingZeroBits` | 100.00% | Verified |
| `0x004a5e10` | `BuildSineTable` | 100.00% | Verified |
| `0x004b4690` | `FUN_004b4690` | 100.00% | Verified |
| `0x004b49e0` | `FUN_004b49e0` | 100.00% | Verified |
| `0x004b4a60` | `FUN_004b4a60` | 100.00% | Verified |
| `0x004b4a80` | `FUN_004b4a80` | 100.00% | Verified |
| `0x004b4b30` | `FUN_004b4b30` | 100.00% | Verified |
| `0x004b4b80` | `GetWindowWidth` | 100.00% | Verified |
| `0x004b4bb0` | `GetWindowHeight` | 100.00% | Verified |
| `0x004b4be0` | `GetWindowLeft` | 100.00% | Verified |
| `0x004b4c00` | `GetWindowTop` | 100.00% | Verified |
| `0x004b4c20` | `GetWindowOwner` | 100.00% | Verified |
| `0x004b4c30` | `GetWindowStyle` | 100.00% | Verified |
| `0x004b4c40` | `SetWindowWidth` | 100.00% | Verified |
| `0x004b4cd0` | `SetWindowHeight` | 100.00% | Verified |
| `0x004b4d60` | `ExpandChildWindowBounds` | 100.00% | Verified |
| `0x004b4de0` | `FitWindowToChildren` | 100.00% | Verified |
| `0x004b4e80` | `ScreenRectToClient` | 100.00% | Verified |
| `0x004bb1f0` | `SetScreenshotDirectory` | 100.00% | Verified |
| `0x004c32b0` | `FUN_004c32b0` | 100.00% | Verified |
| `0x004c33b0` | `FUN_004c33b0` | 100.00% | Verified |
| `0x004c3460` | `FindFirstNonemptyRow` | 100.00% | Verified |
| `0x004c3d70` | `BitmapImage::BitmapImage` | 100.00% | Verified |
| `0x004c3db0` | `BitmapFont::GetGlyphMetrics` | 100.00% | Verified |
| `0x004c3f40` | `Terrain::GetCellVertices` | 63.61% | Work in progress |
| `0x004c4140` | `Terrain::GetMinimumBounds` | 100.00% | Verified |
| `0x004c41a0` | `Terrain::GetMaximumBounds` | 100.00% | Verified |
| `0x004cb340` | `TerrainVertexGrid::SelectEdges` | 100.00% | Verified |
| `0x004cb630` | `Terrain::GetVertex` | 100.00% | Verified |
| `0x004cb680` | `Terrain::GetTile` | 100.00% | Verified |
| `0x004d0f60` | `SplinePath::FUN_004d0f60` | 48.34% | Work in progress |
| `0x004d11c0` | `SplinePath::FUN_004d11c0` | 47.13% | Work in progress |
| `0x004d1390` | `SplinePath::FUN_004d1390` | 47.00% | Work in progress |
| `0x004d15e0` | `SplineBasis::SplineBasis` | 100.00% | Verified |
| `0x004d1610` | `SplineBasis::FUN_004d1610` | 89.45% | Work in progress |
| `0x004d17d0` | `SplineBasis::FUN_004d17d0` | 100.00% | Verified |
| `0x004d1800` | `FUN_004d1800` | 65.28% | Work in progress |
| `0x004d1a20` | `SplinePath::FUN_004d1a20` | 100.00% | Verified |
| `0x004d1a70` | `SplinePath::FUN_004d1a70` | 100.00% | Verified |
| `0x004d1aa0` | `SplinePath::FUN_004d1aa0` | 39.73% | Work in progress |
| `0x004d1d90` | `SplinePath::FUN_004d1d90` | 47.71% | Work in progress |

The latest recovery adds five exact terrain methods: minimum/maximum bounds,
tile lookup, border-height selection and vertex lookup. The cell-to-world
helper and quaternion decoder remain STUB. Spline evaluator source now follows
the retail copy-then-scale order, with improved measured scores. File seeking
returns its position through the facade, as established by a caller that
consumes EAX; the exact wrapper is retained and the manager implementation
remains STUB. One original
generated cleanup at 0x004a2550 has a recorded size but no unique PDB-name
mapping; it is not included in the 98 measured rows or claimed exact.

Evidence and open dependencies are recorded in the [file facade](work-file-facade.md),
[matrix/vector and quaternion notes](work-matrix-helpers.md),
[spline evaluation](work-spline-evaluation.md), [scalar helpers](work-scalar-helpers.md),
[bitmap scanner](work-bitmap-helpers.md), [bitmap font](work-bitmap-font.md),
[terrain](work-terrain.md), [quaternion codec](work-math-helpers.md),
and [screenshot](work-screenshot.md) notes.
The [startup research](work-startup.md) and [application-object research](work-application.md)
trace the real entry point and message-loop dependencies. The temporary entry
point still does not start the game.

Open `build/reports/LEGORACERS2.html` for searchable assembly diffs.
Regenerate reports with `python tools/build.py` and `python tools/compare.py`
using the Wine prefix documented in [validation.md](validation.md).
