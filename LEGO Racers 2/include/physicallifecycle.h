#ifndef LR2_PHYSICALLIFECYCLE_H
#define LR2_PHYSICALLIFECYCLE_H

#include "gamemanager.h"
#include "matrix3.h"
#include "quaternion.h"
#include "vector3.h"

struct PhysicalLifecycleState;

struct LifecycleTransformDispatch {
	typedef LegoBool32(__fastcall* Predicate)(PhysicalLifecycleState*, void*, LegoU32);
	typedef LegoU32(__fastcall* Value)(PhysicalLifecycleState*, void*);
	typedef const Vector3*(__fastcall* Position)(PhysicalLifecycleState*, void*);
	typedef const Matrix3*(__fastcall* RotationMatrix)(PhysicalLifecycleState*, void*);
	typedef const Quaternion*(__fastcall* Orientation)(PhysicalLifecycleState*, void*);
	typedef PhysicalLifecycleState*(__fastcall* Parent)(PhysicalLifecycleState*, void*);
	Predicate m_isActive; // 0x00
	Value m_value0x04;
	undefined m_unk0x08[0x14 - 0x08];
	Position m_position;       // 0x14
	RotationMatrix m_rotation; // 0x18
	Orientation m_orientation; // 0x1c
	undefined m_unk0x20[0x28 - 0x20];
	Parent m_parent; // 0x28
};
DECOMP_SIZE_ASSERT(LifecycleTransformDispatch, 0x2c);

// Evidenced state prefix; this is not a claim about the complete derived object.
struct PhysicalLifecycleState {
	void RebuildWorldMatrix();
	void UpdateAndClampVelocities();
	void GetParentVelocityAtOffset(const Vector3* p_offset, Vector3* p_result);
	const LifecycleTransformDispatch* GetTransformDispatch() const
	{
		return (const LifecycleTransformDispatch*) m_base.m_vtable;
	}

	GameLifecycleComponent m_base;
	Quaternion m_orientation; // 0x10
	Vector3 m_position;       // 0x20
	Matrix3 m_rotation;       // 0x2c
	undefined m_unk0x50[0x7c - 0x50];
	Vector3 m_parentOffset;         // 0x7c
	Quaternion m_parentOrientation; // 0x88
	LegoU32 m_savedParentValue;     // 0x98
	LegoU32 m_parentValue;          // 0x9c
	LegoU32 m_unk0xa0;
	PhysicalLifecycleState* m_parent; // 0xa4
	Vector3 m_linearMomentum;         // 0xa8
	Vector3 m_localAngularVelocity;   // 0xb4
	Vector3 m_linearVelocity;         // 0xc0
	Vector3 m_worldAngularVelocity;   // 0xcc
	Matrix3 m_worldMatrix;            // 0xd8
	Vector3 m_force;                  // 0xfc
	Vector3 m_torque;                 // 0x108
	Vector3 m_scale;                  // 0x114
	LegoFloat m_mass;                 // 0x120
	LegoFloat m_inverseMass;          // 0x124
};
DECOMP_SIZE_ASSERT(PhysicalLifecycleState, 0x128);

void __fastcall ClearLifecycleParent(GameLifecycleComponent* p_lifecycle, void*, LegoU32 p_flag);

#endif // LR2_PHYSICALLIFECYCLE_H
