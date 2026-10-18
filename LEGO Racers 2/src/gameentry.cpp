#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include "gameentry.h"

#include "application.h"
#include "audioservice.h"
#include "commandline.h"
#include "gamewindow.h"
#include "memoryheap.h"
#include "nativewindow.h"
#include "platformservices.h"
#include "screenshot.h"
#include "startup.h"

#include <objbase.h>

// The actual frame path must link before replacing the temporary entry point.
void AdvanceGameFrame(); // retail 0x004a3aa0
extern LegoBool32 g_heapInitializationSucceeded;

// STUB: LEGORACERS2 0x004c3040
int WINAPI RunGameInstance(HINSTANCE p_instance, HINSTANCE p_previous, LPSTR p_commandLine, int p_showCommand)
{
	HANDLE instanceMutex = CreateMutex(NULL, FALSE, "Saracen II");
	if (instanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
		Application* application = GetApplication();
		const LegoChar* title = application->m_vtable->m_title(application, NULL);
		HWND previousWindow = FindWindow("Saracen II", title);
		if (previousWindow != NULL) {
			if (IsIconic(previousWindow)) {
				ShowWindow(previousWindow, SW_RESTORE);
			}
			SetForegroundWindow(previousWindow);
		}
		return 0;
	}

	SetUnhandledExceptionFilter(FUN_004c32b0);
	CoInitializeEx(NULL, 0);
	GetMemoryHeap();
	if (g_heapInitializationSucceeded == FALSE) {
		return 0;
	}
	Application* application = GetApplication();
	application->m_vtable->m_beforeStartup(application, NULL);
	if (FUN_004b4690(p_instance, GameWindowProc, p_commandLine) == FALSE) {
		return 0;
	}
	if (FUN_004b4a60("/backlevel") != FALSE) {
		SetCurrentDirectory("..\\");
	}
	if (InitializePlatformServices() == FALSE) {
		return 0;
	}
	application = GetApplication();
	application->m_vtable->m_afterStartup(application, NULL);
	GetAudioService(TRUE);
	if (FUN_004b4a60("/highqualitysound") == FALSE || FUN_004b4a60("/lowqualitysound") == FALSE) {
		if (FUN_004b4a60("/highqualitysound") != FALSE) {
			AudioService* audio = GetAudioService(TRUE);
			audio->m_vtable->m_setQuality(audio, NULL, 2);
		}
		else if (FUN_004b4a60("/lowqualitysound") != FALSE) {
			AudioService* audio = GetAudioService(TRUE);
			audio->m_vtable->m_setQuality(audio, NULL, 0);
		}
	}

	GetAsyncKeyState(VK_F10);
	MSG message;
	PeekMessage(&message, NULL, 0, 0, PM_NOREMOVE);
	g_messageLoopActive = 1;
	while (message.message != WM_QUIT) {
		application = GetApplication();
		BOOL received;
		if (application->m_vtable->m_pollMessages(application, NULL) == FALSE) {
			received = GetMessage(&message, NULL, 0, 0);
		}
		else {
			received = PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
		}
		LegoBool32 update;
		if (received != FALSE) {
			application = GetApplication();
			if (application->m_vtable->m_dispatchMessage(application, NULL, &message) != FALSE) {
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			application = GetApplication();
			update = application->m_vtable->m_updateAfterMessage(application, NULL);
		}
		else {
			update = TRUE;
		}
		if (update != FALSE && g_messageLoopActive != 0) {
			if ((GetAsyncKeyState(VK_F10) & 1) != 0) {
				CaptureScreenshot();
			}
			AdvanceGameFrame();
		}
		if (g_deferredWindowDestruction == 1) {
			StopNativeGame();
			DestroyWindow(g_mainWindow);
			g_deferredWindowDestruction = 0;
		}
	}
	CoUninitialize();
	return (int) message.wParam;
}
