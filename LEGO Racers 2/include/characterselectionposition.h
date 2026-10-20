#ifndef LR2_CHARACTERSELECTIONPOSITION_H
#define LR2_CHARACTERSELECTIONPOSITION_H

#include "vector3.h"

LegoBool32 __cdecl ContainsSeatPositionTag(const LegoChar*, const LegoChar*);
LegoBool32 __fastcall FindBuilderSeatPosition(void*, void*, LegoS32, LegoS32, Vector3*, LegoU32);

#endif
