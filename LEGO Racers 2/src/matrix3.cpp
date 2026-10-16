#include "matrix3.h"

#include <math.h>

// FUNCTION length: 0xcc bytes (0x004a4d30-0x004a4dfb).
// STUB: LEGORACERS2 0x004a4d30
void Matrix3::FUN_004a4d30(const LegoFloat p_quaternion[4])
{
	Matrix3* matrix = this;
	const LegoFloat* quaternion = p_quaternion;
	LegoFloat xSquared = quaternion[0] * quaternion[0];
	LegoFloat twiceYSquared = quaternion[1] * quaternion[1];
	twiceYSquared += twiceYSquared;
	LegoFloat twiceZSquared = quaternion[2] * quaternion[2];
	twiceZSquared += twiceZSquared;
	LegoFloat twiceW = quaternion[3] + quaternion[3];
	LegoFloat x = quaternion[0];
	LegoFloat y = quaternion[1];
	LegoFloat z = quaternion[2];
	LegoFloat twiceXY = quaternion[1] * quaternion[0] + quaternion[1] * quaternion[0];
	LegoFloat twiceXZ = quaternion[2] * quaternion[0] + quaternion[2] * quaternion[0];
	LegoFloat twiceYZ = quaternion[2] * quaternion[1] + quaternion[2] * quaternion[1];

	matrix->m_values[0][0] = (1.0f - twiceYSquared) - twiceZSquared;
	matrix->m_values[0][1] = twiceXY - twiceW * z;
	matrix->m_values[0][2] = twiceXZ + twiceW * y;
	matrix->m_values[1][0] = twiceXY + twiceW * z;
	matrix->m_values[1][1] = (1.0f - (xSquared + xSquared)) - twiceZSquared;
	matrix->m_values[1][2] = twiceYZ - twiceW * x;
	matrix->m_values[2][0] = twiceXZ - twiceW * y;
	matrix->m_values[2][1] = twiceYZ + twiceW * x;
	matrix->m_values[2][2] = (1.0f - (xSquared + xSquared)) - twiceYSquared;
}

// FUNCTION length: 0x5a bytes (0x004a4ea0-0x004a4ef9).
// STUB: LEGORACERS2 0x004a4ea0
Matrix3* Matrix3::FUN_004a4ea0(const Matrix3* p_left, const Matrix3* p_right)
{
	LegoS32 row = 0;
	LegoS32 rowCount = 3;
	do {
		LegoFloat left0 = p_left->m_values[row][0];
		LegoFloat left1 = p_left->m_values[row][1];
		LegoFloat left2 = p_left->m_values[row][2];
		LegoS32 column = 0;
		LegoS32 columnCount = 3;
		do {
			m_values[row][column] = left0 * p_right->m_values[0][column] + left1 * p_right->m_values[1][column] +
									left2 * p_right->m_values[2][column];
			++column;
		} while (--columnCount != 0);
		++row;
	} while (--rowCount != 0);

	return this;
}

// FUNCTION length: 0x5a bytes (0x004a4f00-0x004a4f59).
// STUB: LEGORACERS2 0x004a4f00
Matrix3* Matrix3::FUN_004a4f00(const Matrix3* p_left, const Matrix3* p_right)
{
	LegoS32 row = 0;
	LegoS32 rowCount = 3;
	do {
		LegoFloat left0 = p_left->m_values[0][row];
		LegoFloat left1 = p_left->m_values[1][row];
		LegoFloat left2 = p_left->m_values[2][row];
		LegoS32 column = 0;
		LegoS32 columnCount = 3;
		do {
			m_values[row][column] = left0 * p_right->m_values[0][column] + left1 * p_right->m_values[1][column] +
									left2 * p_right->m_values[2][column];
			++column;
		} while (--columnCount != 0);
		++row;
	} while (--rowCount != 0);

	return this;
}
// FUNCTION length: 0xd3 bytes (0x004a4f60-0x004a5032).
// STUB: LEGORACERS2 0x004a4f60
void Matrix3::FUN_004a4f60(const LegoFloat p_angles[3])
{
	LegoFloat sinX = (LegoFloat) sin(p_angles[0]);
	LegoFloat cosX = (LegoFloat) cos(p_angles[0]);
	LegoFloat sinY = (LegoFloat) sin(p_angles[1]);
	LegoFloat cosY = (LegoFloat) cos(p_angles[1]);
	LegoFloat sinZ = (LegoFloat) sin(p_angles[2]);
	LegoFloat cosZ = (LegoFloat) cos(p_angles[2]);
	LegoFloat sinXSinY = sinX * sinY;
	LegoFloat cosXSinY = cosX * sinY;

	m_values[0][0] = cosZ * cosY;
	m_values[0][1] = -(sinZ * cosY);
	m_values[0][2] = sinY;
	m_values[1][0] = sinZ * cosX + sinXSinY * cosZ;
	m_values[1][1] = cosZ * cosX - sinXSinY * sinZ;
	m_values[1][2] = -(cosY * sinX);
	m_values[2][0] = sinZ * sinX - cosXSinY * cosZ;
	m_values[2][1] = cosZ * sinX + cosXSinY * sinZ;
	m_values[2][2] = cosY * cosX;
}

// FUNCTION length: 0x32 bytes (0x004a5040-0x004a5071).
// STUB: LEGORACERS2 0x004a5040
void Matrix3::FUN_004a5040(const Matrix3* p_source)
{
	LegoFloat* destination = &m_values[0][0];
	const LegoFloat* source = &p_source->m_values[0][0];
	LegoS32 rowCount = 3;
	do {
		LegoS32 columnCount = 3;
		do {
			*destination++ = *source;
			source += 3;
		} while (--columnCount != 0);
		source -= 8;
	} while (--rowCount != 0);
}

// FUNCTION length: 0x89 bytes (0x004a5080-0x004a5108).
// STUB: LEGORACERS2 0x004a5080
void Matrix3::FUN_004a5080(const LegoFloat p_vector[3])
{
	const LegoFloat x = p_vector[0];
	const LegoFloat y = p_vector[1];
	const LegoFloat z = p_vector[2];

	m_values[2][0] = -(z * x);
	m_values[0][2] = -(z * x);
	m_values[1][2] = -(z * y);
	m_values[2][1] = -(z * y);
	m_values[0][0] = z * z + y * y;
	m_values[1][1] = z * z + x * x;
	m_values[2][2] = y * y + x * x;
	m_values[1][0] = -(y * x);
	m_values[0][1] = -(y * x);
}

// FUNCTION length: 0x3f bytes (0x004a5110-0x004a514e).
// STUB: LEGORACERS2 0x004a5110
void Matrix3::FUN_004a5110(const Matrix3* p_source)
{
	LegoS32 row = 0;
	do {
		m_values[row][0] += p_source->m_values[row][0];
		m_values[row][1] += p_source->m_values[row][1];
		m_values[row][2] += p_source->m_values[row][2];
		++row;
	} while (row < 3);
}

// FUNCTION length: 0x37 bytes (0x004a5250-0x004a5286).
// STUB: LEGORACERS2 0x004a5250
LegoFloat Matrix3::FUN_004a5250() const
{
	LegoFloat value = (m_values[1][1] * m_values[2][2] - m_values[1][2] * m_values[2][1]) * m_values[0][0];
	value += (m_values[1][2] * m_values[2][0] - m_values[1][0] * m_values[2][2]) * m_values[0][1];
	value += (m_values[1][0] * m_values[2][1] - m_values[1][1] * m_values[2][0]) * m_values[0][2];
	return value;
}

// FUNCTION length: 0x42 bytes (0x004a52c0-0x004a5301).
// STUB: LEGORACERS2 0x004a52c0
void Matrix3::FUN_004a52c0(const LegoFloat p_scale[3], const Matrix3* p_source)
{
	LegoS32 row = 0;
	do {
		const LegoFloat scale = p_scale[row];
		m_values[row][0] = scale * p_source->m_values[row][0];
		m_values[row][1] = scale * p_source->m_values[row][1];
		m_values[row][2] = scale * p_source->m_values[row][2];
		++row;
	} while (row < 3);
}
