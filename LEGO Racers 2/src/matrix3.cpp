#include "matrix3.h"

#include <math.h>

// FUNCTION length: 0xcc bytes (0x004a4d30-0x004a4dfb).
// STUB: LEGORACERS2 0x004a4d30
void Matrix3::FUN_004a4d30(const LegoFloat p_quaternion[4])
{
	const LegoFloat* quaternion = p_quaternion;
	LegoFloat xSquared = p_quaternion[0] * p_quaternion[0];
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

	m_values[0][0] = (1.0f - twiceYSquared) - twiceZSquared;
	m_values[0][1] = twiceXY - twiceW * z;
	m_values[0][2] = twiceXZ + twiceW * y;
	m_values[1][0] = twiceXY + twiceW * z;
	const LegoFloat oneMinusTwiceXSquared = 1.0f - (xSquared + xSquared);
	m_values[1][1] = oneMinusTwiceXSquared - twiceZSquared;
	m_values[1][2] = twiceYZ - twiceW * x;
	m_values[2][0] = twiceXZ - twiceW * y;
	m_values[2][1] = twiceYZ + twiceW * x;
	m_values[2][2] = oneMinusTwiceXSquared - twiceYSquared;
}

// FUNCTION length: 0x5a bytes (0x004a4ea0-0x004a4ef9).
// FUNCTION: LEGORACERS2 0x004a4ea0
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
			LegoFloat value = left0 * p_right->m_values[0][column];
			value += left1 * p_right->m_values[1][column];
			value += left2 * p_right->m_values[2][column];
			m_values[row][column] = value;
			++column;
		} while (--columnCount != 0);
		++row;
	} while (--rowCount != 0);

	return this;
}

// FUNCTION length: 0x5a bytes (0x004a4f00-0x004a4f59).
// FUNCTION: LEGORACERS2 0x004a4f00
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
			LegoFloat value = left0 * p_right->m_values[0][column];
			value += left1 * p_right->m_values[1][column];
			value += left2 * p_right->m_values[2][column];
			m_values[row][column] = value;
			++column;
		} while (--columnCount != 0);
		++row;
	} while (--rowCount != 0);

	return this;
}
// FUNCTION length: 0xd3 bytes (0x004a4f60-0x004a5032).
// FUNCTION: LEGORACERS2 0x004a4f60
Matrix3* Matrix3::FUN_004a4f60(const LegoFloat p_angles[3])
{
	LegoFloat sinX = (LegoFloat) sin(p_angles[0]);
	LegoFloat cosX = (LegoFloat) cos(p_angles[0]);
	LegoFloat sinY = (LegoFloat) sin(p_angles[1]);
	LegoFloat cosY = (LegoFloat) cos(p_angles[1]);
	LegoFloat sinZ = (LegoFloat) sin(p_angles[2]);
	LegoFloat cosZ = (LegoFloat) cos(p_angles[2]);
	LegoFloat sinXSinY = sinX * sinY;
	LegoFloat cosXSinY = cosX * sinY;
	LegoFloat negativeSinXSinYSinZ = -(sinXSinY * sinZ);
	LegoFloat negativeCosXSinYCosZ = -(cosXSinY * cosZ);

	m_values[0][0] = cosZ * cosY;
	m_values[0][1] = -(sinZ * cosY);
	m_values[0][2] = sinY;
	m_values[1][0] = sinZ * cosX + sinXSinY * cosZ;
	m_values[1][1] = cosZ * cosX + negativeSinXSinYSinZ;
	m_values[1][2] = -(cosY * sinX);
	m_values[2][0] = sinZ * sinX + negativeCosXSinYCosZ;
	m_values[2][1] = cosZ * sinX + cosXSinY * sinZ;
	m_values[2][2] = cosY * cosX;
	return this;
}

// FUNCTION length: 0x32 bytes (0x004a5040-0x004a5071).
// FUNCTION: LEGORACERS2 0x004a5040
Matrix3* Matrix3::FUN_004a5040(const Matrix3* p_source)
{
	LegoS32 row = 0;
	LegoS32 rowCount = 3;
	do {
		LegoS32 column = 0;
		LegoS32 columnCount = 3;
		do {
			m_values[row][column] = p_source->m_values[column][row];
			++column;
		} while (--columnCount != 0);
		++row;
	} while (--rowCount != 0);
	return this;
}

// FUNCTION length: 0x89 bytes (0x004a5080-0x004a5108).
// STUB: LEGORACERS2 0x004a5080
Matrix3* Matrix3::FUN_004a5080(const LegoFloat p_vector[3])
{
	const LegoFloat xSquared = p_vector[0] * p_vector[0];
	const LegoFloat ySquared = p_vector[1] * p_vector[1];
	const LegoFloat zSquared = p_vector[2] * p_vector[2];
	const LegoFloat y = p_vector[1];
	const LegoFloat x = p_vector[0];
	const LegoFloat z = p_vector[2];
	const LegoFloat xAgain = p_vector[0];
	const LegoFloat zAgain = p_vector[2];
	const LegoFloat yAgain = p_vector[1];
	const LegoFloat zx = z * xAgain;
	const LegoFloat zy = zAgain * yAgain;
	const LegoFloat yx = y * x;

	m_values[2][0] = -zx;
	m_values[0][2] = -zx;
	m_values[1][2] = -zy;
	m_values[2][1] = -zy;
	m_values[0][0] = zSquared + ySquared;
	m_values[1][1] = zSquared + xSquared;
	m_values[2][2] = ySquared + xSquared;
	m_values[1][0] = -yx;
	m_values[0][1] = -yx;
	return this;
}

// FUNCTION length: 0x3f bytes (0x004a5110-0x004a514e).
// FUNCTION: LEGORACERS2 0x004a5110
Matrix3* Matrix3::FUN_004a5110(const Matrix3* p_source)
{
	LegoS32 row = 0;
	do {
		m_values[row][0] += p_source->m_values[row][0];
		m_values[row][1] += p_source->m_values[row][1];
		m_values[row][2] += p_source->m_values[row][2];
		++row;
	} while (row < 3);
	return this;
}

// FUNCTION length: 0xfe bytes (0x004a5150-0x004a524d).
// STUB: LEGORACERS2 0x004a5150
Matrix3* Matrix3::FUN_004a5150(const Matrix3* p_source)
{
	const LegoFloat determinant = p_source->FUN_004a5250();
	LegoFloat reciprocal = 0.0f;
	if (determinant != 0.0f) {
		reciprocal = 1.0f / determinant;
	}

	const LegoS32 cyclicIndices[4] = {1, 2, 0, 1};
	LegoS32 column = 0;
	LegoS32 columnCount = 3;
	do {
		const LegoS32 firstColumn = cyclicIndices[column];
		const LegoS32 secondColumn = cyclicIndices[column + 1];
		LegoS32 row = 0;
		LegoS32 rowCount = 3;
		do {
			const LegoS32 firstRow = cyclicIndices[row];
			const LegoS32 secondRow = cyclicIndices[row + 1];
			m_values[row][column] =
				(p_source->m_values[firstColumn][firstRow] * p_source->m_values[secondColumn][secondRow] -
				 p_source->m_values[firstColumn][secondRow] * p_source->m_values[secondColumn][firstRow]) *
				reciprocal;
			++row;
		} while (--rowCount != 0);
		++column;
	} while (--columnCount != 0);

	return this;
}

// FUNCTION length: 0x37 bytes (0x004a5250-0x004a5286).
// STUB: LEGORACERS2 0x004a5250
LegoFloat Matrix3::FUN_004a5250() const
{
	const LegoFloat firstMinor = m_values[1][1] * m_values[2][2] - m_values[1][2] * m_values[2][1];
	const LegoFloat firstTerm = firstMinor * m_values[0][0];
	const LegoFloat secondMinor = m_values[1][2] * m_values[2][0] - m_values[1][0] * m_values[2][2];
	const LegoFloat secondTerm = secondMinor * m_values[0][1];
	const LegoFloat thirdMinor = m_values[1][0] * m_values[2][1] - m_values[1][1] * m_values[2][0];
	const LegoFloat thirdTerm = thirdMinor * m_values[0][2];
	LegoFloat value = firstTerm;
	value = value + secondTerm;
	value = value + thirdTerm;
	return value;
}

// FUNCTION length: 0x42 bytes (0x004a52c0-0x004a5301).
// FUNCTION: LEGORACERS2 0x004a52c0
Matrix3* Matrix3::FUN_004a52c0(const LegoFloat p_scale[3], const Matrix3* p_source)
{
	LegoS32 row = 0;
	do {
		const LegoFloat scale = p_scale[row];
		m_values[row][0] = scale * p_source->m_values[0][row];
		m_values[row][1] = scale * p_source->m_values[1][row];
		m_values[row][2] = scale * p_source->m_values[2][row];
		++row;
	} while (row < 3);
	return this;
}
