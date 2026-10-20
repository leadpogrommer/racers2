#include "io/files/winfileutil.h"

#include <string.h>
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

// STUB: LEGORACERS2 0x004a4bc0
LegoS32 GetPathDriveKind(const LegoChar* p_path)
{
	LegoChar root[4];
	memcpy(root, p_path, 3);
	root[3] = '\0';
	switch (GetDriveType(root)) {
	case DRIVE_REMOVABLE:
		return 0;
	case DRIVE_FIXED:
		return 1;
	case DRIVE_REMOTE:
		return 2;
	case DRIVE_CDROM:
		return 4;
	case DRIVE_RAMDISK:
		return 3;
	default:
		return 5;
	}
}
