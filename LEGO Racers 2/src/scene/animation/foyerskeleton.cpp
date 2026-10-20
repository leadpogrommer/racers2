#include "scene/animation/foyerskeleton.h"

#include "math/matrix3.h"
#include "physics/dynamics/physicallifecycle.h"

// FUNCTION: LEGORACERS2 0x00460680 FOLDED
FoyerSkeletonRecord* __fastcall ConstructFoyerSkeletonRecord(FoyerSkeletonRecord* p_record, void*)
{
	return p_record;
}

// STUB: LEGORACERS2 0x004c22c0
void __fastcall RebuildFoyerSkeletonTransforms(GameObject* p_object, void*)
{
	LegoU8* object = (LegoU8*) p_object;
	FoyerSkeletonRecord* records = *(FoyerSkeletonRecord**) (object + 0x194);
	const Vector3* offset = (const Vector3*) (object + 0x1ec);
	Vector3 position = records[0].m_localPosition;
	position.m_x += offset->m_x;
	position.m_y += offset->m_y;
	position.m_z += offset->m_z;
	Quaternion orientation = records[0].m_localOrientation;
	records[0].m_worldOrientation = orientation;
	records[0].m_worldPosition = position;

	for (LegoS32 index = 1; index < *(LegoS32*) (object + 0x190); ++index) {
		FoyerSkeletonRecord* record = &records[index];
		Vector3 local = record->m_localPosition;
		if (record->m_parentIndex != index - 1) {
			FoyerSkeletonRecord* parent = &records[record->m_parentIndex];
			orientation = parent->m_worldOrientation;
			position = parent->m_worldPosition;
		}
		Matrix3 rotation;
		rotation.FUN_004a4d30(&orientation.m_x);
		local.FUN_004a4e00(&rotation);
		local.m_x = position.m_x + local.m_x;
		local.m_y = position.m_y + local.m_y;
		local.m_z = position.m_z + local.m_z;
		orientation.MultiplyRight(&record->m_localOrientation);
		record->m_worldOrientation = orientation;
		record->m_worldPosition = local;
		position = local;
	}
}

// STUB: LEGORACERS2 0x004f60f0
void __fastcall BindFoyerSkeletonRecord(
	FoyerSkeletonRecord* p_record,
	void*,
	const FoyerSkeletonDefinition* p_definition,
	GameObject* p_owner
)
{
	p_record->m_owner = p_owner;
	p_record->m_index = p_definition->m_index;
	p_record->m_parentIndex = p_definition->m_parentIndex;
	p_record->m_parent = p_record->m_parentIndex < 0 ? NULL : p_record + (p_record->m_parentIndex - p_record->m_index);
	p_record->m_localPosition = p_definition->m_position;
	p_record->m_localOrientation = p_definition->m_orientation;
	p_record->m_worldOrientation.m_x = 0.0f;
	p_record->m_worldOrientation.m_y = 0.0f;
	p_record->m_worldOrientation.m_z = 0.0f;
	p_record->m_worldOrientation.m_w = 1.0f;
}

// STUB: LEGORACERS2 0x004f61a0
void __fastcall RebuildFoyerSkeletonRecordTransform(FoyerSkeletonRecord* p_record, void*)
{
	if (p_record->m_parentIndex < 0) {
		Quaternion identity = {0.0f, 0.0f, 0.0f, 1.0f};
		p_record->m_worldOrientation = identity;
	}
	else {
		p_record->m_worldOrientation = p_record->m_parent->m_worldOrientation;
	}
	p_record->m_worldOrientation.MultiplyRight(&p_record->m_localOrientation);
	Vector3 position = p_record->m_localPosition;
	Matrix3 rotation;
	if (p_record->m_parentIndex < 0) {
		const Vector3* offset = (const Vector3*) ((const LegoU8*) p_record->m_owner + 0x1ec);
		position.m_x += offset->m_x;
		position.m_y += offset->m_y;
		position.m_z += offset->m_z;
		PhysicalLifecycleState* state = (PhysicalLifecycleState*) ((LegoU8*) p_record->m_owner + 0x58);
		const Quaternion* orientation = state->GetTransformDispatch()->m_orientation(state, NULL);
		rotation.FUN_004a4d30(&orientation->m_x);
		position.FUN_004a4e00(&rotation);
	}
	else {
		rotation.FUN_004a4d30(&p_record->m_parent->m_worldOrientation.m_x);
		position.FUN_004a4e00(&rotation);
		position.m_x += p_record->m_parent->m_worldPosition.m_x;
		position.m_y += p_record->m_parent->m_worldPosition.m_y;
		position.m_z += p_record->m_parent->m_worldPosition.m_z;
	}
	p_record->m_worldPosition = position;
}
