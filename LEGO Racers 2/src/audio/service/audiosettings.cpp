#include "audio/service/audiosettings.h"

#include "audio/reverb/reverbmanager.h"
#include "game/controller/gamecontroller.h"

extern LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];

// FUNCTION: LEGORACERS2 0x0043dcd0
LegoU32 GetConfiguredAudioServiceMode()
{
	GameController* controller = (GameController*) g_gameControllerStorage;
	AudioSettingsContextView* context = (AudioSettingsContextView*) controller->m_reverbManager;
	if (context != NULL) {
		return context->m_audioServiceMode;
	}
	return g_reverbDefaultState;
}
