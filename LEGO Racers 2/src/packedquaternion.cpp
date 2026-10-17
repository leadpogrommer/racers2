#include "packedquaternion.h"

#include "quaternion.h"

// FUNCTION length: 0xb1 bytes (0x004a5380-0x004a5430).
// STUB: LEGORACERS2 0x004a5380
PackedQuaternion* PackedQuaternion::Pack(const Quaternion* p_quaternion)
{
	LegoS32 x = (LegoS32) (p_quaternion->m_x * 511.0f);
	m_bits = (m_bits & ~0x3ff) | (x & 0x3ff);

	LegoS32 y = (LegoS32) (p_quaternion->m_y * 511.0f);
	m_bits = (m_bits & ~0xffc00) | ((y & 0x3ff) << 10);

	LegoS32 z = (LegoS32) (p_quaternion->m_z * 511.0f);
	m_bits = (m_bits & ~0x3ff00000) | ((z & 0x3ff) << 20);

	LegoU32 negativeW = p_quaternion->m_w < 0.0f;
	m_bits = (m_bits & 0x3fffffff) | (negativeW << 30);

	return this;
}
