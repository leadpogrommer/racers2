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
Both passed the strict guard for **51 functions at 100% instruction accuracy**.
Annotation lint, formatting checks and all 10 tooling tests passed. These cover
the match guard and synthetic library-identification fixtures. Game-function
validation uses reccmp; optional behavioral tests were removed.
All 62 reported raw scores and annotation statuses agree across the two builds.
[Current progress](progress.md) lists the functions and the unfinished
implementations. These results cover the reconstructed subset; the executable
still uses a temporary entry point and does not start the game. Data comparison
verified all five recovered globals (four startup variables and the child-window
enumeration handle) with zero issues. The final builds
completed without compiler warnings. Allocator research is excluded from these
counts and from the main build pending provenance investigation.
