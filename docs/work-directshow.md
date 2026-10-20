# DirectShow movie support

The movie renderer derives from the genuine SDK `CBaseVideoRenderer`. Reuse
the static DirectShow base-class implementation supplied with the pinned VC6
toolchain (`VC98/Lib/STRMBASE.LIB`); do not reconstruct it as game code or
replace its COM interfaces. The game-owned texture renderer and graph setup
remain separate reconstructions under review.

The VC6-compatible headers used here come from the DirectX 8 SDK mirror
at `https://github.com/edgeforce/directx8`, revision
`e6fa10f5f92bf523c15fdb068206ce25c8d2121d`. The local ignored tree is
`.tools/directx8-sdk-mirror`. Its DirectShow BaseClasses sample provides
`samples/multimedia/directshow/baseclasses/streams.h`, alongside SDK headers
in `include`. Modern replacement BaseClasses headers were not used.

| Local external input | Bytes | SHA-256 |
| --- | ---: | --- |
| `VC98/Lib/STRMBASE.LIB` | 2081732 | `bc16902ec756e505517b7083c9d8f690bd26d7e34d63983c3ede27b3c8d15c10` |
| SDK `streams.h` | 6294 | `008a4d1948800abedf8ab0e5eb42b29a4d65e8f482acc5b171828969e5bc9452` |

Generate the shared Python/CMake configuration with both external SDK inputs:

```sh
python tools/directx8.py --mode wine --output-dir build/config/directx8 \
  --d3dx8-library .tools/d3dx8-sdk-8.0/lib/d3dx8.lib \
  --directshow-sdk-root .tools/directx8-sdk-mirror
```

The generated include order is the Wine DX8 overlay, SDK BaseClasses, SDK
headers, then VC98 headers. This retains the previously selected DX8 graphics,
input and networking declarations. The library list appends `STRMBASE.LIB`;
both builders consume this list without source-specific compiler switches.
SDK mode also accepts `--directshow-sdk-root`. Paths and proprietary inputs
remain in ignored local configuration and external tool directories.

The observed base prefix is 0x148 bytes; the derived movie renderer is
0x278 bytes. Its emitted callback offsets are +90
for SetMediaType, +ac for DoRenderSample and +b0 for CheckMediaType. Layout and
pointer checks establish these ABI facts, not equivalence of every library
instruction. No additional LIBRARY instruction-match claims are made here.

The EXE supplies the SDK's empty factory template globals (`g_Templates` and
`g_cTemplates`) for the linked base-class objects. This is unclaimed SDK link
glue, with no retail address. It does not alter the CRT entry point or COM
initialization. The complete game startup still needs integration before a
rendered-menu run.
