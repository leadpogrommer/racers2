# D3DX8 static library provenance

The retail bodies at `0x005252f6` and `0x005250c0` are exact relocation-masked
matches to the Microsoft DirectX 8.0 SDK retail static library `d3dx8.lib`.
Both archive members come from `obj\i386\d3dx8tex.obj`:

| Retail VA | COFF symbol | Bytes | Relocation bytes | Compared bytes | Longest unmasked run |
| --- | --- | ---: | ---: | ---: | ---: |
| `0x005252f6` | `_D3DXLoadSurfaceFromSurface@32` | 285 | 4 | 281 | 258 |
| `0x005250c0` | `_D3DXLoadSurfaceFromMemory@40` | 566 | 24 | 542 | 225 |

Each function has zero differences among unmasked bytes. The raw byte
differences are confined to relocation fields, so this establishes exact body
identity modulo linked addresses. The COFF names and decorated stack sizes
also agree with the observed interfaces: the surface copy takes eight stack
arguments and returns with `ret 0x20`; the memory loader takes ten and returns
with `ret 0x28`. The SDK's `d3dx8tex.h` declares these same APIs. At `0x004da16b`
the game calls the surface routine with destination and source surfaces,
null palettes/rectangles, filter `1`, and color key `0`.

The matched artifacts are external build inputs under ignored
`.tools/d3dx8-sdk-8.0/`; they are not committed. `provenance.json` records the
pinned upstream revision and both artifact hashes. The library's Git blob
SHA-1 is `d2a81eb65d7e5caa76a11f0061d6430bcd8c20b2`, its SHA-256 is
`32148094cebbbe9b55f7769787cdde5926ca01014e072844f5152d25072e1f44`, and it
is 1,545,750 bytes. The header's Git blob SHA-1 is
`06e8ec84ad790d2fa85c441750855c1bf1fa0973`, its SHA-256 is
`175d6e45a795faf1c190bdfb83900fc3102e41b23e5f4cfbc79a4423d26a23de`, and it
is 36,755 bytes. Both files are pinned to commit
`e6fa10f5f92bf523c15fdb068206ce25c8d2121d` of the archived DirectX 8.0 SDK.

The SDK's release notes identify retail D3DX as a static library linked with
`d3dx8.lib`; the debug import library is a separate product. This matches the
retail executable, which contains these bodies and imports `d3d8.dll` but no
D3DX DLL. The SDK release included the compiler-compatible library artifact;
the code match directly confirms this exact member's compatibility with the
target's linked build. SDK license metadata is under
`.tools/d3dx8-sdk-8.0/license/`, including `directx sdk eula.txt` and
`directx redist.txt`. Keep all SDK artifacts local and ignored; this work
identifies provenance and does not grant redistribution rights.

The candidate file from the read-only scanner run is ignored at
`build-d3dx8-identify/candidates.csv`. No SDK code or binaries were executed.
