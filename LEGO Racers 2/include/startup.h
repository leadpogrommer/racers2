#ifndef LR2_STARTUP_H
#define LR2_STARTUP_H

#include <windows.h>

extern HINSTANCE g_hInstance;
extern WNDPROC g_startupWndProc;
extern LPSTR g_commandLine;

BOOL FUN_004b4690(HINSTANCE p_instance, WNDPROC p_wndProc, LPSTR p_commandLine);
LONG WINAPI FUN_004c32b0(EXCEPTION_POINTERS* p_exception);

// The identical message-policy body shares the rebuilt exception-filter code.
// FUNCTION: LEGORACERS2 0x004c32b0 FOLDED
// FUN_004c32b0
LONG FUN_004c33b0(HWND p_window, MINMAXINFO* p_info);

#endif // LR2_STARTUP_H
