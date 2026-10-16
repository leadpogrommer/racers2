# LEGO Racers 2 decompilation

A matching decompilation of the 2001 Windows game, bootstrapped from
[isledecomp/racers](https://github.com/isledecomp/racers) and its
[reccmp](https://github.com/isledecomp/reccmp) workflow. The original game
installation stays outside this repository.

The compiler is **Visual C++ 6.0 SP5**, with **cl 12.00.8804** and the
**12.00.8966 code generator**. See [compiler evidence](docs/compiler.md) for
the decoded Rich header, exact binary identity, mixed library stamps, and
settings that still need investigation.

Verified reconstructions include character predicates, line classification,
fixed string buffers, file operations, window geometry, command-line parsing,
startup helpers and spline helpers.
See [current progress](docs/progress.md)
for the measured function list and remaining work. The executable has a
temporary entry point; it does not run the game yet. Whole-game coverage is
not established.
See [initial validation](docs/validation.md) for the checks performed and the
commands for this supplied workspace.

[Research tooling](docs/research.md) describes the local Ghidra project and
reference exports used for parallel decompilation work.

[Library identification](docs/library-identification.md) distinguishes confirmed
MSVC runtime routines from the separate pool allocator whose provenance is
still unresolved. Allocator reconstruction is deferred outside the game build.

## Setup

Use Python **3.12** (tested), Git, and Wine on Linux. Python 3.14 may require
building reccmp's JPype dependency from source; use 3.12 for the pinned tools.
From this directory:

```sh
python3.12 -m venv .venv
source .venv/bin/activate
python -m pip install -r tools/requirements.txt
python tools/setup.py --fetch-toolchain
```

On Windows, use `py -3.12 -m venv .venv`, then `.venv\Scripts\activate`.
If using `uv`, `uv venv --python 3.12 .venv` and
`uv pip install -r tools/requirements.txt` provide the same environment.

Setup checks the executable's SHA-256 and writes an ignored `reccmp-user.yml`.
It defaults to `../LEGO Racers 2/LEGO Racers 2.exe`. To use another location:

```sh
python tools/setup.py --original "/path/to/LEGO Racers 2.exe" --fetch-toolchain
```

`--fetch-toolchain` obtains the pinned SP5 tree in `.tools/msvc6-sp5` and checks
its compiler component versions. An existing installation can be selected with
`--toolchain` or `MSVC6_ROOT`; it must contain `VC98` and `Common`. Toolchain
files and original game files are not added to Git.

## Build and measure

```sh
python tools/build.py
python tools/compare.py
python tools/compare.py --address 0x0045de00
```

The builder uses MSVC natively on Windows and Wine on Linux, emits an x86 PE
and PDB, and writes `build/reccmp-build.yml` using paths for the host running
reccmp. Set `WINEPREFIX` to an absolute prefix path if needed; the same setting
must be used for build and comparison. No game assets are needed for this build.

Open `build/reports/LEGORACERS2.html` for per-function assembly diffs. JSON and
SVG reports are beside it. The comparison command also checks annotations and
fails if a `FUNCTION` claim is absent, stubbed, or below 100% instruction accuracy.
Report coverage is relative to annotated functions; the whole-game function
total is unknown. CRT symbols are excluded with `--nolib`, and remain available
for explicit comparisons.

For direct tool use, run from `build/` with the virtual environment active:

```sh
reccmp-reccmp --target LEGORACERS2 --nolib --print-rec-addr
reccmp-reccmp --target LEGORACERS2 --verbose 0x0045de00 --print-rec-addr
reccmp-datacmp --target LEGORACERS2 --verbose --print-rec-addr
reccmp-decomplint --target LEGORACERS2 --warnfail "../LEGO Racers 2"
```

The original exe is SHA-256
`6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d`.

## Native Windows CMake build

After running the SP5 `VC98\Bin\VCVARS32.BAT`:

```bat
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
cmake --build build
python tools/compare.py
```

The second build handles the legacy NMake behavior described by the sample
repos. Both builders read `sources.txt` and use release optimization with debug
information. CMake rejects other compiler versions; `tools/build.py` additionally
checks the front end, back end and linker file versions.

Read [AGENTS.md](AGENTS.md) and [CONTRIBUTING.md](CONTRIBUTING.md) before adding
functions. [docs/provenance.md](docs/provenance.md) records the template and tool
revisions. Run the regression-guard checks with:

```sh
python -m unittest discover -s tests
```

GitHub Actions runs annotation lint and the match-guard tests without retail
files. Build and accuracy verification run locally using the supported original.
