#ifndef LR2_CONTROLLER_SCENE_OBJECT_H
#define LR2_CONTROLLER_SCENE_OBJECT_H

#include "game/objects/gamemanager.h"

struct ControllerSceneObject : GameObject {
	LegoU32 m_unk0x58;
	LegoU32 m_unk0x5c;
	LegoU32 m_active;
	LegoFloat m_elapsed;
	LegoFloat m_pulse;
	void* m_unk0x6c[3];
	void* m_sound;
	void* m_secondaryObject;
	LegoS32 m_texture;
	ControllerSceneObject();
};
DECOMP_SIZE_ASSERT(ControllerSceneObject, 0x84);

struct ControllerSceneDispatch {
	GameObject*(__fastcall* m_destroy)(GameObject*, void*, LegoU32);
	void(__fastcall* m_transform)(GameObject*, void*, LegoU32, LegoU32);
	void(__fastcall* m_action08)(GameObject*, void*);
	void(__fastcall* m_action0c)(GameObject*, void*);
	void(__fastcall* m_initialize)(GameObject*, void*);
	LegoU32(__fastcall* m_isActive)(GameObject*, void*);
	LegoU32(__fastcall* m_update)(GameObject*, void*, LegoFloat);
	void(__fastcall* m_remove)(GameObject*, void*);
	LegoU32(__fastcall* m_value20)(GameObject*, void*);
	LegoU32(__fastcall* m_value24)(GameObject*, void*);
	void(__fastcall* m_action28)(GameObject*, void*);
	void(__fastcall* m_action2c)(GameObject*, void*);
	void(__fastcall* m_draw)(GameObject*, void*);
	void(__fastcall* m_action34)(GameObject*, void*, LegoU32);
	LegoU32(__fastcall* m_value38)(GameObject*, void*);
	void(__fastcall* m_action3c)(GameObject*, void*, LegoU32, LegoU32);
	LegoU32(__fastcall* m_value40)(GameObject*, void*);
	LegoU32(__fastcall* m_value44)(GameObject*, void*, LegoU32, LegoU32);
	void(__fastcall* m_updateRegion)(GameObject*, void*, LegoU32);
	void(__fastcall* m_action4c)(GameObject*, void*, LegoU32);
};
extern const ControllerSceneDispatch g_controllerSceneObjectDispatch;
void __cdecl InitializeControllerSceneAngularFrequency();
void __cdecl InitializeControllerSceneSoundTemplate();
void __cdecl InitializeControllerSceneObjectTemplate();
GameObject* __cdecl GetControllerSceneObject();
void __cdecl ResetControllerInputState();
void __fastcall InitializeControllerSceneObject(GameObject*, void*);
LegoU32 __fastcall UpdateControllerSceneObject(GameObject*, void*, LegoFloat);
void __fastcall RemoveControllerSceneObject(GameObject*, void*);
void __fastcall SetFoyerSceneInputAnimationState(GameObject*, void*, LegoU32);

#endif
