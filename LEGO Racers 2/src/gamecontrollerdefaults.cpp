#include "application.h"
#include "audioservice.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollerinitialization.h"
#include "gamepreferences.h"
#include "renderer.h"
#include "rendererdispatch.h"
#include "systemclock.h"

void __fastcall ApplySavedAudioLevels(void*, void*);
void __fastcall ApplySavedAudioQuality(void*, void*);

// GLOBAL: LEGORACERS2 0x0053ee68
const LegoU32 g_controllerDefaultWords[25] = {2, 3, 3, 3, 0, 2, 3, 3, 3, 2, 3, 2, 2,
											  2, 2, 3, 3, 3, 3, 3, 3, 4, 3, 4, 3};

// STUB: LEGORACERS2 0x0040a970
void __fastcall InitializeGameControllerDefaults(GameController* p_controller, void*)
{
	GameControllerSettings* settings = (GameControllerSettings*) ((LegoU8*) p_controller + 0x130);
	Application* application = GetApplication();
	settings->m_version = application->m_vtable->m_float0x0c(application, NULL);
	p_controller->m_settingsCookie = 0xf00cf00c;
	GameControllerName name;
	g_gameControllerSettings->SetPlayerName(name.SetNarrowText("1UP"), 0);
	g_gameControllerSettings->SetPlayerName(name.SetNarrowText("2UP"), 1);
	for (LegoU32 i = 0; i < 25; ++i) {
		settings->m_defaultWords[i] = g_controllerDefaultWords[i];
	}
	settings->m_unk0x90 = 1;
	settings->m_unk0x94 = 1;
	settings->m_musicGain = 0.8f;
	settings->m_effectGain = 0.8f;
	settings->m_audioQuality = 1;
	settings->m_unk0xa4 = 1;
	settings->m_unk0xa8 = 0;
	settings->m_unk0xac = 0;
	settings->m_language = 2;
	application = GetApplication();
	if (application->m_vtable->m_value0x7c(application, NULL)) {
		settings->m_language = 3;
	}
	settings->m_unk0xa8 = 1;
	settings->m_unk0xac = 1;
	LegoS32 language;
	if (AccessGameLanguagePreference(0, &language)) {
		settings->m_language = language;
	}
	p_controller->m_unk0x340 = 8;
	p_controller->m_unk0x344 = 3;
	SystemClock* clock = GetSystemClock();
	LegoS32 calibration = (LegoS32) clock->m_vtable->m_getCalibration(clock, NULL);
	DisplayRenderer* renderer = g_displayRenderer;
	if (renderer->m_vtable->m_usesHardwareVertexProcessing(renderer, NULL) == 1) {
		calibration += 10;
	}
	if (calibration < 10) {
		calibration = 10;
	}
	else if (calibration > 46) {
		calibration = 46;
	}
	p_controller->m_unk0x348 = 0;
	p_controller->m_unk0x34c = 0;
	p_controller->m_graphicsQuality = (calibration - 10) / 4 + 1;
	for (LegoU32 j = 0; j < 4; ++j) {
		p_controller->m_unk0x80bc[j] = 0;
	}
	p_controller->m_unk0x80cc = 0;
	for (LegoU32 k = 0; k < 5; ++k) {
		p_controller->m_unk0x80d0[k] = 1;
	}
	for (LegoU32 profile = 0; profile < 2; ++profile) {
		InitializeControllerBindingProfile(p_controller, NULL, &p_controller->m_pairGroups[profile], 1, profile);
	}
	p_controller->m_selection.UnlockWorld(0);
	p_controller->m_selection.UnlockDefaults(1, 0);
	for (LegoU32 item = 0; item < 40; ++item) {
		*(LegoU32*) ((LegoU8*) p_controller + 0xa7c + item * 0x2f4) = 0;
	}
	for (LegoU32 block = 0; block < 2; ++block) {
		for (LegoU32 word = 0; word < 4; ++word) {
			p_controller->m_blockState[block][word] = (LegoU32) -1;
		}
	}
	StartControllerMusic(p_controller, NULL, 0, -1);
	ApplySavedAudioLevels(p_controller, NULL);
	ApplySavedAudioQuality(p_controller, NULL);
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_setParameters(audio, NULL, 0.0f, 0.0f);
}
