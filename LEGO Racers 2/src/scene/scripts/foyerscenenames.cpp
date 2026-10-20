#include "scene/scripts/foyerscenenames.h"

// GLOBAL: LEGORACERS2 0x00541da0
const LegoChar g_worldCompletedScene[] = "world_completed";
// GLOBAL: LEGORACERS2 0x00541db0
const LegoChar g_foyerIntroScene[] = "foyer_intro";

// GLOBAL: LEGORACERS2 0x0055e568
CharBuffer24 g_foyerSceneNames[7];

// STUB: LEGORACERS2 0x004109c0
void InitializeFoyerSceneNames()
{
	g_foyerSceneNames[0] = CharBuffer24("intro");
	g_foyerSceneNames[1] = CharBuffer24("intro2");
	g_foyerSceneNames[2] = CharBuffer24("intro3");
	g_foyerSceneNames[3] = CharBuffer24("workmanchallenge");
	g_foyerSceneNames[4] = CharBuffer24("postmanchallenge");
	g_foyerSceneNames[5] = CharBuffer24("firemanchallenge");
	g_foyerSceneNames[6] = CharBuffer24("policemanchallenge");
}

struct InitializeFoyerSceneNamesAtStartup {
	InitializeFoyerSceneNamesAtStartup() { InitializeFoyerSceneNames(); }
};
static InitializeFoyerSceneNamesAtStartup g_initializeFoyerSceneNamesAtStartup;
