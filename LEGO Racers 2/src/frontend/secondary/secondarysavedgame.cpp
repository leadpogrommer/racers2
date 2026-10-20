#include "frontend/secondary/secondarysavedgame.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00561b60
SecondarySavedGame g_secondarySavedGame;

// The reset is inlined in retail menu action420d30.
void ResetSecondarySavedGameBuffer()
{
	memset(&g_secondarySavedGame, 0, sizeof(g_secondarySavedGame));
}
