#ifndef LR2_COLLISIONRESOURCE_H
#define LR2_COLLISIONRESOURCE_H

#include "collisiongeometry.h"

struct CollisionResource;

struct CollisionTriangleMesh {
	CollisionTriangleRecord* m_triangles;
	LegoU8* m_vertices; // interleaved 0x18-byte vertex records
	LegoS32 m_vertexCount;
	LegoS32 m_triangleCount;
	void RemapVertices(const LegoU8* p_oldVertices);
	LegoU8* Relocate(LegoU8* p_cursor);
};
DECOMP_SIZE_ASSERT(CollisionTriangleMesh, 0x10);

struct CollisionCompoundTriangles {
	LegoU16* m_indices;
	LegoS32 m_indexCount;
	CollisionTriangleMesh m_mesh;
	Vector3 m_center;
	Vector3 m_halfExtents;
	Vector3 m_cornerOffsets[8];
	LegoU32 m_cornerCount;
	LegoU32 m_axisMask;
	LegoU32 m_axisCount;
	Vector3 m_minimum;
	Vector3 m_maximum;
	LegoFloat m_projectionCorners[4][2];
	undefined m_unk0xd4[0x100 - 0xd4];
	LegoU8* Relocate(LegoU8* p_cursor);
	void RebuildBounds();
};
DECOMP_SIZE_ASSERT(CollisionCompoundTriangles, 0x100);
DECOMP_STATIC_ASSERT(offsetof(CollisionCompoundTriangles, m_center) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(CollisionCompoundTriangles, m_cornerCount) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(CollisionCompoundTriangles, m_projectionCorners) == 0xb4);

struct CollisionSimpleTriangles {
	LegoU32 m_unk0x00;
	CollisionTriangleMesh m_mesh;
	LegoU8* Relocate(LegoU8* p_cursor);
};
DECOMP_SIZE_ASSERT(CollisionSimpleTriangles, 0x14);

struct CollisionGeometryStorage {
	CollisionCompoundTriangles* m_compound;
	CollisionSimpleTriangles* m_simple;
	CollisionSphereRecord* m_spheres;
	CollisionBoxRecord* m_boxes;
	LegoS32 m_sphereCount;
	LegoS32 m_boxCount;
	undefined m_unk0x18[0x34 - 0x18];
	LegoU8* Relocate(LegoU8* p_cursor);
};
DECOMP_SIZE_ASSERT(CollisionGeometryStorage, 0x34);
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryStorage, m_spheres) == 8);
DECOMP_STATIC_ASSERT(offsetof(CollisionGeometryStorage, m_sphereCount) == 0x10);

struct CollisionResourceChildren {
	LegoU8* m_transformRecords; // 0x30-byte records
	LegoU32* m_indices;
	LegoS32 m_transformCount;
	CollisionResource** m_children;
	LegoS32 m_childCount;
	LegoU8* Relocate(LegoU8* p_cursor);
};
DECOMP_SIZE_ASSERT(CollisionResourceChildren, 0x14);

struct CollisionResource {
	LegoU32 m_signature;
	CollisionGeometryStorage m_geometry;
	CollisionResourceChildren m_children;
	LegoU32 m_runtimeState;
	LegoU8* Relocate(CollisionResource* p_target);
};
DECOMP_SIZE_ASSERT(CollisionResource, 0x50);
DECOMP_STATIC_ASSERT(offsetof(CollisionResource, m_geometry) == 4);
DECOMP_STATIC_ASSERT(offsetof(CollisionResource, m_children) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(CollisionResource, m_runtimeState) == 0x4c);

#endif
