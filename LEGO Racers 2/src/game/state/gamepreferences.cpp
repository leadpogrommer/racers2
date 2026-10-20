#include "game/state/gamepreferences.h"

#include "core/strings/charbuffer128.h"
#include "io/files/userdatapath.h"
#include "render/capture/screenshot.h"

#include <string.h>

// STUB: LEGORACERS2 0x0040a540
LegoBool32 __stdcall AccessGameLanguagePreference(LegoU32 p_write, LegoS32* p_language)
{
	LegoBool32 result = FALSE;
	HKEY key = NULL;
	DWORD disposition = 0;
	RegCreateKeyEx(
		HKEY_CURRENT_USER,
		"Software\\LEGO media\\games\\LEGO Racers 2",
		0,
		NULL,
		0,
		KEY_ALL_ACCESS,
		NULL,
		&key,
		&disposition
	);
	if (p_write == 0) {
		if (disposition == REG_OPENED_EXISTING_KEY) {
			DWORD size = 4;
			LegoS32 language;
			if (RegQueryValueEx(key, "Language", NULL, NULL, (BYTE*) &language, &size) == ERROR_SUCCESS) {
				*p_language = language;
				if (language >= 0 && language < 9) {
					result = TRUE;
				}
			}
		}
	}
	else if (*p_language >= 0 && *p_language < 9) {
		RegSetValueEx(key, "Language", 0, REG_DWORD, (const BYTE*) p_language, 4);
		result = TRUE;
	}
	RegCloseKey(key);
	return result;
}

// STUB: LEGORACERS2 0x0040a760
void ConfigureGameScreenshotDirectory()
{
	CharBuffer128 directory;
	BuildUserDataPath(directory.m_data, TRUE);
	strcat(directory.m_data, "\\Photos");
	SetScreenshotDirectory(&directory);
}
