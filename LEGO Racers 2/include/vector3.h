#ifndef LEGORACERS2_VECTOR3_H
#define LEGORACERS2_VECTOR3_H

#include "decomp.h"
#include "types.h"

class Matrix3;

class Vector3 {
public:
	LegoFloat m_x; // 0x00
	LegoFloat m_y; // 0x04
	LegoFloat m_z; // 0x08

	LegoFloat Normalize();
	void FUN_004a4ce0(const Vector3* p_first, const Vector3* p_second, LegoFloat p_factor);
	// Retail leaves this in EAX; callers discard the return value.
	Vector3* FUN_004a4e00(const Matrix3* p_matrix);
	Vector3* FUN_004a4e50(const Matrix3* p_matrix);
};

DECOMP_SIZE_ASSERT(Vector3, 0x0c);

#endif // LEGORACERS2_VECTOR3_H
