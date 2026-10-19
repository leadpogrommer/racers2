#ifndef LR2_FOYER_SELECTION_QUERY_H
#define LR2_FOYER_SELECTION_QUERY_H

#include "types.h"

LegoU32 __fastcall IsFoyerModeSelected(void*, void*, LegoU32);
LegoS32 __fastcall GetFoyerSceneIndex(void* p_settings, void*, LegoU32 p_mode);
LegoS32 __fastcall GetFoyerSelectedObjectIndex(void* p_settings, void*, LegoU32 p_mode);

#endif
