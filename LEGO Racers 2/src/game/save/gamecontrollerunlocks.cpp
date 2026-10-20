#include "game/controller/gamecontroller.h"

// STUB: LEGORACERS2 0x0040b0a0
void GameControllerSelection::UnlockWorld(LegoU32 p_world)
{
	m_values[0] |= 1u << (p_world & 31);
	LegoS32 i = 0;
	while (i < 5 && m_values[i + 2] != -1) {
		++i;
	}
	// Retail also writes the final word when the five ordered slots are full.
	m_values[i + 2] = p_world;
}

// STUB: LEGORACERS2 0x0040b0d0
void GameControllerSelection::UnlockTrack(LegoU32 p_world, LegoU32 p_track)
{
	m_values[0] |= 1u << ((p_world * 5 + 4 + p_track) & 31);
}

// FUNCTION: LEGORACERS2 0x0040b0f0
LegoBool32 GameControllerSelection::IsTrackUnlocked(LegoU32 p_world, LegoU32 p_track) const
{
	return ((1u << ((p_world * 5 + 4 + p_track) & 31)) & (LegoU32) m_values[0]) != 0;
}

// STUB: LEGORACERS2 0x0040b160
void GameControllerSelection::UnlockReward(LegoU32 p_world, LegoU32 p_reward, LegoU32 p_includeBonus)
{
	LegoU32 flags = m_values[1] | (1u << ((p_world * 3 + p_reward) & 31));
	m_values[1] = flags;
	if (p_reward == 0 && p_includeBonus == 1) {
		m_values[1] = flags | (1u << ((p_world * 3 + 2) & 31));
	}
}

// FUNCTION: LEGORACERS2 0x0040b1b0
LegoBool32 GameControllerSelection::HasReward(LegoU32 p_world, LegoU32 p_reward) const
{
	return ((1u << ((p_world * 3 + p_reward) & 31)) & (LegoU32) m_values[1]) != 0;
}

// STUB: LEGORACERS2 0x0040b2c0
void GameControllerSelection::UnlockDefaults(LegoU32 p_includeFinalTrack, LegoU32 p_rewards)
{
	for (LegoU32 world = 0; world < 5; ++world) {
		if (((LegoU32) m_values[0] & (1u << world)) != 0) {
			for (LegoU32 track = 1; track <= 5; ++track) {
				if (track != 5 || (p_includeFinalTrack != 0 && world != 0)) {
					if (!IsTrackUnlocked(world, track)) {
						UnlockTrack(world, track);
					}
				}
			}
			if (p_rewards != 0) {
				for (LegoU32 reward = 0; reward < 2; ++reward) {
					if (!HasReward(world, reward)) {
						UnlockReward(world, reward, 0);
					}
				}
			}
		}
	}
}
