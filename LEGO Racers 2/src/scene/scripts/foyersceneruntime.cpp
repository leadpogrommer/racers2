#include "frontend/foyer/controllerfoyertaskmanager.h"
#include "frontend/foyer/foyerselectionhelpers.h"
#include "frontend/menu/menumanager.h"
#include "frontend/overlay/textboxfade.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/session/gamecontrollersession.h"
#include "scene/animation/foyeranimation.h"
#include "scene/effects/controllersceneobject.h"
#include "scene/scripts/foyerscene.h"
#include "scene/scripts/foyerscenecontrolleractions.h"
#include "scene/scripts/foyersceneevents.h"

#include <stddef.h>

LegoU32 __fastcall GetFoyerInputMask(SecondaryManagerMediaState* p_mediaState, void* p_unusedEdx);
void __fastcall ResetFoyerSceneSelection(GameObject*, void*);
void __fastcall ResetFoyerSceneEventResetObject(GameObject*);
void __fastcall ResetFoyerSceneEventResetTarget(void*, void*);

extern LegoU32 g_unk0x00572f94;
void __fastcall RedirectFoyerSceneMode(GameObject*, void*, LegoU32);
void __fastcall SetFoyerSceneAnimationMode(FoyerSceneResource*, void*, LegoU32);
void __fastcall BeginMenuTransition(GameObject*, void*, LegoU32);
void __fastcall SuspendFoyerControllerAction(GameObject*, void*);
void __fastcall ActivateFoyerSceneEvents(FoyerSceneResource*, void*);
LegoU32 __fastcall GetControllerTaskActionStatus(void*, void*, LegoU32);

namespace
{
struct FoyerSceneEventResetView {
	LegoU32 m_vtable;
	LegoU32 m_type;
	LegoU8 m_opaque0x08[4];
	CharBuffer24 m_animationName;
	LegoU32 m_unk0x24;
	CharBuffer24 m_secondName;
	LegoU32 m_unk0x40;
	LegoU32 m_unk0x44;
	LegoU32 m_hasPendingAnimation;
	LegoFloat m_animationElapsed;
	LegoFloat m_animationDuration;
	LegoU32 m_animationLayer;
	LegoU8 m_opaque0x58[8];
	LegoFloat m_layerValue0;
	LegoFloat m_layerValue1;
	LegoU8 m_opaque0x68[0x14];
	GameObject* m_resetObject;
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResetView, m_animationName) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResetView, m_layerValue0) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResetView, m_resetObject) == 0x7c);

struct FoyerSceneActionView {
	void* m_vtable;
	LegoU32 m_type;
	LegoFloat m_time;
};

void __fastcall UpdateFoyerSceneAction(FoyerSceneActionView* p_action, void*)
{
	((void(__fastcall*)(FoyerSceneActionView*, void*)) * (void**) p_action)(p_action, NULL);
}

} // namespace

// STUB: LEGORACERS2 0x0042b330
LegoU32 __fastcall GetFoyerInputMask(SecondaryManagerMediaState* p_state, void*)
{
	LegoU32 mask = 0;
	LegoU32 i;
	p_state->Refresh(0xffffffff);
	for (i = 0; i < 4; ++i) {
		if (i != 3 && p_state->IsSlotEnabled(i)) {
			mask |= p_state->GetSlot(i)->m_state;
		}
	}
	return mask;
}

// STUB: LEGORACERS2 0x0047c370
void __fastcall ResetFoyerSceneEvent(FoyerSceneEventResetView* p_event, void*)
{
	FoyerAnimationControllerView* animation =
		(FoyerAnimationControllerView*) *((LegoU32*) ((LegoU8*) *(LegoU32*) p_event + 0x1e4));
	ResetFoyerSceneEventResetTarget(p_event, NULL);
	StartFoyerAnimation(animation, NULL, p_event->m_animationName.m_data, 0);
	StartFoyerAnimation(animation, NULL, p_event->m_animationName.m_data, 1);
	SetFoyerAnimationLayerValue(animation, NULL, 0, *(LegoU32*) &p_event->m_layerValue0);
	SetFoyerAnimationLayerValue(animation, NULL, 1, *(LegoU32*) &p_event->m_layerValue1);
	if (p_event->m_resetObject != NULL) {
		p_event->m_resetObject->m_flags |= 4;
	}
}

// FUNCTION: LEGORACERS2 0x0047c930
void __fastcall ResetFoyerSceneEventResetTarget(void* p_event, void*)
{
	ResetFoyerSceneEventResetObject(*(GameObject**) ((LegoU8*) p_event + 0x7c));
}

// STUB: LEGORACERS2 0x0047f630
LegoU32 __fastcall UpdateFoyerSceneFade(FoyerSceneResourceView* p_view, void*, LegoFloat p_elapsedSeconds)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_view;
	MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;

	UpdateFoyerSceneEvents(p_view, NULL, p_elapsedSeconds);
	if (menu->m_unk0x208 == 0) {
		if (menu->m_unk0x210 == 0) {
			*(LegoU32*) ((LegoU8*) resource + 0xa3c) = 3;
			ResetFoyerSceneSelection((GameObject*) resource, NULL);
		}
		else {
			resource->m_unk0x58 = 2;
			*(LegoU32*) ((LegoU8*) resource + 0xa3c) = 2;
		}
		SetNpcTextManagerFade(g_gameController.m_npcTextManager, NULL, 1.0f);
	}
	return 1;
}

// STUB: LEGORACERS2 0x0047f730
void __fastcall DispatchDueFoyerSceneActions(FoyerSceneResource* p_resource, void*)
{
	if (!(*(LegoFloat*) &p_resource->m_unk0x9c4 > *(LegoFloat*) &p_resource->m_unk0x9c0)) {
		while (p_resource->m_unk0x9bc < p_resource->m_unk0x9b8) {
			FoyerSceneActionView* action = (FoyerSceneActionView*) p_resource->m_objects[p_resource->m_unk0x9bc];
			if (action->m_time > *(LegoFloat*) &p_resource->m_unk0x9c4) {
				break;
			}
			UpdateFoyerSceneAction(action, NULL);
			++p_resource->m_unk0x9bc;
		}
	}
	else {
		while (p_resource->m_unk0x9bc < p_resource->m_unk0x9b8) {
			FoyerSceneActionView* action = (FoyerSceneActionView*) p_resource->m_objects[p_resource->m_unk0x9bc++];
			UpdateFoyerSceneAction(action, NULL);
		}
		ResetFoyerSceneSelection((GameObject*) p_resource, NULL);
		return;
	}

	if (p_resource->m_unk0x9c8 != 0) {
		LegoU32 mask =
			GetFoyerInputMask(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x78, NULL) & 0x202;
		LegoU32* inputLatch = (LegoU32*) ((LegoU8*) p_resource + 0x9d0);
		LegoU32 mode = *(LegoU32*) ((LegoU8*) p_resource + 0xa3c);
		if (mask == 0) {
			if (*inputLatch == 0) {
				*inputLatch = 1;
			}
		}
		else if (*inputLatch != 0 && mode == 0) {
			*(LegoU32*) ((LegoU8*) p_resource + 0xa3c) = 1;
			ResetFoyerSceneSelection((GameObject*) p_resource, NULL);
			return;
		}
	}
}

// STUB: LEGORACERS2 0x0047fa00
void __fastcall LoadFoyerScene(FoyerSceneResourceView* p_view, void*, GameObject* p_controller, LegoU32 p_immediate)
{
	FoyerSceneResource* scene = (FoyerSceneResource*) p_view;
	LegoU8* controller = (LegoU8*) p_controller;
	if (scene->m_unk0xadc != 0) {
		RedirectFoyerSceneMode(g_gameController.m_foyerManager, NULL, scene->m_unk0xadc);
		return;
	}
	if (*(LegoU32*) (controller + 0x64) != 1) {
		return;
	}
	scene->m_unk0x58 = 2;
	*(GameObject**) ((LegoU8*) scene + 0x60) = p_controller;
	*(LegoU32*) ((LegoU8*) scene + 0x64) = *(LegoU32*) (controller + 0xdc);
	scene->m_unk0x9bc = 0;
	*(LegoU32*) ((LegoU8*) scene + 0xa10) = 0;
	*(LegoU32*) ((LegoU8*) scene + 0xa3c) = 0;
	*(LegoU32*) ((LegoU8*) scene + 0x9d0) = 0;
	SetFoyerSceneAnimationMode(scene, NULL, 0x3f800000);
	GameObject* body = *(GameObject**) (*(LegoU8**) (controller + 0x1f4) + 0x64);
	SetFoyerSceneEventTarget((FoyerSceneEventPrefix*) scene->m_eventStorage, NULL, body);
	body->m_flags |= 2;
	LegoU32 task = *(LegoU32*) ((LegoU8*) GetGameControllerSession() + *(LegoU32*) (controller + 0x58) * 0x344 + 0x5c);
	*(LegoU32*) ((LegoU8*) scene + 0x5c) = GetControllerTaskActionStatus(GetControllerTaskManager(), NULL, task);
	if (scene->m_unk0x9e8 == 0) {
		scene->m_unk0x9c4 = 0;
		ActivateFoyerSceneEvents(scene, NULL);
	}
	else {
		SetFoyerMenuSceneActive((GameObject*) g_gameController.m_unk0x88, NULL, p_controller, 0);
		*(LegoFloat*) &scene->m_unk0x9c4 = p_immediate == 0 ? -0.5f : -0.0333333351f;
		SetFoyerControllerActive(p_controller, NULL, 1);
		((void(__fastcall*)(GameObject*, void*, LegoU32)) *
		 (void**) (*(LegoU8**) p_controller + 0x74))(p_controller, NULL, 1);
		SuspendFoyerControllerAction(p_controller, NULL);
	}
	GetControllerSceneObject()->m_flags |= 8;
	BeginMenuTransition(g_gameController.m_menuManager, NULL, 1);
	ClearFoyerNpcText(g_gameController.m_npcTextManager, NULL);
	g_unk0x00572f94 = 1;
	scene->m_flags &= ~4u;
}

// STUB: LEGORACERS2 0x0047fd10
void __fastcall ActivateFoyerSceneEvents(FoyerSceneResource* p_scene, void*)
{
	FoyerSceneEventPrefix* events = (FoyerSceneEventPrefix*) p_scene->m_eventStorage;
	for (LegoS32 i = 0; i < p_scene->m_eventCount; ++i) {
		SetFoyerSceneEventReady(&events[i], NULL);
	}
	LegoU8* controller = *(LegoU8**) ((LegoU8*) p_scene + 0x60);
	LegoU32 task = *(LegoU32*) ((LegoU8*) GetGameControllerSession() + *(LegoU32*) (controller + 0x58) * 0x344 + 0x5c);
	SetControllerTaskActionStatus(GetControllerTaskManager(), NULL, task, 2);
	p_scene->m_animationValues[0] = p_scene->m_animationValues[1] = 0.0f;
	for (LegoS32 j = 0; j < 2; ++j) {
		p_scene->m_children[j]->m_flags &= ~4u;
	}
}

// STUB: LEGORACERS2 0x0047fda0
void __fastcall ResetFoyerSceneSelection(GameObject* p_object, void*)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_object;
	LegoS32 i;
	FoyerSceneEventResetView* events = (FoyerSceneEventResetView*) resource->m_eventStorage;
	for (i = resource->m_eventCount - 1; i >= 0; --i) {
		ResetFoyerSceneEvent(&events[i], NULL);
	}
	for (i = 0; i < 2; ++i) {
		resource->m_children[i]->m_flags |= 4;
	}
}

// STUB: LEGORACERS2 0x004d65a0
void __fastcall ResetFoyerSceneEventResetObject(GameObject* p_object)
{
	LegoU8* bytes = (LegoU8*) p_object;
	if (*(LegoU32*) (bytes + 0x8c) != 0 && *(LegoU32*) (bytes + 0x88) != 0) {
		LegoU32* state = *(LegoU32**) (*(LegoU8**) (bytes + 0x8c) + 0x54);
		state[3] &= ~1u;
		*(LegoU32*) (bytes + 0x9c) |= 0x20;
	}
}
