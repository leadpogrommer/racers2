#ifndef LR2_GAMEMANAGER_H
#define LR2_GAMEMANAGER_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GameObject;

struct GameObjectDispatchView {
	typedef LegoBool32(__fastcall* Predicate)(GameObject*, void*, LegoU32);
	typedef void(__fastcall* Action)(GameObject*, void*);
	typedef void(__fastcall* Update)(GameObject*, void*, LegoU32);

	Predicate m_predicate0x00; // 0x00
	Action m_destroy;          // 0x04
	Action m_action0x08;       // 0x08
	undefined m_unk0x0c[0x14 - 0x0c];
	Predicate m_isActive; // 0x14
	Update m_update;      // 0x18
	Action m_remove;      // 0x1c
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectDispatchView, m_isActive) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(GameObjectDispatchView, m_update) == 0x18);

// Only the object fields used by the recovered registry loops are typed.
struct GameObject {
	const GameObjectDispatchView* m_vtable;
	LegoU32 m_type;
	LegoU32 m_flags;
	undefined m_unk0x0c[0x54 - 0x0c];
	void* m_lifecycle0x54;
};

DECOMP_STATIC_ASSERT(offsetof(GameObject, m_type) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_flags) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_lifecycle0x54) == 0x54);

class GameManager {
public:
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;

	void RebuildObjectWorkLists();
	void ResizeObjectRegistry(LegoS32 p_capacity);
	void SortObjectsByParent();
	void InitializeRegisteredObjects();
	void UpdateObjects(LegoU32 p_frameParameter);
	void RemoveAllObjects();
};

DECOMP_SIZE_ASSERT(GameManager, 0x08);

GameManager* GetGameManager();
void ShutdownGameManager();

// Registry backing storage shared with the game manager's other entrypoints.
extern GameObject** g_gameObjects;
extern LegoS32 g_gameObjectCount;
extern LegoS32 g_gameObjectCapacity;
extern LegoU32 g_gameRegistryMutationDepth;
extern LegoU32 g_gameLifecycleHooksNeedRefresh;
extern GameObject** g_gameObjectsByType;
extern GameObject** g_gameObjectsWithActiveComponents;
extern LegoS32 g_gameObjectsByTypeCount;
extern LegoS32 g_gameObjectsWithActiveComponentsCount;
extern LegoU32 g_gameObjectListsNeedRebuild;

#endif // LR2_GAMEMANAGER_H
