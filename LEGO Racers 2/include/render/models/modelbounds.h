#ifndef LR2_MODELBOUNDS_H
#define LR2_MODELBOUNDS_H

#include "math/vector3.h"

#include <stddef.h>

struct ModelBounds {
	Vector3 m_corners[8];
	LegoU32 m_isElongatedXZ;
	Vector3 m_center;
	LegoFloat m_radius;

	void ClassifyXZAspectRatio();
};
DECOMP_SIZE_ASSERT(ModelBounds, 0x74);
DECOMP_STATIC_ASSERT(offsetof(ModelBounds, m_isElongatedXZ) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(ModelBounds, m_center) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(ModelBounds, m_radius) == 0x70);

#endif
