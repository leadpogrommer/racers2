#include "collisionresource.h"

#include "collisionservice.h"
#include "memoryheap.h"
#include "modelresourcechunks.h"

void* __fastcall RelocateCollisionResourceData(CollisionServiceObject*, void*, void* p_data);
LegoU32 __fastcall FreeCollisionResourceData(CollisionServiceObject*, void*, void** p_data);

// STUB: LEGORACERS2 0x004b97f0
void* __cdecl LoadCollisionResource(void* p_data)
{
	InitializeCollisionServices();
	return RelocateCollisionResourceData(g_collisionProxyService, NULL, p_data);
}

// STUB: LEGORACERS2 0x004b9890
void __cdecl ReleaseCollisionResource(void** p_data)
{
	InitializeCollisionServices();
	FreeCollisionResourceData(g_collisionProxyService, NULL, p_data);
}

// FUNCTION: LEGORACERS2 0x004f4cc0
void* __fastcall RelocateCollisionResourceData(CollisionServiceObject*, void*, void* p_data)
{
	if (p_data == NULL) {
		return NULL;
	}
	CollisionResource* resource = (CollisionResource*) p_data;
	return resource->Relocate(resource) != NULL ? p_data : NULL;
}

// STUB: LEGORACERS2 0x004f4e10
LegoU32 __fastcall FreeCollisionResourceData(CollisionServiceObject*, void*, void** p_data)
{
	if (*p_data != NULL) {
		GetMemoryHeap()->Free(*p_data);
	}
	*p_data = NULL;
	return 0;
}

// STUB: LEGORACERS2 0x004fa790
LegoU8* CollisionResourceChildren::Relocate(LegoU8* p_cursor)
{
	if (m_childCount == 0) {
		return p_cursor;
	}
	m_transformRecords = p_cursor;
	p_cursor += m_transformCount * 0x30;
	m_indices = (LegoU32*) p_cursor;
	p_cursor += m_transformCount * 4;
	m_children = (CollisionResource**) p_cursor;
	p_cursor += m_childCount * 4;
	for (LegoS32 index = 0; index < m_childCount; ++index) {
		m_children[index] = (CollisionResource*) p_cursor;
		p_cursor = m_children[index]->Relocate(m_children[index]);
	}
	return p_cursor;
}

// FUNCTION: LEGORACERS2 0x004fa8f0
LegoU8* CollisionResource::Relocate(CollisionResource* p_target)
{
	p_target->m_runtimeState = 0;
	if (m_signature != 0x82) {
		return NULL;
	}
	LegoU8* cursor = m_geometry.Relocate((LegoU8*) p_target + 0x50);
	return m_children.Relocate(cursor);
}

// FUNCTION: LEGORACERS2 0x004fa930
LegoU8* CollisionGeometryStorage::Relocate(LegoU8* p_cursor)
{
	if (m_compound != NULL) {
		m_compound = (CollisionCompoundTriangles*) p_cursor;
		p_cursor += 0x100;
	}
	if (m_simple != NULL) {
		m_simple = (CollisionSimpleTriangles*) p_cursor;
		p_cursor += 0x14;
	}
	m_spheres = m_sphereCount != 0 ? (CollisionSphereRecord*) p_cursor : NULL;
	p_cursor += m_sphereCount * sizeof(CollisionSphereRecord);
	m_boxes = m_boxCount != 0 ? (CollisionBoxRecord*) p_cursor : NULL;
	p_cursor += m_boxCount * sizeof(CollisionBoxRecord);
	if (m_compound != NULL) {
		p_cursor = m_compound->Relocate(p_cursor);
	}
	if (m_simple != NULL) {
		p_cursor = m_simple->Relocate(p_cursor);
	}
	return p_cursor;
}

// FUNCTION: LEGORACERS2 0x004fa9a0
LegoU8* CollisionCompoundTriangles::Relocate(LegoU8* p_cursor)
{
	m_indices = m_indexCount != 0 ? (LegoU16*) p_cursor : NULL;
	p_cursor += m_indexCount * 2;
	p_cursor = m_mesh.Relocate(p_cursor);
	RebuildBounds();
	return p_cursor;
}

// STUB: LEGORACERS2 0x004fa9e0
void __fastcall RemapCollisionTriangleVertices(
	CollisionTriangleRecord* p_triangle,
	void*,
	const LegoU8* p_newVertices,
	const LegoU8* p_oldVertices
)
{
	for (LegoS32 index = 0; index < 3; ++index) {
		LegoS32 displacement = (LegoS32) p_triangle->m_vertices[index] - (LegoS32) p_oldVertices;
		LegoS32 vertex = displacement / 0x18;
		p_triangle->m_vertices[index] = (const Vector3*) (p_newVertices + vertex * 0x18);
	}
}

// STUB: LEGORACERS2 0x004faa50
void CollisionTriangleMesh::RemapVertices(const LegoU8* p_oldVertices)
{
	for (LegoS32 index = 0; index < m_triangleCount; ++index) {
		RemapCollisionTriangleVertices(&m_triangles[index], NULL, m_vertices, p_oldVertices);
	}
}

// STUB: LEGORACERS2 0x004faa90
LegoU8* CollisionTriangleMesh::Relocate(LegoU8* p_cursor)
{
	m_triangles = m_triangleCount != 0 ? (CollisionTriangleRecord*) p_cursor : NULL;
	p_cursor += m_triangleCount * sizeof(CollisionTriangleRecord);
	LegoU8* oldVertices = m_vertices;
	m_vertices = m_vertexCount != 0 ? p_cursor : NULL;
	LegoU8* end = p_cursor + m_vertexCount * 0x18;
	RemapVertices(oldVertices);
	return end;
}

// FUNCTION: LEGORACERS2 0x004fad30
LegoU8* CollisionSimpleTriangles::Relocate(LegoU8* p_cursor)
{
	return m_mesh.Relocate(p_cursor);
}

// STUB: LEGORACERS2 0x00508ee0
void CollisionCompoundTriangles::RebuildBounds()
{
	m_minimum.m_x = m_center.m_x - m_halfExtents.m_x;
	m_minimum.m_y = m_center.m_y - m_halfExtents.m_y;
	m_minimum.m_z = m_center.m_z - m_halfExtents.m_z;
	m_maximum.m_x = m_halfExtents.m_x + m_center.m_x;
	m_maximum.m_y = m_halfExtents.m_y + m_center.m_y;
	m_maximum.m_z = m_halfExtents.m_z + m_center.m_z;
	LegoFloat largest = m_halfExtents.m_x <= m_halfExtents.m_y ? m_halfExtents.m_y : m_halfExtents.m_x;
	if (largest < m_halfExtents.m_z) {
		largest = m_halfExtents.m_z;
	}
	LegoBool32 useX = largest / m_halfExtents.m_x < 6.0f;
	LegoBool32 useY = largest / m_halfExtents.m_y < 6.0f;
	LegoBool32 useZ = largest / m_halfExtents.m_z < 6.0f;
	m_axisMask = 0;
	m_axisCount = 0;
	if (useX) {
		m_axisMask = 4;
		m_axisCount = 1;
	}
	if (useY) {
		m_axisMask |= 2;
		++m_axisCount;
	}
	if (useZ) {
		m_axisMask |= 1;
		++m_axisCount;
	}
	LegoS32 index;
	switch (m_axisMask) {
	case 1:
	case 2:
	case 4:
	case 7:
		m_axisMask = 7;
		m_cornerCount = 8;
		for (index = 0; index < 8; ++index) {
			m_cornerOffsets[index].m_x = (index & 1) ? m_halfExtents.m_x : -m_halfExtents.m_x;
			m_cornerOffsets[index].m_y = (index & 2) ? m_halfExtents.m_y : -m_halfExtents.m_y;
			m_cornerOffsets[index].m_z = (index & 4) ? m_halfExtents.m_z : -m_halfExtents.m_z;
		}
		break;
	case 3:
	case 5:
	case 6:
		m_cornerCount = 4;
		LegoFloat first = m_axisMask == 3 ? m_halfExtents.m_y : m_halfExtents.m_x;
		LegoFloat second = m_axisMask == 6 ? m_halfExtents.m_y : m_halfExtents.m_z;
		for (index = 0; index < 4; ++index) {
			LegoFloat a = (index & 1) ? first : -first;
			LegoFloat b = (index & 2) ? second : -second;
			m_projectionCorners[index][0] = a;
			m_projectionCorners[index][1] = b;
			m_cornerOffsets[index].m_x = m_axisMask == 3 ? 0.0f : a;
			m_cornerOffsets[index].m_y = m_axisMask == 5 ? 0.0f : (m_axisMask == 3 ? a : b);
			m_cornerOffsets[index].m_z = m_axisMask == 6 ? 0.0f : b;
		}
		break;
	}
}
