#include "quaternion.h"

#include "packedquaternion.h"

#include <math.h>

// FUNCTION length: 0x66 bytes (0x004a5310-0x004a5375).
// STUB: LEGORACERS2 0x004a5310
LegoFloat Quaternion::Normalize()
{
	LegoFloat magnitude = (LegoFloat) sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);

	if (magnitude == 0.0f) {
		return 0.0f;
	}

	LegoFloat reciprocal = 1.0f / magnitude;
	m_x *= reciprocal;
	m_y *= reciprocal;
	m_z *= reciprocal;
	m_w *= reciprocal;
	return magnitude;
}

// FUNCTION length: 0xb7 bytes (0x004a5440-0x004a54f6).
// STUB: LEGORACERS2 0x004a5440
Quaternion* Quaternion::Decode(const PackedQuaternion* p_source)
{
	const LegoFloat componentScale = 0.001956947147846222f;
	LegoS32 component = ((LegoS32) (p_source->m_bits << 22)) >> 22;
	m_x = component * componentScale;

	component = ((LegoS32) (p_source->m_bits << 12)) >> 22;
	m_y = component * componentScale;

	component = ((LegoS32) (p_source->m_bits << 2)) >> 22;
	m_z = component * componentScale;

	LegoFloat squaredW = 1.0f - (m_x * m_x + m_y * m_y + m_z * m_z);
	if (!(squaredW > 0.0f)) {
		m_w = 0.0f;
		Normalize();
		return this;
	}

	const LegoU32 negativeW = p_source->m_bits & 0xc0000000;
	LegoFloat w = (LegoFloat) sqrt(squaredW);
	if (negativeW != 0) {
		w = -w;
	}
	m_w = w;

	Normalize();
	return this;
}
