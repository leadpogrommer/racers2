#include "application.h"

#include "charbuffer128.h"
#include "filemanager.h"
#include "userdatapath.h"
#include "winfile.h"

#include <stdlib.h>
#include <string.h>
#include <windows.h>

struct DisplayConfigRecord {
	LegoU32 m_marker;
	ApplicationDisplaySettings m_display;
};

DECOMP_SIZE_ASSERT(DisplayConfigRecord, 0x10);

// GLOBAL: LEGORACERS2 0x00549788
const LegoChar* g_savedGamesDirectory = "Saved Games";

// GLOBAL: LEGORACERS2 0x0054978c
const LegoChar* g_displayConfigFilename = "LR2.cfg";

// GLOBAL: LEGORACERS2 0x00578640
static LegoU8 g_savedDisplayConfigInitialized = 0;

// GLOBAL: LEGORACERS2 0x00578760
static DisplayConfigRecord g_savedDisplayConfig;

static void OnSavedDisplayConfigExit();

// FUNCTION: LEGORACERS2 0x0040ec90
ApplicationDisplaySettings* Application::GetDisplaySettings()
{
	return &m_display;
}

// FUNCTION: LEGORACERS2 0x0049a6a0
LegoU32 Application::GetUnknownFlag()
{
	return m_unk0x10;
}

// STUB: LEGORACERS2 0x0049b5d0
void Application::LoadDisplayConfig()
{
	m_display.m_width = 640;
	m_display.m_height = 480;
	m_display.m_bitDepth = 16;
	GetFileManager()->SetFlags(1, TRUE);

	CharBuffer128 path;
	BuildUserDataPath(path.m_data, FALSE);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_savedGamesDirectory);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_displayConfigFilename);

	if (ManagedPathExists(path.m_data)) {
		WinFile file(path.m_data, 0, NULL);
		if (file.GetManagedSize() == sizeof(DisplayConfigRecord)) {
			DisplayConfigRecord record;
			record.m_display.m_width = 640;
			record.m_display.m_height = 480;
			record.m_display.m_bitDepth = 32;
			file.Read(&record, sizeof(record), 1);
			file.CloseManaged();
			if (record.m_marker == 0xc0f1c0f1) {
				m_display = record.m_display;
			}
		}
	}
	GetFileManager()->SetFlags(1, FALSE);
}

// STUB: LEGORACERS2 0x0049b780
void Application::SaveDisplayConfig()
{
	GetFileManager()->SetFlags(1, TRUE);
	CharBuffer128 path;
	BuildUserDataPath(path.m_data, FALSE);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_savedGamesDirectory);
	CreateDirectory(path.m_data, NULL);
	strcat(path.m_data, "\\");
	strcat(path.m_data, g_displayConfigFilename);

	if ((g_savedDisplayConfigInitialized & 1) == 0) {
		g_savedDisplayConfigInitialized |= 1;
		g_savedDisplayConfig.m_display.m_width = 640;
		g_savedDisplayConfig.m_display.m_height = 480;
		g_savedDisplayConfig.m_display.m_bitDepth = 32;
		atexit(OnSavedDisplayConfigExit);
	}
	g_savedDisplayConfig.m_marker = 0xc0f1c0f1;
	g_savedDisplayConfig.m_display = m_display;
	WinFile file(path.m_data, 1, NULL);
	file.WriteExact(&g_savedDisplayConfig, sizeof(g_savedDisplayConfig));
	file.CloseManaged();
	GetFileManager()->SetFlags(1, FALSE);
}

// Retail 0x0049b930: bare RET, folded with the heap exit callback in this build.
// reccmp supports one original address per rebuilt address; the heap callback
// retains that shared mapping, and this retail extent is inventory-only.
static void OnSavedDisplayConfigExit()
{
}
