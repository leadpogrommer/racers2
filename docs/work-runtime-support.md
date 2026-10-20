# Startup platform and archive decoder support

The arena allocator now uses the real platform wrapper at `0x004e7b90`:
`GlobalAlloc(GMEM_ZEROINIT, bytes + alignment)` followed by the observed
power-of-two address rounding. The wrapper matches exactly (31 bytes).
The allocation is process-lifetime storage; its original base is not exposed.
This is not a replacement of the pool with CRT allocation.

Three observed application leaf targets are reconstructed with provisional
names: development-mode policy `0x00403910` (false), diagnostic hook
`0x00403930` (empty, callee pops one argument), and heap reserve
`0x0049a7e0` (25 MiB). Each matches exactly. The free helper spelling does
not establish their original class scope. The application singleton and its
complete dispatch tables are now present in the runtime build. These leaf
matches do not establish equivalence of the unfinished methods they dispatch
to.

The platform error dialog at `0x004e7b50` is exact after recording its
`"Error"` string mapping. It retains MessageBox flags `0x11041`, tests IDOK,
and stores the result at `0x006843b0`. Fatal and print diagnostics at
`0x004ab2a0` and `0x004ab3b0` retain the 256-byte formatting buffer and
observed overflow checks. They remain STUB: fatal uses the Win32 DebugBreak
entry rather than the retail int3 wrapper, the optional DBUG logger is not
reconstructed (it immediately returns in the shipped storage mode 3), and
the print hook bypasses application initialization/dispatch. Final integrated raw scores are 85.25% and 83.87%, including verified
diagnostic string mappings.
The fatal function extent is 103 bytes, including the epilogue after the
break-wrapper call; the automatic 96-byte discovery omitted that epilogue.

`DecompressGtcBlock` reconstructs the cdecl transform at `0x004e7bb0`,
524 bytes. A failed pinned LIBCMT signature scan establishes no provenance;
its authorship/library origin remains unknown. The caller at `0x004a49ae`
passes source, destination and false and ignores the destination-end return.
The optional header skip reads a little-endian DWORD at source+12, then
advances by that length plus 16. Control bits are consumed least significant
first, refilling a 32-bit word when the 16-bit remaining counter reaches zero.
Short backreferences copy 2–5 bytes at a distance of 1–256; long references
use a 13-bit distance with an optional additional 0x2000 and byte/WORD length
escapes. Masked escape 1 terminates; a zero WORD count copies nothing.
Forward byte copies preserve overlapping matches.

An independent second-agent instruction/control-flow review found no semantic
mismatch in the decoder, including refill timing, count wraps, header skip,
escape termination and overlap. Its initial raw instruction match is only
9.78%; the readable helper-based implementation has a substantially different
compiled shape and remains STUB under the current annotation threshold.
No repeated instruction polishing or game behavioral tests were added.

The bootstrap remains temporary. Startup now constructs the application,
accesses its display config, initializes archive metadata, and exits cleanly,
but rendering and the game loop are still required before a playable
milestone. See [application runtime](work-application-runtime.md).

## Bounded metadata startup run

On 2026-10-17 the reconstructed executable ran from the supplied installation
under Wine and exited with code 0. Runtime initialization constructed the
application and accessed the per-user
`C:\users\ilya\Documents\Games\LEGO Racers 2\Saved Games\LR2.cfg` path.
The ignored `build/msvc/runtime-application-startup.log` trace records full reads of
FILELIST.INF (784,312 bytes) and COMPRESS.INF (43,740 bytes), and the redirected
read-only open of GAMEDATA.GTC. A separate ignored asset probe exercises
compressed reads of three shipped files, including a texture spanning three
logical blocks. Neither run creates a display, renders a menu, or starts
gameplay; the texture probe does not claim pixel correctness.
