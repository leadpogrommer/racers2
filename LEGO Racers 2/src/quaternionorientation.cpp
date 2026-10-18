#include "matrix3.h"
#include "quaternion.h"
#include "vector3.h"

#include <math.h>

// STUB: LEGORACERS2 0x004a55f0
Quaternion* Quaternion::FromMatrix(const Matrix3* p_matrix)
{
	const LegoFloat(&m)[3][3] = p_matrix->m_values;
	LegoFloat diagonalPair = m[2][2] + m[1][1];
	LegoFloat trace = m[0][0] + diagonalPair;
	LegoFloat root;
	LegoFloat scale;
	if (trace >= 0.0f) {
		root = (LegoFloat) sqrt(trace + 1.0f);
		m_w = root * 0.5f;
		scale = 0.5f / root;
		m_x = (m[2][1] - m[1][2]) * scale;
		m_y = (m[0][2] - m[2][0]) * scale;
		m_z = (m[1][0] - m[0][1]) * scale;
		return this;
	}
	LegoU32 index = m[0][0] < m[1][1];
	if (m[index][index] < m[2][2]) {
		index = 2;
	}
	if (index == 0) {
		root = (LegoFloat) sqrt(m[0][0] - (m[2][2] + m[1][1]) + 1.0f);
		m_x = root * 0.5f;
		scale = 0.5f / root;
		m_y = (m[1][0] + m[0][1]) * scale;
		m_z = (m[2][0] + m[0][2]) * scale;
		m_w = (m[2][1] - m[1][2]) * scale;
	}
	else if (index == 1) {
		root = (LegoFloat) sqrt(m[1][1] - (m[0][0] + m[2][2]) + 1.0f);
		m_y = root * 0.5f;
		scale = 0.5f / root;
		m_z = (m[2][1] + m[1][2]) * scale;
		m_x = (m[1][0] + m[0][1]) * scale;
		m_w = (m[0][2] - m[2][0]) * scale;
	}
	else {
		root = (LegoFloat) sqrt(m[2][2] - (m[0][0] + m[1][1]) + 1.0f);
		m_z = root * 0.5f;
		scale = 0.5f / root;
		m_x = (m[2][0] + m[0][2]) * scale;
		m_y = (m[2][1] + m[1][2]) * scale;
		m_w = (m[1][0] - m[0][1]) * scale;
	}
	return this;
}

// STUB: LEGORACERS2 0x004a5850
void Quaternion::LookAt(const Vector3* p_position, const Vector3* p_target, const Vector3* p_up, LegoU32 p_preserveSide)
{
	Vector3 direction;
	direction.m_x = p_position->m_x - p_target->m_x;
	direction.m_y = p_position->m_y - p_target->m_y;
	direction.m_z = p_position->m_z - p_target->m_z;
	if (direction.Normalize() != 0.0f) {
		Vector3 up = *p_up;
		FromDirection(&direction, &up, p_preserveSide);
	}
}

// STUB: LEGORACERS2 0x004a5910
void Quaternion::FromDirection(const Vector3* p_direction, Vector3* p_up, LegoU32 p_preserveSide)
{
	Vector3 side;
	if (p_preserveSide == 0) {
		side.Cross(p_up, p_direction);
		if (side.Normalize() == 0.0f) {
			return;
		}
		p_up->Cross(p_direction, &side);
		if (p_up->Normalize() == 0.0f) {
			return;
		}
	}
	else {
		side = *p_up;
		p_up->Cross(p_direction, &side);
		if (p_up->Normalize() == 0.0f) {
			return;
		}
		side.Cross(p_up, p_direction);
		if (side.Normalize() == 0.0f) {
			return;
		}
	}
	Matrix3 basis;
	basis.m_values[0][0] = side.m_x;
	basis.m_values[1][0] = side.m_y;
	basis.m_values[2][0] = side.m_z;
	basis.m_values[0][1] = p_up->m_x;
	basis.m_values[1][1] = p_up->m_y;
	basis.m_values[2][1] = p_up->m_z;
	basis.m_values[0][2] = p_direction->m_x;
	basis.m_values[1][2] = p_direction->m_y;
	basis.m_values[2][2] = p_direction->m_z;
	FromMatrix(&basis);
}
