#ifndef LR2_GAMEMANAGER_H
#define LR2_GAMEMANAGER_H

#include "charbuffer24.h"
#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GameObject;
struct CollisionContact;

struct GameObjectTemplate {
	void NormalizeDevelopmentFlags();
	LegoU32 m_flags;
	CharBuffer24 m_first;
	CharBuffer24 m_second;
};
DECOMP_SIZE_ASSERT(GameObjectTemplate, 0x34);

class CollisionProxy;
struct GameLifecycleComponent;

struct GameLifecycleDispatchView {
	typedef LegoBool32(__fastcall* Predicate)(GameLifecycleComponent*, void*, LegoU32);
	typedef void(__fastcall* Action)(GameLifecycleComponent*, void*);
	Predicate m_isActive; // 0x00
	undefined m_unk0x04[0x40 - 0x04];
	Action m_prepare; // 0x40
};

extern LegoU32 g_gameObjectInitializationValue;

struct GameLifecycleComponent {
	const GameLifecycleDispatchView* m_vtable;
	GameObject* m_owner;              // 0x04
	CollisionProxy* m_collisionProxy; // 0x08
	LegoU32 m_stateFlags;             // 0x0c
};

struct GameObjectDispatchView {
	typedef GameObject*(__fastcall* Destroy)(GameObject*, void*, LegoU32);
	typedef void(__fastcall* TwoArguments)(GameObject*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* Action)(GameObject*, void*);
	typedef LegoU32(__fastcall* Value)(GameObject*, void*);
	typedef LegoU32(__fastcall* ValueArgument)(GameObject*, void*, LegoU32);
	typedef void(__fastcall* Update)(GameObject*, void*, LegoU32);
	typedef LegoU32(__fastcall* ValueTwoArguments)(GameObject*, void*, LegoU32, LegoU32);

	Destroy m_destroy;             // 0x00
	TwoArguments m_action0x04;     // 0x04
	Action m_action0x08;           // 0x08
	Action m_action0x0c;           // 0x0c
	Action m_initialize;           // 0x10
	Value m_isActive;              // 0x14
	ValueArgument m_update;        // 0x18
	Action m_remove;               // 0x1c
	Value m_value0x20;             // 0x20
	Value m_value0x24;             // 0x24
	Action m_action0x28;           // 0x28
	Action m_action0x2c;           // 0x2c
	Action m_action0x30;           // 0x30
	Update m_action0x34;           // 0x34
	Value m_value0x38;             // 0x38
	TwoArguments m_action0x3c;     // 0x3c
	Value m_value0x40;             // 0x40
	ValueTwoArguments m_value0x44; // 0x44
	Update m_action0x48;           // 0x48
	Update m_action0x4c;           // 0x4c
};

DECOMP_SIZE_ASSERT(GameObjectDispatchView, 0x50);
DECOMP_STATIC_ASSERT(offsetof(GameObjectDispatchView, m_isActive) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(GameObjectDispatchView, m_update) == 0x18);
extern const GameObjectDispatchView g_gameObjectBaseDispatch;

// Only the object fields used by the recovered registry loops are typed.
struct GameObject {
	GameObject(LegoU32 p_type, GameObjectTemplate* p_template, void* p_lifecycle);
	~GameObject();
	void RefreshCollisionState();
	void Initialize();
	void Remove();
	LegoBool32 IsVisible();
	void DrawChild();
	void UpdateRegion(void* p_region);
	void ApplyChildTransform(const LegoFloat* p_position, const LegoFloat* p_rotation);
	void SetParent(GameObject* p_parent, LegoU32 p_flag);
	void ClearParent(LegoU32 p_flag);
	const GameObjectDispatchView* m_vtable;
	LegoU32 m_type;
	LegoU32 m_flags;
	LegoU32 m_unk0x0c;
	CharBuffer24 m_first;           // 0x10
	CharBuffer24 m_second;          // 0x28
	GameObject* m_parent;           // 0x40
	GameObjectTemplate* m_template; // 0x44
	void* m_unk0x48;
	LegoU8 m_unk0x4c;
	LegoU8 m_unk0x4d;
	LegoU16 m_unk0x4e;
	LegoFloat m_unk0x50;
	GameLifecycleComponent* m_lifecycle0x54;
};

DECOMP_STATIC_ASSERT(offsetof(GameObject, m_type) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_flags) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_lifecycle0x54) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_first) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_second) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(GameObject, m_parent) == 0x40);

class GameManager {
public:
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;

	void RebuildObjectWorkLists();
	void ResizeObjectRegistry(LegoS32 p_capacity);
	void SortObjectsByParent();
	void RegisterObject(GameObject* p_object);
	void UnregisterObject(GameObject* p_object);
	void InitializeRegisteredObjects();
	void UpdateObjects(LegoU32 p_frameParameter);
	void PrepareComponents();
	void SetControllerFrame(const LegoU32* p_frame);
	void MoveObjects(LegoFloat p_stepSeconds);
	void RefreshCollisionStates();
	LegoS32 SolveContacts(CollisionContact** p_contacts);
	void UpdateComponents(LegoFloat p_frameDelta);
	void RenderFrame(LegoFloat p_frameDelta);
	void RemoveAllObjects();
};

DECOMP_SIZE_ASSERT(GameManager, 0x08);

GameManager* GetGameManager();
void ShutdownGameManager();

// Registry backing storage shared with the game manager's other entrypoints.
extern GameObject** g_gameObjects;
extern LegoS32 g_gameObjectCount;
extern LegoS32 g_gameObjectCapacity;
extern LegoU32 g_gameObjectRegistryUsesStaticStorage;
extern LegoU32 g_gameLifecycleHooksNeedRefresh;
extern GameObject** g_gameObjectsByType;
extern GameObject** g_gameObjectsWithActiveComponents;
extern LegoS32 g_gameObjectsByTypeCount;
extern LegoS32 g_gameObjectsWithActiveComponentsCount;
extern LegoU32 g_gameObjectWorkListsValid;

#endif // LR2_GAMEMANAGER_H
