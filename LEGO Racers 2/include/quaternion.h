#ifndef LEGORACERS2_QUATERNION_H
#define LEGORACERS2_QUATERNION_H

#include "decomp.h"
#include "types.h"

class PackedQuaternion;

class Quaternion {
public:
	LegoFloat m_x; // 0x00
	LegoFloat m_y; // 0x04
	LegoFloat m_z; // 0x08
	LegoFloat m_w; // 0x0c

	LegoFloat Normalize();
	Quaternion* Decode(const PackedQuaternion* p_source);
};

DECOMP_SIZE_ASSERT(Quaternion, 0x10);

#endif // LEGORACERS2_QUATERNION_H
