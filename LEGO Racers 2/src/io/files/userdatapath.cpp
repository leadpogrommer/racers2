#include "io/files/userdatapath.h"

#include <string.h>
#include <windows.h>

// VC6's Win32 headers do not consistently declare this optional Shell API.
extern "C" __declspec(dllimport) long WINAPI
SHGetFolderPathA(HWND p_owner, int p_folder, HANDLE p_token, DWORD p_flags, LPSTR p_path);

// FUNCTION: LEGORACERS2 0x0040a7d0
void WINAPI BuildUserDataPath(LegoChar* p_path, LegoBool32 p_createDirectories)
{
	SHGetFolderPathA(NULL, 0x8005, NULL, 0, p_path);
	strcat(p_path, "\\Games");
	if (p_createDirectories) {
		CreateDirectory(p_path, NULL);
	}
	strcat(p_path, "\\LEGO Racers 2");
	if (p_createDirectories) {
		CreateDirectory(p_path, NULL);
	}
}
