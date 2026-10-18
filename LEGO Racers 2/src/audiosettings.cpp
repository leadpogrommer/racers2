#include "audiosettings.h"

#include "gamecontroller.h"

extern LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];

// STUB: LEGORACERS2 0x0043dcd0
LegoU32 GetConfiguredAudioServiceMode()
{
	GameController* controller = (GameController*) g_gameControllerStorage;
	AudioSettingsContextView* context = (AudioSettingsContextView*) controller->m_reverbManager;
	if (context != NULL) {
		return context->m_audioServiceMode;
	}
	return 1;
}
