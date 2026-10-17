#ifndef LR2_RENDERERVERTEXDATA_H
#define LR2_RENDERERVERTEXDATA_H

#include "decomp.h"
#include "rendererbufferobjects.h"
#include "types.h"

#include <stddef.h>

// A caller-provided view of vertex attributes used by the renderer-buffer
// copy routines. The first four words are byte offsets from the current
// interleaved vertex; callers and the copy routines establish the offsets
// below, but no exact total size or array stride is claimed.
struct RendererVertexDataView {
	LegoU32 m_positionOffset; // 0x00, from m_cursor
	LegoU32 m_normalOffset;   // 0x04, from m_cursor
	LegoU32 m_colorOffset;    // 0x08, from m_cursor
	LegoU32 m_textureOffset;  // 0x0c, from m_cursor
	LegoS32 m_stride;         // 0x10, bytes between source vertices
	LegoS32 m_textureCount;   // 0x14
	LegoU16 m_attributes;     // 0x18
	LegoU16 m_vertexCount;    // 0x1a
	undefined m_unk0x1c[4];
	LegoU8* m_data;   // 0x20, packed vertex data
	LegoU8* m_cursor; // 0x24, advanced by the conversion routine
};

DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_positionOffset) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_stride) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_textureCount) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_attributes) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_vertexCount) == 0x1a);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_data) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(RendererVertexDataView, m_cursor) == 0x24);

#endif // LR2_RENDERERVERTEXDATA_H
