#ifndef LR2_MENUCHARACTERPREVIEWTUNING_H
#define LR2_MENUCHARACTERPREVIEWTUNING_H

#include "decomp.h"
#include "types.h"
#include "vector3.h"

#include <stddef.h>

struct MenuCharacterPreviewTuning {
	LegoFloat m_secondSpecular[4]; // 56ba28
	LegoFloat m_secondDiffuse[4];  // 56ba38
	Vector3 m_cameraPosition;      // 56ba48
	undefined m_gap0x2c[4];
	LegoFloat m_size[2];         // 56ba58
	Vector3 m_characterPosition; // 56ba60
	undefined m_gap0x44[4];
	Vector3 m_targetOffset;   // 56ba70
	LegoU8 m_ambientColor[4]; // 56ba7c
	undefined m_gap0x58[0x20];
	LegoFloat m_secondAmbient[4]; // 56baa0
	LegoFloat m_firstAmbient[4];  // 56bab0
	LegoFloat m_firstDiffuse[4];  // 56bac0
	Vector3 m_secondDirection;    // 56bad0
	undefined m_gap0xb4[4];
	LegoFloat m_firstSpecular[4]; // 56bae0
	Vector3 m_firstDirection;     // 56baf0
};
DECOMP_SIZE_ASSERT(MenuCharacterPreviewTuning, 0xd4);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewTuning, m_size) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewTuning, m_ambientColor) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewTuning, m_firstAmbient) == 0x88);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewTuning, m_firstDirection) == 0xc8);

extern MenuCharacterPreviewTuning g_menuCharacterPreviewTuning;

#endif
