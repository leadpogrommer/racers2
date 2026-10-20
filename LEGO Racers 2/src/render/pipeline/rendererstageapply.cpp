#include "render/core/renderer.h"
#include "render/pipeline/rendererstagebuilders.h"

namespace
{
LegoU32 MakeStageAlphaColor(LegoFloat p_scaled)
{
	volatile LegoFloat stored = p_scaled;
	LegoS32 rounded;
	__asm {
		fld stored
		fistp rounded
	}
	return ((LegoU32) rounded << 24) | 0x00ffffff;
}

// Recovered execution algorithm from 0x004e2160. The original uses the static
// renderer directly; pass its evidenced owner here until that singleton's
// complete constructor and dispatch are available. This helper has no claim
// to the original member ABI.
LegoU32 ExecuteTextureStageSubrecord(
	DisplayRenderer* p_renderer,
	RendererSubrecordGroup::Subrecord* p_group,
	RendererDefaultRecord* p_defaults
)
{
	RendererStageRecordLink* entry = (RendererStageRecordLink*) p_group->m_unk0x20;
	for (; entry != NULL; entry = entry->m_next) {
		const LegoU32 kind = entry->m_values[0];
		const LegoU32 state = entry->m_values[1];
		LegoU32 value;
		if (kind == 1) {
			p_renderer
				->SetCachedTextureStageState(entry->m_values[3], (D3DTEXTURESTAGESTATETYPE) state, entry->m_values[2]);
			continue;
		}
		switch (kind) {
		case 0:
			value = entry->m_values[2];
			break;
		case 2:
			value = p_renderer->GetStageFactorColor();
			break;
		case 3:
			value = p_renderer->GetInverseStageFactorColor();
			break;
		case 4:
			value = MakeStageAlphaColor(p_renderer->m_stageAlpha * 255.0f);
			break;
		case 5:
			value = MakeStageAlphaColor(p_renderer->m_stageAlpha * p_renderer->m_stageBlend * 255.0f);
			break;
		case 6:
			value = MakeStageAlphaColor((1.0f - p_renderer->m_stageBlend) * p_renderer->m_stageAlpha * 255.0f);
			break;
		default:
			continue;
		}
		p_renderer->SetCachedRenderState((D3DRENDERSTATETYPE) state, value);
	}

	for (LegoS32 stage = (LegoS32) p_group->m_unk0x28; stage >= 0; stage--) {
		if (p_group->m_unk0x00[stage] == 0xffffffff) {
			continue;
		}
		RendererDefaultSlot* slot = NULL;
		for (LegoS16 index = 0; index < (LegoS16) p_defaults->m_unk0x04; index++) {
			if (p_defaults->m_slots[index].m_value == p_group->m_unk0x00[stage]) {
				slot = &p_defaults->m_slots[index];
				break;
			}
		}
		// Retail assumes each active stage has a descriptor in the plan.
		const LegoS32 signedKind = (LegoS8) slot->m_kind;
		const LegoU32 coordinates = ((LegoU32) (signedKind & ~0x0f) << 12) | (signedKind & 0x0f);
		const LegoU32 textureIndex = p_renderer->m_textureIndexMap[(LegoS16) slot->m_mask];
		if (p_renderer->m_cachedTextureHandles[stage] != textureIndex) {
			p_renderer->m_cachedTextureHandles[stage] = textureIndex;
			p_renderer->m_device->SetTexture(stage, p_renderer->m_textureRecords[textureIndex].m_texture);
		}
		p_renderer
			->SetCachedTextureStageState(stage, D3DTSS_ADDRESSU, p_renderer->m_textureAddressModes[slot->m_flags & 5]);
		p_renderer
			->SetCachedTextureStageState(stage, D3DTSS_ADDRESSV, p_renderer->m_textureAddressModes[slot->m_flags & 10]);
		p_renderer->SetCachedTextureStageState(stage, D3DTSS_TEXCOORDINDEX, coordinates);
		p_renderer->ApplyTextureTransform(slot, stage);
	}
	return p_group->m_unk0x2c;
}
} // namespace

// FUNCTION: LEGORACERS2 0x004e1950 FOLDED
HRESULT PassThroughStageValidationResult(HRESULT p_result)
{
	return p_result;
}

// STUB: LEGORACERS2 0x004e1e20
LegoU32 RendererTextureStageManager::ApplyStagePlanSubrecord(RendererDefaultRecord* p_defaults, LegoS32 p_index)
{
	RendererSubrecordGroup* group = FindSubrecordGroup(p_defaults->m_unk0x00);
	if (group == NULL || p_index >= (LegoS32) group->m_unk0xc0) {
		return 0;
	}
	DisplayRenderer* renderer = (DisplayRenderer*) ((BYTE*) this - offsetof(DisplayRenderer, m_textureStageManager));
	return ExecuteTextureStageSubrecord(renderer, &group->m_records[p_index], p_defaults);
}

// STUB: LEGORACERS2 0x004e2490
void __fastcall ResetTextureStageCoordinateIndices(RendererSubrecordGroup::Subrecord* p_group, void*)
{
	RendererStageRecordLink* entry = (RendererStageRecordLink*) p_group->m_unk0x20;
	for (; entry != NULL; entry = entry->m_next) {
		if (entry->m_values[0] == 1 && entry->m_values[1] == D3DTSS_TEXCOORDINDEX) {
			entry->m_values[2] = 1;
		}
	}
}
