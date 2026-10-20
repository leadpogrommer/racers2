#include "core/runtime/gameentry.h"

// Adapter for the recovered retail entry path at 0x004c3040.
int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_previous, LPSTR p_commandLine, int p_showCommand)
{
	return RunGameInstance(p_instance, p_previous, p_commandLine, p_showCommand);
}
