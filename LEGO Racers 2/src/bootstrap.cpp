#include "token.h"

#include <windows.h>

// Temporary executable entry point. This is scaffolding, with no retail annotation.
// Calling the initial functions keeps them in the linked image for reccmp.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR p_commandLine, int)
{
	return IsTokenCharacter(p_commandLine[0]) + IsHorizontalSpace(p_commandLine[0]);
}
