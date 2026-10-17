#ifndef LR2_DISPLAYCONFIG_H
#define LR2_DISPLAYCONFIG_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct DisplayConfiguration {
	LegoU32 m_width;                        // 0x00
	LegoU32 m_height;                       // 0x04
	LegoU32 m_bitDepth;                     // 0x08
	LegoU32 m_fullscreen;                   // 0x0c
	LegoU32 m_unk0x10;                      // 0x10
	LegoFloat m_heightToWidthRatio;         // 0x14, height / width
	LegoU32 m_dithering;                    // 0x18
	LegoU32 m_antiAliasing;                 // 0x1c
	LegoU32 m_writeOnlyVBs;                 // 0x20
	LegoU32 m_largeVBs;                     // 0x24
	LegoU32 m_hardwareTransformAndLighting; // 0x28
	LegoU32 m_nvidia;                       // 0x2c
	LegoU32 m_vSync;                        // 0x30
	LegoU32 m_tripleBuffer;                 // 0x34
	LegoU32 m_unk0x38;                      // 0x38
};

DECOMP_SIZE_ASSERT(DisplayConfiguration, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_width) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_height) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_bitDepth) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_fullscreen) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_heightToWidthRatio) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_dithering) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_antiAliasing) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_writeOnlyVBs) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_largeVBs) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_hardwareTransformAndLighting) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_nvidia) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_vSync) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(DisplayConfiguration, m_tripleBuffer) == 0x34);

extern DisplayConfiguration* g_displayConfiguration;

#endif // LR2_DISPLAYCONFIG_H
