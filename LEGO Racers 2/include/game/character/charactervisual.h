#ifndef LR2_CHARACTERVISUAL_H
#define LR2_CHARACTERVISUAL_H

#include "core/strings/charbuffer24.h"
#include "game/objects/gamemanager.h"
#include "math/quaternion.h"
#include "render/models/modelresource.h"

struct CharacterVisualTemplate : GameObjectTemplate {
	LegoU32 m_mode; // +0x34
	LegoU32 m_unk0x38;
	LegoU32 m_unk0x3c;
	LegoU32 m_unk0x40;
	LegoU32 m_unk0x44;
	LegoU32 m_unk0x48;
	CharBuffer24 m_bodypartName; // +0x4c

	void Initialize(
		LegoU32 p_valueAt34,
		LegoU32 p_valueAt38,
		LegoU32 p_valueAt3c,
		LegoU32 p_valueAt44,
		LegoU32 p_valueAt48,
		const CharBuffer24* p_bodypartName
	);
};
DECOMP_SIZE_ASSERT(CharacterVisualTemplate, 0x64);

CharacterVisualTemplate* GetSparkyCharacterVisualTemplate();
CharacterVisualTemplate* GetIntroCharacterVisualTemplate();
CharacterVisualTemplate* GetRocketRacerCharacterVisualTemplate();
CharacterVisualTemplate* GetRigelCharacterVisualTemplate();
void InitializeCharacterVisualHeadBase(GameObject*, GameObjectTemplate*);
struct PhysicalLifecycleState;
struct RendererAuxiliaryResourceRecord;
LegoU32 __cdecl CharacterShieldResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
LegoU32 __cdecl CharacterSheenResourceCallback(RendererAuxiliaryResourceRecord*, LegoU32);
GameObject* __fastcall DestroyCharacterVisualHead(GameObject*, void*, LegoU32);
GameObject* __fastcall DestroyCharacterVisualHeadLifecycle(PhysicalLifecycleState*, void*, LegoU32);

struct CharacterVisual {
	CharacterVisual(GameObjectTemplate* p_template);
	~CharacterVisual();

	GameObject m_object; // 0x00
	LegoS32 m_mode;
	undefined m_unk0x5c[8];
	GameObject* m_body;
	GameObject* m_head;
	ModelResource* m_savedHeadModel;
	undefined m_unk0x70[0x90 - 0x70];
	LegoS32 m_faceGroup;
	undefined m_unk0x94[4];
	LegoS32 m_torsoTexture;
	LegoS32 m_legTexture;
	LegoS32 m_faceTexture;
	undefined m_unk0xa4[0xc8 - 0xa4];
	Quaternion m_previewOrientation;
	undefined m_unk0xd8[4];
	LegoS32 m_defaultFaceTexture;
	undefined m_gap0xe0[0xb8];

	void SetHeadModel(LegoS32);
	void SetTorsoTexture(LegoS32, LegoU32 p_species);
	void SetLegTexture(LegoS32);
	void SetFaceVariant(LegoS32);
	void SetFaceGroup(LegoS32);
	void PrepareForHiddenPreview();
	void EnterHiddenMode();
	void PrepareForIdlePreview();
	void EnterIdlePreviewMode();
};
LegoU32 __fastcall GetCharacterVisualObjectSize(GameObject*, void*);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_body) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_savedHeadModel) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_faceGroup) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_torsoTexture) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_defaultFaceTexture) == 0xdc);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_previewOrientation) == 0xc8);
DECOMP_SIZE_ASSERT(CharacterVisual, 0x198);

#endif
