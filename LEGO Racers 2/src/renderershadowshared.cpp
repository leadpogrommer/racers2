#include "renderershadowshared.h"

#include "memoryheap.h"
#include "modelresource.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererdrawpacketview.h"

#include <new.h>
#include <stddef.h>
#include <string.h>

namespace
{
struct RendererShadowSharedState {
	RendererDrawPacketView* m_packet; // 0x00, DisplayRenderer +0x4b4
	LegoS32 m_materialId;             // 0x04, DisplayRenderer +0x4b8
	LegoS32 m_textureId;              // 0x08, DisplayRenderer +0x4bc
	LegoU32 m_referenceCount;         // 0x0c, DisplayRenderer +0x4c0
};
DECOMP_SIZE_ASSERT(RendererShadowSharedState, 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererShadowSharedState, m_packet) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(RendererShadowSharedState, m_materialId) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(RendererShadowSharedState, m_textureId) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererShadowSharedState, m_referenceCount) == 0x0c);

RendererShadowSharedState* GetShadowState(DisplayRenderer* p_renderer)
{
	return (RendererShadowSharedState*) ((LegoU8*) p_renderer + 0x4b4);
}
} // namespace

// STUB: LEGORACERS2 0x004be650
void __fastcall EnsureSharedShadowGeometry(DisplayRenderer* p_renderer, void* p_unusedEdx)
{
	(void) p_unusedEdx;
	RendererShadowSharedState* state = GetShadowState(p_renderer);
	if (state->m_referenceCount == 0) {
		RendererAuxiliaryResourceRecord material;
		memset(&material, 0, sizeof(material));
		material.m_modeFactor = 0.5f;
		material.m_mode = 6;
		state->m_materialId = p_renderer->AcquireAuxiliaryResource(&material, 0);
		state->m_textureId =
			p_renderer->CreateTextureResource("game data\\common\\effects\\shadow\\shadow.tga", 0, -1, 1, 1);

		RendererDrawPacketView* packet =
			(RendererDrawPacketView*) AllocateHeapMemory(sizeof(ModelPrimitiveTextureRecord));
		if (packet != NULL) {
			new (packet) ModelPrimitiveTextureRecord();
		}
		ModelPrimitiveTextureRecord* primitive = (ModelPrimitiveTextureRecord*) packet;
		state->m_packet = packet;
		packet->m_vertices = p_renderer->CreateVertexBuffer(0x0d, 0x400, 1, 1);
		packet->m_indices = p_renderer->CreateIndexBuffer(0, 0x1686);
		packet->m_flags |= 0x40;
		packet->m_auxiliaryRecord = (LegoS16) state->m_materialId;
		packet->m_primitiveCount = 0;
		packet->m_vertexCount = 0;
		packet->m_defaults.m_unk0x00 = 0;
		packet->m_defaults.m_unk0x04 = 0;
		packet->m_defaults.m_unk0x06 = 0;
		packet->m_defaults.m_unk0x07 = 1;
		primitive->m_slotCount = 1;
		primitive->m_coordinateFlags = 0;
		primitive->m_coordinateSetCount = 1;
		packet->m_defaults.m_slots[0].m_value = 0;
		packet->m_defaults.m_slots[0].m_mask = (LegoU16) state->m_textureId;
		packet->m_defaults.m_slots[0].m_kind = 0;
		packet->m_defaults.m_slots[0].m_flags = 0;
		packet->m_defaults.m_unk0x00 |= 1;
		((const RendererMainDispatch*) p_renderer->m_vtable)
			->m_prepareDrawPlan(p_renderer, NULL, (RendererStagePlanRequest*) packet);
	}
	++state->m_referenceCount;
}

// STUB: LEGORACERS2 0x004be860
void __fastcall ReleaseSharedShadowGeometry(DisplayRenderer* p_renderer, void* p_unusedEdx)
{
	(void) p_unusedEdx;
	RendererShadowSharedState* state = GetShadowState(p_renderer);
	if (--state->m_referenceCount == 0) {
		if (state->m_packet != NULL) {
			ReleaseRendererDrawPacket(state->m_packet, NULL);
			FreeHeapMemory(state->m_packet);
			state->m_packet = NULL;
		}
		p_renderer->ReleaseTextureResourceReference(state->m_textureId, 1);
		p_renderer->ReleaseAuxiliaryResource(state->m_materialId);
	}
}
