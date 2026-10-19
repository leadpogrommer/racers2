#include "physicallifecycle.h"

#include <stdlib.h>

// Keep the retail division; VC6 otherwise replaces /331 with a rounded reciprocal.
// GLOBAL: LEGORACERS2 0x0053537c
extern const volatile LegoFloat g_physicalLifecycleStepDivisor = 331.0f;

// GLOBAL: LEGORACERS2 0x005790c8
LegoU8 g_lifecycleWorldPositionInitialized;
// GLOBAL: LEGORACERS2 0x005790cc
LegoFloat g_physicalLifecycleStep;
// GLOBAL: LEGORACERS2 0x00579118
LegoFloat g_physicalLifecycleNormalizedStep;
// GLOBAL: LEGORACERS2 0x005790e0
Matrix3 g_lifecycleWorldRotation;
// GLOBAL: LEGORACERS2 0x00579108
Vector3 g_lifecycleWorldPosition;
// GLOBAL: LEGORACERS2 0x0057911c
LegoU8 g_lifecycleWorldRotationInitialized;
// GLOBAL: LEGORACERS2 0x0057912c
LegoU8 g_lifecycleWorldOrientationInitialized;
// GLOBAL: LEGORACERS2 0x00579130
Quaternion g_lifecycleWorldOrientation;

static void LifecycleWorldPositionExit();
static void LifecycleWorldRotationExit();
static void LifecycleWorldOrientationExit();

// STUB: LEGORACERS2 0x0042f190
void __fastcall CaptureLifecycleTransform(PhysicalLifecycleState* p_state, void*)
{
	p_state->m_savedPosition = *p_state->GetTransformDispatch()->m_position(p_state, NULL);
	p_state->m_savedOrientation = *p_state->GetTransformDispatch()->m_orientation(p_state, NULL);
}

// FUNCTION: LEGORACERS2 0x0042f1d0
LegoU32 __fastcall GetCapturedLifecycleTransform(
	PhysicalLifecycleState* p_state,
	void*,
	Vector3* p_position,
	Quaternion* p_orientation
)
{
	*p_position = p_state->m_savedPosition;
	*p_orientation = p_state->m_savedOrientation;
	return 1;
}

// FUNCTION: LEGORACERS2 0x0042f210
LegoBool32 __fastcall IsLifecycleActive(PhysicalLifecycleState* p_state, void*, LegoU32 p_checkState)
{
	return p_checkState == 0 ? TRUE : ~p_state->m_base.m_stateFlags & 1;
}

// FUNCTION: LEGORACERS2 0x0042f230
LegoU32 __fastcall GetLifecycleParentValue(PhysicalLifecycleState* p_state, void*)
{
	return p_state->m_parentValue;
}

// STUB: LEGORACERS2 0x0042f240
void __fastcall ClearLifecycleMotion(PhysicalLifecycleState* p_state, void*)
{
	p_state->m_worldAngularVelocity.m_x = 0.0f;
	p_state->m_worldAngularVelocity.m_y = 0.0f;
	p_state->m_worldAngularVelocity.m_z = 0.0f;
	p_state->m_linearVelocity.m_x = 0.0f;
	p_state->m_linearVelocity.m_y = 0.0f;
	p_state->m_linearVelocity.m_z = 0.0f;
	p_state->m_angularMomentum.m_x = 0.0f;
	p_state->m_linearMomentum.m_x = 0.0f;
	p_state->m_angularMomentum.m_y = 0.0f;
	p_state->m_linearMomentum.m_y = 0.0f;
	p_state->m_angularMomentum.m_z = 0.0f;
	p_state->m_linearMomentum.m_z = 0.0f;
}

// FUNCTION: LEGORACERS2 0x0042f2b0
const Vector3* __fastcall GetLifecyclePosition(PhysicalLifecycleState* p_state, void*)
{
	return p_state->m_parent == NULL ? &p_state->m_position : p_state->GetWorldPosition();
}

// FUNCTION: LEGORACERS2 0x0042f2d0
const Matrix3* __fastcall GetLifecycleRotation(PhysicalLifecycleState* p_state, void*)
{
	return p_state->m_parent == NULL ? &p_state->m_rotation : p_state->GetWorldRotation();
}

// FUNCTION: LEGORACERS2 0x0042f2f0
const Quaternion* __fastcall GetLifecycleOrientation(PhysicalLifecycleState* p_state, void*)
{
	return p_state->m_parent == NULL ? &p_state->m_orientation : p_state->GetWorldOrientation();
}

// FUNCTION: LEGORACERS2 0x0042f310
void __fastcall SetLifecyclePosition(PhysicalLifecycleState* p_state, void*, const Vector3* p_position)
{
	if (p_state->m_parent == NULL) {
		p_state->m_position = *p_position;
	}
	else {
		p_state->SetWorldPosition(p_position);
	}
}

// FUNCTION: LEGORACERS2 0x0042f350
void __fastcall SetLifecycleOrientation(PhysicalLifecycleState* p_state, void*, const Quaternion* p_orientation)
{
	if (p_state->m_parent == NULL) {
		p_state->m_orientation = *p_orientation;
		p_state->m_rotation.FUN_004a4d30(&p_state->m_orientation.m_x);
	}
	else {
		p_state->SetWorldOrientation(p_orientation);
	}
}

// FUNCTION: LEGORACERS2 0x00496120
PhysicalLifecycleState* __fastcall GetLifecycleParent(PhysicalLifecycleState* p_state, void*)
{
	return p_state->m_parent;
}

// STUB: LEGORACERS2 0x004a6020
void SetPhysicalLifecycleStep(LegoFloat p_step)
{
	g_physicalLifecycleNormalizedStep = p_step / g_physicalLifecycleStepDivisor;
	g_physicalLifecycleStep = p_step;
}

// STUB: LEGORACERS2 0x004a6e00
const Vector3* PhysicalLifecycleState::GetWorldPosition()
{
	Vector3 position = m_parentOffset;
	position.FUN_004a4e00(m_parent->GetTransformDispatch()->m_rotation(m_parent, NULL));
	const Vector3* parentPosition = m_parent->GetTransformDispatch()->m_position(m_parent, NULL);
	position.m_x += parentPosition->m_x;
	position.m_y += parentPosition->m_y;
	position.m_z += parentPosition->m_z;
	if ((g_lifecycleWorldPositionInitialized & 1) == 0) {
		g_lifecycleWorldPositionInitialized |= 1;
		atexit(LifecycleWorldPositionExit);
	}
	g_lifecycleWorldPosition = position;
	return &g_lifecycleWorldPosition;
}

// FUNCTION: LEGORACERS2 0x004a6eb0 FOLDED
static void LifecycleWorldPositionExit()
{
}

// FUNCTION: LEGORACERS2 0x004a6ff0
const Matrix3* PhysicalLifecycleState::GetWorldRotation()
{
	if ((g_lifecycleWorldRotationInitialized & 1) == 0) {
		g_lifecycleWorldRotationInitialized |= 1;
		atexit(LifecycleWorldRotationExit);
	}
	g_lifecycleWorldRotation.FUN_004a4d30(&GetWorldOrientation()->m_x);
	return &g_lifecycleWorldRotation;
}

// FUNCTION: LEGORACERS2 0x004a7030 FOLDED
static void LifecycleWorldRotationExit()
{
}

// FUNCTION: LEGORACERS2 0x004a7040
const Quaternion* PhysicalLifecycleState::GetWorldOrientation()
{
	if ((g_lifecycleWorldOrientationInitialized & 1) == 0) {
		g_lifecycleWorldOrientationInitialized |= 1;
		atexit(LifecycleWorldOrientationExit);
	}
	g_lifecycleWorldOrientation = *m_parent->GetTransformDispatch()->m_orientation(m_parent, NULL);
	g_lifecycleWorldOrientation.MultiplyRight(&m_parentOrientation);
	return &g_lifecycleWorldOrientation;
}

// FUNCTION: LEGORACERS2 0x004a70b0 FOLDED
static void LifecycleWorldOrientationExit()
{
}

// STUB: LEGORACERS2 0x004a70c0
void PhysicalLifecycleState::SetWorldPosition(const Vector3* p_position)
{
	m_parentOffset = *p_position;
	const Vector3* parentPosition = m_parent->GetTransformDispatch()->m_position(m_parent, NULL);
	m_parentOffset.m_x -= parentPosition->m_x;
	m_parentOffset.m_y -= parentPosition->m_y;
	m_parentOffset.m_z -= parentPosition->m_z;
	Matrix3 rotation;
	rotation.FUN_004a4d30(&m_parent->GetTransformDispatch()->m_orientation(m_parent, NULL)->m_x);
	m_parentOffset.FUN_004a4e50(&rotation);
}

// STUB: LEGORACERS2 0x004a7130
void PhysicalLifecycleState::SetWorldOrientation(const Quaternion* p_orientation)
{
	m_parentOrientation = *m_parent->GetTransformDispatch()->m_orientation(m_parent, NULL);
	m_parentOrientation.m_x = -m_parentOrientation.m_x;
	m_parentOrientation.m_y = -m_parentOrientation.m_y;
	m_parentOrientation.m_z = -m_parentOrientation.m_z;
	m_parentOrientation.MultiplyRight(p_orientation);
}
