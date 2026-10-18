#ifndef LR2_TEXTBOXMANAGER_H
#define LR2_TEXTBOXMANAGER_H

#include "gamemanager.h"

struct GlobalOverlayRecord;
struct GlobalOverlaySprite;
struct AudioSourceRecord;

struct TextBoxFormattingState {
	LegoFloat m_horizontalOrigin; // +0x00
	undefined m_unk0x04[0x0c];
	LegoU16 m_normalizedText[0x80]; // +0x10, observed 0x100-byte copy extent
	undefined m_unk0x110[0x1a0];
	LegoFloat m_rightOffset; // +0x2b0
	LegoFloat m_topOffset;   // +0x2b4
	LegoFloat m_leftOffset;  // +0x2b8
};

DECOMP_STATIC_ASSERT(offsetof(TextBoxFormattingState, m_normalizedText) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(TextBoxFormattingState, m_rightOffset) == 0x2b0);
DECOMP_STATIC_ASSERT(sizeof(TextBoxFormattingState) == 0x2bc);
extern TextBoxFormattingState g_textBoxFormattingState;

struct TextBoxManager : GameObject {
	TextBoxManager(GameObjectTemplate* p_template);
	void Initialize();
	void Remove();
	GameObject* m_nestedManager; // 0x58
};

DECOMP_SIZE_ASSERT(TextBoxManager, 0x5c);
DECOMP_STATIC_ASSERT(offsetof(TextBoxManager, m_nestedManager) == 0x58);

struct NestedTextBoxManagerPrefix : GameObject {
	NestedTextBoxManagerPrefix(GameObjectTemplate* p_template);
	LegoU32 m_active;   // 0x58
	LegoS32 m_selected; // 0x5c
	LegoU32 m_unk0x60;
	LegoU32 m_unk0x64;
	LegoU32 m_unk0x68;
	LegoFloat m_scale; // 0x6c
	LegoU32 m_unk0x70;
	LegoS32 m_selectedResource;  // 0x74
	LegoFloat m_resourceElapsed; // 0x78
	GlobalOverlaySprite* m_sprites0x7c[11];
	GlobalOverlayRecord* m_record0xa8;
	GlobalOverlayRecord* m_records0xac[4];
};

struct TextBoxManagerDispatch {
	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyTransform;
	GameObjectDispatchView::Action m_action0x08;
	GameObjectDispatchView::Action m_action0x0c;
	GameObjectDispatchView::Action m_initialize;
	GameObjectDispatchView::Value m_isActive;
	GameObjectDispatchView::ValueArgument m_update;
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

struct NestedTextBoxManagerDispatch {
	typedef LegoU32(__fastcall* UpdateFloat)(GameObject*, void*, LegoFloat);
	GameObjectDispatchView::Destroy m_destroy;
	GameObjectDispatchView::TwoArguments m_applyTransform;
	GameObjectDispatchView::Action m_action0x08;
	GameObjectDispatchView::Action m_action0x0c;
	GameObjectDispatchView::Action m_initialize;
	GameObjectDispatchView::Value m_isActive;
	UpdateFloat m_update;
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

DECOMP_SIZE_ASSERT(TextBoxManagerDispatch, 0x54);
DECOMP_SIZE_ASSERT(NestedTextBoxManagerDispatch, 0x54);
extern const TextBoxManagerDispatch g_textBoxManagerDispatch;
extern const NestedTextBoxManagerDispatch g_nestedTextBoxManagerDispatch;

GameObject* __fastcall TextBoxManagerDestroy(GameObject*, void*, LegoU32 p_flags);
void __fastcall TextBoxManagerInitialize(GameObject*, void*);
void __fastcall TextBoxManagerRemove(GameObject*, void*);
LegoU32 __fastcall TextBoxManagerGetObjectSize(GameObject*, void*);
GameObject* __fastcall NestedTextBoxManagerDestroy(GameObject*, void*, LegoU32 p_flags);
void __fastcall NestedTextBoxManagerDestructor(GameObject*, void*);
void __fastcall NestedTextBoxManagerInitialize(GameObject*, void*);
void __fastcall NestedTextBoxManagerRemove(GameObject*, void*);
LegoU32 __fastcall NestedTextBoxManagerGetObjectSize(GameObject*, void*);
LegoU32 __fastcall NestedTextBoxManagerUpdate(GameObject*, void*, LegoFloat p_elapsedSeconds);
void __fastcall ClearTextBoxManagerResources(GameObject*, void*);
void __fastcall ReleaseTextBoxManagerResource(GameObject*, void*);
void __fastcall UpdateTextBoxVoiceResource(GameObject*, void*, LegoFloat p_elapsedSeconds);
void __fastcall ResetActiveTextBoxManager(GameObject*, void*);
LegoBool32 __fastcall HasActiveTextBoxPauseTarget(GameObject*, void*);
AudioSourceRecord* __fastcall SelectTextBoxVoice(
	NestedTextBoxManagerPrefix*,
	void*,
	LegoS32 p_voiceIndex,
	LegoS32* p_previousVoiceIndex
);

#endif // LR2_TEXTBOXMANAGER_H
