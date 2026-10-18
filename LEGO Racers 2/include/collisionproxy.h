#ifndef LR2_COLLISIONPROXY_H
#define LR2_COLLISIONPROXY_H

#include "matrix3.h"
#include "vector3.h"

#include <stddef.h>

class CollisionProxy;
struct CollisionModelView;
struct CollisionQueryResult;

struct CollisionModelTransform {
	CollisionModelTransform* ResetIdentity();
	Vector3 m_position;
	Matrix3 m_rotation;
};
DECOMP_SIZE_ASSERT(CollisionModelTransform, 0x30);

struct CollisionChildDefinitions {
	CollisionProxy** CreateProxies(CollisionProxy* p_parent, LegoS32* p_count);
	CollisionModelTransform* m_transforms;
	LegoU32* m_modelIndices;
	LegoS32 m_count;
	CollisionModelView** m_models;
};

struct CollisionPrimitiveRecord {
	Vector3 m_position;
	undefined m_unk0x0c[0x14 - 0x0c];
};
DECOMP_SIZE_ASSERT(CollisionPrimitiveRecord, 0x14);

struct CollisionModelView {
	LegoBool32 IntersectSegment(const Vector3*, const Vector3*, CollisionQueryResult*);
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;
	LegoU32 m_unk0x08;
	CollisionPrimitiveRecord* m_primitives; // 0x0c
	LegoU32 m_unk0x10;
	LegoS32 m_primitiveCount; // 0x14
	LegoU32 m_unk0x18;
	LegoFloat m_radius; // 0x1c
	undefined m_unk0x20[0x38 - 0x20];
	CollisionChildDefinitions m_childDefinitions; // 0x38
};

struct SpatialIndexLink {
	SpatialIndexLink* Initialize(CollisionProxy* p_proxy);
	void Destroy();
	void Detach();
	void RefreshBounds();
	void Attach();
	void SetFilter(const LegoS32* p_filter);
	void Update();

	CollisionProxy* m_proxy; // 0x00
	LegoS32 m_filter;        // 0x04
	LegoS32 m_index;         // 0x08
	union {
		undefined m_unk0x0c[0x1c - 0x0c];
		struct {
			LegoS32 m_gridMinX;
			LegoS32 m_gridMaxX;
			LegoS32 m_gridMinZ;
			LegoS32 m_gridMaxZ;
		};
	};
	LegoFloat m_minX; // 0x1c
	LegoFloat m_minZ; // 0x20
	LegoFloat m_maxX; // 0x24
	LegoFloat m_maxZ; // 0x28
	void* m_xHandle;  // 0x2c
	void* m_zHandle;  // 0x30
};
DECOMP_SIZE_ASSERT(SpatialIndexLink, 0x34);

// Evidenced prefix, shared by root and subordinate collision proxies.
class CollisionProxy {
public:
	CollisionProxy(CollisionModelView* p_model, LegoU32 p_tag);
	~CollisionProxy();
	LegoBool32 IntersectSegment(const Vector3*, const Vector3*, CollisionQueryResult*);
	void DetachFromIndex();
	void AttachToIndex();
	void SetPrimitivePosition(LegoS32 p_index, const Vector3* p_position, LegoU32 p_unused);
	void SetTransform(const Vector3* p_position, const Matrix3* p_rotation, const LegoS32* p_filter);
	void SetChildTransform(
		const CollisionModelTransform* p_parent,
		const CollisionModelTransform* p_child,
		const LegoS32* p_filter
	);

	CollisionModelView* m_model; // 0x00
	Vector3 m_position;          // 0x04
	Matrix3 m_rotation;          // 0x10
	LegoU32 m_tag;               // 0x34
	LegoU32 m_unk0x38;
	SpatialIndexLink m_indexLink; // 0x3c
	LegoU32 m_unk0x70[5];
	CollisionProxy* m_parent;    // 0x84
	LegoU32 m_transformIndex;    // 0x88; high bit excluded when indexing
	CollisionProxy** m_children; // 0x8c
	LegoS32 m_childCount;        // 0x90
	LegoU32 m_complexity;        // 0x94
};

DECOMP_STATIC_ASSERT(offsetof(CollisionProxy, m_rotation) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(CollisionProxy, m_indexLink) == 0x3c);
DECOMP_STATIC_ASSERT(offsetof(CollisionProxy, m_children) == 0x8c);
DECOMP_SIZE_ASSERT(CollisionProxy, 0x98);

CollisionProxy* __cdecl CreateCollisionProxy(CollisionModelView* p_model, LegoU32 p_tag);
LegoU32 __cdecl DestroyCollisionProxy(CollisionProxy** p_proxy);

#endif // LR2_COLLISIONPROXY_H
