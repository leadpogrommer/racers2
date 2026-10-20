#ifndef LR2_COLLISIONGEOMETRY_H
#define LR2_COLLISIONGEOMETRY_H

#include "physics/collision/collisionqueries.h"

struct CollisionGeometryView {
	LegoU32 m_unk0x00;
	void* m_unk0x04;
	const LegoFloat* m_sphereRecords; // 0x08: 5 floats per record
	const LegoFloat* m_boxRecords;    // 0x0c: 15 floats per record
	LegoS32 m_sphereCount;            // 0x10
	LegoS32 m_boxCount;               // 0x14
	LegoU32 m_unk0x18;
};
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryView, m_sphereRecords) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryView, m_boxRecords) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryView, m_sphereCount) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryView, m_boxCount) == 0x14);

struct CollisionSphereRecord {
	Vector3 m_center;
	LegoFloat m_radius;
	LegoU32 m_unk0x10;
};
DECOMP_SIZE_ASSERT(CollisionSphereRecord, 0x14);

struct CollisionBoxRecord {
	LegoFloat m_values[15];
};
DECOMP_SIZE_ASSERT(CollisionBoxRecord, 0x3c);

struct CollisionTriangleRecord {
	const Vector3* m_vertices[3];
	Vector3 m_normal;
	LegoFloat m_planeDistance;
};
DECOMP_SIZE_ASSERT(CollisionTriangleRecord, 0x1c);

struct CollisionTriangleIndexNode {
	LegoU16 m_triangleIndex;
	LegoU16 m_nextIndex;
};
DECOMP_SIZE_ASSERT(CollisionTriangleIndexNode, 4);

struct CollisionTriangleIndexList {
	LegoU16 m_unk0x00;
	LegoU16 m_firstNodeIndex;
	const CollisionTriangleIndexNode* m_nodes;
	const CollisionTriangleIndexNode* m_end;
};
DECOMP_SIZE_ASSERT(CollisionTriangleIndexList, 0x0c);

struct CollisionTriangleRecordBase {
	const CollisionTriangleRecord* m_records;
};
DECOMP_SIZE_ASSERT(CollisionTriangleRecordBase, 4);

struct CollisionTriangleArrayView {
	const CollisionTriangleRecord* m_records;
	LegoU32 m_unk0x04;
	LegoU32 m_unk0x08;
	LegoU32 m_count;
};
DECOMP_SIZE_ASSERT(CollisionTriangleArrayView, 0x10);

LegoBool32 __fastcall TestIndexedCollisionTriangles(
	const CollisionTriangleRecordBase*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionContact*,
	const CollisionTriangleIndexList*
);

LegoBool32 __fastcall IntersectRayWithCollisionSphere(
	const CollisionSphereRecord*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionContact*
);

#endif
