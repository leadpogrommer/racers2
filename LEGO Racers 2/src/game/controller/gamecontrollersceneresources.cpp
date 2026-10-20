#include "frontend/menu/menumanagerresources.h"
#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontrollerinitialization.h"
#include "render/fonts/bitmapfontset.h"

void __fastcall LoadSecondaryManagerAudioResources(SecondaryManager*, void*, LegoU32);
void __fastcall ReleaseSecondaryManagerAudioResources(SecondaryManager*, void*);
void __fastcall LoadControllerHudTextures(void*, void*);    // 434bc0
void __fastcall ReleaseControllerHudTextures(void*, void*); // 434e30

// STUB: LEGORACERS2 0x0040a170
void __fastcall LoadControllerSceneResources(GameController* p_controller, void*, LegoU32 p_mode)
{
	p_controller->m_fontSet->Initialize(p_mode != 0);
	ApplyControllerFonts(p_controller, NULL);
	((GlobalOverlayGlyphReplacementManagerView*) p_controller->m_unk0xb4)->LoadIconTextures();
	LoadSecondaryManagerAudioResources((SecondaryManager*) p_controller->m_secondaryManager, NULL, p_mode);
	LoadControllerHudTextures(g_gameController.m_unk0x90, NULL);
	LoadMenuManagerResources((MenuManager*) p_controller->m_menuManager, NULL, p_mode == 0);
}

// STUB: LEGORACERS2 0x0040a1d0
void __fastcall ReleaseControllerSceneResources(GameController* p_controller, void*, LegoU32 p_mode)
{
	ReleaseMenuManagerResources((MenuManager*) p_controller->m_menuManager, NULL, p_mode == 0);
	ReleaseControllerHudTextures(g_gameController.m_unk0x90, NULL);
	ReleaseSecondaryManagerAudioResources((SecondaryManager*) p_controller->m_secondaryManager, NULL);
	((GlobalOverlayGlyphReplacementManagerView*) p_controller->m_unk0xb4)->ReleaseIconTextures();
	p_controller->m_fontSet->Release(p_mode != 0);
}
