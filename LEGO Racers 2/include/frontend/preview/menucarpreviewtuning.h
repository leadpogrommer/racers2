#ifndef LR2_MENU_CAR_PREVIEW_TUNING_H
#define LR2_MENU_CAR_PREVIEW_TUNING_H

#include "decomp.h"
#include "math/vector3.h"
#include "types.h"

#include <stddef.h>

struct MenuCarPreviewTuning {
	LegoFloat m_secondAmbient[4]; // 56b958
	LegoU8 m_ambientColor[4];     // 56b968
	LegoU32 m_gap0x14;
	LegoFloat m_firstSpecular[4];  // 56b970
	LegoFloat m_firstDiffuse[4];   // 56b980
	LegoFloat m_size[2];           // 56b990
	LegoFloat m_firstAmbient[4];   // 56b998
	LegoFloat m_secondSpecular[4]; // 56b9a8
	LegoFloat m_cameraDistance;    // 56b9b8
	LegoU32 m_gap0x64;
	Vector3 m_cameraPosition; // 56b9c0
	LegoU32 m_gap0x74;
	LegoFloat m_vehicleParameters[3]; // 56b9d0
	LegoU32 m_gap0x84;
	Vector3 m_firstDirection; // 56b9e0
	LegoU32 m_gap0x94;
	LegoFloat m_secondDiffuse[4]; // 56b9f0
	Vector3 m_secondDirection;    // 56ba00
	LegoU32 m_gap0xb4;
	Vector3 m_targetOffset; // 56ba10
};
DECOMP_SIZE_ASSERT(MenuCarPreviewTuning, 0xc4);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_firstSpecular) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_size) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_cameraPosition) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_vehicleParameters) == 0x78);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_firstDirection) == 0x88);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_secondDirection) == 0xa8);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewTuning, m_targetOffset) == 0xb8);

extern MenuCarPreviewTuning g_menuCarPreviewTuning;
void InitializeMenuCarPreviewTuning();

#endif
