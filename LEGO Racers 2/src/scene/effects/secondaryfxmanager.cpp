#include "scene/effects/secondaryfxmanager.h"

#include "frontend/overlay/globaloverlaymanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "memory/memoryheap.h"

#include <new.h>

GameObject* __fastcall SecondaryFxManagerDestroy(GameObject*, void*, LegoU32);
void __fastcall SecondaryFxManagerInitialize(GameObject*, void*);
void __fastcall SecondaryFxManagerRemove(GameObject*, void*);
LegoU32 __fastcall SecondaryFxManagerUpdate(GameObject*, void*, LegoU32);
void __fastcall SecondaryFxManagerDraw(GameObject*, void*);
void __fastcall InitializeGameControllerWord(LegoU32*, void*);

// Explicit ABI table checked by check_scene_dispatch.py.
// GLOBAL: LEGORACERS2 0x00533024
extern const GameObjectDispatchView g_secondaryFxManagerDispatch = {
	&SecondaryFxManagerDestroy,    &GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,           &GameObjectBaseNoOp,
	&SecondaryFxManagerInitialize, &GameObjectBaseZero,
	&SecondaryFxManagerUpdate,     &SecondaryFxManagerRemove,
	&GameObjectBaseZero,           &GameObjectBaseZero,
	&GameObjectBaseNoOp,           &GameObjectBaseNoOp,
	&SecondaryFxManagerDraw,       &GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,           &GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,           &GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,   &GameObjectBaseNoOpOneArgument,
};

// STUB: LEGORACERS2 0x00478bf0
void SecondaryFxState::EnsureSprite()
{
	if (m_sprite == NULL) {
		LegoU32 parameters[21];
		parameters[0] = parameters[1] = 0;
		parameters[2] = parameters[3] = 0x3f800000;
		parameters[4] = parameters[5] = 0;
		parameters[6] = parameters[7] = 0x3f800000;
		parameters[10] = 0xff000000;
		parameters[11] = 0xffffffff;
		parameters[12] = parameters[15] = 0;
		parameters[13] = parameters[14] = 0x3f800000;
		parameters[16] = parameters[17] = 0;
		parameters[18] = 7;
		parameters[19] = 0;
		parameters[20] = 10;
		m_sprite = g_globalOverlayManager.AllocateSprite(parameters);
		m_sprite->SetVisible(0);
	}
}

// FUNCTION: LEGORACERS2 0x00478d00
void SecondaryFxState::ReleaseSprite()
{
	if (m_sprite != NULL) {
		m_sprite->Release();
		m_sprite = NULL;
	}
}

// STUB: LEGORACERS2 0x00478d20
void SecondaryFxState::Update(LegoFloat p_delta)
{
	LegoFloat step = p_delta * m_speed;
	if (m_mode == 3) {
		LegoFloat elapsed = step + m_elapsed;
		m_elapsed = elapsed;
		if (elapsed > m_duration) {
			m_mode = 0;
			m_sprite->SetAlpha(255);
		}
		else {
			m_sprite->SetAlpha((LegoU8) (LegoS32) (elapsed * m_inverseDuration * 255.0f));
		}
	}
	else if (m_mode == 2) {
		LegoFloat elapsed = step + m_elapsed;
		m_elapsed = elapsed;
		if (elapsed > m_duration) {
			m_mode = 0;
			m_sprite->SetVisible(0);
		}
		else {
			m_sprite->SetAlpha((LegoU8) (255 - (LegoU8) (LegoS32) (elapsed * m_inverseDuration * 255.0f)));
		}
	}
}

void __fastcall ResetSecondaryFxController(GameObject* p_state, void*, LegoU32 p_mode, LegoU32 p_duration)
{
	((SecondaryFxState*) p_state)->SetMode(p_mode, *(LegoFloat*) &p_duration);
}

// STUB: LEGORACERS2 0x00478dd0
void SecondaryFxState::SetMode(LegoU32 p_mode, LegoFloat p_duration)
{
	switch (p_mode) {
	case 0:
		ReleaseSprite();
		m_mode = 0;
		break;
	case 1:
		EnsureSprite();
		m_sprite->SetVisible(1);
		m_sprite->SetAlpha(255);
		m_mode = 0;
		break;
	case 2:
	case 3:
		EnsureSprite();
		m_sprite->SetVisible(1);
		m_sprite->SetAlpha(p_mode == 2 ? 255 : 0);
		m_mode = p_mode;
		m_elapsed = 0.0f;
		m_duration = p_duration == 0.0f ? 0.75f : p_duration;
		m_inverseDuration = 1.0f / m_duration;
		break;
	}
}

GameObject* __fastcall ConstructSecondaryFxManager(void* p_storage, void*, GameObjectTemplate* p_template)
{
	return new (p_storage) SecondaryFxManager(p_template);
}

// STUB: LEGORACERS2 0x00478f30
SecondaryFxManager::SecondaryFxManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_vtable = &g_secondaryFxManagerDispatch;
}

// FUNCTION: LEGORACERS2 0x00478f60
GameObject* __fastcall SecondaryFxManagerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	((SecondaryFxManager*) p_object)->~SecondaryFxManager();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// FUNCTION: LEGORACERS2 0x00478f80
SecondaryFxManager::~SecondaryFxManager()
{
	m_vtable = &g_secondaryFxManagerDispatch;
}

void __fastcall SecondaryFxManagerInitialize(GameObject* p_object, void*)
{
	((SecondaryFxManager*) p_object)->Initialize();
}

// STUB: LEGORACERS2 0x00478f90
void SecondaryFxManager::Initialize()
{
	GameObject::Initialize();
	m_state = (SecondaryFxState*) AllocateHeapMemory(sizeof(SecondaryFxState));
	if (m_state != NULL) {
		m_state->m_mode = 0;
		m_state->m_sprite = NULL;
		m_state->m_speed = 1.0f;
	}
	InitializeGameControllerWord((LegoU32*) m_state, NULL);
}

void __fastcall SecondaryFxManagerRemove(GameObject* p_object, void*)
{
	((SecondaryFxManager*) p_object)->Remove();
}

// STUB: LEGORACERS2 0x00478fe0
void SecondaryFxManager::Remove()
{
	m_state->ReleaseSprite();
	if (m_state != NULL) {
		FreeHeapMemory(m_state);
	}
	GameObject::Remove();
}

// FUNCTION: LEGORACERS2 0x00479010
LegoU32 __fastcall SecondaryFxManagerUpdate(GameObject* p_object, void*, LegoU32 p_delta)
{
	((SecondaryFxManager*) p_object)->m_state->Update(*(LegoFloat*) &p_delta);
	return 1;
}

// FUNCTION: LEGORACERS2 0x00479030 FOLDED
void __fastcall SecondaryFxManagerDraw(GameObject*, void*)
{
}
