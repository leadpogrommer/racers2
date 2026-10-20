#ifndef LR2_RENDERERDRAWPACKETVIEW_H
#define LR2_RENDERERDRAWPACKETVIEW_H

#include "decomp.h"
#include "render/buffers/rendererbufferobjects.h"
#include "render/resources/rendererrecords.h"
#include "types.h"

#include <stddef.h>

// The renderer consumes this non-owning prefix at 0x004dd460 and 0x004dd900.
// The recovered callbacks access fields through the end of m_defaults; this
// projection makes no claim about any larger packet allocation.
struct RendererDrawPacketView {
	LegoU16 m_primitiveCount;         // 0x00
	LegoU16 m_vertexCount;            // 0x02
	LegoS16 m_auxiliaryRecord;        // 0x04, sign-extended for the auxiliary table lookup
	LegoU16 m_flags;                  // 0x06
	void* m_payload;                  // 0x08, interpreted by special packet paths
	RendererVertexBuffer* m_vertices; // 0x0c
	RendererIndexBuffer* m_indices;   // 0x10; primitive type selector at +0x0c
	RendererDefaultRecord m_defaults; // 0x14..0x3b
};

DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketView, m_flags) == 0x06);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketView, m_payload) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketView, m_vertices) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketView, m_indices) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererDrawPacketView, m_defaults) == 0x14);

void __fastcall ReleaseRendererDrawPacket(RendererDrawPacketView* p_packet, void* p_unusedEdx);

#endif // LR2_RENDERERDRAWPACKETVIEW_H
