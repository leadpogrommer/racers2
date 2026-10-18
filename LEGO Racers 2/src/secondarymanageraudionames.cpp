#include "secondarymanageraudionames.h"

#include <new>

// GLOBAL: LEGORACERS2 0x00561a40
LegoU32 g_secondaryManagerAudioNameStorage[60];

// STUB: LEGORACERS2 0x00426e50
static void InitializeSecondaryManagerAudioNames()
{
	new ((void*) &g_secondaryManagerAudioNameStorage[0]) CharBuffer24("SecSoundSelect");
	new ((void*) &g_secondaryManagerAudioNameStorage[6]) CharBuffer24("SecSoundCancel");
	new ((void*) &g_secondaryManagerAudioNameStorage[12]) CharBuffer24("SecSoundInvalid");
	new ((void*) &g_secondaryManagerAudioNameStorage[18]) CharBuffer24("SecSoundLeftRight");
	new ((void*) &g_secondaryManagerAudioNameStorage[24]) CharBuffer24("SecSoundCheatActivated");
	new ((void*) &g_secondaryManagerAudioNameStorage[30]) CharBuffer24("SecSoundUpDown");
	new ((void*) &g_secondaryManagerAudioNameStorage[36]) CharBuffer24("SecSoundRotate");
	new ((void*) &g_secondaryManagerAudioNameStorage[42]) CharBuffer24("SecSoundNewBrick");
	new ((void*) &g_secondaryManagerAudioNameStorage[48]) CharBuffer24("SecSoundDelete");
	new ((void*) &g_secondaryManagerAudioNameStorage[54]) CharBuffer24("SecSoundCycle");
}

struct InitializeSecondaryManagerAudioNamesAtStartup {
	InitializeSecondaryManagerAudioNamesAtStartup() { InitializeSecondaryManagerAudioNames(); }
};

static InitializeSecondaryManagerAudioNamesAtStartup g_initializeSecondaryManagerAudioNamesAtStartup;
