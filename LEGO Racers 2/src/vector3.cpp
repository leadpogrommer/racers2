#include "vector3.h"

#include "matrix3.h"

#include <math.h>

// FUNCTION length: 0x53 bytes (0x004a4c80-0x004a4cd2).
// FUNCTION: LEGORACERS2 0x004a4c80
LegoFloat Vector3::Normalize()
{
	LegoFloat magnitude = (LegoFloat) sqrt(m_x * m_x + m_y * m_y + m_z * m_z);

	if (magnitude == 0.0f) {
		return 0.0f;
	}

	LegoFloat reciprocal = 1.0f / magnitude;
	m_x *= reciprocal;
	m_y *= reciprocal;
	m_z *= reciprocal;
	return magnitude;
}

// FUNCTION length: 0x4d bytes (0x004a4ce0-0x004a4d2c).
// FUNCTION: LEGORACERS2 0x004a4ce0
void Vector3::FUN_004a4ce0(const Vector3* p_first, const Vector3* p_second, LegoFloat p_factor)
{
	m_x = p_factor * p_first->m_x;
	m_y = p_factor * p_first->m_y;
	m_z = p_factor * p_first->m_z;

	LegoFloat secondFactor = 1.0f - p_factor;
	m_x += secondFactor * p_second->m_x;
	m_y += secondFactor * p_second->m_y;
	m_z += secondFactor * p_second->m_z;
}

// FUNCTION length: 0x4f bytes (0x004a4e00-0x004a4e4e).
// FUNCTION: LEGORACERS2 0x004a4e00
Vector3* Vector3::FUN_004a4e00(const Matrix3* p_matrix)
{
	LegoFloat x0 = p_matrix->m_values[0][0] * m_x;
	LegoFloat x1 = p_matrix->m_values[0][1] * m_y;
	LegoFloat x2 = p_matrix->m_values[0][2] * m_z;
	LegoFloat x = x0 + x1 + x2;
	LegoFloat y0 = p_matrix->m_values[1][0] * m_x;
	LegoFloat y1 = p_matrix->m_values[1][1] * m_y;
	LegoFloat y2 = p_matrix->m_values[1][2] * m_z;
	LegoFloat y = y0 + y1 + y2;
	LegoFloat z0 = p_matrix->m_values[2][0] * m_x;
	LegoFloat z1 = p_matrix->m_values[2][1] * m_y;
	LegoFloat z2 = p_matrix->m_values[2][2] * m_z;
	LegoFloat z = z0 + z1 + z2;

	m_z = z;
	m_y = y;
	m_x = x;
	return this;
}

// FUNCTION length: 0x4f bytes (0x004a4e50-0x004a4e9e).
// FUNCTION: LEGORACERS2 0x004a4e50
Vector3* Vector3::FUN_004a4e50(const Matrix3* p_matrix)
{
	LegoFloat x0 = p_matrix->m_values[0][0] * m_x;
	LegoFloat x1 = p_matrix->m_values[1][0] * m_y;
	LegoFloat x2 = p_matrix->m_values[2][0] * m_z;
	LegoFloat x = x0 + x1 + x2;
	LegoFloat y0 = p_matrix->m_values[0][1] * m_x;
	LegoFloat y1 = p_matrix->m_values[1][1] * m_y;
	LegoFloat y2 = p_matrix->m_values[2][1] * m_z;
	LegoFloat y = y0 + y1 + y2;
	LegoFloat z0 = p_matrix->m_values[0][2] * m_x;
	LegoFloat z1 = p_matrix->m_values[1][2] * m_y;
	LegoFloat z2 = p_matrix->m_values[2][2] * m_z;
	LegoFloat z = z0 + z1 + z2;

	m_z = z;
	m_y = y;
	m_x = x;
	return this;
}

// STUB: LEGORACERS2 0x004d5be0
void Vector3::Cross(const Vector3* p_first, const Vector3* p_second)
{
	LegoFloat x = p_second->m_z * p_first->m_y - p_second->m_y * p_first->m_z;
	LegoFloat z = p_first->m_x * p_second->m_y - p_second->m_x * p_first->m_y;
	m_y = p_second->m_x * p_first->m_z - p_first->m_x * p_second->m_z;
	m_z = z;
	m_x = x;
}
