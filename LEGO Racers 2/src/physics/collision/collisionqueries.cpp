#include "physics/collision/collisionqueries.h"

#include "math/matrix3.h"
#include "physics/collision/collisiongeometry.h"
#include "physics/collision/collisionservice.h"

#include <math.h>
#include <string.h>

LegoBool32 __fastcall TestBoxAgainstSphereRecord(
	const CollisionBoxRecord* p_box,
	void*,
	const CollisionSphereRecord* p_sphere,
	LegoU32 p_side
);
LegoBool32 __fastcall AppendSphereSphereContact(
	const CollisionSphereRecord* p_first,
	void*,
	const CollisionSphereRecord* p_second
);
LegoBool32 __fastcall TestCollisionGeometryAgainstSphere(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionSphereRecord* p_record
);
LegoBool32 __fastcall TestCollisionGeometryAgainstBox(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionBoxRecord* p_record
);
LegoBool32 __fastcall TestGeometryBoxContact(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionBoxRecord* p_record,
	CollisionContact* p_contact
);

LegoBool32 __fastcall TryAppendBoxContact(
	CollisionPairQueryState* p_state,
	void*,
	const CollisionGeometryView* p_geometry,
	const CollisionBoxRecord* p_record
);
void __fastcall TransformCollisionSphereRecord(
	CollisionSphereRecord* p_record,
	void*,
	const CollisionModelTransform* p_transform
);
LegoBool32 __fastcall TestOrientedBoxAxes(
	const CollisionBoxRecord* p_first,
	void*,
	const CollisionBoxRecord* p_second,
	Vector3* p_centerDelta,
	Vector3* p_axes,
	LegoFloat* p_overlaps
);
LegoBool32 __fastcall SelectMinimumBoxAxis(
	const CollisionBoxRecord* p_first,
	void*,
	const CollisionBoxRecord* p_second,
	Vector3* p_centerDelta,
	Vector3* p_axis,
	LegoFloat* p_penetration,
	LegoS32* p_axisIndex
);
LegoBool32 __fastcall TestBoxAgainstBoxRecord(
	const CollisionBoxRecord* p_box,
	void*,
	const CollisionBoxRecord* p_other
);
LegoBool32 __fastcall IntersectRayWithCollisionTriangle(
	const CollisionTriangleRecord* p_triangle,
	void*,
	const Vector3* p_origin,
	const Vector3* p_direction,
	CollisionContact* p_contact
);
void __fastcall BuildBoxBoundingSphere(const CollisionBoxRecord* p_box, void*, CollisionSphereRecord* p_sphere);

// GLOBAL: LEGORACERS2 0x006ff034
static Vector3* g_collisionQueryFirstPosition = NULL;
// GLOBAL: LEGORACERS2 0x006ff038
static Vector3* g_collisionQuerySecondPosition = NULL;
// GLOBAL: LEGORACERS2 0x006ff03c
static CollisionProxy* g_collisionQueryFirstProxy = NULL;
// GLOBAL: LEGORACERS2 0x006ff040
static CollisionProxy* g_collisionQuerySecondProxy = NULL;

// GLOBAL: LEGORACERS2 0x006fd7c4
static LegoS32 g_collisionQueryVisitedLinks[0x200];
// GLOBAL: LEGORACERS2 0x006ff018
static const CollisionTriangleRecord* g_collisionTriangleArrayBase = NULL;

// STUB: LEGORACERS2 0x004f96a0
LegoBool32 __fastcall TestCollisionGeometryPair(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionGeometryView* p_other
)
{
	LegoBool32 intersects = FALSE;
	const CollisionModelTransform* transform = &g_collisionServiceState->m_firstTransform;

	if (p_other->m_sphereCount != 0) {
		const CollisionSphereRecord* record = (const CollisionSphereRecord*) p_other->m_sphereRecords;
		const CollisionSphereRecord* end = record + p_other->m_sphereCount;
		for (; record != end; ++record) {
			CollisionSphereRecord transformed;
			memcpy(&transformed, record, sizeof(transformed));
			TransformCollisionSphereRecord(&transformed, NULL, transform);
			if (TestCollisionGeometryAgainstSphere(p_geometry, NULL, &transformed) != FALSE) {
				intersects = TRUE;
			}
		}
	}

	if (p_other->m_boxCount != 0) {
		const CollisionBoxRecord* record = (const CollisionBoxRecord*) p_other->m_boxRecords;
		const CollisionBoxRecord* end = record + p_other->m_boxCount;
		for (; record != end; ++record) {
			CollisionBoxRecord transformed;
			memcpy(&transformed, record, sizeof(transformed));
			Vector3* center = (Vector3*) &transformed.m_values[0];
			center->FUN_004a4e00(&transform->m_rotation);
			center->m_x += transform->m_position.m_x;
			center->m_y += transform->m_position.m_y;
			center->m_z += transform->m_position.m_z;
			((Vector3*) &transformed.m_values[3])->FUN_004a4e00(&transform->m_rotation);
			((Vector3*) &transformed.m_values[6])->FUN_004a4e00(&transform->m_rotation);
			((Vector3*) &transformed.m_values[9])->FUN_004a4e00(&transform->m_rotation);
			if (TestCollisionGeometryAgainstBox(p_geometry, NULL, &transformed) != FALSE) {
				intersects = TRUE;
			}
		}
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004f97f0
LegoBool32 __fastcall TestCollisionGeometryAgainstSphere(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionSphereRecord* p_sphere
)
{
	LegoBool32 intersects = FALSE;
	if (p_geometry->m_sphereCount != 0) {
		const CollisionSphereRecord* record = (const CollisionSphereRecord*) p_geometry->m_sphereRecords;
		const CollisionSphereRecord* end = record + p_geometry->m_sphereCount;
		for (; record != end; ++record) {
			if (AppendSphereSphereContact(record, NULL, p_sphere) != FALSE) {
				intersects = TRUE;
			}
		}
	}
	if (p_geometry->m_boxCount != 0) {
		const CollisionBoxRecord* record = (const CollisionBoxRecord*) p_geometry->m_boxRecords;
		const CollisionBoxRecord* end = record + p_geometry->m_boxCount;
		for (; record != end; ++record) {
			if (TestBoxAgainstSphereRecord(record, NULL, p_sphere, 0) != FALSE) {
				intersects = TRUE;
			}
		}
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004f9d50
LegoBool32 __fastcall TestCollisionGeometryAgainstBox(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionBoxRecord* p_box
)
{
	LegoBool32 intersects = FALSE;
	if (p_geometry->m_sphereCount != 0) {
		const CollisionSphereRecord* record = (const CollisionSphereRecord*) p_geometry->m_sphereRecords;
		const CollisionSphereRecord* end = record + p_geometry->m_sphereCount;
		for (; record != end; ++record) {
			if (TestBoxAgainstSphereRecord(p_box, NULL, record, 1) != FALSE) {
				intersects = TRUE;
			}
		}
	}
	if (p_geometry->m_boxCount != 0) {
		const CollisionBoxRecord* record = (const CollisionBoxRecord*) p_geometry->m_boxRecords;
		const CollisionBoxRecord* end = record + p_geometry->m_boxCount;
		for (; record != end; ++record) {
			if (TestBoxAgainstBoxRecord(record, NULL, p_box) != FALSE) {
				intersects = TRUE;
			}
		}
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004fa2b0
LegoBool32 __fastcall TestGeometryBoxContact(
	CollisionGeometryView* p_geometry,
	void*,
	const CollisionBoxRecord* p_box,
	CollisionContact* p_contact
)
{
	LegoBool32 intersects = FALSE;
	Vector3 sweepDirection;
	const LegoFloat* values = p_box->m_values;
	LegoS32 i;
	p_contact->m_penetration = 1.0f;
	sweepDirection.m_x = values[3] * values[12];
	sweepDirection.m_y = values[4] * values[12];
	sweepDirection.m_z = values[5] * values[12];
	for (i = 0; i < p_geometry->m_sphereCount; ++i) {
		const CollisionSphereRecord* sphere = (const CollisionSphereRecord*) p_geometry->m_sphereRecords + i;
		if (IntersectRayWithCollisionSphere(
				sphere,
				NULL,
				(const Vector3*) p_box->m_values,
				&sweepDirection,
				p_contact
			) != FALSE) {
			intersects = TRUE;
		}
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004fb3e0
LegoBool32 __fastcall TestIndexedCollisionTriangles(
	const CollisionTriangleRecordBase* p_geometry,
	void*,
	const Vector3* p_origin,
	const Vector3* p_direction,
	CollisionContact* p_contact,
	const CollisionTriangleIndexList* p_indices
)
{
	LegoBool32 intersects = FALSE;
	const CollisionTriangleIndexNode* node = p_indices->m_nodes + p_indices->m_firstNodeIndex;
	g_collisionTriangleArrayBase = p_geometry->m_records;
	while (node != p_indices->m_end) {
		const CollisionTriangleRecord* triangle = p_geometry->m_records + node->m_triangleIndex;
		if (IntersectRayWithCollisionTriangle(triangle, NULL, p_origin, p_direction, p_contact) != FALSE) {
			intersects = TRUE;
		}
		node = p_indices->m_nodes + node->m_nextIndex;
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004fb460
LegoBool32 __fastcall TestCollisionTriangleArray(
	const CollisionTriangleArrayView* p_geometry,
	void*,
	const Vector3* p_origin,
	const Vector3* p_direction,
	CollisionContact* p_contact
)
{
	LegoBool32 intersects = FALSE;
	if (p_geometry->m_count == 0) {
		return FALSE;
	}
	const CollisionTriangleRecord* triangle = p_geometry->m_records;
	const CollisionTriangleRecord* end = triangle + p_geometry->m_count;
	g_collisionTriangleArrayBase = p_geometry->m_records;
	for (; triangle != end; ++triangle) {
		if (IntersectRayWithCollisionTriangle(triangle, NULL, p_origin, p_direction, p_contact) != FALSE) {
			intersects = TRUE;
		}
	}
	return intersects;
}

// STUB: LEGORACERS2 0x004fb550
CollisionQueryBounds* __fastcall InitializeCollisionQueryBoundsFromEndpoints(
	CollisionQueryBounds* p_bounds,
	void*,
	const Vector3* p_first,
	const Vector3* p_second
)
{
	memset(p_bounds, 0, sizeof(*p_bounds));
	p_bounds->m_minX = p_first->m_x < p_second->m_x ? p_first->m_x : p_second->m_x;
	p_bounds->m_minZ = p_first->m_z < p_second->m_z ? p_first->m_z : p_second->m_z;
	p_bounds->m_maxX = p_first->m_x <= p_second->m_x ? p_second->m_x : p_first->m_x;
	p_bounds->m_maxZ = p_first->m_z <= p_second->m_z ? p_second->m_z : p_first->m_z;
	return p_bounds;
}

// STUB: LEGORACERS2 0x004fb610
CollisionQueryBounds* __fastcall InitializeCollisionQueryBoundsFromRadius(
	CollisionQueryBounds* p_bounds,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius
)
{
	memset(p_bounds, 0, sizeof(*p_bounds));
	p_bounds->m_minX = p_center->m_x - p_radius;
	// Retail writes the Z minimum to +0x1c again, leaving +0x20 zeroed.
	p_bounds->m_minX = p_center->m_z - p_radius;
	p_bounds->m_maxX = p_center->m_x + p_radius;
	p_bounds->m_maxZ = p_center->m_z + p_radius;
	return p_bounds;
}

// STUB: LEGORACERS2 0x00505370
static void __fastcall GetBoxSupportPoint(
	const CollisionBoxRecord* p_box,
	void*,
	const Vector3* p_direction,
	Vector3* p_point
)
{
	const LegoFloat* values = p_box->m_values;
	const LegoFloat projection[3] = {
		p_direction->m_x * values[3] + p_direction->m_y * values[4] + p_direction->m_z * values[5],
		p_direction->m_x * values[6] + p_direction->m_y * values[7] + p_direction->m_z * values[8],
		p_direction->m_x * values[9] + p_direction->m_y * values[10] + p_direction->m_z * values[11]
	};
	LegoFloat coordinate[3];
	LegoS32 axis;
	for (axis = 0; axis < 3; ++axis) {
		if (projection[axis] < -0.05f) {
			coordinate[axis] = values[12 + axis];
		}
		else if (projection[axis] < 0.05f) {
			coordinate[axis] = 0.0f;
		}
		else {
			coordinate[axis] = -values[12 + axis];
		}
	}
	p_point->m_x = values[0] + coordinate[0] * values[3] + coordinate[1] * values[6] + coordinate[2] * values[9];
	p_point->m_y = values[1] + coordinate[0] * values[4] + coordinate[1] * values[7] + coordinate[2] * values[10];
	p_point->m_z = values[2] + coordinate[0] * values[5] + coordinate[1] * values[8] + coordinate[2] * values[11];
}

// STUB: LEGORACERS2 0x005054d0
static void __fastcall GetOpposingBoxSupportPoint(
	const CollisionBoxRecord* p_box,
	void*,
	const Vector3* p_direction,
	Vector3* p_point
)
{
	const LegoFloat* values = p_box->m_values;
	const LegoFloat projection[3] = {
		p_direction->m_x * values[3] + p_direction->m_y * values[4] + p_direction->m_z * values[5],
		p_direction->m_x * values[6] + p_direction->m_y * values[7] + p_direction->m_z * values[8],
		p_direction->m_x * values[9] + p_direction->m_y * values[10] + p_direction->m_z * values[11]
	};
	LegoFloat coordinate[3];
	LegoS32 axis;
	for (axis = 0; axis < 3; ++axis) {
		if (projection[axis] < -0.05f) {
			coordinate[axis] = -values[12 + axis];
		}
		else if (projection[axis] < 0.05f) {
			coordinate[axis] = 0.0f;
		}
		else {
			coordinate[axis] = values[12 + axis];
		}
	}
	p_point->m_x = values[0] + coordinate[0] * values[3] + coordinate[1] * values[6] + coordinate[2] * values[9];
	p_point->m_y = values[1] + coordinate[0] * values[4] + coordinate[1] * values[7] + coordinate[2] * values[10];
	p_point->m_z = values[2] + coordinate[0] * values[5] + coordinate[1] * values[8] + coordinate[2] * values[11];
}

// STUB: LEGORACERS2 0x00505630
LegoBool32 __fastcall TestBoxAgainstSphereRecord(
	const CollisionBoxRecord* p_box,
	void*,
	const CollisionSphereRecord* p_sphere,
	LegoU32 p_side
)
{
	const LegoFloat* values = p_box->m_values;
	LegoFloat projections[3];
	LegoFloat separations[3];
	LegoU32 outsideMask = 0;
	LegoS32 axis;
	const LegoFloat centerDelta[3] =
		{p_sphere->m_center.m_x - values[0], p_sphere->m_center.m_y - values[1], p_sphere->m_center.m_z - values[2]};

	for (axis = 0; axis < 3; ++axis) {
		const LegoFloat* direction = values + 3 + axis * 3;
		const LegoFloat projection =
			centerDelta[1] * direction[1] + centerDelta[0] * direction[0] + centerDelta[2] * direction[2];
		const LegoFloat magnitude = (LegoFloat) fabs(projection);
		const LegoFloat separation = (p_sphere->m_radius + values[12 + axis]) - magnitude;
		if (separation < 0.0f) {
			return FALSE;
		}
		projections[axis] = projection;
		separations[axis] = separation;
		if (values[12 + axis] <= magnitude) {
			outsideMask |= 1 << (axis & 0x1f);
		}
	}

	CollisionContact* contact = &g_collisionServiceState->m_records[g_collisionServiceState->m_currentLinkCount];
	LegoFloat normalX;
	LegoFloat normalY;
	LegoFloat normalZ;
	LegoFloat penetration;
	if (outsideMask == 0 || outsideMask == 1 || outsideMask == 2 || outsideMask == 4) {
		axis = 0;
		if (outsideMask == 0) {
			if (separations[1] < separations[0]) {
				axis = 1;
				separations[0] = separations[1];
			}
			if (separations[0] > separations[2]) {
				axis = 2;
			}
		}
		else if (outsideMask == 2) {
			axis = 1;
		}
		else if (outsideMask == 4) {
			axis = 2;
		}

		const LegoFloat* direction = values + 3 + axis * 3;
		const LegoFloat orientation = projections[axis] > 0.0f ? -1.0f : 1.0f;
		normalX = orientation * direction[0];
		normalY = orientation * direction[1];
		normalZ = orientation * direction[2];
		penetration = separations[axis];
		contact->m_point.m_x = p_sphere->m_center.m_x + (p_sphere->m_radius - penetration) * normalX;
		contact->m_point.m_y = p_sphere->m_center.m_y + (p_sphere->m_radius - penetration) * normalY;
		contact->m_point.m_z = p_sphere->m_center.m_z + (p_sphere->m_radius - penetration) * normalZ;
	}
	else {
		LegoFloat closestX = 0.0f;
		LegoFloat closestY = 0.0f;
		LegoFloat closestZ = 0.0f;
		for (axis = 0; axis < 3; ++axis) {
			if ((outsideMask & (1 << (axis & 0x1f))) != 0) {
				const LegoFloat* direction = values + 3 + axis * 3;
				LegoFloat distance = p_sphere->m_radius - separations[axis];
				if (projections[axis] >= 0.0f) {
					distance = -distance;
				}
				closestX += distance * direction[0];
				closestY += distance * direction[1];
				closestZ += distance * direction[2];
			}
		}
		const LegoFloat distanceSquared = closestX * closestX + closestY * closestY + closestZ * closestZ;
		if (p_sphere->m_radius * p_sphere->m_radius < distanceSquared) {
			return FALSE;
		}
		const LegoFloat distance = (LegoFloat) sqrt(distanceSquared);
		const LegoFloat scale = 1.0f / distance;
		normalX = scale * closestX;
		normalY = scale * closestY;
		normalZ = scale * closestZ;
		penetration = p_sphere->m_radius - distance;
		contact->m_point.m_x = p_sphere->m_center.m_x + distance * normalX;
		contact->m_point.m_y = p_sphere->m_center.m_y + distance * normalY;
		contact->m_point.m_z = p_sphere->m_center.m_z + distance * normalZ;
	}

	contact->m_flags &= (LegoU16) ~2;
	contact->m_normal.m_x = normalX;
	contact->m_normal.m_y = normalY;
	contact->m_normal.m_z = normalZ;
	contact->m_separationAxis = contact->m_normal;
	contact->m_penetration = penetration;
	if (p_side != 0) {
		contact->m_point.m_x += penetration * contact->m_normal.m_x;
		contact->m_point.m_y += penetration * contact->m_normal.m_y;
		contact->m_point.m_z += penetration * contact->m_normal.m_z;
		contact->m_normal.m_x = -contact->m_normal.m_x;
		contact->m_normal.m_y = -contact->m_normal.m_y;
		contact->m_normal.m_z = -contact->m_normal.m_z;
		contact->m_separationAxis = contact->m_normal;
	}
	if (g_collisionServiceState->m_currentLinkCount < 0xff) {
		++g_collisionServiceState->m_currentLinkCount;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x00505ae0
LegoBool32 __fastcall TestOrientedBoxAxes(
	const CollisionBoxRecord* p_first,
	void*,
	const CollisionBoxRecord* p_second,
	Vector3* p_centerDelta,
	Vector3* p_axes,
	LegoFloat* p_overlaps
)
{
	const LegoFloat* a = p_first->m_values;
	const LegoFloat* b = p_second->m_values;
	Vector3 firstAxes[3];
	Vector3 secondAxes[3];
	Vector3 axis;
	LegoS32 i;
	LegoS32 j;
	LegoS32 outIndex = 0;

	p_centerDelta->m_x = b[0] - a[0];
	p_centerDelta->m_y = b[1] - a[1];
	p_centerDelta->m_z = b[2] - a[2];
	for (i = 0; i < 3; ++i) {
		firstAxes[i].m_x = a[3 + i * 3];
		firstAxes[i].m_y = a[4 + i * 3];
		firstAxes[i].m_z = a[5 + i * 3];
		secondAxes[i].m_x = b[3 + i * 3];
		secondAxes[i].m_y = b[4 + i * 3];
		secondAxes[i].m_z = b[5 + i * 3];
	}

	for (i = 0; i < 3; ++i) {
		axis = firstAxes[i];
		p_axes[outIndex] = axis;
		++outIndex;
	}
	for (i = 0; i < 3; ++i) {
		axis = secondAxes[i];
		p_axes[outIndex] = axis;
		++outIndex;
	}
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			axis.m_x = firstAxes[i].m_y * secondAxes[j].m_z - firstAxes[i].m_z * secondAxes[j].m_y;
			axis.m_y = firstAxes[i].m_z * secondAxes[j].m_x - firstAxes[i].m_x * secondAxes[j].m_z;
			axis.m_z = firstAxes[i].m_x * secondAxes[j].m_y - firstAxes[i].m_y * secondAxes[j].m_x;
			p_axes[outIndex] = axis;
			++outIndex;
		}
	}

	for (i = 0; i < 15; ++i) {
		const Vector3* testAxis = &p_axes[i];
		const LegoFloat distance = (LegoFloat) fabs(
			p_centerDelta->m_x * testAxis->m_x + p_centerDelta->m_y * testAxis->m_y + p_centerDelta->m_z * testAxis->m_z
		);
		LegoFloat firstRadius = 0.0f;
		LegoFloat secondRadius = 0.0f;
		for (j = 0; j < 3; ++j) {
			const LegoFloat firstProjection = (LegoFloat) fabs(
				firstAxes[j].m_x * testAxis->m_x + firstAxes[j].m_y * testAxis->m_y + firstAxes[j].m_z * testAxis->m_z
			);
			const LegoFloat secondProjection = (LegoFloat) fabs(
				secondAxes[j].m_x * testAxis->m_x + secondAxes[j].m_y * testAxis->m_y +
				secondAxes[j].m_z * testAxis->m_z
			);
			firstRadius += firstProjection * a[12 + j];
			secondRadius += secondProjection * b[12 + j];
		}
		p_overlaps[i] = firstRadius + secondRadius - distance;
		if (p_overlaps[i] < 0.0f) {
			return FALSE;
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x00506580
LegoBool32 __fastcall SelectMinimumBoxAxis(
	const CollisionBoxRecord* p_first,
	void*,
	const CollisionBoxRecord* p_second,
	Vector3* p_centerDelta,
	Vector3* p_axis,
	LegoFloat* p_penetration,
	LegoS32* p_axisIndex
)
{
	Vector3 axes[15];
	LegoFloat overlaps[15];
	LegoFloat minimum = 1000000.0f;
	LegoS32 index = 0;
	LegoS32 i;
	if (TestOrientedBoxAxes(p_first, NULL, p_second, p_centerDelta, axes, overlaps) == FALSE) {
		return FALSE;
	}
	for (i = 0; i < 15; ++i) {
		const LegoFloat lengthSquared =
			axes[i].m_x * axes[i].m_x + axes[i].m_y * axes[i].m_y + axes[i].m_z * axes[i].m_z;
		if (lengthSquared >= 0.0000001f) {
			const LegoFloat length = (LegoFloat) sqrt(lengthSquared);
			const LegoFloat penetration = overlaps[i] / length;
			if (penetration < minimum) {
				minimum = penetration;
				index = i;
				*p_axis = axes[i];
				*p_axisIndex = i;
			}
		}
	}
	*p_penetration = minimum;
	{
		const LegoFloat lengthSquared =
			p_axis->m_x * p_axis->m_x + p_axis->m_y * p_axis->m_y + p_axis->m_z * p_axis->m_z;
		const LegoFloat inverseLength = 1.0f / (LegoFloat) sqrt(lengthSquared);
		p_axis->m_x *= inverseLength;
		p_axis->m_y *= inverseLength;
		p_axis->m_z *= inverseLength;
	}
	if (p_axis->m_x * p_centerDelta->m_x + p_axis->m_y * p_centerDelta->m_y + p_axis->m_z * p_centerDelta->m_z > 0.0f) {
		p_axis->m_x = -p_axis->m_x;
		p_axis->m_y = -p_axis->m_y;
		p_axis->m_z = -p_axis->m_z;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x005066f0
LegoBool32 __fastcall TestBoxAgainstBoxRecord(const CollisionBoxRecord* p_box, void*, const CollisionBoxRecord* p_other)
{
	CollisionPairQueryState* state = g_collisionServiceState;
	CollisionContact* contact;
	Vector3 centerDelta;
	Vector3 normal;
	Vector3 firstPoint;
	Vector3 secondPoint;
	LegoFloat penetration;
	LegoS32 axisIndex;
	if (SelectMinimumBoxAxis(p_box, NULL, p_other, &centerDelta, &normal, &penetration, &axisIndex) == FALSE) {
		return FALSE;
	}
	contact = &state->m_records[state->m_currentLinkCount];
	if (axisIndex < 3) {
		GetOpposingBoxSupportPoint(p_other, NULL, &normal, &contact->m_point);
		contact->m_point.m_x -= penetration * normal.m_x;
		contact->m_point.m_y -= penetration * normal.m_y;
		contact->m_point.m_z -= penetration * normal.m_z;
	}
	else if (axisIndex < 6) {
		GetBoxSupportPoint(p_box, NULL, &normal, &contact->m_point);
	}
	else {
		const LegoS32 firstAxisIndex = (axisIndex - 6) / 3;
		const LegoS32 secondAxisIndex = (axisIndex - 6) % 3;
		const LegoFloat* a = p_box->m_values;
		const LegoFloat* b = p_other->m_values;
		Vector3 axisA = {a[3 + firstAxisIndex * 3], a[4 + firstAxisIndex * 3], a[5 + firstAxisIndex * 3]};
		Vector3 axisB = {b[3 + secondAxisIndex * 3], b[4 + secondAxisIndex * 3], b[5 + secondAxisIndex * 3]};
		LegoFloat aa;
		LegoFloat ab;
		LegoFloat bb;
		LegoFloat aw;
		LegoFloat bw;
		LegoFloat denominator;
		LegoFloat firstDistance;
		LegoFloat secondDistance;
		GetBoxSupportPoint(p_box, NULL, &normal, &firstPoint);
		GetOpposingBoxSupportPoint(p_other, NULL, &normal, &secondPoint);
		{
			const LegoFloat offsetA = (firstPoint.m_x - a[0]) * axisA.m_x + (firstPoint.m_y - a[1]) * axisA.m_y +
									  (firstPoint.m_z - a[2]) * axisA.m_z;
			const LegoFloat offsetB = (secondPoint.m_x - b[0]) * axisB.m_x + (secondPoint.m_y - b[1]) * axisB.m_y +
									  (secondPoint.m_z - b[2]) * axisB.m_z;
			firstPoint.m_x -= offsetA * axisA.m_x;
			firstPoint.m_y -= offsetA * axisA.m_y;
			firstPoint.m_z -= offsetA * axisA.m_z;
			secondPoint.m_x -= offsetB * axisB.m_x;
			secondPoint.m_y -= offsetB * axisB.m_y;
			secondPoint.m_z -= offsetB * axisB.m_z;
		}
		aa = axisA.m_x * axisA.m_x + axisA.m_y * axisA.m_y + axisA.m_z * axisA.m_z;
		ab = axisA.m_x * axisB.m_x + axisA.m_y * axisB.m_y + axisA.m_z * axisB.m_z;
		bb = axisB.m_x * axisB.m_x + axisB.m_y * axisB.m_y + axisB.m_z * axisB.m_z;
		aw = axisA.m_x * (firstPoint.m_x - secondPoint.m_x) + axisA.m_y * (firstPoint.m_y - secondPoint.m_y) +
			 axisA.m_z * (firstPoint.m_z - secondPoint.m_z);
		bw = axisB.m_x * (firstPoint.m_x - secondPoint.m_x) + axisB.m_y * (firstPoint.m_y - secondPoint.m_y) +
			 axisB.m_z * (firstPoint.m_z - secondPoint.m_z);
		denominator = aa * bb - ab * ab;
		if ((LegoFloat) fabs(denominator) < 0.0000001f) {
			contact->m_point.m_x = (firstPoint.m_x + secondPoint.m_x) * 0.5f;
			contact->m_point.m_y = (firstPoint.m_y + secondPoint.m_y) * 0.5f;
			contact->m_point.m_z = (firstPoint.m_z + secondPoint.m_z) * 0.5f;
		}
		else {
			firstDistance = (ab * bw - bb * aw) / denominator;
			secondDistance = (aa * bw - ab * aw) / denominator;
			firstPoint.m_x += firstDistance * axisA.m_x;
			firstPoint.m_y += firstDistance * axisA.m_y;
			firstPoint.m_z += firstDistance * axisA.m_z;
			secondPoint.m_x += secondDistance * axisB.m_x;
			secondPoint.m_y += secondDistance * axisB.m_y;
			secondPoint.m_z += secondDistance * axisB.m_z;
			contact->m_point.m_x = (firstPoint.m_x + secondPoint.m_x) * 0.5f;
			contact->m_point.m_y = (firstPoint.m_y + secondPoint.m_y) * 0.5f;
			contact->m_point.m_z = (firstPoint.m_z + secondPoint.m_z) * 0.5f;
		}
	}
	contact->m_flags &= (LegoU16) ~2;
	contact->m_normal = normal;
	contact->m_separationAxis = normal;
	contact->m_penetration = penetration;
	if (state->m_currentLinkCount < 0xff) {
		++state->m_currentLinkCount;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x005069e0
void __fastcall BuildBoxBoundingSphere(const CollisionBoxRecord* p_box, void*, CollisionSphereRecord* p_sphere)
{
	const LegoFloat* values = p_box->m_values;
	p_sphere->m_center.m_x = values[0];
	p_sphere->m_center.m_y = values[1];
	p_sphere->m_center.m_z = values[2];
	p_sphere->m_radius = (LegoFloat) sqrt(values[12] * values[12] + values[13] * values[13] + values[14] * values[14]);
}

// STUB: LEGORACERS2 0x00508520
void __fastcall CollectGridCollisionCandidates(StaticSpatialGrid* p_grid, void*, SpatialIndexLink* p_link)
{
	const LegoFloat inverseCellSize = p_grid->m_inverseCellSize;
	LegoS32* range = (LegoS32*) ((LegoU8*) p_link + 0x0c);
	const LegoFloat halfCell = 0.5f;
	LegoS32 x = (LegoS32) ((p_link->m_minX - p_grid->m_origin.m_x) * inverseCellSize - halfCell);
	LegoS32 z = (LegoS32) ((p_link->m_minZ - p_grid->m_origin.m_z) * inverseCellSize - halfCell);
	LegoS32 maxX = (LegoS32) ((p_link->m_maxX - p_grid->m_origin.m_x) * inverseCellSize + halfCell);
	LegoS32 maxZ = (LegoS32) ((p_link->m_maxZ - p_grid->m_origin.m_z) * inverseCellSize + halfCell);
	range[0] = x;
	range[1] = z;
	range[2] = maxX;
	range[3] = maxZ;
	if (range[0] < 0) {
		range[0] = 0;
	}
	if (range[1] < 0) {
		range[1] = 0;
	}
	if (range[2] >= p_grid->m_size) {
		range[2] = p_grid->m_size - 1;
	}
	if (range[3] >= p_grid->m_size) {
		range[3] = p_grid->m_size - 1;
	}

	LegoS32 visitedCount = 0;
	for (x = range[0]; x < range[2]; ++x) {
		for (z = range[1]; z < range[3]; ++z) {
			SpatialGridCell* cell = p_grid->GetCell(x, z);
			for (SpatialSlotNode* node = cell->m_head; node != NULL; node = node->m_next) {
				SpatialIndexLink* candidate = (SpatialIndexLink*) node->m_value;
				LegoS32 i = 0;
				while (i < visitedCount && g_collisionQueryVisitedLinks[i] != candidate->m_index) {
					++i;
				}
				if (i == visitedCount) {
					++visitedCount;
					g_collisionQueryVisitedLinks[i] = candidate->m_index;
					if (visitedCount >= 0x200) {
						return;
					}
					TryRecordCollisionPair(
						(CollisionPairQueryState*) g_collisionServiceState,
						NULL,
						p_link->m_proxy,
						candidate->m_proxy
					);
				}
			}
		}
	}
}

// STUB: LEGORACERS2 0x005088b0
void StaticSpatialGrid::PrepareQuery(DynamicSpatialIndex* p_dynamicIndex)
{
	for (SpatialSlotNode* node = p_dynamicIndex->m_axes[0].m_head; node != NULL; node = node->m_next) {
		SpatialIndexLink* link = (SpatialIndexLink*) node->m_value;
		if (link->m_filter != -1 && link->m_filter != -2 && link->m_filter != -3) {
			CollectGridCollisionCandidates(this, NULL, link);
		}
	}
}

// STUB: LEGORACERS2 0x00508a10
LegoBool32 __fastcall TryRecordCollisionPair(
	CollisionPairQueryState* p_state,
	void*,
	CollisionProxy* p_first,
	CollisionProxy* p_second
)
{
	p_state->m_firstProxy = p_first;
	p_state->m_secondProxy = p_second;
	p_state->m_unk0x4c6c = 0;

	CollisionContact* contact = &p_state->m_records[p_state->m_currentLinkCount];
	if (p_first->m_indexLink.m_filter == -3 || p_second->m_indexLink.m_filter == -3) {
		contact->m_point.m_x = 0.0f;
		contact->m_point.m_y = 0.0f;
		contact->m_point.m_z = 0.0f;
		contact->m_normal.m_x = 0.0f;
		contact->m_normal.m_y = 1.0f;
		contact->m_normal.m_z = 0.0f;
		contact->m_separationAxis.m_x = 0.0f;
		contact->m_separationAxis.m_y = 1.0f;
		contact->m_separationAxis.m_z = 0.0f;
		contact->m_penetration = 0.0f;
		contact->m_first = (GameLifecycleComponent*) p_first->m_tag;
		contact->m_second = (GameLifecycleComponent*) p_second->m_tag;
		if (p_state->m_currentLinkCount < 0xff) {
			++p_state->m_currentLinkCount;
		}
		return TRUE;
	}

	if ((LegoS32) p_first->m_complexity < (LegoS32) p_second->m_complexity) {
		p_state->m_firstProxy = p_second;
		p_state->m_secondProxy = p_first;
	}
	p_first = p_state->m_firstProxy;
	p_second = p_state->m_secondProxy;

	p_state->m_firstTransform.m_position.m_x = p_second->m_position.m_x - p_first->m_position.m_x;
	p_state->m_firstTransform.m_position.m_y = p_second->m_position.m_y - p_first->m_position.m_y;
	p_state->m_firstTransform.m_position.m_z = p_second->m_position.m_z - p_first->m_position.m_z;
	p_state->m_firstTransform.m_position.FUN_004a4e50(&p_first->m_rotation);
	p_state->m_firstTransform.m_rotation.FUN_004a4f00(&p_first->m_rotation, &p_second->m_rotation);

	const LegoS32 oldCount = p_state->m_currentLinkCount;
	CollisionModelView* firstModel = p_first->m_model;
	CollisionModelView* secondModel = p_second->m_model;
	LegoBool32 intersects;
	if ((p_second->m_transformIndex & 0x80000000U) == 0) {
		g_collisionQueryFirstPosition = &p_first->m_position;
		g_collisionQuerySecondPosition = &p_second->m_position;
		g_collisionQueryFirstProxy = p_first;
		g_collisionQuerySecondProxy = p_second;
		intersects = TestCollisionGeometryPair(
			(CollisionGeometryView*) ((LegoU8*) firstModel + sizeof(LegoU32)),
			NULL,
			(const CollisionGeometryView*) ((const LegoU8*) secondModel + sizeof(LegoU32))
		);
		g_collisionQueryFirstProxy = NULL;
		g_collisionQuerySecondProxy = NULL;
	}
	else {
		intersects = TryAppendBoxContact(
			p_state,
			NULL,
			(const CollisionGeometryView*) ((const LegoU8*) firstModel + sizeof(LegoU32)),
			(const CollisionBoxRecord*) ((const LegoU8*) secondModel + 0x10)
		);
	}
	if (intersects == FALSE) {
		return FALSE;
	}

	for (LegoS32 i = oldCount; i < p_state->m_currentLinkCount; ++i) {
		contact = &p_state->m_records[i];
		contact->m_first = (GameLifecycleComponent*) p_first->m_tag;
		contact->m_second = (GameLifecycleComponent*) p_second->m_tag;
		contact->m_point.FUN_004a4e00(&p_first->m_rotation);
		contact->m_normal.FUN_004a4e00(&p_first->m_rotation);
		contact->m_separationAxis.FUN_004a4e00(&p_first->m_rotation);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x00508ca0
LegoBool32 __fastcall TryAppendBoxContact(
	CollisionPairQueryState* p_state,
	void*,
	const CollisionGeometryView* p_geometry,
	const CollisionBoxRecord* p_record
)
{
	if (p_state->m_currentLinkCount >= 0xff) {
		return FALSE;
	}

	CollisionContact* contact = &p_state->m_records[p_state->m_currentLinkCount];
	CollisionBoxRecord transformed = *p_record;
	CollisionModelTransform* transform = &p_state->m_firstTransform;
	Vector3* center = (Vector3*) &transformed.m_values[0];
	center->FUN_004a4e00(&transform->m_rotation);
	center->m_x += transform->m_position.m_x;
	center->m_y += transform->m_position.m_y;
	center->m_z += transform->m_position.m_z;
	((Vector3*) &transformed.m_values[3])->FUN_004a4e00(&transform->m_rotation);
	((Vector3*) &transformed.m_values[6])->FUN_004a4e00(&transform->m_rotation);
	((Vector3*) &transformed.m_values[9])->FUN_004a4e00(&transform->m_rotation);
	if (TestGeometryBoxContact((CollisionGeometryView*) p_geometry, NULL, &transformed, contact) == FALSE) {
		return FALSE;
	}
	++p_state->m_currentLinkCount;
	return TRUE;
}

// STUB: LEGORACERS2 0x00509600
LegoBool32 __fastcall AppendSphereSphereContact(
	const CollisionSphereRecord* p_first,
	void*,
	const CollisionSphereRecord* p_second
)
{
	const LegoFloat deltaX = p_first->m_center.m_x - p_second->m_center.m_x;
	const LegoFloat deltaY = p_first->m_center.m_y - p_second->m_center.m_y;
	const LegoFloat deltaZ = p_first->m_center.m_z - p_second->m_center.m_z;
	const LegoFloat distanceSquared = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
	const LegoFloat radius = p_second->m_radius + p_first->m_radius;
	if (radius * radius < distanceSquared || distanceSquared < 0.000001f) {
		return FALSE;
	}

	CollisionPairQueryState* state = g_collisionServiceState;
	CollisionContact* contact = &state->m_records[state->m_currentLinkCount];
	contact->m_flags &= (LegoU16) ~2;
	const LegoFloat inverseDistance = 1.0f / (LegoFloat) sqrt(distanceSquared);
	contact->m_normal.m_x = inverseDistance * deltaX;
	contact->m_normal.m_y = inverseDistance * deltaY;
	contact->m_normal.m_z = inverseDistance * deltaZ;
	contact->m_separationAxis = contact->m_normal;
	contact->m_penetration = radius - (LegoFloat) sqrt(distanceSquared);
	contact->m_point.m_x = p_first->m_center.m_x - p_first->m_radius * contact->m_normal.m_x;
	contact->m_point.m_y = p_first->m_center.m_y - p_first->m_radius * contact->m_normal.m_y;
	contact->m_point.m_z = p_first->m_center.m_z - p_first->m_radius * contact->m_normal.m_z;
	if (state->m_currentLinkCount < 0xff) {
		++state->m_currentLinkCount;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x00509760
LegoBool32 __fastcall IntersectRayWithCollisionSphere(
	const CollisionSphereRecord* p_sphere,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionContact* p_result
)
{
	LegoFloat x = p_start->m_x - p_sphere->m_center.m_x;
	LegoFloat y = p_start->m_y - p_sphere->m_center.m_y;
	LegoFloat z = p_start->m_z - p_sphere->m_center.m_z;
	LegoFloat a = p_delta->m_z * p_delta->m_z + p_delta->m_y * p_delta->m_y + p_delta->m_x * p_delta->m_x;
	LegoFloat b = y * p_delta->m_y + x * p_delta->m_x + z * p_delta->m_z;
	b += b;
	LegoFloat radiusSquared;
	memcpy(&radiusSquared, &p_sphere->m_unk0x10, sizeof(radiusSquared));
	LegoFloat discriminant = b * b - ((x * x + y * y + z * z) - radiusSquared) * a * 4.0f;
	if (discriminant >= 0.0f) {
		LegoFloat distance = (-b - (LegoFloat) sqrt(discriminant)) / (a + a);
		if (distance <= p_result->m_penetration && distance >= 0.0f) {
			p_result->m_unk0x4a &= 0xfffd;
			p_result->m_penetration = distance;
			p_result->m_point.m_x = distance * p_delta->m_x + p_start->m_x;
			p_result->m_point.m_y = distance * p_delta->m_y + p_start->m_y;
			p_result->m_point.m_z = distance * p_delta->m_z + p_start->m_z;
			p_result->m_normal = p_result->m_point;
			p_result->m_normal.m_x -= p_sphere->m_center.m_x;
			p_result->m_normal.m_y -= p_sphere->m_center.m_y;
			p_result->m_normal.m_z -= p_sphere->m_center.m_z;
			LegoFloat inverseRadius = 1.0f / p_sphere->m_radius;
			p_result->m_normal.m_x *= inverseRadius;
			p_result->m_normal.m_y *= inverseRadius;
			p_result->m_normal.m_z *= inverseRadius;
			p_result->m_separationAxis = p_result->m_normal;
			return TRUE;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x00509a40
void __fastcall TransformCollisionSphereRecord(
	CollisionSphereRecord* p_record,
	void*,
	const CollisionModelTransform* p_transform
)
{
	Vector3* center = &p_record->m_center;
	center->FUN_004a4e00(&p_transform->m_rotation);
	center->m_x += p_transform->m_position.m_x;
	center->m_y += p_transform->m_position.m_y;
	center->m_z += p_transform->m_position.m_z;
}

// STUB: LEGORACERS2 0x0050bfd0
LegoBool32 __fastcall IntersectRayWithCollisionTriangle(
	const CollisionTriangleRecord* p_triangle,
	void*,
	const Vector3* p_origin,
	const Vector3* p_direction,
	CollisionContact* p_contact
)
{
	const LegoFloat originSide = p_triangle->m_planeDistance -
								 (p_triangle->m_normal.m_x * p_origin->m_x + p_triangle->m_normal.m_y * p_origin->m_y +
								  p_triangle->m_normal.m_z * p_origin->m_z);
	const LegoFloat directionSide = p_triangle->m_normal.m_x * p_direction->m_x +
									p_triangle->m_normal.m_y * p_direction->m_y +
									p_triangle->m_normal.m_z * p_direction->m_z;
	LegoFloat distance;
	Vector3 point;
	LegoS32 edge;
	if (originSide > 0.0000001f || directionSide > -0.0000001f) {
		return FALSE;
	}
	distance = originSide / directionSide;
	if (distance > p_contact->m_penetration || distance < 0.0f) {
		return FALSE;
	}
	point.m_x = p_origin->m_x + distance * p_direction->m_x;
	point.m_y = p_origin->m_y + distance * p_direction->m_y;
	point.m_z = p_origin->m_z + distance * p_direction->m_z;
	for (edge = 0; edge < 3; ++edge) {
		const Vector3* first = p_triangle->m_vertices[edge];
		const Vector3* second = p_triangle->m_vertices[(edge + 1) % 3];
		const LegoFloat edgeX = second->m_x - first->m_x;
		const LegoFloat edgeY = second->m_y - first->m_y;
		const LegoFloat edgeZ = second->m_z - first->m_z;
		const LegoFloat offsetX = first->m_x - point.m_x;
		const LegoFloat offsetY = first->m_y - point.m_y;
		const LegoFloat offsetZ = first->m_z - point.m_z;
		const LegoFloat crossX = edgeY * offsetZ - edgeZ * offsetY;
		const LegoFloat crossY = edgeZ * offsetX - edgeX * offsetZ;
		const LegoFloat crossZ = edgeX * offsetY - edgeY * offsetX;
		const LegoFloat side =
			crossX * p_triangle->m_normal.m_x + crossY * p_triangle->m_normal.m_y + crossZ * p_triangle->m_normal.m_z;
		if (side < -0.0000001f) {
			return FALSE;
		}
	}
	p_contact->m_penetration = distance;
	p_contact->m_normal = p_triangle->m_normal;
	p_contact->m_point = point;
	p_contact->m_unk0x4a |= 2;
	{
		const LegoS32 byteOffset =
			(LegoS32) ((const LegoU8*) p_triangle - (const LegoU8*) g_collisionTriangleArrayBase);
		const LegoS32 triangleIndex = byteOffset / (LegoS32) sizeof(CollisionTriangleRecord);
		const LegoU16 flags = p_contact->m_unk0x4a & 3;
		p_contact->m_unk0x4a = (LegoU16) (triangleIndex * 4 | flags);
	}
	p_contact->m_separationAxis = p_triangle->m_normal;
	return TRUE;
}
