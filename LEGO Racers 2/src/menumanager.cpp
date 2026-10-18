#include "menumanager.h"

#include "gameobjectdispatch.h"
#include "globaloverlaymanager.h"
#include "memoryheap.h"

static GameObject* __fastcall MenuManagerDestroyBridge(GameObject* p_object, void*, LegoU32 p_flags);
static void __fastcall MenuManagerInitializeBridge(GameObject* p_object, void*);
static void __fastcall MenuManagerRemoveBridge(GameObject* p_object, void*);
static LegoU32 __fastcall MenuManagerAllocationSizeBridge(GameObject*, void*);

const MenuManagerDispatchView g_menuManagerDispatch = {
	{
		&MenuManagerDestroyBridge,
		&GameObjectBaseApplyChildTransform,
		&GameObjectBaseNoOp,
		&GameObjectBaseNoOp,
		&MenuManagerInitializeBridge,
		&GameObjectBaseZero,
		&GameObjectBaseZeroOneArgument,
		&MenuManagerRemoveBridge,
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
	&MenuManagerAllocationSizeBridge,
};

// STUB: LEGORACERS2 0x0041c310
MenuManager::MenuManager(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, NULL), m_unk0x198(0), m_unk0x19c(0xffffffff), m_unk0x1a0(0xffffffff), m_unk0x1a4(0),
	  m_unk0x1a8(0), m_unk0x1d8(0), m_unk0x1dc(0), m_unk0x1e0(0), m_unk0x1e8(0), m_unk0x1ec(0), m_unk0x204(0),
	  m_unk0x208(0), m_extraChoiceSelector(NULL), m_unk0x210(0), m_unk0x214(0xffffffff), m_unk0x218(3), m_unk0x21c(0),
	  m_unk0x220(0xbf800000), m_unk0x224(0xbf800000), m_unk0x228(0xffffffff), m_unk0x22c(0), m_transitionCover(NULL)
{
	LegoU32 i;
	for (i = 0; i < 14; ++i) {
		m_overlayRecords[i] = NULL;
	}
	for (i = 0; i < 0x12; ++i) {
		m_widgets[i] = NULL;
	}
	m_vtable = (const GameObjectDispatchView*) &g_menuManagerDispatch;
}

// STUB: LEGORACERS2 0x0041c3f0
static LegoU32 __fastcall MenuManagerAllocationSizeBridge(GameObject*, void*)
{
	return 0x2b4;
}

// STUB: LEGORACERS2 0x0041c400
static GameObject* __fastcall MenuManagerDestroyBridge(GameObject* p_object, void*, LegoU32 p_flags)
{
	MenuManager* manager = (MenuManager*) p_object;
	manager->~MenuManager();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(manager);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0041c420
MenuManager::~MenuManager()
{
	m_vtable = (const GameObjectDispatchView*) &g_menuManagerDispatch;
}

// STUB: LEGORACERS2 0x0041c430
void MenuManager::Initialize()
{
	union FloatBits {
		LegoU32 m_word;
		LegoFloat m_float;
	};
	LegoU32 parameters[13];
	FloatBits one;

	GameObject::Initialize();
	one.m_float = 1.0f;
	parameters[0] = 0;
	parameters[1] = 0xffffffff;
	parameters[4] = 0;
	parameters[5] = 0;
	parameters[6] = one.m_word;
	parameters[7] = one.m_word;
	parameters[8] = 0;
	parameters[9] = 9;
	parameters[10] = one.m_word;
	parameters[11] = 0;
	parameters[12] = 0;
	for (LegoU32 i = 0; i < 14; ++i) {
		m_overlayRecords[i] = g_globalOverlayManager.AllocateRecord(parameters, -1);
	}
}

// STUB: LEGORACERS2 0x0041c4d0
void MenuManager::Remove()
{
	for (LegoU32 i = 14; i != 0; --i) {
		((GlobalOverlayRecord*) m_overlayRecords[i - 1])->Release();
	}
	GameObject::Remove();
}

static void __fastcall MenuManagerInitializeBridge(GameObject* p_object, void*)
{
	((MenuManager*) p_object)->Initialize();
}

static void __fastcall MenuManagerRemoveBridge(GameObject* p_object, void*)
{
	((MenuManager*) p_object)->Remove();
}
