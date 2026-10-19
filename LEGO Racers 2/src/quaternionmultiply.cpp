#include "quaternion.h"

// STUB: LEGORACERS2 0x004a5500
Quaternion* Quaternion::MultiplyRight(const Quaternion* p_right)
{
	LegoFloat rightX = m_w * p_right->m_x;
	LegoFloat rightY = m_w * p_right->m_y;
	LegoFloat rightZ = m_w * p_right->m_z;
	LegoFloat leftX = p_right->m_w * m_x;
	LegoFloat leftY = p_right->m_w * m_y;
	LegoFloat leftZ = p_right->m_w * m_z;
	m_w = p_right->m_w * m_w - ((m_z * p_right->m_z + m_y * p_right->m_y) + p_right->m_x * m_x);
	LegoFloat crossX = m_y * p_right->m_z - m_z * p_right->m_y;
	LegoFloat crossY = m_z * p_right->m_x - p_right->m_z * m_x;
	LegoFloat crossZ = p_right->m_y * m_x - m_y * p_right->m_x;
	m_z = crossZ;
	m_x = (crossX + leftX) + rightX;
	m_y = (leftY + crossY) + rightY;
	m_z = (crossZ + leftZ) + rightZ;
	return this;
}
