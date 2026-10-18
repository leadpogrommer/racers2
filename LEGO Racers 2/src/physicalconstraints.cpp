#include "physicallifecycle.h"

// STUB: LEGORACERS2 0x004a6a60
void PhysicalLifecycleState::UpdateSleepState()
{
	typedef LegoBool32(__fastcall * Predicate)(PhysicalLifecycleState*, void*);
	struct SleepDispatch {
		undefined m_unknown[8];
		Predicate m_canSleep;
	};
	const SleepDispatch* dispatch = (const SleepDispatch*) m_base.m_vtable;
	if (dispatch->m_canSleep(this, NULL) && m_linearVelocity.m_z * m_linearVelocity.m_z +
													m_linearVelocity.m_y * m_linearVelocity.m_y +
													m_linearVelocity.m_x * m_linearVelocity.m_x <=
												0.2f) {
		LegoFloat angularSpeedSquared = m_worldAngularVelocity.m_z * m_worldAngularVelocity.m_z +
										m_worldAngularVelocity.m_y * m_worldAngularVelocity.m_y +
										m_worldAngularVelocity.m_x * m_worldAngularVelocity.m_x;
		if (angularSpeedSquared < 5.0f) {
			m_base.m_stateFlags |= 2;
			return;
		}
		if (angularSpeedSquared < 50.0f) {
			m_angularMomentum.m_x *= 0.7f;
			m_angularMomentum.m_y *= 0.7f;
			m_angularMomentum.m_z *= 0.7f;
			m_worldAngularVelocity.m_x *= 0.5f;
			m_worldAngularVelocity.m_y *= 0.5f;
			m_worldAngularVelocity.m_z *= 0.5f;
			m_torque.m_x = 0;
			m_torque.m_y = 0;
			m_torque.m_z = 0;
		}
	}
	m_base.m_stateFlags &= ~2;
}

// STUB: LEGORACERS2 0x004a6ec0
void PhysicalLifecycleState::GetWorldParentOffset(Vector3* p_result)
{
	*p_result = m_parentOffset;
	p_result->FUN_004a4e00(m_parent->GetTransformDispatch()->m_rotation(m_parent, NULL));
}
