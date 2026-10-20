#include "game/character/charactervisual.h"

#include "game/character/bodypartmanager.h"
#include "game/controller/gamecontroller.h"

struct CharacterModelBinding {
	undefined m_prefix[8];
	ModelResource* m_model;
};
DECOMP_STATIC_ASSERT(offsetof(CharacterModelBinding, m_model) == 8);

static ModelResource* GetCharacterObjectModel(GameObject* p_object)
{
	return ((CharacterModelBinding*) p_object->m_unk0x48)->m_model;
}

// STUB: LEGORACERS2 0x0044f570
void CharacterVisual::SetHeadModel(LegoS32 p_index)
{
	BodypartManager* manager = (BodypartManager*) g_gameController.m_bodypartManager;
	if (m_savedHeadModel == NULL) {
		m_savedHeadModel = GetCharacterObjectModel(m_head);
	}
	GetCharacterObjectModel(m_head)->RemapTextureIndices(m_faceTexture, (LegoS16) m_defaultFaceTexture);
	((CharacterModelBinding*) m_head->m_unk0x48)->m_model = manager->GetLoadedHeadModel(p_index);
	m_faceTexture = m_defaultFaceTexture;
}

// STUB: LEGORACERS2 0x0044f5d0
void CharacterVisual::SetTorsoTexture(LegoS32 p_index, LegoU32 p_species)
{
	LegoS32 texture = ((BodypartManager*) g_gameController.m_bodypartManager)->GetTorsoTexture(p_index, p_species);
	GetCharacterObjectModel(m_body)->RemapTextureIndices(m_torsoTexture, (LegoS16) texture);
	m_torsoTexture = texture;
}

// STUB: LEGORACERS2 0x0044f610
void CharacterVisual::SetLegTexture(LegoS32 p_index)
{
	LegoS32 texture = ((BodypartManager*) g_gameController.m_bodypartManager)->GetLegTexture(p_index);
	GetCharacterObjectModel(m_body)->RemapTextureIndices(m_legTexture, (LegoS16) texture);
	m_legTexture = texture;
}

// STUB: LEGORACERS2 0x0044f650
void CharacterVisual::SetFaceVariant(LegoS32 p_variant)
{
	LegoS32 texture = ((BodypartManager*) g_gameController.m_bodypartManager)->GetFaceTexture(m_faceGroup, p_variant);
	GetCharacterObjectModel(m_head)->RemapTextureIndices(m_faceTexture, (LegoS16) texture);
	m_faceTexture = texture;
}

// STUB: LEGORACERS2 0x0044f690
void CharacterVisual::SetFaceGroup(LegoS32 p_group)
{
	LegoS32 texture = ((BodypartManager*) g_gameController.m_bodypartManager)->GetFaceTexture(p_group, 0);
	GetCharacterObjectModel(m_head)->RemapTextureIndices(m_faceTexture, (LegoS16) texture);
	m_faceTexture = texture;
	m_faceGroup = p_group;
}
