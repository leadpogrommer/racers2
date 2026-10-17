# Renderer texture-state helpers

The local helper pair at `0x004e1e60` and `0x004e1fa0` implements texture
binding and stage-state restoration used by the renderer's action dispatcher.
Their receiver is the renderer (`ECX`); the first helper has one signed stack
argument and returns with `ret 4`, while the second has no stack arguments.

The texture slots are an observed 800-entry array of `0xa0`-byte records at
renderer offset `0x38d8`. The default slot is at `0x22cd8`, immediately before
the 800-entry slot-to-record map at `0x22cdc`. Each record's Direct3D texture
pointer is at `+0x98`. This makes slot `-1` the default entry and nonnegative
slots map through the 800-entry map. These facts come from the access pattern
in both helpers and constructor/destructor walks; they do not establish a
complete renderer object size.

`BindMappedTexture` first restores the default texture and the ten cached stage
states touched by `RestoreTextureStageDefaults`. It then selects either the
default index for `-1` or the mapped index, updates the stage-0 texture cache
before calling `IDirect3DDevice8::SetTexture`, and sets stage-0 states 18, 13,
and 14 to 2, 3, and 3. The reset helper similarly updates each cache before
issuing `SetTextureStageState`; it binds stage 0 only, and writes states 11,
1–6, and 18 on stage 0 plus states 1 and 4 on stage 1. It does not reset all
eight texture stages; that separate behavior is implemented by
`DisplayRenderer::ResetTextureStages`.

These implementations preserve retail ordering and cache guards. An isolated
build produced raw instruction similarities of 46.81% for `BindMappedTexture`
and 34.00% for `RestoreTextureStageDefaults`; both remain annotated `STUB`.
The difference is mainly from routing retail's repeated inline cache checks
through the existing shared stage-state helper. Their full retail extents are
188 bytes (`0x004e1e60..0x004e1f1b`, including `ret 4`) and 443 bytes
(`0x004e1fa0..0x004e215a`, including `ret`).

`ApplyTextureOperation` at `0x004e0560` now contains all retail operation cases
0 through 13 and the operation-14 transition. Operations 1, 2, 9 and 10 call
the renderer's typed depth-state dispatch slot; operation 1 also calls the
observed slot at `+0x8c` when the vertex-format guard is clear. Cases 3 and 5
use the recovered vertex/index wrapper getters and `SetVertexStream`. Case 5
also builds three dependent 4x4 transforms through named fields recovered from
the constructor and transform setters: world-view goes to
`m_transform0x67c`, then projection to `m_transform0x6bc`, then viewport to
`m_transform0x6fc`. The default world matrix is separately stored in
`m_defaultWorldTransform` at `+0x73c`.

`MultiplyMatrix4` at `0x004ddad0` writes each result cell from a row of the left
matrix and a column of the right matrix, storing each successive partial sum
back to the result as retail does. Its renderer call sites keep the output
distinct from both inputs. `ApplyDefaultWorldTransform` at `0x004de110` copies
the default world matrix into the active world matrix, submits it as
`D3DTS_WORLD`, then clears the transform-dirty flag at `+0x788`. It is the
renderer dispatch target at slot `+0x34`, called without stack arguments by
the action dispatcher. The adjacent matrix-and-position world setter at
`0x004de080` occupies slot `+0x30`.
`FlushSpecularVertexStream` at `0x004df760` unlocks
the specular vertex buffer, issues `DrawIndexedPrimitive(D3DPT_TRIANGLELIST)`
when the stream count is nonzero, clears that count, and relocks the buffer.
Operation 6 then performs an additional unconditional unlock, as retail does.
The HRESULT from the draw is stored in the existing global at `0x0067bd58`;
the action state and vertex-upload pass count are the two annotated globals at
`0x0067bd70` and `0x0067bd74`.

The stream flush matches all 108 bytes exactly (`0x004df760..0x004df7cb`,
including `ret`) and is marked `FUNCTION`. Its signed right shift is explicit:
retail uses `SAR` on the stream count before passing that bit pattern as the
primitive count.

The matrix helper matches all 95 bytes exactly (`0x004ddad0..0x004ddb2e`,
including `ret 0xc`) with its corrected `__stdcall` prototype, and is marked
`FUNCTION`. The world-transform helper has the recovered 80-byte extent
(`0x004de110..0x004de15f`) and its source follows the observed copy, device
call, and dirty-flag order; it remains `STUB` at 60.00% because the compiler
emits a different copy sequence.

The other new functions remain `STUB` because their measured diffs are not
near matches: `ApplyTextureOperation` scores 31.77% and `SetVertexStream`
56.72%. The dispatcher is implemented across all
cases, but its many retail inline cache checks differ substantially from
routing those operations through the existing render-state helper; this score
does not establish that every re-entrant cache effect is identical. Matrix
products preserve the retail intermediate stores; the renderer's known product
call sites use distinct output/input matrices.
`SetVertexStream` preserves the observed early return when the stream pointer
is unchanged, including skipping the shader-cache check. The two texture
helpers remain `STUB` at 46.81% and 34.00% because they share repeated cache
checks through a helper.

Retail extents are: dispatcher 1925 bytes (`0x004e0560..0x004e0ce4`, including
`ret 4`), stream setter 110 bytes (`0x004e0d40..0x004e0dad`, including
`ret 0xc`), matrix multiplication 95 bytes (`0x004ddad0..0x004ddb2e`, including
`ret 0xc`), default-world transform 80 bytes (`0x004de110..0x004de15f`,
including `ret`), texture binding 188 bytes (`0x004e1e60..0x004e1f1b`, including
`ret 4`), and stage reset 443 bytes (`0x004e1fa0..0x004e215a`, including
`ret`).
