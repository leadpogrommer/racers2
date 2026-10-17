# Renderer texture upload recovery

`src/renderertextureupload.cpp` now contains the CPU mip copy, texture
creation/dimension helpers, and the record upload/update path. All bodies remain
`STUB` except the exact mip-count helper at `0x004d9030`; raw reccmp results are
from the isolated worker build with the pinned VC6 and D3DX8 toolchains.

| Address | Length | Raw match | Status |
| --- | ---: | ---: | --- |
| `0x004d8c90` | 919 (`0x397`) | 4.29% | STUB |
| `0x004d9030` | 32 (`0x020`) | 100% | FUNCTION |
| `0x004d9050` | 264 (`0x108`) | 7.10% | STUB |
| `0x004d9160` | 316 (`0x13c`) | 35.85% | STUB |
| `0x004d9970` | 359 (`0x167`) | 31.13% | STUB |
| `0x004d9ae0` | 139 (`0x08b`) | 60.61% | STUB |
| `0x004da020` | 443 (`0x1bb`) | 34.63% | STUB |
| `0x004da540` | 157 (`0x09d`) | 35.40% | STUB |

The extents include each full three-byte `ret`. The isolated full-source build
links successfully, including the pinned DirectX 8.0 `d3dx8.lib`.

`RendererTextureRecord::UpdateGpuTexture` marks its primary texture dirty,
creates a temporary system-memory texture for the compressed-format path,
copies mip surfaces with `_D3DXLoadSurfaceFromSurface@32`, then updates the
device texture. That D3DX8 routine was identified against the pinned SDK
library; it is not reimplemented as game code. `UploadBitmapImage` locks the
primary level, dispatches the recovered CPU copy, unlocks on success, releases
the surface, and invokes the GPU update path.

Both record methods use the actual reserved static renderer allocation. The
ID-derived owner formula initially used during recovery fails for default
textures whose IDs have not yet been assigned; see [owner evidence](work-renderer-owner.md).
The selected-format and device fields belong to that allocation.

The renderer's ordinary creation path disables compressed selection for cube
records. Retail nevertheless retains a compressed-plus-cube allocation branch,
followed by calls using the two-dimensional `GetSurfaceLevel` ABI throughout
the mip loop. The source preserves that call sequence through the actual 2D
interface type, including this unusual combination. No positive-X face or
additional face argument is synthesized. No runtime or behavioral tests were
run.

The original ID-based owner assumption was corrected after review: direct
default creation leaves IDs unassigned. Upload now uses the actual reserved
static renderer allocation; see [owner evidence](work-renderer-owner.md).
Earlier isolated scores precede this ownership correction.
