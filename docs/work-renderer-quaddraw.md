# GUI rectangle and quad drawing

## Rectangle packet evidence

The two-stack-argument call through renderer slot `0xf4` in `0x004ad97d`
passes a `0x54`-byte rectangle first and a `RendererDefaultRecord` second.
The callee at `0x004df260` ends with `ret 8`. It copies 21 DWORDs when it
needs to clip the packet, then passes the second pointer to the texture-stage
manager. The current `RendererGuiRectangle` view therefore names only fields
whose use is visible in the body: bounds at `0x00..0x0c`, clip bounds at
`0x10..0x1c`, packed color at `0x28`, texture index at `0x2c`, UV values at
`0x30..0x3c`, rotation at `0x40`, and blend mode at `0x44`. The remaining
words stay opaque. The settings caller initializes color to packed white,
blend mode to zero, and UVs to `{0, 1, 1, 0}`.

## Clipping helper

`0x004fb880` takes a mutable rectangle pointer, returns a boolean in EAX, and
cleans its one stack argument with `ret 4`. It rejects nonzero ordered rotation,
uses zero scale for nonpositive extents, and clips left, right, top, then
bottom. Each accepted edge updates the matching UV coordinate. Lower-edge
unordered comparisons enter the clipping branch; the resulting unordered
fraction is rejected. The implementation retains these details and stays
annotated `STUB` pending comparison and review.

The helper's verified code extent is 394 bytes (`0x004fb880..0x004fba09`,
including the return instruction). `DrawGuiRectangle` spans 1100 bytes
(`0x004df260..0x004df6ab`, including `ret 8`).

## Recovered drawing path and remaining review

The current `DrawGuiRectangle` body covers clipping, blend-mode selection,
stage-plan application, viewport conversion, optional rotation, immediate
vertex setup, FVF selection, and the `D3DPT_TRIANGLESTRIP` draw. Its raw
extent is 1100 bytes. The rotation branch is expressed with standard-library
trigonometry; x87 intermediate precision and exceptional inputs still need
comparison review, so it remains `STUB`. The helper also remains `STUB` until
its reccmp result and unordered comparisons are reviewed in the integrated
build.

The quad packet method at `0x004e0db0` dispatches several packet variants and
depends on renderer state and resource callbacks; it remains separate work.
No runtime test was run.
