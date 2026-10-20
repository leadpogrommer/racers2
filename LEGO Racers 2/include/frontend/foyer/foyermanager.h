#ifndef LR2_FOYERMANAGER_H
#define LR2_FOYERMANAGER_H

#include "game/objects/gamemanager.h"

class FoyerManager : public GameObject {
public:
	FoyerManager(GameObjectTemplate* p_template);

	LegoU32 m_state;                // 0x58
	LegoU32 m_previousState;        // 0x5c
	LegoS32 m_previousSelection;    // 0x60
	GameObject* m_sceneState;       // 0x64
	void* m_objectStates[13];       // 0x68
	void* m_sceneObjects[13];       // 0x9c
	LegoU32 m_unk0xd0[4];           // 0xd0
	LegoS32 m_previousSessionState; // 0xe0
	LegoU32 m_unk0xe4[3];           // 0xe4
	LegoU32 m_unk0xf0[24];          // 0xf0
	void* m_menuResource;           // 0x150
	LegoS32 m_menuResourceType;     // 0x154
	LegoFloat m_menuTimeout;        // 0x158
};

DECOMP_SIZE_ASSERT(FoyerManager, 0x15c);
DECOMP_STATIC_ASSERT(offsetof(FoyerManager, m_state) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(FoyerManager, m_objectStates) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(FoyerManager, m_sceneObjects) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(FoyerManager, m_menuResource) == 0x150);

struct FoyerManagerDispatch {
	typedef LegoU32(__fastcall* FrameUpdate)(GameObject*, void*, LegoFloat);

	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyTransform;
	GameObjectDispatchView::Action m_action0x08;
	GameObjectDispatchView::Action m_action0x0c;
	GameObjectDispatchView::Action m_initialize;
	GameObjectDispatchView::Value m_action0x14;
	FrameUpdate m_update;
	GameObjectDispatchView::Action m_remove;
	GameObjectDispatchView::Value m_value0x20;
	GameObjectDispatchView::Value m_value0x24;
	GameObjectDispatchView::Action m_action0x28;
	GameObjectDispatchView::Action m_action0x2c;
	GameObjectDispatchView::Action m_drawChild;
	GameObjectDispatchView::Update m_action0x34;
	GameObjectDispatchView::Value m_value0x38;
	GameObjectDispatchView::TwoArguments m_action0x3c;
	GameObjectDispatchView::Value m_value0x40;
	GameObjectDispatchView::ValueTwoArguments m_value0x44;
	GameObjectDispatchView::Update m_updateRegion;
	GameObjectDispatchView::Update m_action0x4c;
	GameObjectDispatchView::Value m_objectSize;
};

DECOMP_SIZE_ASSERT(FoyerManagerDispatch, 0x54);

extern const FoyerManagerDispatch g_foyerManagerDispatch;
GameObject* __fastcall FoyerManagerDestroy(GameObject*, void*, LegoU32 p_flags);
LegoU32 __fastcall FoyerManagerGetObjectSize(GameObject*, void*);
LegoU32 __fastcall FoyerManagerUpdate(GameObject*, void*, LegoFloat p_elapsedSeconds);
void __fastcall FoyerManagerInitialize(GameObject*, void*);
void __fastcall SetFoyerManagerMenuMode(GameObject*, void*);

#endif // LR2_FOYERMANAGER_H
