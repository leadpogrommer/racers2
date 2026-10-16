# Decompilation progress

Latest combined Python/SP5/Wine and CMake/NMake verification: 2026-10-17. The strict match guard
verified **51 functions at 100% instruction accuracy**. 11 additional
implementations remain `STUB` because their instruction streams differ.

This is the annotated subset, not whole-game coverage. The whole-game total
is unknown. Identified CRT routines are mapped for call comparison and excluded
with `--nolib`. Unattributed allocator research is outside the game build and
these counts; see [library identification](library-identification.md).

| Original VA | Function | Instruction accuracy | Status |
| --- | --- | ---: | --- |
| `0x0045dcb0` | `FUN_0045dcb0` | 100.00% | Verified |
| `0x0045dda0` | `IsTokenCharacter` | 100.00% | Verified |
| `0x0045de00` | `IsHorizontalSpace` | 100.00% | Verified |
| `0x0045de20` | `FUN_0045de20` | 57.94% | Work in progress |
| `0x0045df40` | `FUN_0045df40` | 100.00% | Verified |
| `0x0045dfd0` | `FUN_0045dfd0` | 100.00% | Verified |
| `0x004a2130` | `CharBuffer24::CharBuffer24(char const *)` | 100.00% | Verified |
| `0x004a2170` | `CharBuffer24::CharBuffer24(void)` | 100.00% | Verified |
| `0x004a2320` | `CharBuffer128::CharBuffer128(char const *)` | 100.00% | Verified |
| `0x004a2360` | `CharBuffer128::CharBuffer128(void)` | 100.00% | Verified |
| `0x004a2380` | `CharBuffer128::AppendTilde` | 100.00% | Verified |
| `0x004a23c0` | `CharBuffer128::FindAfterDot` | 100.00% | Verified |
| `0x004a23e0` | `CharBuffer128::SetText` | 100.00% | Verified |
| `0x004a2420` | `CharBuffer128::CopyBasenameTo` | 100.00% | Verified |
| `0x004a2460` | `CharBuffer128::Uppercase` | 100.00% | Verified |
| `0x004a2480` | `CharBuffer128::CopyPathWithSeparatorTo` | 100.00% | Verified |
| `0x004a2c20` | `FileManager::Close` | 100.00% | Verified |
| `0x004a2c40` | `FileManager::GetSize` | 100.00% | Verified |
| `0x004a4620` | `WinFile::CreateAlways` | 100.00% | Verified |
| `0x004a46a0` | `WinFile::OpenOrCreate` | 100.00% | Verified |
| `0x004a4720` | `WinFile::SeekEnd` | 100.00% | Verified |
| `0x004a4790` | `WinFile::SeekCurrent` | 100.00% | Verified |
| `0x004a47b0` | `WinFile::OpenAppend` | 100.00% | Verified |
| `0x004a4ac0` | `WinFile::ReadExact` | 100.00% | Verified |
| `0x004a4b00` | `WinFile::WriteExact` | 100.00% | Verified |
| `0x004a4b40` | `WinFile::GetSize` | 100.00% | Verified |
| `0x004a4b60` | `WinFile::Close` | 100.00% | Verified |
| `0x004a4b80` | `PathExists` | 100.00% | Verified |
| `0x004a4c80` | `Vector3::Normalize` | 100.00% | Verified |
| `0x004a4ce0` | `Vector3::FUN_004a4ce0` | 100.00% | Verified |
| `0x004a4d30` | `Matrix3::FUN_004a4d30` | 25.71% | Work in progress |
| `0x004a4e00` | `Vector3::FUN_004a4e00` | 100.00% | Verified |
| `0x004a4e50` | `Vector3::FUN_004a4e50` | 100.00% | Verified |
| `0x004a4ea0` | `Matrix3::FUN_004a4ea0` | 84.62% | Work in progress |
| `0x004a4f00` | `Matrix3::FUN_004a4f00` | 84.62% | Work in progress |
| `0x004a4f60` | `Matrix3::FUN_004a4f60` | 65.19% | Work in progress |
| `0x004a5040` | `Matrix3::FUN_004a5040` | 29.27% | Work in progress |
| `0x004a5080` | `Matrix3::FUN_004a5080` | 11.63% | Work in progress |
| `0x004a5110` | `Matrix3::FUN_004a5110` | 21.28% | Work in progress |
| `0x004a5250` | `Matrix3::FUN_004a5250` | 52.38% | Work in progress |
| `0x004a52c0` | `Matrix3::FUN_004a52c0` | 34.62% | Work in progress |
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
| `0x004c32b0` | `FUN_004c32b0` | 100.00% | Verified |
| `0x004c33b0` | `FUN_004c33b0` | 100.00% | Verified |
| `0x004d15e0` | `SplineBasis::SplineBasis` | 100.00% | Verified |
| `0x004d1610` | `SplineBasis::FUN_004d1610` | 87.39% | Work in progress |
| `0x004d17d0` | `SplineBasis::FUN_004d17d0` | 100.00% | Verified |

The latest work adds path-string composition, file creation/opening and seeking,
file-manager wrappers, startup initialization/callbacks, command-line helpers,
two exact vector transforms, and eleven window geometry/fitting helpers.
Both build paths agree on all 62 measured scores and annotation statuses.
The real entry point and message-loop dependencies are traced in
[startup research](work-startup.md), with application-object/vtable evidence in
[application research](work-application.md). Window creation dependencies now
include the exact [geometry helpers](work-window-helpers.md).
The temporary entry point still does not start the game.

[Matrix/vector operations](work-matrix-helpers.md) have initial reconstructions but remain
unfinished where their instruction streams differ. The token reader remains
at 57.94% and the spline initializer at 87.39%; their matching work is documented
in [token notes](work-token-reader.md) and [spline notes](work-math-helpers.md).

Open `build/reports/LEGORACERS2.html` for the current searchable assembly diffs.
Regenerate reports with `python tools/build.py` and `python tools/compare.py`
using the Wine prefix documented in [validation.md](validation.md).
