#ifndef LR2_RENDERERSTAGEBUILDERS_H
#define LR2_RENDERERSTAGEBUILDERS_H

#include "render/buffers/rendererbufferobjects.h"
#include "render/pipeline/rendererrecordgroup.h"
#include "render/pipeline/rendererstageplan.h"
#include "render/pipeline/renderertexturemanager.h"
#include "render/resources/rendererrecords.h"
#include "types.h"

#include <stddef.h>

// A manager entry stores four descriptor words and a link to the preceding
// entry in its group subrecord.
struct RendererStageRecordLink {
	LegoU32 m_values[4];
	RendererStageRecordLink* m_next;
};

DECOMP_SIZE_ASSERT(RendererStageRecordLink, 0x14);

// Profile builders use this prefix for indexed values and the highest written
// slot; the containing profile-table extent is not asserted here.
struct TextureStageProfileIndexMapView {
	LegoU32 m_values[10];
	LegoS32 m_highestIndex;
};

DECOMP_STATIC_ASSERT(offsetof(TextureStageProfileIndexMapView, m_highestIndex) == 0x28);

HRESULT PassThroughStageValidationResult(HRESULT p_result);
void __fastcall ResetTextureStageCoordinateIndices(RendererSubrecordGroup::Subrecord* p_group, void*);

RendererStageRecordLink* __fastcall InitializeTextureStageRecord(
	RendererStageRecordLink* p_entry,
	void*,
	LegoU32 p_value0,
	LegoU32 p_value1,
	LegoU32 p_value2
);

void __fastcall SetTextureStageProfileValue(
	TextureStageProfileIndexMapView* p_map,
	void*,
	LegoU32 p_value,
	LegoS32 p_index
);

RendererStageRecordLink* __fastcall InitializeAndLinkTextureStageRecord(
	RendererSubrecordGroup::Subrecord* p_group,
	void*,
	RendererStageRecordLink* p_entry,
	LegoU32 p_value0,
	LegoU32 p_value1,
	LegoU32 p_value2
);

void __fastcall LinkTextureStageRecord(
	RendererSubrecordGroup::Subrecord* p_group,
	void*,
	RendererStageRecordLink* p_entry
);

void __fastcall AddTextureStageSubrecords(
	RendererTextureStageManager* p_manager,
	void*,
	RendererSubrecordGroup::Subrecord* p_groupBase,
	LegoU32 p_groupIndex,
	const RendererArrayMarker* p_sourceRecords,
	LegoS32 p_count
);

// 0x004e5f30 adapts the manager call ABI to the owning renderer's plan apply.
LegoU32 __fastcall ApplyTextureStagePlanForManager(
	RendererTextureStageManager* p_manager,
	void*,
	RendererSubrecordGroup* p_group,
	RendererStagePlanRequest* p_request,
	LegoU32 p_mode
);

// Owner-explicit form of the renderer operation at 0x004e5f90. The first
// explicit group argument is retained because the original callee cleans up
// three stack arguments, though the body does not read that argument.
LegoU32 __fastcall ApplyRendererTextureStagePlan(
	class DisplayRenderer* p_renderer,
	void*,
	RendererSubrecordGroup* p_ignoredGroup,
	RendererStagePlanRequest* p_request,
	LegoU32 p_mode
);

#endif // LR2_RENDERERSTAGEBUILDERS_H
