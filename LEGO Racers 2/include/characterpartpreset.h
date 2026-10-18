#ifndef LR2_CHARACTERPARTPRESET_H
#define LR2_CHARACTERPARTPRESET_H

#include "vector3.h"

// The exact interpretation of the three vectors is still being traced.
// Character/player setup selects these records at a 0x28-byte stride.
struct CharacterPartPreset {
	LegoU32 m_kind;
	Vector3 m_first;
	Vector3 m_second;
	Vector3 m_third;
	CharacterPartPreset* Set(LegoU32, const Vector3*, const Vector3*, const Vector3*);
	CharacterPartPreset* Clear();
};
DECOMP_SIZE_ASSERT(CharacterPartPreset, 0x28);
extern CharacterPartPreset g_characterPartPresets[41];
void InitializeCharacterPartPresets();

#endif
