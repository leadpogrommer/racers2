#include "gameanimatedobject.h"

#include "matrix3.h"

// STUB: LEGORACERS2 0x00452a40
void GameAnimatedObject::UpdateFacing(const LegoFloat* p_orientation)
{
	Matrix3 matrix;
	matrix.FUN_004a4d30(p_orientation);
	m_facing.m_x = matrix.m_values[0][2];
	m_facing.m_y = matrix.m_values[1][2];
	m_facing.m_z = matrix.m_values[2][2];
}

// STUB: LEGORACERS2 0x004c1e10
void GameAnimatedObject::SetFrozen(LegoBool32 p_frozen)
{
	if (p_frozen != 0) {
		if ((m_flags & 8) != 8) {
			m_flags |= 8;
		}
		m_behaviorFlags |= 4;
	}
	else {
		if ((m_flags & 8) != 0) {
			m_flags &= ~8;
			m_flags |= 0x02000000;
		}
		m_behaviorFlags &= ~4;
	}
}

// STUB: LEGORACERS2 0x004c26e0
void GameAnimatedObject::SetAnimationTransformEnabled(LegoU32 p_enabled)
{
	if (m_animationTransformIndex >= 0) {
		if (p_enabled != 0) {
			m_animationTransformValues[0] = 0;
			m_animationTransformValues[1] = 0;
			m_animationTransformEnabled = 1;
			m_animationTransformValues[4] = 1;
			m_animationTransformValues[2] = 0;
			m_animationTransformValues[3] = 1;
		}
		else {
			m_animationTransformEnabled = 0;
		}
	}
}
