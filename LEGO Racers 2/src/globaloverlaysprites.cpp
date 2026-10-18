#include "globaloverlaymanager.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x004b2de0
void GlobalOverlaySprite::Initialize(const LegoU32* p_parameters)
{
	memcpy(&m_left, p_parameters, 21 * sizeof(LegoU32));
	m_flags = 3;
}

// STUB: LEGORACERS2 0x004b2e00
void GlobalOverlaySprite::Release()
{
	m_flags = 0;
	g_globalOverlayManager.MoveSprite(this, m_listId, -1);
}

// FUNCTION: LEGORACERS2 0x004b2e20
void GlobalOverlaySprite::SetVisible(LegoU32 p_visible)
{
	if (p_visible == 1) {
		m_flags |= 2;
	}
	else {
		m_flags &= ~2u;
	}
}

// FUNCTION: LEGORACERS2 0x004b33b0
GlobalOverlaySprite* GlobalOverlayManager::AllocateSprite(const LegoU32* p_parameters)
{
	GlobalOverlayListNode* node = m_overlayNodePool;
	if (node == NULL) {
		return NULL;
	}
	MoveSprite((GlobalOverlaySprite*) node->m_value, -1, (LegoS32) p_parameters[20]);
	GlobalOverlaySprite* sprite = (GlobalOverlaySprite*) node->m_value;
	sprite->Initialize(p_parameters);
	return sprite;
}

// STUB: LEGORACERS2 0x004b33f0
void GlobalOverlayManager::MoveSprite(GlobalOverlaySprite* p_sprite, LegoS32 p_sourceList, LegoS32 p_destinationList)
{
	GlobalOverlayListNode** source = p_sourceList == -1 ? &m_overlayNodePool : &m_overlayHeads[p_sourceList];
	GlobalOverlayListNode* node = *source;
	GlobalOverlayListNode* previous = NULL;
	LegoU32 count = 0;
	while (node != NULL) {
		if (node->m_value == p_sprite) {
			break;
		}
		++count;
		previous = node;
		node = node->m_next;
		if (count >= 300) {
			break;
		}
	}
	if (node != NULL) {
		if (previous != NULL) {
			previous->m_next = node->m_next;
		}
		else {
			*source = node->m_next;
		}
		GlobalOverlayListNode** destination =
			p_destinationList == -1 ? &m_overlayNodePool : &m_overlayHeads[p_destinationList];
		node->m_next = *destination;
		*destination = node;
	}
}
