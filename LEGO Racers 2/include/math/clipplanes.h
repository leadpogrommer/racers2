#ifndef LR2_CLIPPLANES_H
#define LR2_CLIPPLANES_H

#include "types.h"

extern LegoFloat g_modelClipNear;
extern LegoFloat g_modelClipFar;
void __fastcall SetClipPlanesForFoyerTarget(void*, void*, LegoU32);
void __fastcall SetFoyerSceneTargetAnimation(void*, void*, LegoU32);

#endif // LR2_CLIPPLANES_H
