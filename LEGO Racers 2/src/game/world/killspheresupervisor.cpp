#include "game/world/killspheresupervisor.h"

#include "game/objects/gameobjectdispatch.h"

#include <new.h>
#include <string.h>

static LegoU32 __fastcall KillSphereSupervisorAllocationSize(GameObject*, void*);
static void __fastcall InitializeKillSphereSupervisor(GameObject*, void*);
static void __fastcall RemoveKillSphereSupervisor(GameObject*, void*);

// Retail table 0x00535d18, including its allocation-size callback at +0x50.
const KillSphereSupervisorDispatch g_killSphereSupervisorDispatch = {
	{
		&GameObjectBaseDestroy,
		&GameObjectBaseApplyChildTransform,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&InitializeKillSphereSupervisor,
		&GameObjectBaseZero,
		&GameObjectBaseZeroOneArgument,
		&RemoveKillSphereSupervisor,
		&GameObjectBaseZero,
		&GameObjectBaseZero,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&GameObjectBaseDrawChild,
		&GameObjectBaseNoOpOneArgument,
		&GameObjectBaseZero,
		&GameObjectBaseNoOpTwoArguments,
		&GameObjectBaseZero,
		&GameObjectBaseZeroTwoArguments,
		&GameObjectBaseUpdateRegion,
		&GameObjectBaseNoOpOneArgument,
	},
	&KillSphereSupervisorAllocationSize,
};

// FUNCTION: LEGORACERS2 0x0048e510
static LegoU32 __fastcall KillSphereSupervisorAllocationSize(GameObject*, void*)
{
	return sizeof(KillSphereSupervisor);
}

// STUB: LEGORACERS2 0x004b5460
KillSphereSupervisor::KillSphereSupervisor(GameObjectTemplate* p_template) : GameObject(0x800, p_template, NULL)
{
	m_vtable = &g_killSphereSupervisorDispatch.m_base;
}

// STUB: LEGORACERS2 0x004b5490
void KillSphereSupervisor::Initialize()
{
	memset(m_firstObjects, 0, sizeof(m_firstObjects));
	memset(m_secondObjects, 0, sizeof(m_secondObjects));
	GameObject::Initialize();
}

// STUB: LEGORACERS2 0x004b54c0
void KillSphereSupervisor::Remove()
{
	for (LegoS32 index = 0; index < 64; ++index) {
		if (m_firstObjects[index] != NULL) {
			m_firstObjects[index]->m_vtable->m_remove(m_firstObjects[index], NULL);
		}
	}
	for (LegoS32 second = 0; second < 64; ++second) {
		if (m_secondObjects[second] != NULL) {
			m_secondObjects[second]->m_vtable->m_remove(m_secondObjects[second], NULL);
		}
	}
	GameObject::Remove();
}

GameObject* __fastcall ConstructKillSphereSupervisor(void* p_storage, void*, GameObjectTemplate* p_template)
{
	return new (p_storage) KillSphereSupervisor(p_template);
}

static void __fastcall InitializeKillSphereSupervisor(GameObject* p_object, void*)
{
	((KillSphereSupervisor*) p_object)->Initialize();
}

static void __fastcall RemoveKillSphereSupervisor(GameObject* p_object, void*)
{
	((KillSphereSupervisor*) p_object)->Remove();
}
