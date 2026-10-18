#ifndef LR2_GAMEPREFERENCES_H
#define LR2_GAMEPREFERENCES_H
#include "types.h"

#include <windows.h>
LegoBool32 __stdcall AccessGameLanguagePreference(LegoU32 p_write, LegoS32* p_language);
void ConfigureGameScreenshotDirectory();
#endif
