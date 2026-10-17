#ifndef LR2_USERDATAPATH_H
#define LR2_USERDATAPATH_H

#include "types.h"

#include <windows.h>

// Builds the per-user "Games/LEGO Racers 2" directory path. When requested,
// creates both appended directories. The caller supplies a writable buffer.
void WINAPI BuildUserDataPath(LegoChar* p_path, LegoBool32 p_createDirectories);

#endif // LR2_USERDATAPATH_H
