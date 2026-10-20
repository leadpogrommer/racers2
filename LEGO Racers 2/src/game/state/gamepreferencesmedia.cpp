#include "core/strings/charbuffer128.h"
#include "game/state/gamepreferences.h"
#include "io/files/filemanager.h"
#include "io/files/winfileutil.h"

#include <stdio.h>
#include <string.h>

// GLOBAL: LEGORACERS2 0x005542f4
static LegoChar g_registryMediaPath[0x100];

// STUB: LEGORACERS2 0x0040a600
LegoBool32 __stdcall GetRegistryMediaPath(const LegoChar** p_path, LegoU32 p_useCdPath)
{
	HKEY key = NULL;
	DWORD disposition = 0;
	DWORD pathBytes = 0x100;
	const LegoChar* valueName = p_useCdPath ? "CDpath" : "moviespath";
	LegoBool32 result = FALSE;

	RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		"SOFTWARE\\LEGO media\\games\\LEGO Racers 2",
		0,
		NULL,
		0,
		0xf003f,
		NULL,
		&key,
		&disposition
	);

	if (disposition == 2 &&
		RegQueryValueEx(key, valueName, NULL, NULL, (BYTE*) g_registryMediaPath, &pathBytes) == ERROR_SUCCESS) {
		LegoU32 length = (LegoU32) strlen(g_registryMediaPath);
		if (length != 0 && g_registryMediaPath[length - 1] != '\\') {
			g_registryMediaPath[length] = '\\';
			g_registryMediaPath[length + 1] = '\0';
		}
		*p_path = g_registryMediaPath;
		result = TRUE;
	}

	RegCloseKey(key);
	return result;
}

// STUB: LEGORACERS2 0x0040a6d0
LegoBool32 HasCdLanguageExport()
{
	const LegoChar* mediaPath;
	LegoBool32 result = FALSE;
	if (GetRegistryMediaPath(&mediaPath, 1)) {
		CharBuffer128 path;
		sprintf(path.m_data, "%slanguage\\export.Swe", mediaPath);

		FileManager* fileManager = GetFileManager();
		fileManager->SetFlags(1, TRUE);
		if (ManagedPathExists(path.m_data)) {
			result = GetPathDriveKind(path.m_data) == 4;
		}
		fileManager->SetFlags(1, FALSE);
	}
	return result;
}
