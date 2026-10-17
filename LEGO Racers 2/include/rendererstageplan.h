#ifndef LR2_RENDERERSTAGEPLAN_H
#define LR2_RENDERERSTAGEPLAN_H

#include "rendererrecords.h"
#include "types.h"

#include <stddef.h>

// Request passed to the renderer's texture-stage-plan resolver. The first
// four words and callback contexts are observed storage whose meanings are
// still being recovered; the embedded defaults record has a known layout.
struct RendererStagePlanRequest {
	LegoU16 m_unk0x00;
	LegoU16 m_unk0x02;
	LegoU16 m_unk0x04;
	LegoU16 m_unk0x06;
	void* m_heap; // 0x08
	void* m_unk0x0c;
	void* m_unk0x10;
	RendererDefaultRecord m_defaults; // 0x14..0x3b
};

DECOMP_SIZE_ASSERT(RendererStagePlanRequest, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(RendererStagePlanRequest, m_heap) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererStagePlanRequest, m_defaults) == 0x14);

// Removes a stage entry and its bit from a renderer default-operation record.
void __fastcall RemoveStageFromOperationPlan(RendererDefaultRecord* p_record, void*, LegoU32 p_stage);

// Copies one 8-byte stage default slot. EDX is unused in the original member
// call ABI and is explicit here so the helper can be called without an
// unmodeled class method.
RendererDefaultSlot* __fastcall CopyTextureStageDefaultSlot(
	RendererDefaultSlot* p_destination,
	void*,
	const RendererDefaultSlot* p_source
);

#endif // LR2_RENDERERSTAGEPLAN_H
