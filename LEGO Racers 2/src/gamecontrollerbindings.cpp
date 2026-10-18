#include "gamecontrollerinitialization.h"

// STUB: LEGORACERS2 0x0040a000
void __fastcall InitializeControllerBindingProfile(
	GameController*,
	void*,
	GameControllerPairGroups* p_profile,
	LegoU32 p_enabled,
	LegoU32 p_index
)
{
	p_profile->m_index = p_index;
	p_profile->m_enabled = p_enabled;
	p_profile->m_unk0x08 = 1;
	LegoU32 i;
	for (i = 0; i < 9; ++i) {
		p_profile->m_firstGroups[i].m_first = 0;
		p_profile->m_secondGroups[i].m_first = 0;
	}
	for (i = 0; i < 2; ++i) {
		p_profile->m_pairs[i].m_first = 0;
	}
	p_profile->m_secondGroups[5].m_second = 1;
	p_profile->m_secondGroups[7].m_second = 1;
	p_profile->m_secondGroups[8].m_second = 1;
	p_profile->m_secondGroups[8].m_first = 1;
	p_profile->m_secondGroups[0].m_second = 0;
	p_profile->m_secondGroups[1].m_second = 3;
	p_profile->m_secondGroups[2].m_second = 4;
	p_profile->m_secondGroups[3].m_second = 6;
	p_profile->m_secondGroups[4].m_second = 7;
	p_profile->m_secondGroups[6].m_second = 2;
	p_profile->m_secondGroups[7].m_first = 2;
	const LegoU32 firstKeys[11] = {0xcb, 0xcd, 200, 0x39, 0xd0, 2, 3, 4, 5, 6, 7};
	const LegoU32 secondKeys[11] = {0x4b, 0x4d, 0x48, 0x4c, 0x50, 0xd2, 199, 0xc9, 0xd3, 0xcf, 0xd1};
	const LegoU32* keys = p_index == 0 ? firstKeys : secondKeys;
	p_profile->m_pairs[0].m_second = keys[0];
	p_profile->m_pairs[1].m_second = keys[1];
	for (i = 0; i < 9; ++i) {
		p_profile->m_firstGroups[i].m_second = keys[i + 2];
	}
}
