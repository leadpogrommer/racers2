#include "bitmapfontset.h"
#include "gamecontroller.h"
#include "gamecontrollerinitialization.h"
#include "globaloverlaymanager.h"
#include "menumanager.h"
#include "screenprinter.h"

// FUNCTION: LEGORACERS2 0x0040a230
void __fastcall ApplyControllerFonts(GameController* p_controller, void*)
{
	((MenuManager*) p_controller->m_menuManager)->ApplyFonts();
	((ScreenPrinter*) p_controller->m_screenPrinter)->ApplyFont();
}

// STUB: LEGORACERS2 0x0041c6d0
void MenuManager::ApplyFonts()
{
	BitmapFont* regular = g_gameController.m_fontSet->m_fonts[2];
	BitmapFont* alternate = g_gameController.m_fontSet->m_fonts[1];
	for (LegoU32 i = 0; i < 14; ++i) {
		if (m_overlayRecords[i] != NULL) {
			((GlobalOverlayRecord*) m_overlayRecords[i])->SetFont(i == 6 ? alternate : regular);
		}
	}
}

// FUNCTION: LEGORACERS2 0x004b3e40
void GlobalOverlayRecord::SetFont(BitmapFont* p_font)
{
	m_font = p_font;
}
