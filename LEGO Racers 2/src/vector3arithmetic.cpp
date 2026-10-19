#include "vector3.h"

// FUNCTION: LEGORACERS2 0x00404100
Vector3* Vector3::SetValues(LegoFloat p_x, LegoFloat p_y, LegoFloat p_z)
{
	m_x = p_x;
	m_y = p_y;
	m_z = p_z;
	return this;
}

// FUNCTION: LEGORACERS2 0x00431ef0
LegoFloat Vector3::Dot(const Vector3* p_other) const
{
	return p_other->m_z * m_z + p_other->m_y * m_y + p_other->m_x * m_x;
}

// FUNCTION: LEGORACERS2 0x0043d1d0
Vector3* Vector3::Scale(LegoFloat p_scale)
{
	m_x = p_scale * m_x;
	m_y = p_scale * m_y;
	m_z = p_scale * m_z;
	return this;
}

// FUNCTION: LEGORACERS2 0x0043d200
Vector3* Vector3::Add(const Vector3* p_other)
{
	m_x = p_other->m_x + m_x;
	m_y = p_other->m_y + m_y;
	m_z = p_other->m_z + m_z;
	return this;
}

// FUNCTION: LEGORACERS2 0x0043d230
Vector3* Vector3::Subtract(const Vector3* p_other)
{
	m_x = m_x - p_other->m_x;
	m_y = m_y - p_other->m_y;
	m_z = m_z - p_other->m_z;
	return this;
}
