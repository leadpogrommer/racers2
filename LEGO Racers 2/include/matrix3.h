#ifndef LEGORACERS2_MATRIX3_H
#define LEGORACERS2_MATRIX3_H

#include "decomp.h"
#include "types.h"

class Matrix3 {
public:
	LegoFloat m_values[3][3]; // 0x00: row-major 3x3 matrix.

	void FUN_004a4d30(const LegoFloat p_quaternion[4]);
	Matrix3* FUN_004a4ea0(const Matrix3* p_left, const Matrix3* p_right);
	Matrix3* FUN_004a4f00(const Matrix3* p_left, const Matrix3* p_right);
	Matrix3* FUN_004a4f60(const LegoFloat p_angles[3]);
	Matrix3* FUN_004a5040(const Matrix3* p_source);
	Matrix3* FUN_004a5080(const LegoFloat p_vector[3]);
	Matrix3* FUN_004a5110(const Matrix3* p_source);
	Matrix3* FUN_004a5150(const Matrix3* p_source);
	LegoFloat FUN_004a5250() const;
	Matrix3* FUN_004a52c0(const LegoFloat p_scale[3], const Matrix3* p_source);
};

DECOMP_SIZE_ASSERT(Matrix3, 0x24);

#endif // LEGORACERS2_MATRIX3_H
