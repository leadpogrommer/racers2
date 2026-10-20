#include "frontend/preview/cardisplay.h"
#include "game/character/charactervisual.h"
#include "game/objects/gameobjectdispatch.h"
#include "scene/objects/foyermesh.h"
#include "scene/objects/foyertemplateinstance.h"

extern LegoFloat g_currentSimulationStep;

static LegoU32 __fastcall IntegrateFoyerTemplateLifecycle(PhysicalLifecycleState*, void*);
static LegoU32 __fastcall ApplyFoyerTemplateLifecycleForces(PhysicalLifecycleState*, void*);

#define FOYER_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
// Retail ABI function-pointer table: 0x00536500.
const PhysicalLifecycleDispatch g_foyerTemplateLifecycleDispatch = {
	{FOYER_CALLBACK(&IsLifecycleActive),
	 FOYER_CALLBACK(&GetLifecycleParentValue),
	 FOYER_CALLBACK(&GameObjectBaseZero),
	 FOYER_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 FOYER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 FOYER_CALLBACK(&GetLifecyclePosition),
	 FOYER_CALLBACK(&GetLifecycleRotation),
	 FOYER_CALLBACK(&GetLifecycleOrientation),
	 FOYER_CALLBACK(&SetLifecyclePosition),
	 FOYER_CALLBACK(&SetLifecycleOrientation),
	 FOYER_CALLBACK(&GetLifecycleParent),
	 FOYER_CALLBACK(&IntegrateFoyerTemplateLifecycle),
	 FOYER_CALLBACK(&ApplyFoyerTemplateLifecycleForces),
	 FOYER_CALLBACK(&GameObjectBaseZeroOneArgument),
	 FOYER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 FOYER_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 FOYER_CALLBACK(&CaptureLifecycleTransform),
	 FOYER_CALLBACK(&GetCapturedLifecycleTransform),
	 FOYER_CALLBACK(&DestroyCharacterVisualHeadLifecycle),
	 FOYER_CALLBACK(&ClearLifecycleMotion)}
};
#undef FOYER_CALLBACK

// STUB: LEGORACERS2 0x00493420
static LegoU32 __fastcall IntegrateFoyerTemplateLifecycle(PhysicalLifecycleState* p_state, void*)
{
	return IntegrateFoyerTemplateMotion((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

// STUB: LEGORACERS2 0x00493430
static LegoU32 __fastcall ApplyFoyerTemplateLifecycleForces(PhysicalLifecycleState* p_state, void*)
{
	return ApplyFoyerTemplateForces((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

// STUB: LEGORACERS2 0x004c1da0
LegoU32 __fastcall UpdateFoyerTemplateInstance(GameObject* p_object, void*, LegoFloat p_step)
{
	FoyerTemplateInstance* object = (FoyerTemplateInstance*) p_object;
	if ((object->m_state & 1) == 0) {
		return 0;
	}
	RebuildFoyerSkeletonTransforms(object, NULL);
	if (object->m_lookEnabled == 1) {
		UpdateFoyerTemplateLookOrientation(object, NULL, p_step);
	}
	object->m_state |= 2;
	if (object->m_attachment != NULL) {
		*(LegoU32*) ((LegoU8*) object->m_attachment + 0x14) = 1;
		object->m_state |= 0x2000;
	}
	return 1;
}

// STUB: LEGORACERS2 0x004c1e70
void __fastcall UpdateFoyerTemplateAttachment(GameObject* p_object, void*)
{
	FoyerTemplateInstance* object = (FoyerTemplateInstance*) p_object;
	LegoU32 state = object->m_state;
	object->m_state = state | 0x4000;
	// Retail additionally compares (state & 4) with 1; that comparison never equals.
	if ((state & 0x2000) != 0 && (state & 0x1000) != 0) {
		if (object->m_attachment != NULL &&
			TryUpdateFoyerMeshWithRenderer((FoyerMeshAttachment*) object->m_attachment) == FALSE) {
			UpdateFoyerMeshVertices((FoyerMeshAttachment*) object->m_attachment, NULL);
			UploadFoyerMeshVertices((FoyerMeshAttachment*) object->m_attachment, NULL);
		}
		object->m_state &= ~0x2000u;
	}
}

// STUB: LEGORACERS2 0x004c24e0
LegoU32 __fastcall IntegrateFoyerTemplateMotion(GameObject* p_object, void*)
{
	FoyerTemplateInstance* object = (FoyerTemplateInstance*) p_object;
	if ((object->m_state & 0x400) == 0) {
		if ((object->m_state & 0x40000) != 0) {
			object->m_state &= ~0x40000u;
			Vector3 zero = {0.0f, 0.0f, 0.0f};
			object->m_physical.m_force = zero;
		}
		if ((object->m_state & 0x20000) != 0) {
			object->m_physical.m_force.m_x += object->m_impulse.m_x;
			object->m_physical.m_force.m_y += object->m_impulse.m_y;
			object->m_physical.m_force.m_z += object->m_impulse.m_z;
			object->m_state &= ~0x20000u;
		}
		return IntegratePhysicalLifecycleMotion(&object->m_physical, NULL);
	}
	if ((object->m_state & 0x10000) == 0) {
		object->m_state |= 0x10000;
		PhysicalLifecycleState* state = (PhysicalLifecycleState*) object->m_lifecycle0x54;
		object->m_movementStart = *state->GetTransformDispatch()->m_position(state, NULL);
		object->m_movementInitialized = 0;
	}
	PhysicalLifecycleState* state = &object->m_physical;
	Vector3 position = *state->GetTransformDispatch()->m_position(state, NULL);
	Vector3 velocity = object->m_localVelocity;
	const Quaternion* orientation = state->GetTransformDispatch()->m_orientation(state, NULL);
	Matrix3 rotation;
	rotation.FUN_004a4d30(&orientation->m_x);
	velocity.FUN_004a4e00(&rotation);
	velocity.m_x *= g_currentSimulationStep;
	velocity.m_y *= g_currentSimulationStep;
	velocity.m_z *= g_currentSimulationStep;
	position.m_x += velocity.m_x;
	position.m_y += velocity.m_y;
	position.m_z += velocity.m_z;
	state = (PhysicalLifecycleState*) object->m_lifecycle0x54;
	state->GetTransformDispatch()->m_setPosition(state, NULL, &position);
	return 1;
}

// STUB: LEGORACERS2 0x004c26c0
LegoU32 __fastcall ApplyFoyerTemplateForces(GameObject* p_object, void*)
{
	FoyerTemplateInstance* object = (FoyerTemplateInstance*) p_object;
	if ((object->m_state & 0x400) == 0) {
		return ApplyPhysicalLifecycleDampingAndGravity(&object->m_physical, NULL);
	}
	return 0;
}

// STUB: LEGORACERS2 0x004c2790
void __fastcall UpdateFoyerTemplateLookOrientation(GameObject* p_object, void*, LegoFloat p_step)
{
	FoyerTemplateInstance* object = (FoyerTemplateInstance*) p_object;
	if (object->m_attachmentSkeletonIndex < 0) {
		return;
	}
	FoyerSkeletonRecord* record = object->m_skeletons + object->m_attachmentSkeletonIndex;
	FoyerSkeletonRecord* parent = object->m_skeletons + record->m_parentIndex;
	Vector3 target = object->m_lookTarget;
	const Vector3* position = object->m_physical.GetTransformDispatch()->m_position(&object->m_physical, NULL);
	target.m_x -= position->m_x;
	target.m_y -= position->m_y;
	target.m_z -= position->m_z;
	const Quaternion* orientation = object->m_physical.GetTransformDispatch()->m_orientation(&object->m_physical, NULL);
	Quaternion inverse = *orientation;
	inverse.m_x = -inverse.m_x;
	inverse.m_y = -inverse.m_y;
	inverse.m_z = -inverse.m_z;
	Matrix3 rotation;
	rotation.FUN_004a4d30(&inverse.m_x);
	target.FUN_004a4e00(&rotation);
	inverse = parent->m_worldOrientation;
	inverse.m_x = -inverse.m_x;
	inverse.m_y = -inverse.m_y;
	inverse.m_z = -inverse.m_z;
	rotation.FUN_004a4d30(&inverse.m_x);
	target.FUN_004a4e00(&rotation);
	target.Normalize();
	Vector3 zero = {0.0f, 0.0f, 0.0f};
	Quaternion candidate = {0.0f, 0.0f, 0.0f, 1.0f};
	candidate.LookAt(&target, &zero, &object->m_lookUp, 1);
	Vector3 forward = {0.0f, 0.0f, 1.0f};
	rotation.FUN_004a4d30(&inverse.m_x);
	forward.FUN_004a4e00(&rotation);
	LegoFloat dot = forward.m_z * target.m_z + target.m_y * forward.m_y + forward.m_x * target.m_x;
	if (!(dot >= object->m_lookThreshold)) {
		if ((object->m_lookFlags & 1) != 0) {
			object->m_lookBlend = 0.0f;
			object->m_lookFlags &= ~1u;
		}
		LegoFloat blend = object->m_lookBlend;
		if (!(blend >= 1.0f)) {
			Quaternion local = record->m_localOrientation;
			Quaternion saved = object->m_lookOrientation;
			candidate.Interpolate(&local, &saved, blend);
			object->m_lookBlend += p_step;
		}
		else {
			candidate = record->m_localOrientation;
		}
	}
	else if (object->m_lookBlend > 0.0f) {
		object->m_lookBlend -= p_step;
		if (!(object->m_lookBlend >= 0.0f)) {
			object->m_lookBlend = 0.0f;
		}
		Quaternion saved = object->m_lookOrientation;
		Quaternion targetOrientation = candidate;
		candidate.Interpolate(&saved, &targetOrientation, object->m_lookBlend);
	}
	record->m_localOrientation = candidate;
	RebuildFoyerSkeletonRecordTransform(record, NULL);
	object->m_appliedLookOrientation = candidate;
	object->m_lookOrientation = candidate;
}

// STUB: LEGORACERS2 0x004c8150
LegoU32 __fastcall GetFoyerTemplateObjectCategory(GameObject*, void*)
{
	return 2;
}
