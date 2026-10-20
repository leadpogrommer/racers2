#ifndef LEGORACERS2_PACKEDQUATERNION_H
#define LEGORACERS2_PACKEDQUATERNION_H

#include "decomp.h"
#include "types.h"

class Quaternion;

class PackedQuaternion {
public:
	LegoU32 m_bits;

	PackedQuaternion* Pack(const Quaternion* p_quaternion);
};

#endif // LEGORACERS2_PACKEDQUATERNION_H
