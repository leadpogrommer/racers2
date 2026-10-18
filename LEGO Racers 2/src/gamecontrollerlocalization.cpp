#include "gamecontrollerinitialization.h"

// GLOBAL: LEGORACERS2 0x0053e3c4
const LegoChar g_gameLanguageSuffixes[9][4] = {"dan", "dut", "bri", "ame", "fre", "ger", "ita", "spa", "swe"};

// GLOBAL: LEGORACERS2 0x0055dfe0
CharBuffer128 g_gameLocalizationPath;

// STUB: LEGORACERS2 0x00406f90
void InitializeGameLocalizationPath()
{
	g_gameLocalizationPath = CharBuffer128("Game Data\\Text\\Lego Strings");
}

struct GameLocalizationPathInitialization {
	GameLocalizationPathInitialization() { InitializeGameLocalizationPath(); }
};
static GameLocalizationPathInitialization g_gameLocalizationPathInitialization;
