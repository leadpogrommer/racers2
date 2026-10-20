# Repository layout

The game has parallel `LEGO Racers 2/src/` and `LEGO Racers 2/include/` trees.
The reorganization moved 441 project compilation units and 247 headers without
renaming files. The additional Ogg/Vorbis units remain under `3rdparty/`.

| Subsystem | Contents |
| --- | --- |
| `audio/` | Devices, sound sources, music, commentary and reverb |
| `core/` | Application lifecycle, entry point, strings and diagnostics |
| `frontend/` | Menus, widgets, overlays, dialogue, previews and Foyer management |
| `game/` | Controller, game objects, sessions, state, saving and pause logic |
| `input/` | Controller mappings, devices, force feedback and input service |
| `io/` | Files, archive compression, localization and token parsing |
| `math/` | Vectors, matrices, quaternions and splines |
| `memory/` | Heap, pool and platform memory interfaces |
| `network/` | Network application interfaces and deferred boundaries |
| `physics/` | Dynamics, collision, spatial indices and simulation timing |
| `platform/` | Windows, system APIs and platform clocks |
| `render/` | Renderer, device state, buffers, textures, fonts and model resources |
| `scene/` | Scene scripts, animation, cameras, effects, lighting and terrain |
| `video/` | Startup movie playback and DirectShow support |

Related implementation/header pairs share their subsystem path. Include project
headers from the include root, for example `#include "io/files/winfile.h"`.
Both builders retain the same include root and compile options. `sources.txt`
keeps its original order; it is the single list of compilation units.
Reccmp recursively scans the unchanged game source root and retains original
addresses, annotations, literal inventories and library identification.

## Generated output

All build output is ignored under `build/`:

- `msvc/`: default Python build, executable, PDB, linker map and reccmp reports.
- `cmake/`: fresh CMake/NMake configuration and its independent reports.
- `config/directx8/` and `config/directshow/`: generated local SDK configurations.
- `resources/`: extracted retail `.res` output.
- `workers/`: isolated builds, with loose worker manifests collected in `manifests/`.
- `archive/cmake-before-reorganization/`: preserved previous CMake tree. Its cache
  contains old absolute paths; the active tree is configured afresh.

Other archived worker outputs can contain historical absolute paths. Rebuild them
using the current manifest before reuse. Local Ghidra databases, research exports,
media and runtime evidence remain under ignored `data/`.

## Paused recovery

The working-menu commit is `204d683`. Unverified New Game reset recovery is
preserved separately in `data/paused-new-game/session-reset.patch` and its source
snapshot. It is excluded from the build. Resume only when requested; translate
its old source/include paths to the matching subsystem before applying it.

## Verification

The reorganized Python and CMake builds both link 465 units and preserve all
2,715 measured scores/statuses from the working menu checkpoint. All 619
implemented function claims, twenty folded aliases, twelve dispatch checks per
build, input-format checks and 22 tooling/library tests pass. The data check
retains only the documented equivalent string aliases. See
[validation](validation.md#repository-reorganization-2026-10-20).
