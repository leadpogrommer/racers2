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
python tools/directx8.py --mode wine --output-dir build-dx8-config \
  --d3dx8-library /path/to/DirectX8/Lib/d3dx8.lib \
  --directshow-sdk-root /path/to/DirectX8
python tools/build.py --directx8-config build-dx8-config/directx8-config.json
python tools/compare.py
python tools/compare.py --address 0x0045de00
python -m unittest discover -s tests
```

Linux requires Wine; `WINEPREFIX` selects the prefix for both build and reccmp.
The user-data path imports SHGetFolderPathA from SHFOLDER. On Linux the builder
selects Wine's PE-i386 archive; on Windows provide an SDK library through
SHFOLDER_LIB or the SDK LIB path. CMake also accepts LR2_SHFOLDER_LIBRARY.
For a local genuine SDK, generate the configuration with
`python tools/directx8.py --mode sdk --sdk-root <path>`. CMake reads the same
JSON path from `LR2_DIRECTX8_CONFIG`. Use `util/directx8.h` for the real
SDK/Wine interfaces; never define replacement COM tables. Startup movie rendering
uses the VC6-compatible SDK DirectShow BaseClasses headers and genuine
STRMBASE.LIB; see `docs/work-directshow.md`. Both builders must put project
headers before the external SDK paths so `util/d3dx8.h` is selected.

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
Reserve runtime testing for significant milestones, such as a build expected
to open and correctly render the menu. Do not run incremental startup or asset
probes between small recovery batches. Ordinary build, matching and data-layout
checks read the original and emit PE/PDB/report files without running the game.

## Primary objective: a playable game

The user's top priority is to get the rebuilt game running with the supplied
installation assets. Prioritize the real startup path, application lifecycle,
resource/file loading, graphics/input/audio initialization, main loop and
connections needed to reach gameplay. Track observable milestones such as
startup, a rendered menu and a playable race. The current temporary bootstrap
is not a running game; do not describe a placeholder window as gameplay.
The current bounded runtime milestone constructs the application, accesses its
per-user display config, reads both archive metadata files, and opens the GTC
archive. It still does not create a window or render gameplay. Reuse the retail
dialog/icon resources through `tools/extract_resources.py`; keep generated
`.res` files in ignored build output. Renderer builds require the tested local
DirectX 8 configuration from `tools/directx8.py` and the real DX8 interfaces.

Instruction matching is a diagnostic and secondary goal. Do not postpone a
needed subsystem to polish register allocation or instruction scheduling in
an otherwise recovered leaf helper. Use builds and eventual game runs to
validate runnable milestones when the implementation supports them. Continue
library identification; do not spend the main effort rebuilding known libraries.

## Agent workflow and priorities

Use the user's requested cheaper-model subagents for independent areas, with
separate file ownership and isolated build directories. Keep shared manifests,
size/literal CSVs and combined validation with the integration owner. Use the
available parallel capacity when there is independent decompilation work;
do not assign several agents to edit the same files concurrently.

Every agent must attempt to infer descriptive function names from the recovered
code, callers and data. Use the inferred working name once the behavior is
understood, including for non-exact reconstructions, and explain uncertain
interpretations in the notes. These are descriptive names, not claims to have
recovered original symbols. Keep `FUN_XXXXXXXX` only when there is not enough
evidence to describe what the function does. Rename declarations, definitions
and callers together when improving an existing name.

Prioritize recovering more behavior over spending successive batches on small
instruction-match improvements. Build and measure each reconstruction, then
use bounded matching attempts. If the code is close and the agent is confident
that it behaves identically, retain the readable implementation, record the
measured score and the reasoning for behavioral equivalence, and move to the
next function. Register allocation and instruction scheduling differences do
not require endless polishing. A similarity score alone does not establish
equivalence; unresolved branch, aliasing, rounding or caller-contract differences
must remain documented as unresolved, rather than called functionally recovered.

A function confidently recovered with identical behavior and a near-perfect
instruction match is implemented, not a stub. Use `FUNCTION` and document the
remaining code-generation differences and equivalence reasoning in work notes.
The initial near-match threshold is 95% raw instruction accuracy. A score alone
is insufficient: do not promote code with known behavioral differences or
unresolved semantics. Exact matches and reviewed near matches are counted
separately, with their actual scores visible. Use `STUB` for unfinished or
unresolved reconstructions. Move on once a function is sufficiently recovered;
perfect matching must not block progress toward a playable game.

## Annotations and evidence

- Check runtime/SDK provenance before reconstructing generic library-looking
  routines. Known library implementations should come from their library when
  available, with verified `LIBRARY` mappings. See `docs/library-identification.md`;
  a failed signature search does not establish game authorship. Pool research
  is under `research/allocator/`. The pool is a verified startup/asset-loading
  dependency: resolve its interfaces and required behavior for the playable
  build while keeping its provenance explicit and reusing known libraries.
- Use the exact module `LEGORACERS2` and lowercase, eight-digit original VAs.
- `// FUNCTION: LEGORACERS2 0x0045de00` marks an implemented function: an exact
  match or a reviewed, semantically identical near match (initially at least
  95% raw instruction accuracy). Keep raw accuracy and review reasoning visible.
  Use `// STUB:` for unfinished/unresolved work; `STUB(address)` prevents
  accidental folding of placeholders.
- Use `// LIBRARY:` for CRT/third-party symbols, `// SYNTHETIC:` for generated
  methods, `// GLOBAL:` for variable storage, and `// VTABLE:` for vtables.
- The colon is required. Globals identify the variable, not a pointed-to string.
- Order functions by ascending original address within each compilation unit.
- `VTABLE` annotations require a C++ class with compiler-emitted virtual methods.
  For explicit ABI structs containing function pointers, leave the table's
  retail VA in a plain comment and verify its complete CSV inventory against
  both PEs and linker maps. A `VTABLE` marker on such a struct is invalid.
- Keep genuine folded functions marked `FOLDED`; never add artificial global
  writes or assembly blobs to make an implemented function appear matched.
- Record proven function lengths in `reccmp/function-sizes.csv` (decimal bytes).
  Include all branches after early returns; do not stop at the first `ret`.
  Exclude alignment padding after the final code byte.
- Read initialized global values from the PE before defining their defaults;
  do not assume a discovered global starts at zero. Verify initial pointers
  against real backing storage and check annotated data after integration.
- Calculate compound offsets, array ends and signed displacements with a script,
  then confirm the actual addressing instruction. Do not rely on mental hex
  arithmetic when defining storage, overlaps or class bounds.
- Derive each callback ABI from argument pushes, receiver setup and stack
  cleanup. Use fastcall bridges for evidenced thiscall methods, and retain
  cdecl/stdcall for callbacks that actually use those conventions. Trace pushes
  across intervening calls: an argument can be staged for a later call while
  a no-argument getter runs. A push immediately before a call does not by
  itself establish that callee takes an argument.
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
working names inferred from behavior as described above. Document member and
vtable slot offsets.
Use one root class per header, coherent class implementation units, and gap
arrays expressed as `end - start`. Recover member access rather than copying
decompiler pointer arithmetic. Preserve behavior while matching, including bugs.

Run clang-format on changed C++ files. Run `tools/compare.py` before claiming a
match; it lints, generates HTML/JSON/SVG, and rejects missing, stubbed or
below-threshold FUNCTION claims. `tools/check_matches.py --require-exact`
provides the optional 100% instruction-match check. No naming-convention CI from LR1 is inherited.

## Repository hygiene

Keep original executables, assets, compiler binaries, Wine prefixes, databases,
virtual environments and generated reports ignored. The installation directory
is outside this repo. The `template` remote records LR1 provenance and is not a
publishing destination. Do not push to it.
