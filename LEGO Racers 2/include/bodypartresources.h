#ifndef LR2_BODYPARTRESOURCES_H
#define LR2_BODYPARTRESOURCES_H

#include "bodypartmanager.h"

void __fastcall LoadCharacterVisualHeadModels(BodypartManager*, void*);
void __fastcall ReleaseCharacterVisualHeadModels(BodypartManager*, void*);
void __fastcall SetBodypartLegTextureLoaded(BodypartManager*, void*, LegoS32, LegoU32);
void __fastcall SetBodypartTorsoTextureLoaded(BodypartManager*, void*, LegoS32, LegoU32, LegoU32);
void __fastcall InitializeSecondaryCharacterResources(void*, void*);
void __fastcall ReleaseSecondaryCharacterResources(void*, void*);

#endif
