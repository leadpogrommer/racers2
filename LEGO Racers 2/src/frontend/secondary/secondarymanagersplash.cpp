#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/state/gamepreferences.h"
#include "io/files/filemanager.h"
#include "render/fonts/bitmapfontset.h"

// STUB: LEGORACERS2 0x0042ac90
LegoBool32 SecondaryManager::PrepareSplash(LegoU32 p_enable)
{
	if (GetFileStorageMode() != 3) {
		return TRUE;
	}

	if (m_unk0x4c0 != 0) {
		if (m_state == 9) {
			m_unk0x78->Refresh(0xffffffff);
		}
		if (m_unk0x78->HasActiveSlot() == FALSE) {
			return FALSE;
		}
	}

	LegoBool32 hasCdLanguage = HasCdLanguageExport();
	if (p_enable == 0) {
		return hasCdLanguage;
	}
	if (hasCdLanguage != FALSE) {
		for (LegoU32 index = 0; index < 2; ++index) {
			if (m_splashOverlays[index] != NULL) {
				((GlobalOverlayRecord*) m_splashOverlays[index])->Release();
				m_splashOverlays[index] = NULL;
			}
		}
		m_unk0x4c0 = 0;
		return hasCdLanguage;
	}

	if (m_splashOverlays[0] == NULL) {
		union FloatBits {
			LegoU32 m_word;
			LegoFloat m_float;
		};
		LegoU32 parameters[13];
		FloatBits one;
		one.m_float = 1.0f;
		parameters[0] = (LegoU32) g_gameController.m_fontSet->m_fonts[1];
		parameters[1] = 0xffffffff;
		// Retail leaves the initial position words untouched on the stack.
		parameters[4] = 0;
		parameters[5] = 0;
		parameters[6] = one.m_word;
		parameters[7] = one.m_word;
		parameters[8] = 0;
		parameters[9] = 0x0f;
		parameters[10] = one.m_word;
		parameters[11] = 0;
		parameters[12] = 0;

		m_splashOverlays[0] = g_globalOverlayManager.AllocateRecord(parameters, -1);
		m_splashOverlays[1] = g_globalOverlayManager.AllocateRecord(parameters, -1);

		GlobalOverlayRecord* first = (GlobalOverlayRecord*) m_splashOverlays[0];
		first->SetScale(0.5f);
		first->InitializeTextByIndex(0x39f, 9, 0.0f);
		LegoFloat scale = 0.5f;
		if (0.8f < first->m_measuredWidth) {
			scale = (0.8f / first->m_measuredWidth) * 0.5f;
		}
		first->SetScale(scale);
		LegoFloat firstPosition[2] = {0.5f, 0.45f};
		first->SetPosition(firstPosition, 0xffffffff);
		first->InitializeTextByIndex(0x39f, 9, 0.0f);

		GlobalOverlayRecord* second = (GlobalOverlayRecord*) m_splashOverlays[1];
		second->SetScale(0.5f);
		second->InitializeTextByIndex(0x2b, 9, 0.0f);
		scale = 0.5f;
		if (0.8f < second->m_measuredWidth) {
			scale = (0.8f / second->m_measuredWidth) * 0.5f;
		}
		second->SetScale(scale);
		LegoFloat secondPosition[2] = {0.5f, 0.52f};
		second->SetPosition(secondPosition, 0xffffffff);
		second->InitializeTextByIndex(0x2b, 9, 0.0f);
	}

	m_unk0x4c0 = 1;
	return hasCdLanguage;
}
