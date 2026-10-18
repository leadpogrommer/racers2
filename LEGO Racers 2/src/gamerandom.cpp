#include "gamerandom.h"

#include "inputcontrollermanager.h"

#include <stdlib.h>

// These LCG constants differ from the MSVC CRT rand implementation. Their
// occurrence alone does not identify a third-party library or original class.

// GLOBAL: LEGORACERS2 0x00679e40
LegoU8 g_gameRandomInitializationFlags;
// GLOBAL: LEGORACERS2 0x00679e48
GameRandomState g_gameRandomStateStorage;
// GLOBAL: LEGORACERS2 0x00679e94
GameRandomState* g_gameRandomState;

// Retail4b9a10 is one of the identical bare-RET exit callbacks.
static void GameRandomExit()
{
}

// FUNCTION: LEGORACERS2 0x004b9940
GameFloatRandom* GameFloatRandom::Initialize()
{
	m_multiplier = 1664525;
	m_increment = 1013904223;
	m_state = 0x3f800000;
	m_mantissaMask = 0x7fffff;
	m_frame = 0;
	m_frame = g_inputControllerManager->GetCurrentFrame();
	return this;
}

// FUNCTION: LEGORACERS2 0x004b9980
GameIntegerRandom* GameIntegerRandom::Initialize()
{
	m_multiplier = 1664525;
	m_increment = 1013904223;
	m_state = 0;
	m_state = g_inputControllerManager->GetCurrentFrame();
	return this;
}

// FUNCTION: LEGORACERS2 0x004b99b0
GameRandomState* GetGameRandomState()
{
	if (g_gameRandomState == NULL) {
		if ((g_gameRandomInitializationFlags & 1) == 0) {
			g_gameRandomInitializationFlags |= 1;
			g_gameRandomStateStorage.m_floatStreams[0].Initialize();
			g_gameRandomStateStorage.m_floatStreams[1].Initialize();
			g_gameRandomStateStorage.m_integerStreams[0].Initialize();
			g_gameRandomStateStorage.m_integerStreams[1].Initialize();
			atexit(GameRandomExit);
		}
		g_gameRandomState = &g_gameRandomStateStorage;
	}
	return g_gameRandomState;
}
