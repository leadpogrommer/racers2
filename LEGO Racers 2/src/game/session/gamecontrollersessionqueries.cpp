#include "game/session/gamecontrollersessionqueries.h"
LegoU32 __fastcall GetFoyerModeProgressValue(const LegoU32*, void*, LegoU32);

#include "game/controller/gamecontrollerconfig.h"
#include "game/session/gamecontrollersession.h"

#include <string.h>
LegoU32 __fastcall CountFoyerTrackAndBonusProgress(const LegoU32*, void*);

// FUNCTION: LEGORACERS2 0x0040afc0
GameControllerScore* __fastcall GetControllerTrackScore(void* p_scores, void*, LegoU32 p_world, LegoU32 p_track)
{
	return (GameControllerScore*) ((LegoU8*) p_scores + (p_world * 5 + p_track) * 0x18 - 0x18);
}

// FUNCTION: LEGORACERS2 0x0040afe0
GameControllerScore* __fastcall GetControllerWorldScore(void* p_scores, void*, LegoU32 p_world, LegoU32 p_index)
{
	return (GameControllerScore*) ((LegoU8*) p_scores + (p_index + p_world * 2 + 0x32) * 0x18);
}

// FUNCTION: LEGORACERS2 0x00410bf0
GameControllerItemBlock* __fastcall GetSecondarySessionCar(void* p_state, void*, LegoU32 p_player, LegoU32 p_carSet)
{
	return (GameControllerItemBlock*) ((LegoU8*) p_state + 0x58 + p_carSet * 0x2f4 + p_player * 0xec4);
}

// FUNCTION: LEGORACERS2 0x00410c10
const LegoU32* __fastcall GetSecondarySessionCharacter(void* p_state, void*, LegoU32 p_player, LegoU32* p_kind)
{
	*p_kind = *(LegoU32*) ((LegoU8*) p_state + 0x50 + p_player * 4);
	return (const LegoU32*) ((LegoU8*) p_state + (p_player + 3) * 0x10);
}

// STUB: LEGORACERS2 0x00410f10
void __fastcall IncrementSecondaryModeProgress(LegoU32* p_progress, void*, LegoU32 p_mode)
{
	LegoU32 value = GetFoyerModeProgressValue(p_progress, NULL, p_mode);
	if ((LegoS32) value < 10) {
		LegoU32 shift = (p_mode * 5) & 31;
		p_progress[0] = (p_progress[0] & ~(0xf << shift)) | (((value + 1) & 0xf) << shift);
	}
}

// FUNCTION: LEGORACERS2 0x00411690
void __fastcall ApplySecondarySessionData(void* p_object, void*)
{
	LegoU32* words = (LegoU32*) p_object;
	memcpy(p_object, (LegoU8*) p_object + 0x1e34, 0x1e34);
	for (LegoS32 player = 0; player < (LegoS32) words[1]; ++player) {
		g_gameControllerSettings->SetPlayerName((GameControllerName*) (words + 2 + player * 5), player);
	}
	g_gameControllerSession->m_selectedMode = words[0x778];
	g_gameControllerSession->m_mode = 7;
	words[0x77a] = 0;
}

// STUB: LEGORACERS2 0x00411760
LegoBool32 __fastcall IsControllerBodyInputEnabled(void* p_state, void*)
{
	GameControllerSession* session = GetGameControllerSession();
	if (session->m_unk0x04 == 0 || session->m_selectedMode != 0 || *(LegoU32*) ((LegoU8*) p_state + 0x1e00) == 0) {
		return FALSE;
	}
	return (LegoS32) CountFoyerTrackAndBonusProgress((const LegoU32*) ((LegoU8*) p_state + 0x1df4), NULL) >= 35;
}
