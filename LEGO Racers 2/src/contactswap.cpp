#include "collisioncontacts.h"
#include "physicallifecycle.h"

// STUB: LEGORACERS2 0x004b96f0
void CollisionContact::SwapParticipants(
	const Vector3* p_offset,
	GameLifecycleComponent* p_first,
	GameLifecycleComponent* p_second
)
{
	Vector3 offset;
	if (p_offset == NULL) {
		PhysicalLifecycleState* first = (PhysicalLifecycleState*) p_first;
		PhysicalLifecycleState* second = (PhysicalLifecycleState*) p_second;
		offset = *first->GetTransformDispatch()->m_position(first, NULL);
		const Vector3* position = second->GetTransformDispatch()->m_position(second, NULL);
		offset.m_x -= position->m_x;
		offset.m_y -= position->m_y;
		offset.m_z -= position->m_z;
	}
	else {
		offset = *p_offset;
	}
	LegoFloat penetration = m_penetration;
	m_point.m_x = penetration * m_separationAxis.m_x + m_point.m_x;
	m_point.m_y = penetration * m_separationAxis.m_y + m_point.m_y;
	m_point.m_z = penetration * m_separationAxis.m_z + m_point.m_z;
	m_separationAxis.m_x *= -1.0f;
	m_separationAxis.m_y *= -1.0f;
	m_separationAxis.m_z *= -1.0f;
	m_normal.m_x *= -1.0f;
	m_normal.m_y *= -1.0f;
	m_normal.m_z *= -1.0f;
	GameLifecycleComponent* first = m_first;
	m_first = m_second;
	m_second = first;
	m_point.m_x = offset.m_x + m_point.m_x;
	m_point.m_y = offset.m_y + m_point.m_y;
	m_point.m_z = offset.m_z + m_point.m_z;
}
