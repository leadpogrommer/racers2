#include "audio/service/audioservice.h"
#include "core/application/commandline.h"
#include "core/strings/charbuffer128.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/controller/gamecontrollerinitialization.h"
#include "game/session/gamecontrollersession.h"
#include "game/state/gamepreferences.h"

#include <stdio.h>

// STUB: LEGORACERS2 0x0040a290
void __fastcall StartControllerMusic(GameController*, void*, LegoS32 p_mode, LegoS32 p_track)
{
	if (FUN_004b4a60("_nomusic")) {
		return;
	}
	AudioService* audio = GetAudioService(TRUE);
	CharBuffer128 path;
	const LegoChar* mediaPath = "";
	GetRegistryMediaPath(&mediaPath, 0);
	const LegoChar* music;
	if (p_mode == 1 || p_mode == 4) {
		switch (g_gameControllerSession->m_selectedMode) {
		case 1:
			music = "arctic";
			break;
		case 2:
			music = "dino";
			break;
		case 3:
			music = "mars";
			break;
		case 4:
			music = "xalax";
			break;
		default:
			music = "sandybay";
			break;
		}
	}
	else {
		music = p_mode == 3 ? "bonus" : "menu";
	}
	sprintf(path.m_data, "%sgame data\\music\\%s", mediaPath, music);
	audio->m_vtable->m_stopCdTrack(audio, NULL);
	audio->m_vtable->m_setDigitalAudioPath(audio, NULL, path.m_data, 1);
	if (p_track > 0) {
		audio->m_vtable->m_playCdTrack(audio, NULL, p_track);
	}
	union GainBits {
		LegoFloat value;
		LegoU32 bits;
	} gain;
	gain.value = g_gameControllerSettings->m_musicGain;
	audio->m_vtable->m_setMixerVolume(audio, NULL, gain.value, gain.bits);
}
