#include "render/capture/screenshot.h"

// GLOBAL: LEGORACERS2 0x00679ef8
CharBuffer128 g_screenshotDirectory;

// GLOBAL: LEGORACERS2 0x00679f80
LegoBool32 g_screenshotEnabled = FALSE;

// FUNCTION: LEGORACERS2 0x004bb1f0
void SetScreenshotDirectory(const CharBuffer128* p_directory)
{
	g_screenshotDirectory = *p_directory;
	g_screenshotEnabled = TRUE;
}
