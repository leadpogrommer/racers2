#include "winfileutil.h"

#include <windows.h>

// FUNCTION: LEGORACERS2 0x004a4b80
LegoBool32 PathExists(const LegoChar* p_path)
{
	WIN32_FIND_DATA findData;
	HANDLE handle = FindFirstFile(p_path, &findData);
	if (handle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	FindClose(handle);
	return TRUE;
}
