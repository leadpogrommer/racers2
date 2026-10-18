#include "audiosettings.h"

// GLOBAL: LEGORACERS2 0x005544bc
AudioSettingsContextView* g_audioSettingsContext = NULL;

// STUB: LEGORACERS2 0x0043dcd0
LegoU32 GetConfiguredAudioServiceMode()
{
	if (g_audioSettingsContext != NULL) {
		return g_audioSettingsContext->m_audioServiceMode;
	}
	return 1;
}
