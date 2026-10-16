# LEGO Racers 2 decompilation

Target: the supplied PC `LEGO Racers 2.exe`, module **LEGORACERS2**. This is
an early matching decompilation, not a playable replacement. Read
`docs/compiler.md`, `README.md`, and `CONTRIBUTING.md` before changing code.

## Compiler and build

Use **Visual C++ 6.0 SP5**: CL.EXE **12.00.8804**, C1XX.DLL **12.00.8964**,
C2.DLL **12.00.8966**, LINK.EXE **6.00.8447**. The original's dominant Rich
record is `Utc12_CPP / 8966`, not LR1's `8168`. `tools/toolchain.py` verifies
these components; the compiler banner alone cannot distinguish the backends.

The executable uses DirectX **8**, static CRT, x86, base `0x00400000`, and
entry `0x0051a33b`. No GolDP target. Do not import LR1 addresses, class layouts,
CRT patches, function totals, or code-generation conclusions without LR2 evidence.
The exact original linker patch is not independently recorded by its PE header.
The `/O2 /Ob1 /MT /GX` settings produce the current verified helper matches;
other translation units may require different optimization settings.

From this repository, with reccmp installed in the active Python environment:

```sh
python tools/setup.py --fetch-toolchain
python tools/build.py
python tools/compare.py
python tools/compare.py --address 0x0045de00
python -m unittest discover -s tests
```

Linux requires Wine; `WINEPREFIX` selects the prefix for both build and reccmp.
The build manifest is `sources.txt`; add new compilation units there. Keep the
flags in CMake and `tools/build.py` aligned. `bootstrap.cpp` is temporary entry
scaffolding with no retail address; replace it when reconstructing real startup.
The comparison harness links with `/OPT:NOREF,ICF` so that recovered functions
remain measurable before their original callers exist. Parallel workers can
use `tools/build.py --source-list <manifest> --build-dir build-<worker>`;
source paths in the manifest remain relative to this repository.
Use per-function reccmp comparison to validate recovered game functions.
Do not add behavioral tests for reconstructed game functions unless requested.
Keep library identification and the tests protecting identification/match tooling.
Only execute a recompiled game when explicitly needed; ordinary matching reads
the original and emits PE/PDB/report files without running the game.

## Annotations and evidence

- Check runtime/SDK provenance before reconstructing generic library-looking
  routines. Known library implementations should come from their library when
  available, with verified `LIBRARY` mappings. See `docs/library-identification.md`;
  a failed signature search does not establish game authorship. The unattributed
  pool allocator is deferred under `research/allocator/`, outside game progress.
- Use the exact module `LEGORACERS2` and lowercase, eight-digit original VAs.
- `// FUNCTION: LEGORACERS2 0x0045de00` means a measured 100% instruction match.
  Use `// STUB:` for incomplete work; `STUB(address)` prevents accidental folding.
- Use `// LIBRARY:` for CRT/third-party symbols, `// SYNTHETIC:` for generated
  methods, `// GLOBAL:` for variable storage, and `// VTABLE:` for vtables.
- The colon is required. Globals identify the variable, not a pointed-to string.
- Order functions by ascending original address within each compilation unit.
- Keep genuine folded functions marked `FOLDED`; never add artificial global
  writes or assembly blobs to make an implemented function appear matched.
- Record proven function lengths in `reccmp/function-sizes.csv` (decimal bytes).
  Include all branches after early returns; do not stop at the first `ret`.
- Compare functions and vtables after layout changes and run `reccmp-datacmp`
  when changing annotated data. A type, calling convention, or class hierarchy
  must be supported by disassembly/callers and matching output.
- Never report percentages over the annotated subset as whole-game completion.
  There is no verified whole-game function count yet.

## C++ style

Write C++98 accepted by VC6. Use `util/compat.h` for `override` compatibility,
`util/decomp.h` for layout assertions and unknown `undefined/undefined2/undefined4`
types, and `util/types.h` for `LegoS32`, `LegoU32`, `LegoFloat`, `LegoChar`, etc.
Use original Win32 types at API boundaries and the unsuffixed ANSI API macros.
Use `NULL` for pointers and `TRUE/FALSE` for Lego boolean types.

Unknown functions: `FUN_XXXXXXXX`; globals: `g_unk0xXXXXXXXX`; members:
`m_unk0xNN`; parameters: `p_...`; virtual slots: `VTable0xNN`. Give semantic
names when supported by evidence. Document member and vtable slot offsets.
Use one root class per header, coherent class implementation units, and gap
arrays expressed as `end - start`. Recover member access rather than copying
decompiler pointer arithmetic. Preserve behavior while matching, including bugs.

Run clang-format on changed C++ files. Run `tools/compare.py` before claiming a
match; it lints, generates HTML/JSON/SVG, and rejects missing/sub-100% FUNCTION
claims. No naming-convention CI from LR1 is inherited.

## Repository hygiene

Keep original executables, assets, compiler binaries, Wine prefixes, databases,
virtual environments and generated reports ignored. The installation directory
is outside this repo. The `template` remote records LR1 provenance and is not a
publishing destination. Do not push to it.
