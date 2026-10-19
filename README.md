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
startup helpers, bitmap scanning, terrain lookup, spline helpers, and
foundational renderer/input operations.
The current checkpoint has **504 exact function matches**, **twenty-one reviewed
near matches**, and 1,176 unfinished implementations across 1,701 measured
functions. See
[current progress](docs/progress.md) for the measured function list and
remaining work. The application singleton and its two complete 38-slot
dispatch tables, all three 25-slot input-device tables, and the five-slot
input service table are wired. The full DirectInput initializer and joystick
axis/effect dependencies are reconstructed, pending startup integration.
Renderer source now covers transforms, viewports, lighting selection, buffer
pools and uploads, TGA loading, resource lookup, and the full display-initializer
body. The initializer remains under review at 22.87% raw similarity; the
reserved renderer storage is now present. Texture upload, animation resources
and draw-plan builders also have source. The renderer constructor, full dispatch wiring and lookup callbacks now have compiled source. Both complete renderer tables and all six lookup callbacks pass static pointer verification; their large bodies remain under review. Startup integration remains open.
Bitmap loading and mip preprocessing, texture maintenance, buffer dispatch,
sprite submission, immediate drawing and animation records now have source;
large bodies still require semantic review. Device reset/resource restoration, scene startup, GUI rectangle clipping, all eight quad-packet cases and bump-vector generation also have compiled source. Controller records, recorded-input access, timer events and named mutexes have source; the controller manager, system clock, and platform readiness helper now have compiled source. The fixed-step scheduler, game-object registry loops and renderer resource cleanup also have compiled source. Registry allocation, insertion, removal and initialization now have compiled source. Concrete menu-object creation and startup integration remain open.
Controller defaults, player car setup, bodypart lists, shared menu sprites,
key-icon/string replacement, ScreenPrinter and HUD/pause/builder textures now
have linked source. The recording service, pause detector, nested textbox, track/network controls
and character-preview callbacks now link. Character preview initialization and foyer animation/event callbacks now link.
Startup movie playback now links against genuine DirectShow base classes.
Input-binding and text-entry controls now link, along with model cache, reference,
bounds and skin-reading dependencies. Full model geometry, collision relocation
and shared shadow loading now link; see [model recovery](docs/work-model-loading.md).
Car preview, shared model bindings and frontend widget/resource creation now link;
see [car-preview recovery](docs/work-car-preview.md).
Menu backdrop transitions and the KillSphere startup lifecycle now link; see
[backdrop and supervisor recovery](docs/work-menu-background.md).
Generated shadow geometry and menu display/input configuration now link; see
[shadow and configuration recovery](docs/work-shadow-and-configuration.md).
Model draw queues and visibility submission now link; see
[model drawing](docs/work-model-drawing.md).
Loading-screen textures, sprites, progress animation and fade-controller lifecycle
now link; see [loading-screen recovery](docs/work-loading-progress.md).
Foyer resource loading and remaining frontend closures are still being integrated; see [frontend notes](docs/work-controller-frontend.md).
Earlier runtime checks exercised config,
archive metadata, and selected asset reads. The executable still has a
temporary entry point and does not reach a window, rendered menu, or gameplay.
No game or probe was run for the current renderer/input implementation batch;
future runtime checks are reserved for significant milestones expected to
reach a rendered menu or gameplay. Whole-game coverage is not established.
Ghidra currently discovers 4,568 function starts,
including 22 thunks and linked libraries; this is a provisional inventory,
not a count of game-authored functions. See the
[inventory notes](docs/research.md#automatic-function-inventory).
See [initial validation](docs/validation.md) for the checks performed and the
commands for this supplied workspace.

[Research tooling](docs/research.md) describes the local Ghidra project and
reference exports used for parallel decompilation work.

[Library identification](docs/library-identification.md) distinguishes confirmed
MSVC runtime routines from the separate pool allocator whose provenance is
still unresolved. The required pool, archive-reading path, extracted retail
resources, and application startup support are included in the build. Renderer
and main-loop work remains; see the [runtime roadmap](docs/runtime-roadmap.md)
and [runtime startup notes](docs/work-application-runtime.md).

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
See the [runtime roadmap](docs/runtime-roadmap.md) for the concrete startup,
asset-loading and DirectX 8 blockers.

## Build and measure

Renderer sources need a local DirectX 8 configuration before the first build.
For Linux with Wine, generate it once and pass it to the builder:

```sh
python tools/directx8.py --mode wine --output-dir build-dx8-config \
  --d3dx8-library /path/to/DirectX8/Lib/d3dx8.lib \
  --directshow-sdk-root /path/to/DirectX8
python tools/build.py --directx8-config build-dx8-config/directx8-config.json
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
and PDB, and writes `build/reccmp-build.yml` using paths for the host running
reccmp. Set `WINEPREFIX` to an absolute prefix path if needed; the same setting
must be used for build and comparison. Game data archives are not needed to
link; extract and link the retail `.res` to include the original dialog and
icons. The user-data helper also needs an x86 SHFOLDER import library. Linux
builds use the installed Wine archive; on Windows set `SHFOLDER_LIB` to an SDK
library if it is not in LIB.
[Shell API setup](docs/work-userdatapath.md#import-library-setup) documents both builders.

To link the retail dialog and icon resources, run
`python tools/extract_resources.py`; builders pick up the ignored
`build-resources/retail.res` by default. See
[resource extraction](docs/work-resources.md).

Open `build/reports/LEGORACERS2.html` for per-function assembly diffs. JSON and
SVG reports are beside it. The comparison command also checks annotations and
fails if a `FUNCTION` claim is absent, stubbed, or below the initial 95% raw
accuracy threshold. `FUNCTION` includes exact matches and reviewed semantically
identical near matches; their actual scores stay visible. The optional
`python tools/check_matches.py build/reports/LEGORACERS2.json --require-exact`
check requires 100% raw matches.
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
