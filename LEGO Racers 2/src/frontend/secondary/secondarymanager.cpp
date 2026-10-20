#include "frontend/secondary/secondarymanager.h"

#include "game/objects/gameobjectdispatch.h"
#include "memory/memoryheap.h"

static GameObject* __fastcall SecondaryManagerDestroyBridge(GameObject* p_object, void*, LegoU32 p_flags);
static void __fastcall SecondaryManagerInitializeBridge(GameObject* p_object, void*);
static LegoU32 __fastcall SecondaryManagerUpdateBridge(GameObject* p_object, void*, LegoU32 p_frameParameter);
static void __fastcall SecondaryManagerRemoveBridge(GameObject* p_object, void*);
static LegoU32 __fastcall SecondaryManagerAllocationSizeBridge(GameObject*, void*);

const SecondaryManagerDispatchView g_secondaryManagerDispatch = {
	{
		&SecondaryManagerDestroyBridge,
		&GameObjectBaseApplyChildTransform,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&SecondaryManagerInitializeBridge,
		&GameObjectBaseZero,
		&SecondaryManagerUpdateBridge,
		&SecondaryManagerRemoveBridge,
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
	&SecondaryManagerAllocationSizeBridge,
};

// STUB: LEGORACERS2 0x00426f20
SecondaryManager::SecondaryManager(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, NULL), m_state(9), m_unk0x5c(0), m_requestedState(-1), m_unk0x64(0),
	  m_unk0x74(NULL), m_unk0x78(NULL), m_unk0x7c(NULL), m_startupPath(), m_unk0x12c(0), m_unk0x130(-1),
	  m_unk0x134(NULL), m_unk0x138(-1), m_unk0x154(), m_unk0x17c(NULL), m_unk0x23c(0), m_unk0x240(1), m_unk0x244(0),
	  m_unk0x248(0), m_unk0x24c(0), m_unk0x250(0), m_unk0x258(0), m_unk0x25c(0), m_unk0x260(0), m_unk0x4bc(0),
	  m_unk0x4c0(0), m_unk0x4cc(2), m_unk0x4d0(0)
{
	LegoU32 i;
	for (i = 0; i < 3; ++i) {
		m_resourcePointers[i] = 0;
		m_resourceIds[i] = -1;
		m_moreResourcePointers[i] = NULL;
		m_moreResourceIds[i] = -1;
	}
	for (i = 0; i < 10; ++i) {
		m_unk0x154[i] = 0;
	}
	for (i = 0; i < 0x2e; ++i) {
		m_secondaryData[i] = NULL;
	}
	for (i = 0; i < 9; ++i) {
		m_unk0x4d4[i] = 0;
	}
	m_vtable = (const GameObjectDispatchView*) &g_secondaryManagerDispatch;
}

// STUB: LEGORACERS2 0x00427060
static LegoU32 __fastcall SecondaryManagerAllocationSizeBridge(GameObject*, void*)
{
	return 0x4f8;
}

// STUB: LEGORACERS2 0x00427070
static GameObject* __fastcall SecondaryManagerDestroyBridge(GameObject* p_object, void*, LegoU32 p_flags)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	manager->~SecondaryManager();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(manager);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00427090
SecondaryManager::~SecondaryManager()
{
	m_vtable = (const GameObjectDispatchView*) &g_secondaryManagerDispatch;
}

// The retail virtual dispatch slot enters Initialize at 0x004270a0.
static void __fastcall SecondaryManagerInitializeBridge(GameObject* p_object, void*)
{
	((SecondaryManager*) p_object)->Initialize();
}

// The retail virtual dispatch slot enters Update at 0x00427a00.
static LegoU32 __fastcall SecondaryManagerUpdateBridge(GameObject* p_object, void*, LegoU32 p_frameParameter)
{
	return ((SecondaryManager*) p_object)->Update(p_frameParameter);
}

// The retail virtual dispatch slot enters Remove at 0x00427480.
static void __fastcall SecondaryManagerRemoveBridge(GameObject* p_object, void*)
{
	((SecondaryManager*) p_object)->Remove();
}
