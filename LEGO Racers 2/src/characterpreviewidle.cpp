#include "charactervisual.h"
#include "collisionservice.h"
#include "foyeranimation.h"
#include "gameanimatedobject.h"

// Complete fixed-call effects of44f6e0(6), without a claim for the generic
// dispatcher or its unneeded racing-mode branches.
void CharacterVisual::PrepareForIdlePreview()
{
	GameAnimatedObject* body = (GameAnimatedObject*) m_body;
	body->SetFrozen(FALSE);
	if ((m_head->m_flags & 8) != 0) {
		m_head->m_flags &= ~8;
		m_head->m_flags |= 0x02000000;
	}
	body->m_transform.m_stateFlags &= ~1;
	EnterIdlePreviewMode();
}

// STUB: LEGORACERS2 0x00450c30
void CharacterVisual::EnterIdlePreviewMode()
{
	m_mode = 6;
	FoyerAnimationControllerView* animation = ((GameAnimatedObject*) m_body)->m_animation;
	animation->m_enabled = 1;
	StartFoyerAnimation(animation, NULL, "IDLE_LOOP", 0);
	StartFoyerAnimation(animation, NULL, "IDLE_LOOP", 1);
	StartFoyerAnimation(animation, NULL, "IDLE_LOOP", 2);
	SetFoyerAnimationLayerValues(animation, NULL, 0, 0x3f800000, 0, 0);
	SetFoyerAnimationLayerValues(animation, NULL, 1, 0x3f800000, 0x3f800000, 0);
	SetFoyerAnimationBlend(animation, NULL, 1.0f);
	AttachCollisionProxy(((GameAnimatedObject*) m_body)->m_transform.m_collisionProxy);
	GameAnimatedObject* body = (GameAnimatedObject*) m_body;
	GameLifecycleComponent* transform = &body->m_transform;
	LegoU32 active = transform->m_vtable->m_isActive(transform, NULL, 0) != 0;
	transform->m_stateFlags = (transform->m_stateFlags & ~1) | active;
	body = (GameAnimatedObject*) m_body;
	if ((body->m_flags & 2) != 2) {
		body->m_flags |= 2;
	}
	((GameAnimatedObject*) m_body)->UpdateFacing(&m_previewOrientation.m_x);
}
