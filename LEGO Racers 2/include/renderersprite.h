#ifndef LR2_RENDERERSPRITE_H
#define LR2_RENDERERSPRITE_H

#include "decomp.h"
#include "vector3.h"

#include <stddef.h>

struct RendererSpriteRecord {
	undefined m_unk0x00[0x10];
	Vector3 m_position;               // 0x10
	LegoU32 m_color;                  // 0x1c
	LegoS32 m_textureId;              // 0x20
	LegoFloat m_u0;                   // 0x24
	LegoFloat m_v0;                   // 0x28
	LegoFloat m_u1;                   // 0x2c
	LegoFloat m_v1;                   // 0x30
	LegoFloat m_maximumWidthFraction; // 0x34
	LegoFloat m_halfWidth;            // 0x38
	LegoFloat m_halfHeight;           // 0x3c
	LegoS32 m_angleIndex;             // 0x40
	LegoFloat m_depthBias;            // 0x44
	LegoU32 m_blendMode;              // 0x48
};
DECOMP_SIZE_ASSERT(RendererSpriteRecord, 0x4c);
DECOMP_STATIC_ASSERT(offsetof(RendererSpriteRecord, m_position) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererSpriteRecord, m_blendMode) == 0x48);

struct RendererSpriteVertex {
	LegoFloat m_x;
	LegoFloat m_y;
	LegoFloat m_z;
	LegoFloat m_rhw;
	LegoU32 m_diffuse;
	LegoU32 m_specular;
	LegoFloat m_u;
	LegoFloat m_v;
};
DECOMP_SIZE_ASSERT(RendererSpriteVertex, 0x20);

#endif
