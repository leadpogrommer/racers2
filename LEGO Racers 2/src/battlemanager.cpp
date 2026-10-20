#include "foyermanager.h"
#include "gamecontrollerinitialization.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"

#include <new>

struct BattleManager : GameObject {
	LegoU32 m_state;
	LegoFloat m_elapsed;
	LegoU8 m_unk0x60[0xc4 - 0x60];
	BattleManager(GameObjectTemplate*);
	~BattleManager();
};
DECOMP_SIZE_ASSERT(BattleManager, 0xc4);
static LegoU32 __fastcall GetBattleManagerSize(GameObject*, void*);
static GameObject* __fastcall DestroyBattleManager(GameObject*, void*, LegoU32);
static LegoU32 __fastcall UpdateBattleManager(GameObject*, void*, LegoU32);
void __fastcall RunDeferredBattleState(GameObject*, void*, LegoU32);
struct BattleManagerDispatch {
	GameObjectDispatchView m_object;
	GameObjectDispatchView::Value m_size;
};
// Retail52f444, complete21-slotinterface.
extern const BattleManagerDispatch g_battleManagerDispatch = {
	{&DestroyBattleManager,       &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &FoyerManagerInitialize,     &GameObjectBaseTrue,
	 &UpdateBattleManager,        &GameObjectBaseRemove,
	 &GameObjectBaseZero,         &GameObjectBaseZero,
	 &GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	 &GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument},
	&GetBattleManagerSize
};

// STUB: LEGORACERS2 0x00406040
BattleManager::BattleManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_state = 8;
	m_vtable = &g_battleManagerDispatch.m_object;
}
GameObject* __fastcall ConstructBattleManager(void* p_storage, void*, GameObjectTemplate* p_template)
{
	return new (p_storage) BattleManager(p_template);
}
// STUB: LEGORACERS2 0x00406070
static LegoU32 __fastcall GetBattleManagerSize(GameObject*, void*)
{
	return 0xc4;
}
// STUB: LEGORACERS2 0x00406080
static GameObject* __fastcall DestroyBattleManager(GameObject* p_object, void*, LegoU32 p_flags)
{
	((BattleManager*) p_object)->~BattleManager();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}
// STUB: LEGORACERS2 0x004060a0
BattleManager::~BattleManager()
{
	m_vtable = &g_battleManagerDispatch.m_object;
}
// STUB: LEGORACERS2 0x004060b0
static LegoU32 __fastcall UpdateBattleManager(GameObject* p_object, void*, LegoU32 p_frame)
{
	BattleManager* manager = (BattleManager*) p_object;
	union {
		LegoU32 m_word;
		LegoFloat m_float;
	} elapsed;
	elapsed.m_word = p_frame;
	manager->m_elapsed += elapsed.m_float;
	// Construction sets8; the retail switch has cases0..6 and returns1 for8.
	// Gameplay changes this state only after a battle is selected in the menu.
	if (manager->m_state <= 6) {
		RunDeferredBattleState(p_object, NULL, p_frame);
	}
	return 1;
}
