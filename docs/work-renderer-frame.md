# Renderer frame helpers

This batch models the renderer's texture-record accessor, viewport slots, the
viewport projection transform, and the quaternion-based world transform. It
does not create or alter renderer dispatch tables. The field and slot offsets
used below were confirmed in the retail disassembly and added to the observed
`DisplayRenderer` prefix by the integration owner.

`GetTextureRecord` at `0x004ac040` is a 20-byte `thiscall` accessor with one
stack argument and `ret 4`. It returns `this + 0x38d8 + index * 0xa0`. The
typed table contains 800 `RendererTextureRecord` entries, matching the
constructor's initialized extent; the accessor itself has no bounds check.
Its isolated raw instruction score is 100%.

`AddViewport` at `0x004ae360` scans sixteen `0x1c`-byte records at `+0x24360`
for a zero in-use word. It marks the selected record in use before calling
dispatch slot `+0xb4` with the slot and four supplied coordinates. If all
records are occupied, it returns `-1`. Its extent is 105 bytes including the
`ret 0x10`; the implementation remains `STUB` at 55.88% raw because the
current C++ call shape differs from the retail sequence.

`UpdateViewport` at `0x004ae3d0` clears the six DWORDs beginning at record
offset `+4`, copies X, Y, width and height, and sets depth range to 0.0 through
1.0. The extent is 82 bytes including `ret 0x14`; the isolated raw score is
100%.

`SelectViewport` at `0x004ae430` passes the selected viewport to the DX8
device's `SetViewport`, stores the HRESULT in `g_lastDirect3DResult`, records
the index at `+0x24524`, and updates projection members from the selected
viewport. Its extent is 69 bytes including `ret 4`; it scores 100% and is
marked `FUNCTION` after the COM method was checked against the local DirectX 8
SDK header.

`SetProjectionFromViewport` at `0x004ae480` converts the signed viewport
coordinates and dimensions to floats, computes right/bottom and half-size
members, zeros the 4x4 matrix at `+0x63c`, then writes the six observed matrix
elements. In particular the unit value at `+0x664` is matrix element `[2][2]`.
The extent is 287 bytes including `ret 4`. Its raw score is 20.51%; the source
keeps the recovered behavior readable while retaining `STUB` because the
instruction match is low.

`SetWorldTransform` at `0x004ddf30` accepts pointers to a four-float
`Quaternion` and a three-float `Vector3`, fills the world matrix at `+0x57c`,
calls `IDirect3DDevice8::SetTransform(D3DTS_WORLD, ...)`, and clears the DWORD
at `+0x788`. Its complete body is 335 bytes including `ret 8`. The quaternion
matrix signs follow the retail equations. Retail stores several products to
32-bit float locals before combining them while retaining other products in
x87 extended precision; the current expression form does not establish those
rounding points, so this remains a `STUB` at 36.46% pending a rounding-aware
review.

The `Matrix3` overload at `0x004de080` transposes the 3x3 input into the world
matrix, copies the position into row 3, sets the homogeneous components, calls
`SetTransform(D3DTS_WORLD, ...)`, and clears `+0x788`. It is a separate
thiscall with two pointer arguments; its function length and isolated score
is 137 bytes and 12.37%, respectively. Its source now calls the SDK
`SetTransform(D3DTS_WORLD, ...)` and clears `+0x788`; it remains `STUB`.

`SetViewTransform` at `0x004de160` takes the same rotation/position pair. It
transposes the input rotation to compute the inverse position, fills the view
matrix at `+0x5bc`, and sends it through `SetTransform(D3DTS_VIEW, ...)`. It
also builds a rotation-only transform at `+0x53c`, multiplies it by the
fixed Euler rotation for `(0, pi/2, 0)`, then multiplies by the retail Y
reflection matrix at `0x0054add0`. The original matrix was read from the PE as
the diagonal `(1, -1, 1, 1)`. The body length is 383 bytes including `ret 8`;
the isolated score is 13.53%. The implementation calls the already recovered
`Matrix3::FUN_004a4f60` helper. It remains `STUB` while the current matrix
composition shape and floating-point equivalence are reviewed.

`SetProjection` at `0x004de2e0` receives an orthographic scale, horizontal and
vertical field-of-view angles, near and far planes, and a mode word. Mode zero
builds the orthographic matrix using the display configuration's
height-to-width ratio. Other modes build a perspective matrix, including the
`far / (far - near)` depth terms. It updates the cached cull mode, computes
projection pixel scale from the signed display width and retail factors
`1/640` and `400`, negates the horizontal projection element when clockwise
culling is exactly one, and calls `SetTransform(D3DTS_PROJECTION, ...)`. Its
369-byte extent includes `ret 0x18`; the isolated score is 29.94%. It remains
`STUB` while x87 trigonometry and intermediate rounding are unresolved.

`SelectLights` at `0x004bf150` takes a center vector, a radius, a mode and a
mask/direction buffer. With a supplied mask it updates each light's selected
bit, synchronizes the device-enabled bit with `LightEnable`, and copies the
three direction DWORDs. With the `0xffffffff` sentinel it selects enabled
lights whose radius includes the center, accumulates directional and nearby
spot/point contributions in mode 1, normalizes the result (using the observed
`(0.35, -1.0, 0.35)` fallback), and removes the farthest selected point lights
until at most four remain. The 32 records at `+0xc6290` have a `0x70` stride:
flags and device index at `+0/+2`, a radius at `+4`, and a `D3DLIGHT8` at `+8`.
The signed count is at `+0xc7090`, with aggregate direction at `+0xc7094`.
The record extent is confirmed by the SDK `D3DLIGHT8` size of `0x68`; there is
no trailing padding in the `0x70` record.

The retail helper `0x004bf680` takes the light record as `this` and a boolean;
it toggles the record's device-enabled bit and calls `LightEnable` through the
static renderer's device pointer. The recovered `SelectLights` uses an
unannotated translation-unit helper with the renderer's `m_device` instead of
claiming that helper's original ABI. `SelectLights` remains `STUB` until the
expanded 1320-byte body compiles, but the isolated raw instruction score is
24.23%. Float spill and rounding points still need review before treating the
source as behaviorally equivalent.

The isolated worker compiled and linked with the configured VC6 SP5/DX8
toolchain. Reccmp measured `GetTextureRecord`, `UpdateViewport`, and
`SelectViewport` at 100% each. `AddViewport`,
`SetProjectionFromViewport`, `SelectLights`, and `SetWorldTransform` scored
55.88%, 20.51%, 24.23%, and 36.46%, respectively, and remain `STUB`. No game
or runtime probe was executed.
