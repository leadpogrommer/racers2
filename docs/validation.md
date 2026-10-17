# Initial validation

Validated on Linux with Wine, Python 3.12.15, the pinned reccmp revision and
MSVC SP5 toolchain described in [provenance.md](provenance.md).

Both `tools/build.py` and the Windows CMake 3.26.6 / NMake build were exercised
under Wine. Both produced readable PE/PDB pairs and these instruction matches:

| Original address | Function | Bytes | reccmp accuracy |
| --- | --- | --- | --- |
| `0x0045dda0` | `IsTokenCharacter` | 81 | 100% |
| `0x0045de00` | `IsHorizontalSpace` | 21 | 100% |

The compiler-produced `token.obj` carries `@comp.id = 0x000b2306`, matching
the dominant C++ Rich record in the original. The original Rich checksum and
SHA-256 were verified, and fresh inspection agrees with `original-binary.json`.

Annotation lint, clang-format checks on the new C++ files, and five tests for
the match guard passed. Those tests cover exact success, effective-only
similarity, missing/unlinked functions, stubs, empty claims and unsupported
report formats. The GitHub Actions commands were exercised locally; no remote
CI run or publication was performed.

The generated reports are ignored build artifacts:
`build/reports/LEGORACERS2.{html,json,svg}`. These two matches validate the
workflow; they do not measure whole-game coverage. The CRT startup remains a
library comparison, and the temporary entry point does not start the game.

In the supplied workspace, `.venv` and `.tools/msvc6-sp5` are already installed.
To rebuild and regenerate reports using its existing Wine prefix:

```sh
cd /home/ilya/work/lr2_decomp/racers2
source .venv/bin/activate
export WINEPREFIX=/home/ilya/work/lr2_decomp/wine_pfx
python tools/build.py
python tools/compare.py
```

## Continued decompilation

The combined SP5/Wine and CMake/NMake builds were revalidated on 2026-10-17.
Both passed the strict guard for **79 functions at 100% instruction accuracy**.
All 98 measured raw scores and annotation statuses agree across the two builds:
79 exact reconstructions and 19 unfinished implementations. The unmatched
10-byte generated cleanup at 0x004a2550 retains an original-size inventory row
but has no unique name mapping, so it is excluded from that measured total.
Annotation lint, formatting checks and all 13 tooling tests passed. These cover
the match guard and library-identification fixtures. Game-function validation
uses reccmp, and no behavioral tests or game execution were added.

Data comparison verified 14 recovered globals with zero issues: four startup
variables, the child-window enumeration handle, the compression path and file
manager pointers, the file-manager initialization guard, two spline cache
variables, the screenshot directory/enable state and two terrain edge-selection
words. Both final builds completed
without compiler warnings. Allocator research is excluded from the main build
pending provenance investigation. [Current progress](progress.md) lists all
measured functions and their remaining mismatches.

The latest exact matches are five terrain bounds/tile/border/vertex methods.
The terrain cell helper and quaternion decoder have recovered source but
remain STUB. Spline evaluators retain improved copy/scale ordering. A consumed
file-seek result established a DWORD return contract; the facade still matches
100%, while the corrected manager method remains unfinished at 79.49%.
New function extents were checked through their final returns or tail jumps.
The original synthetic cleanup has an ambiguous generated name and is documented
without an address-forcing workaround. Library identification remains separate,
including CRT helpers and the verified DirectShow DeleteMediaType body.

These results cover the recovered subset. The automatic 4,228-start inventory
includes libraries and thunks and is not a whole-game completion denominator.
The executable still uses a temporary entry point and does not start the game.

## Functional recovery policy

The primary objective is a playable rebuilt game. A reviewed function with
identical behavior and a near-perfect raw instruction match (initially at
least 95%) uses FUNCTION instead of STUB. The normal guard verifies those
implemented claims without replacing their real scores. Exact and reviewed
near matches are reported separately. The optional `--require-exact` mode
continues to reject every sub-100% raw match, including effective-only matches.
The current 79 exact functions pass both modes; no existing partial function
was promoted solely because the policy changed. The 13 tooling tests cover
the near-match boundary, strict mode, invalid scores, unresolved/missing
functions and library-identification fixtures.
