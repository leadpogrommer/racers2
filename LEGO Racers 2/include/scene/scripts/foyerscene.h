#ifndef LR2_FOYERSCENE_H
#define LR2_FOYERSCENE_H

#include "core/strings/charbuffer128.h"
#include "game/objects/gameobjectdispatch.h"

struct FoyerSceneResourceView {
	LegoU8 m_prefix[0xa40];
	CharBuffer128 m_resourcePath;
	CharBuffer24 m_resourceName;
	LegoU32 m_unk0xad8;
	LegoU32 m_unk0xadc;
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceView, m_resourcePath) == 0xa40);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceView, m_resourceName) == 0xac0);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceView, m_unk0xad8) == 0xad8);

struct FoyerSceneResourceRegistry {
	FoyerSceneResourceView* m_slotBeforeFirst;
	FoyerSceneResourceView* m_entries[89];
	LegoS32 m_count;
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceRegistry, m_entries) == 4);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceRegistry, m_count) == 0x168);
DECOMP_SIZE_ASSERT(FoyerSceneResourceRegistry, 0x16c);

struct FoyerSceneObjectDispatch {
	typedef void(__fastcall* Action)(void*, void*);
	void* m_destroy;
	Action m_update;
	Action m_destroyState;
};

struct FoyerSceneObjectView {
	const FoyerSceneObjectDispatch* m_vtable;
	LegoU32 m_type;
};

struct FoyerSceneResourceDispatchView {
	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyTransform;
	GameObjectDispatchView::Action m_action0x08;
	GameObjectDispatchView::Action m_action0x0c;
	GameObjectDispatchView::Action m_initialize;
	GameObjectDispatchView::Value m_isActive;
	LegoU32(__fastcall* m_update)(GameObject*, void*, LegoFloat);
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
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResourceDispatchView, m_update) == 0x18);
DECOMP_SIZE_ASSERT(FoyerSceneResourceDispatchView, 0x50);

struct AudioSourceRecord;

struct FoyerSceneInputState {
	CharBuffer24 m_text;
	CharBuffer128 m_path;
	LegoU8 m_opaque0x98[0x08];
	AudioSourceRecord* m_audioSourceRecord; // 0xa0, released through AudioService on removal
	void* m_animationTarget;                // 0xa4, called through slot +0x1c by animation-mode updates
};

DECOMP_SIZE_ASSERT(FoyerSceneInputState, 0xa8);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneInputState, m_audioSourceRecord) == 0xa0);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneInputState, m_animationTarget) == 0xa4);

class FoyerSceneResource : public GameObject {
public:
	FoyerSceneResource(const CharBuffer128* p_resourcePath);

	LegoU32 m_unk0x58;
	LegoU8 m_opaque0x5c[0x0c];
	LegoU32 m_unk0x68;
	void* m_eventStorage; // 0x6c, allocation header precedes 12 entries
	LegoS32 m_eventCount; // 0x70
	FoyerSceneInputState m_inputStates[8];
	LegoU32 m_unk0x5b4;
	GameObject* m_objects[256]; // retail object pointer area spans 0x400 bytes
	LegoU32 m_unk0x9b8;
	LegoU32 m_unk0x9bc;
	LegoU32 m_unk0x9c0;
	LegoU32 m_unk0x9c4;
	LegoU32 m_unk0x9c8;
	LegoU32 m_unk0x9cc;
	LegoU8 m_opaque0x9d0[0x18];
	LegoU32 m_unk0x9e8;
	LegoU32 m_unk0x9ec;
	LegoU32 m_unk0x9f0;
	LegoU8 m_opaque0x9f4[0x20];
	LegoU32 m_animationMode; // 0xa14
	LegoU32 m_opaque0xa18;
	GameObject* m_children[2];
	LegoFloat m_animationValues[2];
	LegoU32 m_unk0xa2c;
	LegoU32 m_unk0xa30;
	LegoU32 m_unk0xa34;
	LegoU32 m_unk0xa38;
	LegoU8 m_opaque0xa3c[4];
	LegoChar m_resourcePath[128];
	CharBuffer24 m_resourceName;
	LegoU32 m_unk0xad8;
	LegoU32 m_unk0xadc;
};

DECOMP_SIZE_ASSERT(FoyerSceneResource, 0xae0);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_eventStorage) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_eventCount) == 0x70);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_inputStates) == 0x74);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_resourcePath) == 0xa40);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_resourceName) == 0xac0);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_animationMode) == 0xa14);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_children) == 0xa1c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneResource, m_animationValues) == 0xa24);

extern const FoyerSceneResourceDispatchView g_foyerSceneResourceDispatch;
GameObject* __fastcall FoyerSceneResourceDestroy(GameObject*, void*, LegoU32 p_flags);
void __fastcall FoyerSceneResourceInitialize(GameObject*, void*);
LegoU32 __fastcall FoyerSceneResourceUpdate(GameObject*, void*, LegoFloat p_elapsedSeconds);
void __fastcall FoyerSceneResourceRemove(GameObject*, void*);

extern FoyerSceneResourceRegistry g_foyerSceneResourceRegistry;
extern LegoU8 g_foyerSceneResourceInitFlags;

FoyerSceneResourceView* __cdecl FindFoyerSceneResource(const LegoChar* p_name);
FoyerSceneResourceView* __cdecl FindOrCreateFoyerSceneResource(const CharBuffer128* p_path);
void __cdecl ReleaseFoyerSceneResource(const CharBuffer128* p_path);

#endif // LR2_FOYERSCENE_H
