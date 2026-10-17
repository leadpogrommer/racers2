# Playable rebuild: current blockers

The primary objective is to run the reconstructed game using the supplied
installation assets. The current executable still has a temporary WinMain
that exercises character predicates and exits. No rendered menu or gameplay
milestone has been reached.

## Startup and memory

The retail WinMain is at 0x004c3040. Before proceeding, it requires the arena
manager at 0x004a3ed0 to initialize successfully and set 0x00578c9c. Its
constructor at 0x004a3f80 queries application slot +0x4c for a 26 MiB arena,
allocates through 0x004e7b90, and initializes the pool. Existing research is
under `research/allocator/`. This is a required startup dependency, so it must
now be resolved while keeping provenance research and identified CRT routines
separate. Do not substitute a fabricated success flag.

The application singleton at 0x0049a760 constructs via 0x0049a610 and
0x0049b5d0. Its config path uses the user-data directory, Saved Games/LR2.cfg,
and a 16-byte configuration record with marker 0xc0f1c0f1. That initialization
needs the real file-opening and reading path. The observed application vtable
has 38 entries; a sparse declaration would move slots and break the ABI.
See [application evidence](work-application.md).

## Shipped asset loading

The installation contains GAMEDATA.GTC (135,813,120 bytes), FILELIST.INF
(784,312 bytes), and COMPRESS.INF (43,740 bytes). Most game assets are listed
in the archive metadata; music and movies have extracted directories. There
is no root GameData.Dat. The retail path policy at 0x004a4570 redirects
GameData.dat to GameData.gtc and enables compressed reads. This redirect is
part of the intended installed-data path, not an absent-file fallback to omit.

The resource-loading dependency chain is:

1. WinFile initialization/open dispatch: 0x004a21b0 and 0x004a21f0.
2. Manager metadata/archive initialization: 0x004a25c0; archive lookup at
   0x004a2a30 and bookkeeping at 0x004a2810.
3. Read facade at 0x004a2290, manager read at 0x004a2ba0, and dispatch at
   0x004a4a90, using the existing native ReadExact helper where appropriate.
4. Compressed block fill/copy at 0x004a4930/0x004a49f0 and transform at
   0x004e7bb0. The cache uses 0x20000-byte blocks.

The manager's initial mode is 3, so the archive initialization path is live.
Recover metadata formats and block transformation rather than forcing native
file mode or returning success without loading assets. Allocator interfaces
are also needed for metadata and cache storage.

## Display, readiness and the main loop

The actual window-creation route is inside display initialization at
0x004ac130 (3,013 bytes), which calls 0x004b46c0 at 0x004ac38b. That helper
registers/creates the window through 0x004b4750/0x004b4800. This path also
sets display configuration and device state; a standalone placeholder window
would not complete it.

WinMain's readiness gate at 0x004f6310 depends on service globals
0x00549dfc/0x00549e80 and 0x004b7c50. Audio setup uses 0x004af2f0 and
constructor 0x004af380. The loop reaches update/frame work at 0x004a3aa0.
Recover these interfaces and their initialization order alongside the display
path, then replace the temporary bootstrap with the real WinMain and loop.
See [startup evidence](work-startup.md).

## DirectX 8 build and runtime prerequisites

The pinned VC6 installation and sample repositories contain pre-DX8 SDK
headers/libraries. They do not provide the D3D8 and DI8 interfaces/import
libraries needed by this game. Retail imports Direct3DCreate8, DirectInput8Create,
and DSOUND ordinal 11 (DirectSoundCreate8 in the local Wine spec).

System Wine supplies D3D8/DI8/DS8 headers under `/usr/include/wine/windows`,
PE-i386 COFF import archives under `/usr/lib/wine/i386-windows`, and matching
source under `/home/ilya/work/a4tech_wine/wine-source`. They are Wine
LGPL-2.1-or-later artifacts. Compatibility with the pinned VC6 compiler/linker
must be established in an isolated compile/link probe before integration;
the read-only survey's sandboxed probe could not start Wine. Do not invent
partial COM vtables or use older DX5/6 interfaces as DX8 replacements.

Wine, 32-bit runtime DLLs, Xvfb and Mesa software rendering are installed.
Display access and an Xvfb rendering session remain unverified in this
environment. Establish those when a real initialization/rendering build is
ready for a runtime test. No original or reconstructed game was launched by
this dependency survey.

Instruction accuracy remains visible throughout. Functional reconstruction,
inferred names and reviewed near matches should support these runtime
milestones; isolated leaf-function polishing is secondary.
