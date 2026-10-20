#include "frontend/secondary/secondarymanagerleases.h"
#include "game/character/charactervisual.h"
#include "game/objects/gameanimatedobject.h"
#include "scene/lighting/gamelight.h"

// STUB: LEGORACERS2 0x004291e0
void __fastcall ReleaseMenuPreviewCharacter(SecondaryManager* p_manager, void*, void** p_output, LegoS32)
{
	p_manager->m_unk0x130 = -1;
	*p_output = NULL;
	CharacterVisual* character = (CharacterVisual*) p_manager->m_unk0x12c;
	character->PrepareForHiddenPreview();
	((GameAnimatedObject*) character->m_body)->SetAnimationTransformEnabled(0);
}

// FUNCTION: LEGORACERS2 0x00429320
void __fastcall ReleaseMenuPreviewLight(SecondaryManager* p_manager, void*, void** p_output, LegoS32 p_key)
{
	for (LegoS32 index = 0; index < 3; ++index) {
		if (p_manager->m_moreResourceIds[index] == p_key) {
			p_manager->m_moreResourceIds[index] = -1;
			*p_output = NULL;
			((GameDirectionalLight*) p_manager->m_moreResourcePointers[index])->m_properties.SetEnabled(0);
			return;
		}
	}
}
