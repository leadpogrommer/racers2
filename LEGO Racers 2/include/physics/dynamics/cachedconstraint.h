#ifndef LR2_CACHEDCONSTRAINT_H
#define LR2_CACHEDCONSTRAINT_H

#include "physics/dynamics/physicallifecycle.h"

#include <stddef.h>

struct CollisionContact;
struct ConstraintEndpointBinding {
	PhysicalLifecycleState* m_body;
	Vector3 m_position;
};

struct ConstraintEndpoint {
	void SetBinding(const ConstraintEndpointBinding* p_binding, LegoU32 p_fixed);
	void SetWorldBinding(const ConstraintEndpointBinding* p_binding);
	void GetWorldPosition(Vector3* p_result);
	void UpdateWorldOffset();
	void GetPointVelocity(Vector3* p_result);
	void UpdatePointAcceleration();
	void GetPointAcceleration(Vector3* p_result);
	void GetEffectiveMassMatrix(Matrix3* p_result);
	PhysicalLifecycleState* m_body;
	Vector3 m_localPosition;
	Vector3 m_acceleration;
	Vector3 m_worldOffset;
	LegoU32 m_fixed;
};
DECOMP_SIZE_ASSERT(ConstraintEndpoint, 0x2c);

class CachedConstraint;
struct ConstraintBaseDispatch {
	CachedConstraint*(__fastcall* m_destroy)(CachedConstraint*, void*, LegoU32);
};
struct ContactConstraintDispatch {
	ConstraintBaseDispatch m_base;
	LegoBool32(__fastcall* m_active)(CachedConstraint*, void*);
	LegoU32(__fastcall* m_prepare)(CachedConstraint*, void*);
	LegoU32(__fastcall* m_update)(CachedConstraint*, void*);
};

class CachedConstraint {
public:
	CachedConstraint();
	~CachedConstraint();
	LegoBool32 IsInactive();
	void GetPositionDifference(Vector3* p_result);
	void GetVelocityDifference(Vector3* p_result);
	void ApplyForces(LegoU32 p_reverse);
	void CopyContact(const CollisionContact* p_contact);
	void GetAccelerationDifference(Vector3* p_result);
	void ComputeNormalForce(Vector3* p_result, const Vector3* p_acceleration, const Vector3* p_normal);
	void RebuildEffectiveMassMatrix(LegoU32 p_invert);
	const ConstraintBaseDispatch* m_vtable;
	ConstraintEndpoint m_endpoints[2];
	Vector3 m_force;                // 0x5c
	Matrix3 m_effectiveMass;        // 0x68
	Matrix3 m_inverseEffectiveMass; // 0x8c
	undefined m_unk0xb0[0xbc - 0xb0];
};
DECOMP_STATIC_ASSERT(offsetof(CachedConstraint, m_force) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(CachedConstraint, m_effectiveMass) == 0x68);
DECOMP_SIZE_ASSERT(CachedConstraint, 0xbc);

class CachedCollisionContact : public CachedConstraint {
public:
	CachedCollisionContact();
	LegoU32 PrepareContact();
	LegoU32 UpdateContact();
	LegoU32 m_prepared; // 0xbc
	Vector3 m_normal;   // 0xc0
};
DECOMP_SIZE_ASSERT(CachedCollisionContact, 0xcc);
extern const ConstraintBaseDispatch g_constraintBaseDispatch;
extern const ConstraintBaseDispatch g_inertConstraintDispatch;
extern const ContactConstraintDispatch g_contactConstraintDispatch;
#endif
