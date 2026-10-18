#include "globaloverlaymanager.h"

#include <string.h>

// STUB: LEGORACERS2 0x004b30c0
GlobalOverlayManager::GlobalOverlayManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	LegoU8* pool = (LegoU8*) this + 0x58;
	LegoU32 index;
	for (index = 0; index < 300; ++index) {
		LegoU8* item = pool + index * 0x58;
		*(LegoU32*) (item + 0x00) = 0;
		memset(item + 0x04, 0, 8);
		*(LegoU32*) (item + 0x0c) = 0;
		*(LegoU32*) (item + 0x10) = 0;
		*(LegoU32*) (item + 0x14) = 0x3f800000;
		*(LegoU32*) (item + 0x18) = 0x3f800000;
		*(LegoU32*) (item + 0x2c) = 0xffffffff;
		*(LegoU32*) (item + 0x30) = 0;
		memset(item + 0x34, 0, 8);
		memset(item + 0x44, 0, 0x14);
	}

	for (index = 0; index < 200; ++index) {
		GlobalOverlayRecord* record = &m_records[index];
		record->m_vtable = &g_globalOverlayRecordDispatch;
		record->m_text[0] = 0;
		record->m_flags = 0;
		record->m_font = NULL;
		record->m_color = 0xffffffff;
		record->m_positionX = 0.0f;
		record->m_positionY = 0.0f;
		record->m_clipLeft = 0.0f;
		record->m_clipTop = 0.0f;
		record->m_clipRight = 1.0f;
		record->m_clipBottom = 1.0f;
		record->m_animationFlags = 0;
		*(LegoU32*) ((LegoU8*) record + 0x22c) = 0;
		record->m_scale = 1.0f;
		record->m_elapsed = 0.0f;
		record->m_type = 0;
		record->m_animationTime = 0.0f;
		*(LegoU32*) ((LegoU8*) record + 0x250) = 0;
	}

	for (index = 0; index < 300; ++index) {
		m_overlayNodes[index].m_value = pool + index * 0x58;
		m_overlayNodes[index].m_next = index + 1 < 300 ? &m_overlayNodes[index + 1] : NULL;
	}
	*(LegoU32*) ((LegoU8*) this + 0x70d4) = 0;
	m_overlayNodePool = m_overlayNodes;
	for (index = 0; index < 16; ++index) {
		m_overlayHeads[index] = NULL;
	}

	for (index = 0; index < 200; ++index) {
		m_recordNodes[index].m_value = &m_records[index];
		m_recordNodes[index].m_next = index + 1 < 200 ? &m_recordNodes[index + 1] : NULL;
	}
	*(LegoU32*) ((LegoU8*) this + 0x248f8) = 0;
	m_recordNodeRoot = m_recordNodes;
	for (index = 0; index < 16; ++index) {
		m_recordHeads[index] = NULL;
	}
	m_recordListCount = 0;
	m_unk0x24944 = 1;
	m_localizedStrings = NULL;
	m_unk0x2494c = 0;
	m_vtable = &g_globalOverlayRecordDispatch.m_managerDispatch;
}
