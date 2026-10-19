#include "cardisplay.h"
#include "gameobjectdispatch.h"
#include "physicallifecycle.h"

// GLOBAL: LEGORACERS2 0x00549bb4
LegoU32 g_nextPhysicalLifecycleId = 1;

#define PHYSICAL_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
// Retail ABI function-pointer table: 0x00535398; complete inventory verified
// against both the original PE and the rebuilt linker map.
const PhysicalLifecycleDispatch g_physicalLifecycleDispatch = {
	{PHYSICAL_CALLBACK(&IsLifecycleActive),
	 PHYSICAL_CALLBACK(&GetLifecycleParentValue),
	 PHYSICAL_CALLBACK(&GameObjectBaseTrue),
	 PHYSICAL_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 PHYSICAL_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 PHYSICAL_CALLBACK(&GetLifecyclePosition),
	 PHYSICAL_CALLBACK(&GetLifecycleRotation),
	 PHYSICAL_CALLBACK(&GetLifecycleOrientation),
	 PHYSICAL_CALLBACK(&SetLifecyclePosition),
	 PHYSICAL_CALLBACK(&SetLifecycleOrientation),
	 PHYSICAL_CALLBACK(&GetLifecycleParent),
	 PHYSICAL_CALLBACK(&GameObjectBaseZero),
	 PHYSICAL_CALLBACK(&GameObjectBaseZero),
	 PHYSICAL_CALLBACK(&GameObjectBaseZeroOneArgument),
	 PHYSICAL_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 PHYSICAL_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 PHYSICAL_CALLBACK(&CaptureLifecycleTransform),
	 PHYSICAL_CALLBACK(&GetCapturedLifecycleTransform),
	 PHYSICAL_CALLBACK(&DestroyCarDisplayBaseTransform),
	 PHYSICAL_CALLBACK(&ClearLifecycleMotion)}
};
#undef PHYSICAL_CALLBACK

// STUB: LEGORACERS2 0x004a6070
PhysicalLifecycleState* __fastcall ConstructPhysicalLifecycleState(
	PhysicalLifecycleState* p_state,
	void*,
	GameObject* p_owner,
	const PhysicalLifecycleParameters* p_parameters
)
{
	p_state->m_base.m_owner = p_owner;
	p_state->m_base.m_stateFlags &= ~1u;
	p_state->m_base.m_collisionProxy = NULL;
	p_state->m_orientation = p_parameters->m_orientation;
	p_state->m_position = p_parameters->m_position;
	p_state->m_contactFriction = p_parameters->m_contactFriction;
	p_state->m_contactResponse = p_parameters->m_contactResponse;
	p_state->m_unk0x58 = p_parameters->m_propertyIndex;
	p_state->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_carDisplayBaseTransformDispatch;
	p_state->m_base.m_stateFlags &= ~2u;
	p_state->m_orientation.Normalize();
	p_state->m_rotation.FUN_004a4d30(&p_state->m_orientation.m_x);
	p_state->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_physicalLifecycleDispatch;
	((LegoU8*) &p_state->m_unk0xa0)[0] = 0;
	((LegoU8*) &p_state->m_unk0xa0)[1] = 0;
	p_state->m_parentValue = g_nextPhysicalLifecycleId++;
	p_state->m_mass = p_parameters->m_mass;
	p_state->m_inverseMass = 1.0f / p_state->m_mass;
	p_state->m_inverseInertia = p_parameters->m_inertia;
	LegoFloat inverseMass = p_state->m_inverseMass;
	p_state->m_inverseInertia.m_x *= inverseMass;
	p_state->m_inverseInertia.m_y *= inverseMass;
	p_state->m_inverseInertia.m_z *= inverseMass;
	p_state->m_parameters = p_parameters;
	ResetPhysicalLifecycleState(p_state, NULL, p_owner);
	return p_state;
}

// FUNCTION: LEGORACERS2 0x004a61b0
void AdvancePhysicalLifecycleParentValue()
{
	++g_gameObjectInitializationValue;
	if ((LegoS32) g_gameObjectInitializationValue > 499) {
		g_gameObjectInitializationValue = 100;
	}
}

// STUB: LEGORACERS2 0x004a61d0
void __fastcall ResetPhysicalLifecycleState(PhysicalLifecycleState* p_state, void*, GameObject*)
{
	Vector3 zero = {0.0f, 0.0f, 0.0f};
	p_state->m_base.m_stateFlags &= ~2u;
	p_state->m_linearMomentum = zero;
	p_state->m_angularMomentum = zero;
	p_state->m_linearVelocity = zero;
	p_state->m_worldAngularVelocity = zero;
	p_state->m_force = zero;
	p_state->m_torque = zero;
	p_state->RebuildInverseInertiaTensor();
	p_state->UpdateAndClampVelocities();
	p_state->m_parentOffset = zero;
	Quaternion identity = {0.0f, 0.0f, 0.0f, 1.0f};
	p_state->m_parentOrientation = identity;
	p_state->m_parentValue = g_gameObjectInitializationValue;
	p_state->m_savedParentValue = g_gameObjectInitializationValue;
	p_state->m_parent = NULL;
	AdvancePhysicalLifecycleParentValue();
	if (p_state->m_parameters != NULL) {
		if (p_state->m_parameters->m_mass > 999999.875f) {
			LegoU32 active = p_state->GetTransformDispatch()->m_isActive(p_state, NULL, 0) != 0;
			p_state->m_base.m_stateFlags =
				((p_state->m_base.m_stateFlags ^ active) & 1u) ^ p_state->m_base.m_stateFlags;
		}
		p_state->m_position = p_state->m_parameters->m_position;
		p_state->m_orientation = p_state->m_parameters->m_orientation;
		p_state->m_rotation.FUN_004a4d30(&p_state->m_orientation.m_x);
		((LegoU8*) &p_state->m_unk0xa0)[0] = 0;
		p_state->RebuildInverseInertiaTensor();
	}
}
