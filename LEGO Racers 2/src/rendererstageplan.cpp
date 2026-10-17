#include "rendererstageplan.h"

#include "renderertexturemanager.h"

#include <new>

// FUNCTION: LEGORACERS2 0x004e1c30
LegoU32 RendererTextureStageManager::MaskWithoutHighestPriorityStage(LegoU32 p_stageMask) const
{
	LegoU32 highestPriority = 0;
	LegoU32 selectedStage = 0;
	for (LegoU32 stage = 0; stage < sizeOfArray(m_stageRoutes); stage++) {
		if (highestPriority <= m_stageRoutes[stage].m_priority && (p_stageMask & (1 << stage)) != 0) {
			highestPriority = m_stageRoutes[stage].m_priority;
			selectedStage = stage;
		}
	}
	return p_stageMask & ~(1 << selectedStage);
}

// STUB: LEGORACERS2 0x004e24c0
RendererSubrecordGroup* RendererTextureStageManager::FindSubrecordGroup(LegoU32 p_key)
{
	LegoS8 groupIndex = (LegoS8) m_recordIndexCache[p_key];
	if (groupIndex == -1) {
		return NULL;
	}
	return &m_subrecordGroups[groupIndex];
}

// FUNCTION: LEGORACERS2 0x004e2730
RendererDefaultSlot* __fastcall CopyTextureStageDefaultSlot(
	RendererDefaultSlot* p_destination,
	void*,
	const RendererDefaultSlot* p_source
)
{
	*p_destination = *p_source;
	return p_destination;
}

// STUB: LEGORACERS2 0x004e2750
void __fastcall RemoveStageFromOperationPlan(RendererDefaultRecord* p_record, void*, LegoU32 p_stage)
{
	LegoS16 count = (LegoS16) p_record->m_unk0x04;
	for (LegoS16 index = 0; index < count; index++) {
		if (p_record->m_slots[index].m_value == p_stage) {
			for (LegoS16 next = index; next < count - 1; next++) {
				p_record->m_slots[next] = p_record->m_slots[next + 1];
			}
			p_record->m_unk0x04 = (LegoU16) (count - 1);
			break;
		}
	}
	p_record->m_unk0x00 &= ~(1 << (p_stage & 0x1f));
}

// STUB: LEGORACERS2 0x004e27c0
RendererSubrecordGroup* RendererTextureStageManager::CreateSubrecordGroup(LegoU32 p_key)
{
	LegoU32 groupIndex = m_unk0x3480;
	m_recordIndexCache[p_key] = (LegoU8) groupIndex;
	RendererSubrecordGroup* group = &m_subrecordGroups[groupIndex];
	new (group) RendererSubrecordGroup;
	m_unk0x3488 = m_unk0x3480;
	m_unk0x348c = m_unk0x3484;
	m_unk0x3480++;
	return group;
}

// FUNCTION: LEGORACERS2 0x004e6360
void RendererTextureStageManager::CopySubrecordGroupCacheEntry(LegoU32 p_destinationKey, LegoU32 p_sourceKey)
{
	m_recordIndexCache[p_destinationKey] = m_recordIndexCache[p_sourceKey];
}
