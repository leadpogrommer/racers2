# Renderer buffer dispatch records

The renderer owns two plain function-pointer records at retail addresses
`0x00535488` and `0x00535538`. The first has 44 DWORD entries; the second has
7. They are not annotated as C++ vtables because the reconstructed buffer
types are concrete records with manually recovered callbacks, not a recovered
source inheritance hierarchy. The emitted objects are named
`g_rendererVertexBufferDispatch` and `g_rendererIndexBufferDispatch` for
binary wiring verification.

The table entries retain the retail order. For the vertex record, slots `0x00`
through `0x18` bind release, lock/unlock, vertex-copy operations, mode and
storage-policy setters; `0x20` is the original CRT `_purecall`; `0x24` through
`0x70` bind buffer getters and vertex attribute accessors; `0x74` through
`0x90` bind attribute setters; `0x94` retains the unusual conditional free
callback; and `0x98` through `0xac` are the original `_purecall` target. The
index record binds release, lock, unlock, flag and storage-policy setters,
`_purecall`, and the device-buffer getter in its seven original slots.

The retail entry to emitted callback mapping is:

```text
vertex +00 004e9540 RendererVertexBufferDispatchRelease
vertex +04 004e9770 RendererVertexBufferDispatchLock
vertex +08 004e9810 RendererVertexBufferDispatchUnlock
vertex +0c 004e9b20 RendererVertexBufferDispatchCopyVertices
vertex +10 004e9960 RendererVertexBufferDispatchCopyFromData
vertex +14 004e9d00 RendererVertexBufferDispatchCopyToData
vertex +18 004e9df0 RendererVertexBufferDispatchSetMode
vertex +1c 004e9d80 RendererVertexBufferDispatchSetPolicy
vertex +20 0051a08f _purecall
vertex +24 004ab590 RendererVertexBufferDispatchGetDeviceBuffer
vertex +28 004ab5a0 RendererVertexBufferDispatchGetAttributes
vertex +2c 004ab850 RendererVertexBufferDispatchGetStride
vertex +30 004ab5b0 RendererVertexBufferDispatchGetCount
vertex +34 004ab5c0 RendererVertexBufferDispatchGetTextureCount
vertex +38 004ab5d0 RendererVertexBufferDispatchGetVertex
vertex +3c 004ab5e0 RendererVertexBufferDispatchGetPositionOffset
vertex +40 004ab5f0 RendererVertexBufferDispatchGetNormalOffset
vertex +44 0049a6a0 RendererVertexBufferDispatchGetColorOffset
vertex +48 004ab600 RendererVertexBufferDispatchGetTextureOffset
vertex +4c 004ab610 RendererVertexBufferDispatchSetCursor
vertex +50 004ab630 RendererVertexBufferDispatchAdvanceCursor
vertex +54 004ab650 RendererVertexBufferDispatchGetPosition
vertex +58 004ab640 RendererVertexBufferDispatchGetCurrentPosition
vertex +5c 004ab680 RendererVertexBufferDispatchGetNormal
vertex +60 004ab670 RendererVertexBufferDispatchGetCurrentNormal
vertex +64 004ab6b0 RendererVertexBufferDispatchGetColor
vertex +68 004ab6a0 RendererVertexBufferDispatchGetCurrentColor
vertex +6c 004ab6f0 RendererVertexBufferDispatchGetTextureCoordinate
vertex +70 004ab6d0 RendererVertexBufferDispatchGetCurrentTextureCoordinate
vertex +74 004ab730 RendererVertexBufferDispatchSetPosition
vertex +78 004ab710 RendererVertexBufferDispatchSetCurrentPosition
vertex +7c 004ab780 RendererVertexBufferDispatchSetNormal
vertex +80 004ab760 RendererVertexBufferDispatchSetCurrentNormal
vertex +84 004ab7d0 RendererVertexBufferDispatchSetColor
vertex +88 004ab7b0 RendererVertexBufferDispatchSetCurrentColor
vertex +8c 004ab820 RendererVertexBufferDispatchSetTextureCoordinate
vertex +90 004ab7f0 RendererVertexBufferDispatchSetCurrentTextureCoordinate
vertex +94 004ab560 RendererVertexBufferLegacyConditionalFree
vertex +98 0051a08f _purecall
vertex +9c 0051a08f _purecall
vertex +a0 0051a08f _purecall
vertex +a4 0051a08f _purecall
vertex +a8 0051a08f _purecall
vertex +ac 0051a08f _purecall

index  +00 004ab860 RendererIndexBufferDispatchRelease
index  +04 004ab890 RendererIndexBufferDispatchLock
index  +08 004ab900 RendererIndexBufferDispatchUnlock
index  +0c 004ab9c0 RendererIndexBufferDispatchSetFlag4
index  +10 004e9fa0 RendererIndexBufferDispatchSetPolicy
index  +14 0051a08f _purecall
index  +18 004ab850 RendererIndexBufferDispatchGetDeviceBuffer
```

Each callable buffer method is reached through a typed `__fastcall` bridge.
The bridge passes the object in ECX, consumes the otherwise-unused EDX value,
and leaves explicit arguments on the stack with the observed cleanup size.
This preserves the retail call boundary without asserting a C++ virtual
hierarchy or casting member-function pointers. The shared stride/device getter
at `0x004ab850` is represented by separate vertex and index bridges because
the same field offset has a different meaning in each record.

The vertex constructor at `0x004abde0` is 74 bytes and initializes only the
observed fields: texture count `1`, attributes `0x0b`, position/normal/texture
offsets `0/12/24`, stride `32`, count and the observed zero fields, its table
pointer, and the mask `m_flags &= 0xffffffcc`. It leaves the other fields
untouched. The index constructor at `0x004abe30` is 29 bytes and clears fields
at `+0x08`, `+0x18`, and `+0x20`, installs its table pointer, and applies
`m_flags &= 0xffffff98`; it also leaves other fields untouched.

Vertex slot `0x94` targets `0x004ab560`. The body tests the DWORD at `this+8`
for exactly `1`; if so, and the DWORD at `this+0x10` is nonzero, it frees that
value through `MemoryHeap::Free` and writes zero back. The recovered record
currently calls these locations `m_positionOffset` and `m_colorOffset`, which
makes the operation look unlike a normal vertex-buffer destructor. The bridge
preserves the raw offsets and condition and does not reinterpret it as
`Release()`.

The two storage-policy callbacks at `0x004e9d80` and `0x004e9fa0` update the
vertex flags at `+0x4c` and index flags at `+0x2c`. Both consult the real
renderer flag at `0x00653010`, now represented by
`DisplayRenderer::m_bufferStoragePolicy` at `+0xc7ed0`. Table targets and
constructor lengths are grounded in the retail PE table contents and complete
disassembly blocks, including their final `ret` instructions. New bodies stay
annotated as stubs until the root's combined build and comparison verifies
them.

Combined validation retains and measures all five new annotated bodies: the
legacy free callback 75.86%, vertex constructor 61.11%, index constructor
90.00%, vertex policy 13.64%, and index policy 4.65%. All remain STUB. The
51 buffer slots pass `check_renderer_resource_dispatch.py` in both builds,
within its complete 78-pointer resource inventory. No renderer constructor
call has been integrated yet.
