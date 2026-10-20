# LEGO Racers 2 decompilation

A matching decompilation of the 2001 Windows game, bootstrapped from
[isledecomp/racers](https://github.com/isledecomp/racers) and
[reccmp](https://github.com/isledecomp/reccmp). The original installation and
proprietary assets stay outside this repository.

The rebuilt menu renders correctly, accepts keyboard input and plays sound,
confirmed on 2026-10-20. New Game recovery is paused; a playable race has not
been verified. The working-menu checkpoint is `204d683`. See
[menu validation](docs/work-menu-startup.md), [current progress](docs/progress.md)
and [remaining work](docs/runtime-roadmap.md).

The compiler is **Visual C++ 6.0 SP5**: CL **12.00.8804**, with the
**12.00.8966 code generator**. See [compiler evidence](docs/compiler.md).
Matching remains secondary to getting the game running. Reports use an original
binary inventory of **6,612 function entries**, including libraries and thunks;
discovery is not proven exhaustive. The measured subset and library identification
are tracked in the progress and [library notes](docs/library-identification.md).

## Repository layout

Source and header trees share subsystem paths, for example:

```text
LEGO Racers 2/
  src/frontend/foyer/foyermanager.cpp
  include/frontend/foyer/foyermanager.h
  src/render/core/renderer.cpp
  include/render/core/renderer.h
build/
  msvc/              # Python builder: executable, PDB and reports
  cmake/             # CMake/NMake build
  config/directx8/   # Generated SDK configuration and header overlay
  config/directshow/
  resources/         # Extracted retail resources
  workers/           # Isolated worker outputs and manifests
  archive/           # Preserved generated output from before relocation
```

See [the source tree](docs/repository-layout.md) for all fourteen subsystems.
Includes use paths such as `"render/core/renderer.h"`; `sources.txt` remains the
ordered build manifest. All generated build output stays under ignored `build/`.

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

The main priority is a playable rebuild: startup, resource loading, rendering,
input and the main loop take precedence over polishing instruction differences.
See the [runtime roadmap](docs/runtime-roadmap.md) for the remaining gameplay dependencies and unfinished boundaries.

## Build and measure

Renderer sources need a local DirectX 8 configuration before the first build.
For Linux with Wine, generate it once and pass it to the builder:

```sh
python tools/directx8.py --mode wine --output-dir build/config/directx8 \
  --d3dx8-library /path/to/DirectX8/Lib/d3dx8.lib \
  --directshow-sdk-root /path/to/DirectX8
python tools/build.py --directx8-config build/config/directx8/directx8-config.json
python tools/compare.py
python tools/compare.py --address 0x0045de00
```

Texture conversion needs the genuine retail static D3DX8 library; see the
[verified SDK artifact](docs/work-d3dx8.md). For a local genuine SDK, use
`--mode sdk --sdk-root <path>` instead (it finds D3DX8 in the SDK library
directory). CMake
reads the same JSON path from `LR2_DIRECTX8_CONFIG`; see
[DirectX setup](docs/work-directx8.md) and [DirectShow setup](docs/work-directshow.md).

The builder uses MSVC natively on Windows and Wine on Linux, emits an x86 PE
and PDB, and writes `build/msvc/reccmp-build.yml` using paths for the host running
reccmp. Set `WINEPREFIX` to an absolute prefix path if needed; the same setting
must be used for build and comparison. Game data archives are not needed to
link; extract and link the retail `.res` to include the original dialog and
icons. The user-data helper also needs an x86 SHFOLDER import library. Linux
builds use the installed Wine archive; on Windows set `SHFOLDER_LIB` to an SDK
library if it is not in LIB.
[Shell API setup](docs/work-userdatapath.md#import-library-setup) documents both builders.

To link the retail dialog and icon resources, run
`python tools/extract_resources.py`; builders pick up the ignored
`build/resources/retail.res` by default. See
[resource extraction](docs/work-resources.md).

Open `build/msvc/reports/LEGORACERS2.html` for per-function assembly diffs. JSON and
SVG reports are beside it. The comparison command also checks annotations and
fails if a `FUNCTION` claim is absent, stubbed, or below the initial 95% raw
accuracy threshold. `FUNCTION` includes exact matches and reviewed semantically
identical near matches; their actual scores stay visible. The optional
`python tools/check_matches.py build/msvc/reports/LEGORACERS2.json --require-exact`
check requires 100% raw matches.
The console, JSON, HTML and SVG use the tracked
[`reccmp/function-inventory.csv`](reccmp/function-inventory.csv) for their total,
including original functions with no source annotation. The current summary is
**619 / 6,612 implemented (9.36%)**, **99.83% accuracy**, **9.35% progress**.
Verified folded bodies contribute before the summary is printed. Known libraries
are excluded from implemented comparisons with `--nolib`; they remain in the
whole-binary denominator and can be compared explicitly. This inventory includes
compiler helpers and thunks, excludes external imports, and may grow as discovery
improves. See [inventory scope and refresh instructions](docs/research.md).

For direct tool use, run from `build/msvc/` with the virtual environment active:

```sh
reccmp-reccmp --target LEGORACERS2 --nolib --print-rec-addr
reccmp-reccmp --target LEGORACERS2 --verbose 0x0045de00 --print-rec-addr
reccmp-datacmp --target LEGORACERS2 --verbose --print-rec-addr
reccmp-decomplint --target LEGORACERS2 --warnfail "../../LEGO Racers 2"
```

The original exe is SHA-256
`6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d`.

## Native Windows CMake build

After running the SP5 `VC98\Bin\VCVARS32.BAT`:

```bat
cmake -S . -B build/cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/cmake
cmake --build build/cmake
python tools/compare.py --build-dir build/cmake
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
