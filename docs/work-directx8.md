# DirectX 8 build prerequisites

## Repeatable setup

Wine support is explicit and generated into an ignored build directory:

```sh
python tools/directx8.py --mode wine --output-dir build-dx8-config
```

The helper validates all six required Wine headers and four PE/i386 import
archives, creates a narrow header overlay of symlinks (or local copies where
symlinks are unavailable), and writes `directx8-config.json` with the include
directories, library directories, archives, define, and linker option. Source
files select the compatibility bridge with `#include "directx8.h"`; the build
must pass the generated include directories and define, add the listed
archives, and preserve `/OPT:NOREF`.

The Python builder consumes the generated configuration directly:

```sh
python tools/build.py --directx8-config build-dx8-config/directx8-config.json
```

`LR2_DIRECTX8_CONFIG` may also supply that path. The full Python build and
CMake/NMake build passed with the generated Wine configuration. An isolated
probe through the Python builder compiled the real interfaces and linked
all three DirectX API imports; the probe was not executed.

A genuine DirectX SDK can be selected without the Wine bridge:

```sh
python tools/directx8.py --mode sdk --sdk-root /path/to/DirectX-SDK
```

The helper checks the SDK's `Include` directory and either `Lib/x86` or `Lib`
for the required interfaces and libraries. `DXSDK_DIR` or `DXSDK_ROOT` may
supply the SDK root. No Microsoft SDK was installed here, so this branch has
not been compiler-verified. The helper does not download or vendor either
SDK's files.

For CMake, include `cmake/directx8.cmake` and call
`lr2_configure_directx8(<target>)`. Set `LR2_DIRECTX8_CONFIG` to the generated
JSON path, or set the environment variable of the same name before configuring
CMake. An empty value leaves DirectX integration disabled. The module validates
configured directories and required headers/import archives, then applies their
include paths, definitions, link directories, exact library paths and options to
the target. Wine's Unix absolute paths are translated to `Z:/...` for Windows
CMake running under Wine; native Windows SDK paths are preserved. The JSON's
Wine include overlay stays ahead of VC98 so `windows.h` and normal Win32 headers
still come from the pinned toolchain.

The pinned VC6 SP5 compiler and linker can build against the locally installed
Wine DirectX 8 headers and PE/i386 import archives. A minimal probe compiled
and linked with CL 12.00.8804 and LINK 6.00.8447. Its imports are
`d3d8.dll!Direct3DCreate8`, `dinput8.dll!DirectInput8Create`, and
`dsound.dll!DirectSoundCreate8`. The probe also compiled the first renderer
calls: adapter enumeration and display-mode query, device creation, clear,
begin/end scene, and present. It did not execute the probe or start the game.

The original local prerequisite inventory has no Microsoft DirectX 8 SDK.
`VC98/Include/DINPUT.H` defaults to DirectInput 5 and lacks DirectInput 8; the
VC98 DirectSound and D3D headers are also pre-DX8. Its libraries include
`DINPUT.LIB`, `DSOUND.LIB`, and `DXGUID.LIB`, but no D3D8 or DInput8 import
library. The neighboring sample repositories carry DirectX 5 and 6 assets,
not DX8.

The Wine header overlay contains only `d3d8.h`, `d3d8types.h`, `d3d8caps.h`,
`dinput.h`, `dsound.h`, and `basetsd.h`. Keep the pinned VC98 headers first for
`windows.h`, `objbase.h`, and normal Win32 declarations. The reusable
compatibility bridge is [`util/directx8.h`](../util/directx8.h); it defines
the interface versions and the VC6 adaptations used in the passing probe.

`multimon.h` supplies VC98's `HMONITOR` declaration. Wine's `basetsd.h`
supplies pointer-sized aliases such as `DWORD_PTR`. The DirectInput alias
macros select its ANSI interfaces, matching the game's unsuffixed ANSI Win32
API use. VC98 does not define Wine's `DECLARE_INTERFACE_IID_`; mapping it to
VC98's `DECLARE_INTERFACE_` uses the unchanged Wine interface declarations and
method order, while dropping only the UUID attribute on the C++ type. Code
that needs an IID should use the explicit `IID_*` constants and link
`libdxguid.a`, rather than depending on `__uuidof` for these declarations.
This bridge does not create replacement interface definitions or vtables.

Do not add the whole Wine Windows include directory ahead of VC98. The first
all-Wine include attempt failed in Wine's base headers with VC6 C2010 errors
on variadic macros and a missing `CONTEXT` selection, followed by newer
intrinsic declarations. The narrow DirectX-header overlay avoids those
unrelated platform-header incompatibilities. With the VC98 Win32 headers,
the first compile then identified only the missing interface UUID macro,
`HMONITOR`, Wine ANSI alias helpers, `DWORD_PTR`, and `__MSABI_LONG`; the
preamble above plus `multimon.h` and `basetsd.h` addressed those cases.

Use the matching 32-bit import archives from
`/usr/lib/wine/i386-windows`: `libd3d8.a`, `libdinput8.a`, `libdsound.a`, and
`libdxguid.a`. LINK 6 accepts these PE/i386 COFF archives. Keep `/OPT:NOREF`
enabled: without it, VC6 discards Wine's import-descriptor members and the
resulting PE does not contain the DirectX import descriptors. The main repo's
existing `/OPT:NOREF,ICF` link setting already handles this. Wine's DSOUND
archive imports the named `DirectSoundCreate8` export; the retail executable
imports DSOUND ordinal 11. The local Wine DLL exports that same function by
name at ordinal 11, so the probe resolves the same API, though its import form
does not match retail's ordinal form.

Wine's DirectX and `basetsd.h` declarations are copyright Wine contributors
and carry the LGPL version 2.1-or-later notices in their headers. The system
archives and DLLs are Wine build outputs. This is not a Microsoft SDK drop-in;
the compatibility macros above are needed with VC6, and builds that use this
route must retain the Wine notices and comply with the LGPL. No Wine header or
library has been copied into the repository. The ignored probe artifacts are
under `build-dx8-probe/`.

The reusable configuration and compatibility header were verified with a
fresh isolated probe using CL 12.00.8804 and LINK 6.00.8447. It compiled the
adapter enumeration/display-mode and device creation/scene/present calls, plus
DirectInput8 and DirectSound8 creation/release, then linked a PE32 executable
whose import table contains `Direct3DCreate8`, `DirectInput8Create`, and
`DirectSoundCreate8`. The probe was not executed. For integration, use the
generated include overlay ahead of VC98 and append its Wine archive directory
to LINK's library path alongside the current Win32 libraries. Keep this path
host-specific; a portable checkout still needs a genuine DX8 SDK or Wine
development-package prerequisite.

Wine 11.19 and its 32-bit `d3d8.dll`, `dinput8.dll`, and `dsound.dll` runtime
are present in the environment. Xvfb and Mesa's software rasterizer files are
also installed, but this sandbox could not connect to the configured `:0`
display or create the Xvfb display socket. `vulkaninfo` found no working
Vulkan driver; Wine's default D3D8 path through wined3d uses OpenGL, so this is
not by itself a blocker. Before a game run, use an X display reachable by Wine
and confirm its OpenGL path; software rendering under Xvfb remains unverified.

## Verified retail D3DX8 surface conversion

Texture compression/upload also requires the DirectX 8.0 SDK's retail static
`d3dx8.lib`. Configure this separate local input explicitly when using Wine:

```sh
python tools/directx8.py --mode wine --output-dir build-dx8-config \
  --d3dx8-library /path/to/DirectX8/Lib/d3dx8.lib
```

SDK mode automatically selects `d3dx8.lib` if present in its chosen library
directory; `--d3dx8-library` can override that location. Both builders consume
the same generated library list. The Wine D3D8 import archive supplies the
Direct3D interface, while the verified Microsoft static library supplies D3DX8
surface conversion. `util/d3dx8.h` declares just the two verified APIs with
actual DirectX interface types. See [exact library evidence](work-d3dx8.md)
for the pinned artifact and relocation-masked retail matches. SDK artifacts
remain external, ignored build inputs.

Startup movie support also needs the VC6-compatible DirectShow BaseClasses
headers and genuine STRMBASE static library. Add `--directshow-sdk-root` when
generating the same configuration; see [setup and artifact identities](work-directshow.md).
