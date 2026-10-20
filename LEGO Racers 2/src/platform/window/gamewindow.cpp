#include "platform/window/gamewindow.h"

#include "audio/service/audioservice.h"
#include "core/application/application.h"
#include "core/runtime/startup.h"
#include "platform/system/platformservices.h"
#include "platform/window/nativewindow.h"

// GLOBAL: LEGORACERS2 0x0067a4fc
LegoU32 g_messageLoopActive;

// GLOBAL: LEGORACERS2 0x0067a500
LegoU32 g_deferredWindowDestruction;

// FUNCTION: LEGORACERS2 0x004c3020
LegoU32 StopNativeGame()
{
	g_messageLoopActive = 0;
	ShutdownPlatformServices();
	return 0;
}

// STUB: LEGORACERS2 0x004c32c0
LRESULT CALLBACK GameWindowProc(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam)
{
	Application* application = GetApplication();
	if (application->m_vtable->m_windowMessage(application, NULL, p_window, p_message, p_wParam, p_lParam) == FALSE) {
		return 0;
	}
	switch (p_message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		StopNativeGame();
		DestroyWindow(g_mainWindow);
		return 0;
	case WM_GETMINMAXINFO:
		FUN_004c33b0(p_window, (MINMAXINFO*) p_lParam);
		return 0;
	case WM_SYSCOMMAND:
		if (p_wParam == SC_SCREENSAVE || p_wParam == SC_MONITORPOWER) {
			return 0;
		}
		break;
	case WM_POWERBROADCAST:
		return 0x424d5144;
	case 0x3b9:
		HandlePrivateAudioMessage(p_wParam, p_lParam);
		return 0;
	}
	return DefWindowProc(p_window, p_message, p_wParam, p_lParam);
}

// STUB: LEGORACERS2 0x004c33f0
void HandlePrivateAudioMessage(WPARAM p_wParam, LPARAM p_lParam)
{
	if (p_wParam == 1) {
		AudioService* audio = GetAudioService(TRUE);
		audio->m_vtable->m_playCdTrack(audio, NULL, -1);
	}
}
