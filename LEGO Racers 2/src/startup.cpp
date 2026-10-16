#include "startup.h"

#include "types.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x0067a4a8
HINSTANCE g_hInstance = NULL;

// GLOBAL: LEGORACERS2 0x0067a4ec
WNDPROC g_startupWndProc = NULL;

// GLOBAL: LEGORACERS2 0x0067a4f0
LPSTR g_commandLine = NULL;

// This zero-initialized global points to a heap-allocated 0x3c-byte display
// configuration record. Its first two fields are dimensions; the record's
// exact type is not yet recovered.
// GLOBAL: LEGORACERS2 0x0065307c
LegoS32* g_unk0x0065307c = NULL;

// FUNCTION: LEGORACERS2 0x004b4690
BOOL FUN_004b4690(HINSTANCE p_instance, WNDPROC p_wndProc, LPSTR p_commandLine)
{
	g_hInstance = p_instance;
	g_startupWndProc = p_wndProc;
	g_commandLine = p_commandLine;
	_strlwr(p_commandLine);
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004c32b0
LONG WINAPI FUN_004c32b0(EXCEPTION_POINTERS* p_exception)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

// FUNCTION: LEGORACERS2 0x004c33b0
LONG FUN_004c33b0(HWND p_window, MINMAXINFO* p_info)
{
	p_info->ptMinTrackSize.x = g_unk0x0065307c[0];
	p_info->ptMinTrackSize.y = g_unk0x0065307c[1];
	p_info->ptMaxTrackSize.x = g_unk0x0065307c[0];
	p_info->ptMaxTrackSize.y = g_unk0x0065307c[1];
	return 0;
}
