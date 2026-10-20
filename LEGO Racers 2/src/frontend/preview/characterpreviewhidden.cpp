#include "game/character/charactervisual.h"
#include "game/objects/gameanimatedobject.h"
#include "physics/collision/collisionservice.h"

// This is the complete effect of the fixed frontend call 44f6e0(5), inlined
// here until the generic character-mode dispatcher and its racing modes are
// reconstructed. It has no separate retail-address or matching claim.
void CharacterVisual::PrepareForHiddenPreview()
{
	GameAnimatedObject* body = (GameAnimatedObject*) m_body;
	body->SetFrozen(TRUE);
	if ((m_head->m_flags & 8) != 8) {
		m_head->m_flags |= 8;
	}
	body->m_transform.m_stateFlags &= ~1;
	EnterHiddenMode();
}

// FUNCTION: LEGORACERS2 0x00450c10
void CharacterVisual::EnterHiddenMode()
{
	m_mode = 5;
	DetachCollisionProxy(((GameAnimatedObject*) m_body)->m_transform.m_collisionProxy);
}
