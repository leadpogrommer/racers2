#ifndef LR2_FOYERSCENEEVENTSTATE_H
#define LR2_FOYERSCENEEVENTSTATE_H

#include "game/objects/gamemanager.h"

class FoyerSceneEventStateObject;

struct FoyerSceneEventStateDispatchView {
	GameObjectDispatchView m_object;
	LegoU32*(__fastcall* m_getTypeAndFlags)(FoyerSceneEventStateObject*, void*);
	FoyerSceneEventStateObject*(__stdcall* m_create)(void*, GameObjectTemplate*);
	GameObjectDispatchView::Value m_value0x58;
	LegoU32(__stdcall* m_prepareRecord)(void*, LegoU32*, LegoU32, LegoU32);
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventStateDispatchView, m_create) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventStateDispatchView, m_prepareRecord) == 0x5c);

class FoyerSceneEventStateObject : public GameObject {
public:
	FoyerSceneEventStateObject(GameObjectTemplate* p_template);
	~FoyerSceneEventStateObject();

	CharBuffer24 m_firstName;  // 0x58
	CharBuffer24 m_secondName; // 0x70
	LegoU32 m_unk0x88;
	LegoU32 m_unk0x8c;
	LegoU32 m_unk0x90;
	LegoU32 m_unk0x94;
	LegoU32 m_unk0x98;
	LegoU32 m_stateFlags; // 0x9c
	LegoU32 m_unk0xa0;
	LegoU32 m_unk0xa4;
	LegoU32 m_unk0xa8;
	LegoU32 m_templateValues[4]; // 0xac..0xb8
	LegoU32 m_unk0xbc;
};

DECOMP_SIZE_ASSERT(FoyerSceneEventStateObject, 0xc0);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventStateObject, m_firstName) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventStateObject, m_stateFlags) == 0x9c);

extern const FoyerSceneEventStateDispatchView g_foyerSceneEventStateDispatch;
GameObject* __fastcall FoyerSceneEventStateDestroy(GameObject*, void*, LegoU32);
LegoU32* __fastcall GetFoyerSceneEventStateTypeAndFlags(FoyerSceneEventStateObject*, void*);
void __fastcall InitializeFoyerSceneEventStateObject(GameObject*, void*);
LegoU32 __fastcall UpdateFoyerSceneEventStateObject(GameObject*, void*, LegoFloat);
FoyerSceneEventStateObject* __stdcall CreateFoyerSceneEventStateObject(void*, GameObjectTemplate*);
LegoU32 __stdcall PrepareFoyerSceneEventStateRecord(void*, LegoU32*, LegoU32, LegoU32);

#endif // LR2_FOYERSCENEEVENTSTATE_H
