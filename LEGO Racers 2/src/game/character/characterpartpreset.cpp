#include "game/character/characterpartpreset.h"

// GLOBAL: LEGORACERS2 0x00570e08
CharacterPartPreset g_characterPartPresets[41];

// Retail 466470 initializes the first 20 inline, then calls 467380 for the
// next 20, and clears the final record. These are the observed scalar values.
static const CharacterPartPreset g_characterPartPresetDefaults[40] = {
	{0, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.20000005f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, -0.0500000007f, 0.0f}, {0.800000012f, 0.0f, 0.800000012f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, -0.0500000007f, 0.0f}, {0.800000012f, 0.0f, 1.20000005f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.200000003f, 0.0f}, {0.800000012f, 0.0f, 1.20000005f}, {0.0f, -0.100000001f, 0.0f}},
	{0, {0.0f, -0.0500000007f, 0.0f}, {0.800000012f, 0.0f, 0.800000012f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.100000001f, 0.0f}, {0.800000012f, 0.0f, 2.0f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, -0.0500000007f, 0.0f}, {0.800000012f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.0f, 0.0f}, {1.39999998f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{9, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.300000012f, 0.0f}},
	{11, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.400000006f, 0.0f}},
	{0, {0.0f, 0.0f, 0.0f}, {0.899999976f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
	{12, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.300000012f, 0.0f}},
	{12, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.300000012f, 0.0f}},
	{11, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.400000006f, 0.0f}},
	{10, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.400000006f, 0.0f}},
	{12, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, -0.300000012f, 0.0f}},
	{1, {0.0f, 0.200000003f, 0.0f}, {1.0f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{1, {0.0f, 0.200000003f, 0.0f}, {1.0f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{12, {0.0f, 0.0f, 0.0f}, {1.60000002f, 0.0f, 1.5f}, {0.0f, -0.300000012f, 0.0f}},
	{1, {0.0f, 0.200000003f, 0.5f}, {1.0f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{1, {0.0f, 0.100000001f, 0.100000001f}, {1.0f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{1, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{12, {0.0f, 0.0f, 0.0f}, {1.60000002f, 0.0f, 1.5f}, {0.0f, -0.400000006f, 0.0f}},
	{1, {0.0f, 0.100000001f, 0.0f}, {1.0f, 0.0f, 2.20000005f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, -0.0500000007f, 0.200000003f}, {0.800000012f, 0.0f, 1.20000005f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.0f, 0.400000006f}, {0.800000012f, 0.0f, 1.10000002f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.0f, 0.0f}, {0.800000012f, 0.0f, 1.5f}, {0.0f, 0.0f, 0.0f}},
	{13, {0.0f, 0.0f, 0.0f}, {1.20000005f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{13, {0.0f, 0.0f, 0.0f}, {1.20000005f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{13, {0.0f, 0.0f, 0.0f}, {1.20000005f, 0.0f, 1.60000002f}, {0.0f, 0.0f, 0.0f}},
	{0, {0.0f, 0.100000001f, 0.0f}, {0.800000012f, 0.0f, 0.800000012f}, {0.0f, 0.0f, 0.0f}},
	{2, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{6, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{3, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{7, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{4, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{8, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, -0.100000001f, 0.0f}},
	{5, {0.0f, -0.100000001f, 0.0f}, {0.699999988f, 0.0f, 0.800000012f}, {0.0f, 0.100000001f, 0.0f}},
	{0, {0.0f, 0.0f, -0.824999988f}, {1.10000002f, 0.0f, 1.27499998f}, {0.0f, 0.0f, 0.0f}},
};

// STUB: LEGORACERS2 0x00466470
void InitializeCharacterPartPresets()
{
	for (LegoU32 i = 0; i < 40; ++i) {
		const CharacterPartPreset* preset = &g_characterPartPresetDefaults[i];
		g_characterPartPresets[i].Set(preset->m_kind, &preset->m_first, &preset->m_second, &preset->m_third);
	}
	g_characterPartPresets[40].Clear();
}

// FUNCTION: LEGORACERS2 0x00467380
CharacterPartPreset* CharacterPartPreset::Set(
	LegoU32 p_kind,
	const Vector3* p_first,
	const Vector3* p_second,
	const Vector3* p_third
)
{
	m_kind = p_kind;
	m_first = *p_first;
	m_second = *p_second;
	m_third = *p_third;
	return this;
}

// STUB: LEGORACERS2 0x004673e0
CharacterPartPreset* CharacterPartPreset::Clear()
{
	m_kind = 0;
	m_first.m_x = m_first.m_y = m_first.m_z = 0.0f;
	m_second.m_x = m_second.m_y = m_second.m_z = 0.0f;
	m_third.m_x = m_third.m_y = m_third.m_z = 0.0f;
	return this;
}

struct CharacterPartPresetInitialization {
	CharacterPartPresetInitialization() { InitializeCharacterPartPresets(); }
};
static CharacterPartPresetInitialization g_characterPartPresetInitialization;
