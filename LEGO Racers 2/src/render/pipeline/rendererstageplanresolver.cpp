#include "render/pipeline/rendererstageplanresolver.h"

#include "render/core/renderer.h"
#include "render/core/rendererdispatch.h"
#include "render/pipeline/rendererstagebuilders.h"
#include "render/pipeline/renderertexturemanager.h"

// STUB: LEGORACERS2 0x004e24f0
void __fastcall ResolveRendererTextureStagePlan(DisplayRenderer* p_renderer, void*, RendererStagePlanRequest* p_request)
{
	RendererDefaultRecord* defaults = &p_request->m_defaults;
	const LegoU32 originalMask = defaults->m_unk0x00;
	RendererTextureStageManager* manager = &p_renderer->m_textureStageManager;

	for (LegoS16 initialIndex = 0; initialIndex < (LegoS16) defaults->m_unk0x04; initialIndex++) {
		p_renderer->m_vtable->m_updateTextureStageFlags(p_renderer, NULL, &defaults->m_slots[initialIndex], 0);
	}

	if (manager->FindSubrecordGroup(originalMask) != NULL) {
		return;
	}

	if (p_renderer->m_unk0x4d0 < 2) {
		for (LegoS16 stageIndex = 0; stageIndex < (LegoS16) defaults->m_unk0x04; stageIndex++) {
			const LegoU32 stage = defaults->m_slots[stageIndex].m_value;
			if ((originalMask & 0xfffffdff & (1 << (stage & 0x1f))) == 0) {
				RemoveStageFromOperationPlan(defaults, NULL, stage);
			}
		}

		LegoU32 filteredMask = defaults->m_unk0x00 & originalMask & 0xfffffdff;
		defaults->m_unk0x00 = filteredMask;
		filteredMask &= 0xfffffffd;
		for (LegoS16 filteredIndex = 0; filteredIndex < (LegoS16) defaults->m_unk0x04; filteredIndex++) {
			const LegoU32 stage = defaults->m_slots[filteredIndex].m_value;
			if ((filteredMask & (1 << (stage & 0x1f))) == 0) {
				RemoveStageFromOperationPlan(defaults, NULL, stage);
			}
		}
		defaults->m_unk0x00 &= filteredMask;
	}

	for (;;) {
		if (manager->BuildDefaultStagePlan(p_request, p_renderer->m_nvidiaWorkaround) == 1) {
			manager->CopySubrecordGroupCacheEntry(originalMask, defaults->m_unk0x00);
			return;
		}
		if (manager->BuildProfileStagePlan(p_request, p_renderer->m_nvidiaWorkaround) == 1) {
			manager->CopySubrecordGroupCacheEntry(originalMask, defaults->m_unk0x00);
			return;
		}

		const LegoU32 priorMask = defaults->m_unk0x00;
		const LegoU32 supportedMask = manager->MaskWithoutHighestPriorityStage(priorMask);
		LegoS16 index = 0;
		while (index < (LegoS16) defaults->m_unk0x04) {
			const LegoU32 stage = defaults->m_slots[index].m_value;
			if ((supportedMask & (1 << (stage & 0x1f))) != 0) {
				index++;
				continue;
			}

			for (LegoS16 find = 0; find < (LegoS16) defaults->m_unk0x04; find++) {
				if (defaults->m_slots[find].m_value != stage) {
					continue;
				}
				for (LegoS16 destination = find; destination < (LegoS16) defaults->m_unk0x04 - 1; destination++) {
					CopyTextureStageDefaultSlot(
						&defaults->m_slots[destination],
						NULL,
						&defaults->m_slots[destination + 1]
					);
				}
				defaults->m_unk0x04--;
				break;
			}
			defaults->m_unk0x00 &= ~(1 << (stage & 0x1f));
			index++;
		}

		defaults->m_unk0x00 &= supportedMask;
		if (manager->FindSubrecordGroup(defaults->m_unk0x00) != NULL) {
			manager->CopySubrecordGroupCacheEntry(originalMask, defaults->m_unk0x00);
			return;
		}
		if (defaults->m_unk0x00 == priorMask) {
			return;
		}
	}
}
