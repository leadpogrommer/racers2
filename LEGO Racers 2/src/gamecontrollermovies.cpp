#include "audioservice.h"
#include "charbuffer128.h"
#include "gamecontroller.h"
#include "gameframe.h"
#include "gamepreferences.h"
#include "movieplayer.h"

#include <stdio.h>

// STUB: LEGORACERS2 0x0040a440
void GameController::PlayStartupMovies()
{
	m_unk0x68 = 1;
	const LegoChar* movies[3] = {
		"Game Data\\Movies\\LEGO splash.mpg",
		"Game Data\\Movies\\ATD splash.mpg",
		"Game Data\\Movies\\LR2 splash.mpg"
	};
	const LegoChar* prefix = "";
	GetRegistryMediaPath(&prefix, 0);
	AudioService* audio = GetAudioService(TRUE);
	if (audio->m_vtable->m_getStatus(audio, NULL) == 0) {
		audio->m_vtable->m_shutdown(audio, NULL);
	}
	MoviePlayer* player = GetMoviePlayer();
	CharBuffer128 path;
	for (LegoU32 i = 0; i < 3; ++i) {
		sprintf(path.m_data, "%s%s", prefix, movies[i]);
		player->m_vtable->m_play(player, NULL, path.m_data, i != 0);
	}
	GetInitializedSimulationScheduler()->Reset(0);
	audio->m_vtable->m_restart(audio, NULL);
}
