#include "frontend/overlay/textboxfade.h"

#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/overlay/textboxmanager.h"

namespace
{
void SetNestedTextBoxFadeImpl(NestedTextBoxManagerPrefix* p_nested, LegoFloat p_fade)
{
	p_nested->m_scale = p_fade;
	if (p_nested->m_unk0x70 != 0) {
		LegoFloat childFade = p_fade == 0.0f ? 1.0f : p_fade;
		void* child = (void*) p_nested->m_unk0x70;
		void** vtable = *(void***) child;
		((void(__fastcall*)(void*, void*, LegoFloat)) vtable[0x1c / 4])(child, NULL, childFade);
	}

	for (LegoU32 i = 0; i < 4; ++i) {
		GlobalOverlayRecord* record = p_nested->m_records0xac[i];
		if (record != NULL) {
			record->m_elapsed =
				p_fade == 0.0f ? 3.402823466e+38f : *(LegoFloat*) ((LegoU8*) p_nested + 0x18dc) / p_fade;
		}
	}
}

void UpdateTextBoxManagerFadeImpl(TextBoxManager* p_manager, LegoFloat p_fade)
{
	SetNestedTextBoxFadeImpl((NestedTextBoxManagerPrefix*) p_manager->m_nestedManager, p_fade);
}
} // namespace

// STUB: LEGORACERS2 0x00438b50
void __fastcall SetNestedTextBoxFade(NestedTextBoxManagerPrefix* p_nested, void*, LegoFloat p_fade)
{
	SetNestedTextBoxFadeImpl(p_nested, p_fade);
}

// STUB: LEGORACERS2 0x00439040
void __fastcall UpdateTextBoxManagerFade(TextBoxManager* p_manager, void*, LegoFloat p_fade)
{
	UpdateTextBoxManagerFadeImpl(p_manager, p_fade);
}

// STUB: LEGORACERS2 0x0045fad0
void __fastcall SetNpcTextManagerFade(GameObject* p_manager, void*, LegoFloat p_fade)
{
	*(LegoFloat*) ((LegoU8*) p_manager + 0x68) = p_fade;
	UpdateTextBoxManagerFade((TextBoxManager*) *(void**) ((LegoU8*) p_manager + 0x58), NULL, p_fade);
}
