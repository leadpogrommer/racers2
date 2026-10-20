#ifndef LR2_CONTROLLERCAMERA_H
#define LR2_CONTROLLERCAMERA_H

#include "gamemanager.h"

LegoU32 __fastcall UpdateControllerFollowCamera(void*, void*, GameObject*, LegoU32*, LegoFloat, LegoFloat);
const LegoU32* __fastcall GetCharacterControllerInput(void*, void*, LegoU32);

#endif
