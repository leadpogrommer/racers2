# Renderer floating-point hooks

The renderer initializer calls `SaveFloatingPointControl` (`0x004e7b30`,
8 bytes), then `InitializeRendererFloatingPoint` (`0x004e7b10`, 31 bytes)
before `Direct3DCreate8`. Its shutdown calls `RestoreFloatingPointControl`
(`0x004e7b40`, 7 bytes). The frame/update path also calls the initializer.
All three hooks take no explicit arguments and have no meaningful result.

Saving uses the waiting x87 `FSTCW` instruction to copy the raw 16-bit control
word into process-wide storage at `0x006843ac`. Initialization executes
`FINIT`, reads the resulting control word, clears precision-control bits
`0x0300` with mask `0xfcff`, loads the word, and waits. This selects 24-bit
precision after resetting x87 state. Restoration loads the saved control word;
it does not restore the register stack or status. The global begins at zero,
as in the retail BSS, and callers must preserve the observed save/setup/restore
order.

The implementation uses inline assembly solely for x87 hardware operations
that portable C++ cannot express. The mask operation remains C++; there are
no encoded byte arrays, naked function bodies, or artificial matching writes.

Provenance is not established as game-authored code. The pinned LIBCMT scan
found no candidate for the 31-byte setup body with 16 unmasked bytes required;
the shorter save/restore wrappers fall below that comparison threshold. The
archive's `__fpreset` in `ieee87.obj` is a distinct 43-byte implementation,
and its `__control87`/`__controlfp` have 53/22-byte bodies. These identifications
do not justify substituting those CRT operations for the observed raw x87
state sequence or declaring the wrappers a known library. The local scan is
retained in ignored `data/fpu-library-candidates.csv`.

This batch uses build and binary comparison only. No runtime launch is needed
for this recovery; runtime validation is reserved for a significant menu or
gameplay milestone.

Both combined VC6 builds measure save and restore at 100% raw instruction
accuracy; their annotations are `FUNCTION`. Initialization measures 71.43%
and remains `STUB`. The C++ mask compiles differently from the retail
register/stack sequence; it was retained without replacing it with matching
assembly. Data comparison verifies the saved 16-bit global in both builds.
