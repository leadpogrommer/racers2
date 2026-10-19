#ifndef LR2_KILLSPHERESUPERVISOR_H
#define LR2_KILLSPHERESUPERVISOR_H

#include "gamemanager.h"

struct KillSphereSupervisor : GameObject {
	GameObject* m_firstObjects[64];  // 0x58
	GameObject* m_secondObjects[64]; // 0x158

	KillSphereSupervisor(GameObjectTemplate* p_template);
	void Initialize();
	void Remove();
};
DECOMP_SIZE_ASSERT(KillSphereSupervisor, 0x258);
DECOMP_STATIC_ASSERT(offsetof(KillSphereSupervisor, m_firstObjects) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(KillSphereSupervisor, m_secondObjects) == 0x158);

struct KillSphereSupervisorDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_getAllocationSize)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(KillSphereSupervisorDispatch, 0x54);

extern const KillSphereSupervisorDispatch g_killSphereSupervisorDispatch;
GameObject* __fastcall ConstructKillSphereSupervisor(void*, void*, GameObjectTemplate*);

#endif
