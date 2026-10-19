#ifndef LR2_CHARACTERVISUAL_H
#define LR2_CHARACTERVISUAL_H

#include "gamemanager.h"
#include "modelresource.h"
#include "quaternion.h"

struct CharacterVisual {
	undefined m_prefix[0x58];
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
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_body) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_savedHeadModel) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_faceGroup) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_torsoTexture) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_defaultFaceTexture) == 0xdc);
DECOMP_STATIC_ASSERT(offsetof(CharacterVisual, m_previewOrientation) == 0xc8);

#endif
