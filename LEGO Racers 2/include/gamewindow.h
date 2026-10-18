#ifndef LR2_GAMEWINDOW_H
#define LR2_GAMEWINDOW_H

#include "types.h"

#include <windows.h>

extern LegoU32 g_messageLoopActive;
extern LegoU32 g_deferredWindowDestruction;

LegoU32 StopNativeGame();
LRESULT CALLBACK GameWindowProc(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam);
void HandlePrivateAudioMessage(WPARAM p_wParam, LPARAM p_lParam);

#endif // LR2_GAMEWINDOW_H
