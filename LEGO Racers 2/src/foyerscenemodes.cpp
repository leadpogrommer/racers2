#include "controllersceneobject.h"
#include "foyeranimation.h"
#include "foyerscene.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gametemplateinstance.h"
#include "globaloverlaymanager.h"
#include "secondaryfxmanager.h"
#include "secondarymanager.h"
#include "textboxfade.h"

LegoU32 __fastcall GetFoyerInputMask(SecondaryManagerMediaState*, void*);
void* __cdecl GetControllerTaskManager();
void __fastcall SetFoyerSceneAnimationMode(FoyerSceneResource*, void*, LegoU32 p_modeBits);

// STUB: LEGORACERS2 0x0045bca0
void __fastcall SetControllerTaskManagerModeValue(void* p_manager, void*, LegoU32 p_index, LegoU32 p_value)
{
	LegoU8* state = *(LegoU8**) ((LegoU8*) p_manager + 0x58);
	*(LegoU32*) (state + p_index * 0x100 + 0x90) = p_value;
}

// STUB: LEGORACERS2 0x0047e770
void __fastcall SetFoyerSceneInputAnimationState(GameObject* p_object, void*, LegoU32 p_enabled)
{
	LegoU8* bytes = (LegoU8*) p_object;
	*(LegoU32*) (bytes + 0x60) = p_enabled;
	for (LegoU32 i = 0; i < 3; ++i) {
		((GlobalOverlaySprite*) *(void**) (bytes + 0x6c + i * 4))->SetVisible(p_enabled);
	}
	GameObject* nested = *(GameObject**) (bytes + 0x78);
	void** vtable = *(void***) nested;
	((void(__fastcall*)(GameObject*, void*)) vtable[(p_enabled != 0 ? 0x34 : 0x38) / 4])(nested, NULL);
}

// STUB: LEGORACERS2 0x0047fe60
void __fastcall UpdateFoyerSceneInputMode(FoyerSceneResource* p_resource, void*, LegoFloat* p_elapsedSeconds)
{
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	LegoU32 pressed = GetFoyerInputMask(secondary->m_unk0x78, NULL) & 1;
	LegoU32* state = (LegoU32*) ((LegoU8*) p_resource + 0xa10);
	LegoFloat* elapsed = (LegoFloat*) ((LegoU8*) p_resource + 0xa18);

	if (*state == 0) {
		if (pressed != 0) {
			*state = 1;
			*elapsed = 0.0f;
		}
		return;
	}

	if (*state == 1) {
		if (pressed == 0) {
			*state = 0;
			return;
		}
		*elapsed += *p_elapsedSeconds;
		if (*elapsed > 0.25f) {
			*state = 2;
			*elapsed = 0.0f;
			SetFoyerSceneInputAnimationState((GameObject*) GetControllerSceneObject(), NULL, 1);
		}
		return;
	}

	if (*state == 2) {
		if (pressed == 0) {
			*state = 0;
			SetFoyerSceneInputAnimationState((GameObject*) GetControllerSceneObject(), NULL, 0);
			SetFoyerSceneAnimationMode(p_resource, NULL, 0x3f800000);
			return;
		}

		*elapsed += *p_elapsedSeconds;
		LegoFloat fade;
		if (*elapsed >= 1.25f) {
			fade = 3.0f;
		}
		else {
			fade = (3.0f - 1.0f) * (*elapsed / 1.25f) + 1.0f;
		}
		*p_elapsedSeconds *= fade;
		SetFoyerSceneAnimationMode(p_resource, NULL, *(LegoU32*) &fade);
	}
}

// STUB: LEGORACERS2 0x0047ff90
void __fastcall SetFoyerSceneAnimationMode(FoyerSceneResource* p_resource, void*, LegoU32 p_modeBits)
{
	LegoU8* controllerMode = *(LegoU8**) ((LegoU8*) p_resource + 0x60);
	LegoU32 modeIndex = *(LegoU32*) (controllerMode + 0x58);
	LegoU32 recordIndex = *(LegoU32*) ((LegoU8*) g_gameControllerSession + modeIndex * 0x344 + 0x5c);
	LegoU8* taskManager = (LegoU8*) GetControllerTaskManager();
	LegoU32 i;
	LegoU8* inputState = (LegoU8*) p_resource + 0x74;
	GameObject* sceneObject;

	p_resource->m_animationMode = p_modeBits;
	SetControllerTaskManagerModeValue(taskManager, NULL, recordIndex, p_modeBits);
	g_foyerAnimationBlend = *(LegoFloat*) &p_modeBits;
	for (i = 0; (LegoS32) i < (LegoS32) p_resource->m_unk0x5b4; ++i) {
		void* target = *(void**) (inputState + i * 0xa8 + 0xa4);
		void** vtable = *(void***) target;
		((void(__fastcall*)(void*, void*, LegoU32)) vtable[0x1c / 4])(target, NULL, p_modeBits);
	}

	sceneObject = (GameObject*) GetControllerSceneObject();
	void* nested = *(void**) ((LegoU8*) sceneObject + 0x78);
	void** nestedVtable = *(void***) nested;
	((void(__fastcall*)(void*, void*, LegoU32)) nestedVtable[0x1c / 4])(nested, NULL, p_modeBits);
	SetNpcTextManagerFade(g_gameController.m_npcTextManager, NULL, *(LegoFloat*) &p_modeBits);

	for (i = 0; i < 2; ++i) {
		GameTemplateInstance* child = (GameTemplateInstance*) p_resource->m_children[i];
		*(LegoFloat*) &child->m_unk0x94 = p_resource->m_animationValues[i] * *(LegoFloat*) &p_modeBits;
	}
	((SecondaryFxManager*) g_gameController.m_secondaryFxManager)->m_state->m_speed = *(LegoFloat*) &p_modeBits;
}
