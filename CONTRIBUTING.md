# Contributing

The primary goal is to get the rebuilt game running with the supplied assets.
Recover readable C++ and retain per-function instruction measurements, with
exact matching as a secondary goal. Use the SP5 compiler documented in [docs/compiler.md](docs/compiler.md).
The full coding and annotation rules are in [AGENTS.md](AGENTS.md).

1. Inspect the original function, all its control-flow paths, callers, and
   relevant data. Confirm the function start, size and calling convention.
2. Add a coherent source/header pair under `LEGO Racers 2` and list the source
   in `sources.txt`. Infer descriptive working names from behavior and callers.
   Use `STUB` annotations for unfinished/unresolved implementations and unknown callees,
   with genuine original addresses.
3. Build with `python tools/build.py`, then inspect a function diff with
   `python tools/compare.py --address 0xADDRESS`. Use bounded matching attempts;
   when behavior is confidently recovered and remaining differences are code
   generation, record the score and evidence and move to the next function.
   Reviewed semantically identical near matches (initially at least 95% raw
   accuracy) use `FUNCTION`; unfinished or unresolved work uses `STUB`.
   Keep exact and near matches separately counted. Compiler-generated and library functions use their
   corresponding annotation types.
4. Format changed C++ files with clang-format and run `python tools/compare.py`
   to lint and remeasure every existing FUNCTION claim. Layout changes may
   affect other classes, vtables and initialized data; compare those explicitly.
5. Describe the binary evidence and measured results with the change. Keep
   whole-game progress distinct from accuracy of the annotated subset.

The initial predicates demonstrate this process without speculative class
layouts or external callees. Their semantic names are descriptive; original
source names are unknown. The temporary `bootstrap.cpp` has no matching claim.

Do not add game assets, original executables, proprietary toolchain files,
build outputs or disassembler databases to Git. Preserve the inherited LGPL
license and template provenance when reusing its infrastructure.
