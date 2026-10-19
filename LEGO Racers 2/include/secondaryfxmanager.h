#ifndef LR2_SECONDARYFXMANAGER_H
#define LR2_SECONDARYFXMANAGER_H

#include "gamemanager.h"

struct GlobalOverlaySprite;

struct SecondaryFxState {
	LegoU32 m_mode;
	GlobalOverlaySprite* m_sprite;
	LegoFloat m_elapsed;
	LegoFloat m_duration;
	LegoFloat m_inverseDuration;
	LegoFloat m_speed;
	void EnsureSprite();
	void ReleaseSprite();
	void Update(LegoFloat p_delta);
	void SetMode(LegoU32 p_mode, LegoFloat p_duration);
};
DECOMP_SIZE_ASSERT(SecondaryFxState, 0x18);

struct SecondaryFxManager : GameObject {
	SecondaryFxManager(GameObjectTemplate* p_template);
	~SecondaryFxManager();
	void Initialize();
	void Remove();
	SecondaryFxState* m_state;
};
DECOMP_SIZE_ASSERT(SecondaryFxManager, 0x5c);
DECOMP_STATIC_ASSERT(offsetof(SecondaryFxManager, m_state) == 0x58);

extern const GameObjectDispatchView g_secondaryFxManagerDispatch;
void __fastcall SecondaryFxManagerDraw(GameObject*, void*);
GameObject* __fastcall ConstructSecondaryFxManager(void*, void*, GameObjectTemplate*);
// Existing controller call sites pass the nested state in ECX.
void __fastcall ResetSecondaryFxController(GameObject*, void*, LegoU32, LegoU32);

#endif
