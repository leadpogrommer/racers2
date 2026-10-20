#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/controller/gamecontrollerinitialization.h"
#include "io/localization/localizedstringtable.h"

// STUB: LEGORACERS2 0x0040a3d0
void InitializeLocalizedPlayerNames()
{
	const LegoU16* text;
	GameControllerName name;
	g_gameController.m_localizedStrings->GetString(0x391, &text);
	g_gameControllerSettings->SetPlayerName(name.SetWideText(text), 0);
	g_gameController.m_localizedStrings->GetString(0x392, &text);
	g_gameControllerSettings->SetPlayerName(name.SetWideText(text), 1);
}

// STUB: LEGORACERS2 0x0040ad30
void GameControllerSettings::SetPlayerName(const GameControllerName* p_name, LegoS32 p_index)
{
	m_names[p_index] = *p_name;
	if (g_globalOverlayManager.m_unk0x2494c != 0) {
		((GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c)
			->SetPlayerName(p_name, p_index);
	}
}
