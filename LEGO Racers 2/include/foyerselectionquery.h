#ifndef LR2_FOYER_SELECTION_QUERY_H
#define LR2_FOYER_SELECTION_QUERY_H

#include "types.h"

struct FoyerProgressThreshold {
	LegoU32 m_tracks;
	LegoU32 m_tracksAndBonuses;
	LegoU32 m_modes;
};
extern const FoyerProgressThreshold g_foyerProgressThresholds[5];

LegoU32 __fastcall IsFoyerModeSelected(void*, void*, LegoU32);
LegoS32 __fastcall GetFoyerSceneIndex(void* p_settings, void*, LegoU32 p_mode);
LegoS32 __fastcall GetFoyerSelectedObjectIndex(void* p_settings, void*, LegoU32 p_mode);

#endif
