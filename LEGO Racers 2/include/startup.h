#ifndef LR2_STARTUP_H
#define LR2_STARTUP_H

#include <windows.h>

extern HINSTANCE g_hInstance;
extern WNDPROC g_startupWndProc;
extern LPSTR g_commandLine;

BOOL FUN_004b4690(HINSTANCE p_instance, WNDPROC p_wndProc, LPSTR p_commandLine);
LONG WINAPI FUN_004c32b0(EXCEPTION_POINTERS* p_exception);
LONG FUN_004c33b0(HWND p_window, MINMAXINFO* p_info);

#endif // LR2_STARTUP_H
