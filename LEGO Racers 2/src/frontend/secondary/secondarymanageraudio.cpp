#include "audio/service/audioservice.h"
#include "core/application/commandline.h"
#include "game/controller/gamecontrollerconfig.h"

// STUB: LEGORACERS2 0x00427910
void __fastcall ApplySavedAudioLevels(void*, void*)
{
	AudioService* audio = GetAudioService(TRUE);
	GameControllerSettings* settings = GetGameControllerSettings();
	audio->m_vtable->m_setVolumes(audio, NULL, settings->m_effectGain, settings->m_effectGain);

	audio = GetAudioService(TRUE);
	LegoFloat musicGain = settings->m_musicGain;
	union GainBits {
		LegoFloat m_float;
		LegoU32 m_word;
	} gain;
	gain.m_float = musicGain;
	audio->m_vtable->m_setMixerVolume(audio, NULL, musicGain, gain.m_word);
}

// STUB: LEGORACERS2 0x00427970
void __fastcall ApplySavedAudioQuality(void*, void*)
{
	GameControllerSettings* settings = GetGameControllerSettings();
	LegoU32 quality = settings->m_audioQuality;
	if (FUN_004b4a60("_nosfx") != FALSE) {
		quality = 3;
	}

	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_setQuality(audio, NULL, quality);
}
