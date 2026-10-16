# Compiler identification

## Result

The supplied PC executable was primarily compiled with **Microsoft Visual C++
6.0 SP5**, whose optimizer stamps **8966**. The matching preserved toolchain has
`cl.exe` **12.00.8804**, `C1XX.DLL` **12.00.8964**, and `C2.DLL` **12.00.8966**.
This differs from the `12.00.8168` RTM compiler used by the LEGO Racers 1 sample.

This conclusion comes from inspecting the local executable, not its release
date or the PE linker's major version. Reproduce the inspection with:

```sh
python tools/inspect_binary.py "../LEGO Racers 2/LEGO Racers 2.exe"
```

[original-binary.json](original-binary.json) records the complete result.

## Executable identity

| Property | Value |
| --- | --- |
| File | `LEGO Racers 2.exe` |
| File size | 1,400,832 bytes |
| SHA-256 | `6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d` |
| MD5 | `341bfcf7b1164e71f0f27a13aca5babe` |
| Machine | PE32, Intel i386 (`0x014c`) |
| Image base | `0x00400000` |
| Entry point | `0x0051a33b` |
| COFF timestamp | 2001-08-13 10:18:04 UTC |
| PE linker version | 6.0 |
| Debug directory | Absent; no original PDB reference |

The timestamp and version fields are metadata, not independent authenticity
proof. The hash defines the specific supported executable. Another release or
patch needs separate inspection and address mappings.

## Rich-header evidence

The Rich marker is at file offset `0x110`. XOR key `0xfd9cd74f` decodes `DanS`
at `0x80` and sixteen product/build/count records. The Rich checksum validates.
The dominant compiler records are:

| Encoded ID | Product | Build | Contributions |
| --- | --- | --- | --- |
| `0x000b2306` | `Utc12_CPP` | 8966 | 281 |
| `0x000a2306` | `Utc12_C` | 8966 | 20 |
| `0x000b1fe8` | `Utc12_CPP` | 8168 | 7 |
| `0x000b1f6f` | `Utc12_CPP` | 8047 | 6 |
| `0x000a1f6f` | `Utc12_C` | 8047 | 128 |
| `0x001c227e` | `Utc13_C` | 8830 | 4 |
| `0x001d227e` | `Utc13_CPP` | 8830 | 57 |
| `0x003022ef` | `Utc12_2_C` | 8943 | 2 |

The [richprint identifier catalog](https://github.com/dishather/richprint/blob/master/comp_id.txt)
maps `Utc12_CPP / 8966` and `Utc12_C / 8966` to VS6 SP5. Product IDs separate
C, C++, linker/import objects and assembler objects. Counts describe linked
contributions, **not functions**, and cannot supply a progress denominator.

To check the mapping experimentally, the pinned SP5 compiler was used to build
`token.cpp`. Its COFF `@comp.id` is **`0x000b2306`**, exactly the dominant retail
stamp. This corroborates the preserved toolchain's mapping independently of
the compiler banner. The reconstructed predicates at `0x0045dda0` and
`0x0045de00` also match 100% under reccmp with this toolchain.

The older compiler stamps and VC7-era library records show a mixed link. Their
precise library provenance has not yet been established. They do not justify
building all game code with VC7 or the LR1 RTM compiler. The two `Utc12_2_C`
objects likewise do not establish that all game code used Processor Pack.

## Matching toolchain

The pinned [preserved SP5 installation](https://github.com/archaic-msvc/msvc600_sp5/tree/b0b07e29108e2695eb0274c2a377a7b7d7326150)
contains these file versions, verified from their `VS_FIXEDFILEINFO` resources:

| Component | File version |
| --- | --- |
| `VC98/Bin/CL.EXE` | 12.00.8804.0 |
| `VC98/Bin/C1XX.DLL` | 12.00.8964.0 |
| `VC98/Bin/C2.DLL` | 12.00.8966.0 |
| `VC98/Bin/LINK.EXE` | 6.00.8447.0 |
| `Common/MSDev98/Bin/MSPDB60.DLL` | 6.00.8168.0 |

`cl.exe`'s banner version and the optimizer's Rich stamp differ. Checking only
`_MSC_VER == 1200` or the banner cannot distinguish all VC6 service packs.
The retail header identifies the SP5 backend family, not every original front
end DLL's file hash. We use the consistent preserved SP5 set above.

The original PE header proves a 6.0 linker, but not its patch build. Records
such as `Linker512 / 9049` and `Linker620 / 8755` belong to linked inputs and
must not be mistaken for the linker that produced the final EXE. LINK
6.00.8447 is the SP5 reconstruction tool, rather than a separately proven
original linker patch.

## Runtime, SDK and initial flags

Imports include `d3d8.dll`, `DINPUT8.dll` and `DSOUND.dll`: this is a **DirectX 8**
target. The LR1 sample's DirectX 6 headers/libraries are inappropriate. No
DirectX declarations are needed by the initial predicates; supply a compatible
DX8 SDK when adding graphics/input code instead of silently using DX6.

No MSVCRT/MSVCP runtime DLL is imported, and the executable embeds CRT startup.
The call at `0x0051a3a6` reaches `0x0051e4eb`, corresponding to the multithreaded
CRT initialization slot in the SP5 `/MT` startup. The initial build therefore
uses **static multithreaded CRT (`/MT`)**. This is also consistent with the TLS
imports. Detailed CRT matching still needs symbol/data annotations; the startup
is classified as a library function and is not claimed as reconstructed code.

The starting settings are `/O2 /Ob1 /MT /GX`, with `/Z7`, linker `/DEBUG`,
`/PDBTYPE:SEPT` and `/INCREMENTAL:NO` for usable VC6 PDBs. `/OPT:REF,ICF` keeps
normal folding behavior in the initial bootstrap. The expanding comparison
harness now uses `/OPT:NOREF,ICF` to retain recovered functions before their
callers exist, while preserving identical-code folding. This linker retention
choice is scaffolding, not a claim about the original settings.
`/Z7` and PDB output serve analysis; the original's
debug settings are unknown. Matching the seed functions does not prove all
optimization, inlining, exception, packing or floating-point settings throughout
the game. Establish changes per translation unit from actual comparisons.
