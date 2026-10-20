#include "render/models/modelbounds.h"

// STUB: LEGORACERS2 0x004a8ed0
void ModelBounds::ClassifyXZAspectRatio()
{
	m_isElongatedXZ = 1;
	LegoFloat minimumX = 2147483648.0f;
	LegoFloat minimumY = 2147483648.0f;
	LegoFloat minimumZ = 2147483648.0f;
	LegoFloat maximumX = -2147483648.0f;
	LegoFloat maximumY = -2147483648.0f;
	LegoFloat maximumZ = -2147483648.0f;
	for (LegoS32 index = 0; index < 8; ++index) {
		const Vector3* corner = &m_corners[index];
		if (corner->m_x < minimumX) {
			minimumX = corner->m_x;
		}
		if (corner->m_y < minimumY) {
			minimumY = corner->m_y;
		}
		if (corner->m_z < minimumZ) {
			minimumZ = corner->m_z;
		}
		if (maximumX < corner->m_x) {
			maximumX = corner->m_x;
		}
		if (maximumY < corner->m_y) {
			maximumY = corner->m_y;
		}
		if (maximumZ < corner->m_z) {
			maximumZ = corner->m_z;
		}
	}
	LegoFloat width = maximumX - minimumX;
	LegoFloat depth = maximumZ - minimumZ;
	if (depth > width) {
		if (depth > 0.0f && width / depth > 0.5f) {
			m_isElongatedXZ = 0;
		}
	}
	else if (width > 0.0f && depth / width > 0.5f) {
		m_isElongatedXZ = 0;
	}
}
