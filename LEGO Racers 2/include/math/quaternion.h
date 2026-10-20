#ifndef LEGORACERS2_QUATERNION_H
#define LEGORACERS2_QUATERNION_H

#include "decomp.h"
#include "types.h"

class PackedQuaternion;
class Matrix3;
class Vector3;

class Quaternion {
public:
	LegoFloat m_x; // 0x00
	LegoFloat m_y; // 0x04
	LegoFloat m_z; // 0x08
	LegoFloat m_w; // 0x0c

	LegoFloat Normalize();
	Quaternion* MultiplyRight(const Quaternion* p_right);
	Quaternion* Decode(const PackedQuaternion* p_source);
	Quaternion* FromMatrix(const Matrix3* p_matrix);
	void LookAt(const Vector3* p_position, const Vector3* p_target, const Vector3* p_up, LegoU32 p_preserveSide);
	void FromDirection(const Vector3* p_direction, Vector3* p_up, LegoU32 p_preserveSide);
	void Interpolate(const Quaternion* p_first, const Quaternion* p_second, LegoFloat p_factor);
};

DECOMP_SIZE_ASSERT(Quaternion, 0x10);

#endif // LEGORACERS2_QUATERNION_H
