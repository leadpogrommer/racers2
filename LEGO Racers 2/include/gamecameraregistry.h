#ifndef LR2_GAMECAMERAREGISTRY_H
#define LR2_GAMECAMERAREGISTRY_H

#include "gamemanager.h"

LegoS32 CountActiveGameCameras();
GameObject* __fastcall FindActiveGameCamera(GameManager*, void*, LegoS32 p_index);

#endif
