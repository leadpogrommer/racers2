# Renderer draw packet callbacks

The packet callbacks at `0x004dd460` and `0x004dd900` take one stack pointer
and return with `ret 4`. The proven prefix is four 16-bit values at `+0x00`,
the special-path payload at `+0x08`, vertex and index buffer pointers at
`+0x0c` and `+0x10`, and a `RendererDefaultRecord` at `+0x14`. The draw code
uses the index-buffer record's `+0x0c` value as its primitive selector. The
view intentionally does not assert a complete packet size.

`PrepareDrawPacketState` (`0x004dcf30`, 372 bytes) resolves the signed
auxiliary index, invokes the optional resource callback, copies the device
material, applies the color override to diffuse/ambient/emissive channels,
updates cached specular state, and dispatches the resource's blend mode through
renderer slot `+0xc8`. The nearby draw callback at `0x004dd460` has a
1125-byte body and contains several draw modes. Its initial vertex/index
pointers and flags are saved before the state callback; storage, stride, base
vertex and later flag tests are read after it, matching retail's
callback-visible mutation behavior.

`DrawIndexedPacket` (`0x004dd900`, 226 bytes) updates the stream source and
vertex shader only when the native vertex buffer changes. It obtains the
native index buffer from the index record's getter at `+0x18`, passes the
vertex buffer's base vertex to `SetIndices`, then draws using the six-entry
retail primitive map `[4, 5, 6, 2, 3, 1]`. The prior interpretation of
`+0x10` as a shader resolver was incorrect; raw instructions show it is a
`RendererIndexBuffer*`.

The latest isolated raw scores are 44.00% for `PrepareDrawPacketState`, 37.67%
for `DrawRendererPacket`, 25.35% for `DrawIndexedPacket`, and 19.64% for
`ApplySpecialDrawPacketEffect`. All remain `STUB`: these scores are far below
the project's near-match threshold, and some helper arithmetic still needs
integrated review.

`ApplySpecialDrawPacketEffect` (`0x004de990`, 1090 bytes) is reached for
packets whose default-record flags include bit 3. Its two paths update either
packed vertex colors or animated texture coordinates. The texture-coordinate
path uses the packet payload as six floats per vertex and looks up source and
target texture stages from the defaults. The packed-color path generates a
direction from a changing angle, transforms vertex positions by a quaternion
rotation and the current world matrix, then packs three signed channels. The
packet helper calls the retail quaternion-between-vectors routine with
`up = (0, 1, 0)` and a normalized `(sin(angle), 1, cos(angle))` direction. The
implementation expresses its ordinary positive-dot branch using the
normalized cross product and half-angle sine/cosine; the retail antiparallel
fallback is outside this caller's finite-input range. Retail literal-pool
values used here were read from the executable: angle steps `0.02f` at
`0x0052f418` and `0.002f` at `0x005326e8`, and channel conversion constants
`127.0f`, `128.0f`, `255.0f` at `0x00531934`, `0x00530920`, `0x0052f89c`.
The retail `-0.999f` quaternion threshold is `0x0053536c`. Channel conversion
uses an explicit float spill followed by FISTP, preserving the active x87
rounding mode. The full special-path equivalence still needs integrated
review, so this function also remains a `STUB`.

The packet-state helper's material destination is the renderer's observed
embedded material at `+0x24528`. The `+0x48` operation field previously
attributed to the packet was an erroneous decompiler interpretation: the
actual alpha-blend mode is read from the selected auxiliary resource record.
