#include "frontend/foyer/foyerselectionquery.h"

#include "decomp.h"
#include "game/controller/gamecontroller.h"
#include "game/session/gamecontrollersession.h"

DECOMP_SIZE_ASSERT(FoyerProgressThreshold, 0x0c);
// GLOBAL: LEGORACERS2 0x0052f9d0
const FoyerProgressThreshold g_foyerProgressThresholds[5] = {{0, 0, 0}, {4, 9, 0}, {4, 4, 0}, {4, 8, 0}, {4, 16, 3}};
namespace
{
LegoU32 IsFoyerSelectionBitSet(const LegoU32* p_flags, LegoU32 p_bit)
{
	return (p_flags[0] & (1u << (p_bit & 31))) != 0;
}

void SetFoyerSelectionBit(LegoU32* p_flags, LegoU32 p_bit)
{
	p_flags[0] |= 1u << (p_bit & 31);
}

LegoU32 HasFoyerTrackProgress(const LegoU32* p_progress, LegoU32 p_world, LegoU32 p_track)
{
	LegoU32 bit = p_track - 1 + p_world * 4;
	return (p_progress[0] >> (bit & 31)) & 1;
}

LegoU32 HasFoyerBonusProgress(const LegoU32* p_progress, LegoU32 p_world, LegoU32 p_bonus)
{
	LegoU32 bit = p_bonus + p_world * 3;
	return (p_progress[1] >> (bit & 31)) & 1;
}

LegoS32 MapFoyerProgressIndex(LegoU32 p_index)
{
	if (p_index == 1) {
		return 0;
	}
	if (p_index == 2) {
		return 2;
	}
	if (p_index == 3) {
		return 1;
	}
	return -1;
}

LegoU32 HasFoyerModeProgress(const LegoU32* p_flags, LegoU32 p_mode)
{
	return (p_flags[0] >> ((p_mode * 5 + 4) & 31)) & 1;
}

LegoU32 CountFoyerCompletedProgress(const LegoU32* p_progress)
{
	LegoU32 count = 0;
	for (LegoU32 world = 0; world < 5; ++world) {
		for (LegoU32 track = 1; track <= 4; ++track) {
			count += HasFoyerTrackProgress(p_progress, world, track);
		}
	}
	return count;
}

LegoU32 CountFoyerUnlockedBonus(const LegoU32* p_progress)
{
	LegoU32 count = 0;
	for (LegoU32 world = 0; world < 5; ++world) {
		for (LegoU32 bonus = 0; bonus < 3; ++bonus) {
			count += HasFoyerBonusProgress(p_progress, world, bonus);
		}
	}
	return count;
}
} // namespace

// STUB: LEGORACERS2 0x0040b120
void __fastcall SetFoyerModeUnlocked(void* p_flags, void*, LegoU32 p_mode)
{
	SetFoyerSelectionBit((LegoU32*) ((LegoU8*) p_flags + 0x1c), p_mode);
}

// STUB: LEGORACERS2 0x0040b140
LegoU32 __fastcall IsFoyerModeUnlocked(void* p_flags, void*, LegoU32 p_mode)
{
	return IsFoyerSelectionBitSet((const LegoU32*) ((const LegoU8*) p_flags + 0x1c), p_mode);
}

// STUB: LEGORACERS2 0x00410cb0
LegoU32 __fastcall CountFoyerTrackAndBonusProgress(const LegoU32* p_progress, void*)
{
	return CountFoyerCompletedProgress(p_progress) + CountFoyerUnlockedBonus(p_progress);
}

// STUB: LEGORACERS2 0x00410cd0
LegoU32 __fastcall CountFoyerTrackProgress(const LegoU32* p_progress, void*)
{
	return CountFoyerCompletedProgress(p_progress);
}

// STUB: LEGORACERS2 0x00410d10
LegoU32 __fastcall CountFoyerBonusProgress(const LegoU32* p_progress, void*)
{
	return CountFoyerUnlockedBonus(p_progress);
}

// STUB: LEGORACERS2 0x00410d70
LegoU32 __fastcall IsFoyerTrackProgressSet(const LegoU32* p_progress, void*, LegoU32 p_world, LegoU32 p_track)
{
	return HasFoyerTrackProgress(p_progress, p_world, p_track);
}

// STUB: LEGORACERS2 0x00410dc0
LegoU32 __fastcall IsFoyerBonusProgressSet(const LegoU32* p_progress, void*, LegoU32 p_world, LegoU32 p_bonus)
{
	return HasFoyerBonusProgress(p_progress, p_world, p_bonus);
}

// STUB: LEGORACERS2 0x00410de0
LegoS32 __fastcall GetFoyerProgressModeIndex(void*, void*, LegoU32 p_mode)
{
	return MapFoyerProgressIndex(p_mode);
}

// STUB: LEGORACERS2 0x00410e70
LegoU32 __fastcall IsFoyerModeProgressSet(const LegoU32* p_flags, void*, LegoU32 p_mode)
{
	return HasFoyerModeProgress(p_flags, p_mode);
}

// STUB: LEGORACERS2 0x00410eb0
LegoU32 __fastcall IsFoyerModeProgressMapped(const LegoU32* p_flags, void*, LegoU32 p_mode)
{
	LegoS32 index = MapFoyerProgressIndex(p_mode);
	return index < 0 ? 0 : HasFoyerModeProgress(p_flags, (LegoU32) index);
}

// STUB: LEGORACERS2 0x00410ee0
LegoU32 __fastcall CountFoyerModeProgress(const LegoU32* p_flags, void*)
{
	return HasFoyerModeProgress(p_flags, 0) + HasFoyerModeProgress(p_flags, 1) + HasFoyerModeProgress(p_flags, 2);
}

// STUB: LEGORACERS2 0x00410f50
LegoU32 __fastcall GetFoyerModeProgressValue(const LegoU32* p_values, void*, LegoU32 p_mode)
{
	return (p_values[0] >> ((p_mode * 5) & 31)) & 0xf;
}

// STUB: LEGORACERS2 0x00410f70
LegoFloat __fastcall GetFoyerModeProgressFraction(const LegoU32* p_values, void*, LegoU32 p_mode)
{
	LegoFloat completed = IsFoyerModeProgressSet(p_values, NULL, p_mode) != 0 ? 0.2f : 0.0f;
	return GetFoyerModeProgressValue(p_values, NULL, p_mode) * 0.08f + completed;
}

// STUB: LEGORACERS2 0x00411a30
LegoS32 __fastcall GetFoyerSceneIndex(void* p_settings, void*, LegoU32 p_mode)
{
	GameControllerSession* session = GetGameControllerSession();
	LegoU8* settings = (LegoU8*) p_settings;
	if (session->m_unk0x04 == 0 || *(LegoU32*) ((LegoU8*) g_gameController.m_foyerManager + 0xe4) != 0) {
		return -1;
	}

	LegoS32 result = -1;
	LegoU32 progress = IsFoyerModeSelected(p_settings, NULL, p_mode);
	if (progress == 0) {
		if (IsFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode) == 0) {
			if (p_mode == 0) {
				*(LegoU32*) (settings + 0x1de4) = 1;
				SetFoyerModeUnlocked(settings + 0x1e04, NULL, 0);
				return 0;
			}
			result = 12;
		}
	}
	else if (p_mode == 4) {
		if (CountFoyerTrackAndBonusProgress((const LegoU32*) (settings + 0x1df4), NULL) < 0x23) {
			SetFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode);
			return 13;
		}
	}
	else if (*(LegoU32*) (settings + 0x1e00) == 0) {
		SetFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode);
		return 13;
	}
	SetFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode);
	return result;
}

// FUNCTION: LEGORACERS2 0x00411c80
LegoU32 __fastcall IsFoyerModeSelected(void* p_settings, void*, LegoU32 p_mode)
{
	LegoU8* settings = (LegoU8*) p_settings;
	if (p_mode == 0) {
		return 0;
	}
	if (p_mode == 4) {
		return *(LegoU32*) (settings + 0x1e00);
	}
	return IsFoyerModeProgressMapped((const LegoU32*) (settings + 0x1dfc), NULL, p_mode);
}

// STUB: LEGORACERS2 0x00411cb0
LegoS32 __fastcall GetFoyerSelectedObjectIndex(void* p_settings, void*, LegoU32 p_mode)
{
	LegoU8* settings = (LegoU8*) p_settings;
	LegoU32 progress = CountFoyerTrackProgress((const LegoU32*) (settings + 0x1df4), NULL);
	const FoyerProgressThreshold* thresholds = &g_foyerProgressThresholds[p_mode];
	if (progress < thresholds->m_tracks) {
		return 9;
	}
	if (CountFoyerTrackAndBonusProgress((const LegoU32*) (settings + 0x1df4), NULL) < thresholds->m_tracksAndBonuses) {
		return 10;
	}
	if (CountFoyerModeProgress((const LegoU32*) (settings + 0x1dfc), NULL) < thresholds->m_modes) {
		return 11;
	}
	return IsFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode) != 0 ? 7 : 8;
}
