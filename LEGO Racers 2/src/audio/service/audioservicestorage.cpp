#include "audio/service/audioservice.h"

// GLOBAL: LEGORACERS2 0x00653124
AudioBackendStateView* g_audioBackend = NULL;

// GLOBAL: LEGORACERS2 0x00653128
HWND g_audioWindow = NULL;

// GLOBAL: LEGORACERS2 0x0065312c
AudioService* g_audioService = NULL;

// The retail data byte is initially one and is cleared after the first
// singleton creation so the process-exit callback is registered once.
// GLOBAL: LEGORACERS2 0x00549f50
LegoU32 g_audioServiceExitCallbackRegistered = TRUE;
