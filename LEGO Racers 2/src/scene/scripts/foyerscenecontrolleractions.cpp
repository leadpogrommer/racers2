#include "scene/scripts/foyerscenecontrolleractions.h"

#include "game/character/charactervisual.h"
#include "game/character/charactervisualbehavior.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameanimatedobject.h"
#include "physics/collision/collisionservice.h"
#include "scene/animation/foyeranimation.h"

void __fastcall ApplyFoyerControllerSceneAction(GameObject*, void*, GameObject*, GameObject*);

// Retail helpers called from the recovered controller actions (44e00/44e40).
void __fastcall AddFoyerControllerPreviewAngle(GameObject* p_object, void*, LegoFloat p_angle)
{
	((GameAnimatedObject*) p_object)->m_facing.m_y += p_angle;
}

// Retail helper called from the recovered controller actions (44e40).
void __fastcall ResetFoyerControllerPreviewObject(GameObject* p_object, void*)
{
	LegoU8* bytes = (LegoU8*) p_object;
	LegoU32* flags = (LegoU32*) (bytes + 8);
	if ((*flags & 8) != 0) {
		*flags = (*flags & ~8u) | 0x02000000;
	}
	*flags &= ~6u;
	DetachCollisionProxy(*(CollisionProxy**) (bytes + 0x60));
}

// STUB: LEGORACERS2 0x004330c0
void __fastcall SetFoyerMenuSceneActive(GameObject* p_menu, void*, GameObject* p_target, LegoU32 p_enabled)
{
	LegoU32 count = *(LegoU32*) ((LegoU8*) p_menu + 0x60);
	for (LegoU32 i = 0; i < count; ++i) {
		GameObject* entry = *(GameObject**) ((LegoU8*) p_menu + 0x58 + i * 4);
		if (*(LegoU32*) ((LegoU8*) entry + 0x64) != (LegoU32) p_target) {
			continue;
		}
		GameObject* state = *(GameObject**) ((LegoU8*) entry + 0x1a0);
		if (state == NULL) {
			return;
		}
		LegoU32* flags = (LegoU32*) ((LegoU8*) state + 8);
		LegoU32 bit = (*flags >> 3) & 1;
		LegoU32 desired = p_enabled == 0 ? 1 : 0;
		if (bit != desired) {
			*flags = (*flags & ~8u) | (desired << 3);
			if (desired == 0) {
				*flags |= 0x02000000;
			}
		}
		return;
	}
}

// STUB: LEGORACERS2 0x00447990
void __fastcall SetFoyerControllerRunMenu(GameObject* p_controller)
{
	GameObject* previewObject = *(GameObject**) ((LegoU8*) p_controller + 0x1f8);
	CharacterVisual* visual = (CharacterVisual*) *(LegoU32*) ((LegoU8*) p_controller + 0x1f4);
	AddFoyerControllerPreviewAngle(previewObject, NULL, 1.0f);
	ResetFoyerControllerPreviewObject(previewObject, NULL);
	SetCharacterVisualMode(visual, NULL, 1);
	ApplyFoyerControllerSceneAction(
		p_controller,
		NULL,
		*(GameObject**) ((LegoU8*) p_controller + 0x1e8),
		previewObject
	);
	*(LegoU32*) ((LegoU8*) p_controller + 0xdc) = 1;
	if (*(LegoU32*) ((LegoU8*) p_controller + 0xa8) != 0) {
		*(LegoU32*) ((LegoU8*) *(LegoU32*) ((LegoU8*) p_controller + 0xa8) + 0x180) =
			*(LegoU32*) ((LegoU8*) p_controller + 0x1f8);
	}
}

// STUB: LEGORACERS2 0x00447a00
void __fastcall SetFoyerControllerMenuLoop(GameObject* p_controller, void*)
{
	CharacterVisual* visual = (CharacterVisual*) *(LegoU32*) ((LegoU8*) p_controller + 0x1f4);
	SetCharacterVisualMode(visual, NULL, 6);
	*(LegoU32*) ((LegoU8*) p_controller + 0xdc) = 4;
	ApplyFoyerControllerSceneAction(
		p_controller,
		NULL,
		*(GameObject**) ((LegoU8*) p_controller + 0x1e8),
		visual->m_body
	);
}

// STUB: LEGORACERS2 0x004496d0
void __fastcall ApplyFoyerControllerSceneAction(
	GameObject* p_controller,
	void*,
	GameObject* p_sceneState,
	GameObject* p_target
)
{
	if (g_gameController.m_state != 3) {
		GameObject* match = NULL;
		GameObject** entries = (GameObject**) ((LegoU8*) p_sceneState + 0x88c);
		for (LegoU32 i = 0; i < 32; ++i) {
			if (entries[i] != NULL && *(GameObject**) ((LegoU8*) entries[i] + 0x58) == p_target) {
				match = entries[i];
				break;
			}
		}
		if (match != NULL) {
			*(GameObject**) ((LegoU8*) match + 0x58) = p_target;
			for (LegoU32 offset = 0x74; offset < 0x1f4; offset += 12) {
				LegoU32* values = (LegoU32*) ((LegoU8*) match + offset);
				values[0] = values[1] = values[2] = 0;
			}
			*(LegoU32*) ((LegoU8*) match + 0x1f4) = 0;
		}
		if (g_gameController.m_state == 4 && p_target != NULL) {
			LegoU32 count = *(LegoU32*) ((LegoU8*) p_sceneState + 0x138);
			for (LegoU32 i = 0; i < count; ++i) {
				LegoU8* record = (LegoU8*) p_sceneState + 0x140 + i * 0x44;
				if (*(GameObject**) record != p_target) {
					continue;
				}
				*(GameObject**) record = p_target;
				void** objectVtable = *(void***) p_target;
				LegoFloat* position =
					((LegoFloat * (__fastcall*) (GameObject*, void*) ) objectVtable[0x14 / 4])(p_target, NULL);
				((LegoFloat*) (record + 4))[0] = position[0];
				((LegoFloat*) (record + 4))[1] = position[1];
				((LegoFloat*) (record + 4))[2] = position[2];
				break;
			}
		}
	}
	*(GameObject**) ((LegoU8*) p_controller + 0x1e8) = p_target;
}

// FUNCTION: LEGORACERS2 0x0044a4c0
void __fastcall SuspendFoyerControllerAction(GameObject* p_controller, void*)
{
	LegoU8* bytes = (LegoU8*) p_controller;
	LegoU32 previous = *(LegoU32*) (bytes + 0xdc);
	*(LegoU32*) (bytes + 0xdc) = 5;
	*(LegoU32*) (bytes + 0xe0) = previous;
}

// FUNCTION: LEGORACERS2 0x0044a4e0
void __fastcall RestoreFoyerControllerActionState(GameObject* p_controller, void*)
{
	*(LegoU32*) ((LegoU8*) p_controller + 0xdc) = *(LegoU32*) ((LegoU8*) p_controller + 0xe0);
}

// STUB: LEGORACERS2 0x0044a550
void __fastcall SetFoyerControllerActive(GameObject* p_controller, void*, LegoU32 p_active)
{
	LegoU32* flags = (LegoU32*) ((LegoU8*) p_controller + 0xd8);
	if (p_active == 0) {
		*flags &= ~1u;
	}
	else {
		*flags |= 1u;
	}
	// Retail calls 0x00489970 here; its body is a verified RET 8.
}

// STUB: LEGORACERS2 0x00480340
void __fastcall StartFoyerControllerRunLoop(GameObject* p_target)
{
	LegoU8* controller = (LegoU8*) *(LegoU32*) ((LegoU8*) p_target + 0x1f4);
	LegoU8* nested = (LegoU8*) *(LegoU32*) (controller + 0x64);
	FoyerAnimationControllerView* animation = (FoyerAnimationControllerView*) *(LegoU32*) (nested + 0x1e4);
	StartFoyerAnimation(animation, NULL, "RUN_LOOP", 1);
}
