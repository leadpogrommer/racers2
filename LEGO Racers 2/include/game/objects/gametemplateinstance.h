#ifndef LR2_GAMETEMPLATEINSTANCE_H
#define LR2_GAMETEMPLATEINSTANCE_H

#include "game/objects/gamemanager.h"

class SplinePath;
class Vector3;
class GameTemplateInstance;

struct GameTemplateInstanceTargetDispatch {
	LegoU32(__fastcall* m_isActive)(void*, void*); // 0x00
	undefined m_unk0x04[0x10];
	const Vector3*(__fastcall* m_getPosition)(void*, void*); // 0x14
	undefined m_unk0x18[0x08];
	void(__fastcall* m_setPosition)(void*, void*, const Vector3*);      // 0x20
	void(__fastcall* m_setOrientation)(void*, void*, const LegoFloat*); // 0x24
};
DECOMP_SIZE_ASSERT(GameTemplateInstanceTargetDispatch, 0x28);

struct GameTemplateInstanceTargetView {
	LegoU8 m_prefix[0x58];
	const GameTemplateInstanceTargetDispatch* m_dispatch;
};
DECOMP_STATIC_ASSERT(offsetof(GameTemplateInstanceTargetView, m_dispatch) == 0x58);

struct GameTemplateInstanceExtendedDispatchView {
	GameObjectDispatchView m_object;
	LegoU32*(__fastcall* m_getTypeAndFlags)(GameTemplateInstance*, void*);
	GameTemplateInstance*(__stdcall* m_create)(void*, GameObjectTemplate*);
	GameObjectDispatchView::Value m_value0x58;
	LegoU32(__stdcall* m_prepareRecord)(void*, LegoU32*, LegoU32, LegoU32);
};
DECOMP_STATIC_ASSERT(offsetof(GameTemplateInstanceExtendedDispatchView, m_create) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(GameTemplateInstanceExtendedDispatchView, m_prepareRecord) == 0x5c);

// Runtime object built from the extended 0x84-byte foyer template.
class GameTemplateInstance : public GameObject {
public:
	GameTemplateInstance(GameObjectTemplate* p_template);
	~GameTemplateInstance();

	CharBuffer24 m_firstDefaultName;  // 0x58
	CharBuffer24 m_secondDefaultName; // 0x70
	LegoU32 m_unk0x88;
	LegoU32 m_unk0x8c;
	LegoU32 m_unk0x90;
	LegoU32 m_unk0x94;
	LegoU32 m_unk0x98;
	LegoU32 m_templateFlags;     // 0x9c
	LegoU32 m_templateValues[5]; // 0xa0..0xb0
	LegoU8 m_unk0xb4[0x0c];
};

DECOMP_SIZE_ASSERT(GameTemplateInstance, 0xc0);
DECOMP_STATIC_ASSERT(offsetof(GameTemplateInstance, m_firstDefaultName) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(GameTemplateInstance, m_templateFlags) == 0x9c);

extern const GameTemplateInstanceExtendedDispatchView g_gameTemplateInstanceDispatch;
GameObject* __fastcall GameTemplateInstanceDestroy(GameObject*, void*, LegoU32);
LegoU32* __fastcall GetGameTemplateInstanceTypeAndFlags(GameTemplateInstance*, void*);
void __fastcall InitializeGameTemplateInstance(GameObject*, void*);
LegoU32 __fastcall UpdateGameTemplateInstance(GameObject*, void*, LegoFloat);
void __fastcall UpdateGameTemplateInstancePath(GameTemplateInstance*, void*, SplinePath*, GameObject*, LegoFloat);
GameTemplateInstance* __stdcall CreateGameTemplateInstance(void*, GameObjectTemplate*);
LegoU32 __stdcall PrepareGameTemplateInstanceRecord(void*, LegoU32*, LegoU32, LegoU32);

#endif // LR2_GAMETEMPLATEINSTANCE_H
