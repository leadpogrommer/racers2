#include "collisionqueries.h"
#include "terrain.h"

#include <math.h>

extern Terrain* g_activeTerrainObject;

// GLOBAL: LEGORACERS2 0x0067b848
static Vector3* g_terrainSegmentNormal;
// GLOBAL: LEGORACERS2 0x0067b84c
static Vector3* g_terrainSegmentPoint;
// GLOBAL: LEGORACERS2 0x0067b864
static LegoFloat* g_terrainSegmentFraction;
// GLOBAL: LEGORACERS2 0x0067ba68
static LegoBool32 g_terrainSegmentHit;
// GLOBAL: LEGORACERS2 0x0067bae0
static const Vector3* g_terrainSegmentStart;
// GLOBAL: LEGORACERS2 0x0067baf0
static Vector3 g_terrainSegmentDelta;

static LegoBool32 __cdecl IntersectTerrainTriangle(
	const Vector3*,
	const Vector3*,
	const Vector3*,
	const Vector3*,
	const Vector3*,
	LegoFloat*,
	Vector3*,
	Vector3*
);
LegoU32 __cdecl TraverseTerrainGridRay(
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoS32,
	LegoS32,
	LegoBool32(__cdecl*)(LegoS32, LegoS32)
);

// FUNCTION: LEGORACERS2 0x004d6050
static LegoBool32 __cdecl IntersectTerrainQuad(
	const Vector3* p_vertices,
	const Vector3* p_start,
	const Vector3* p_delta,
	LegoFloat* p_fraction,
	Vector3* p_point,
	Vector3* p_normal
)
{
	if (IntersectTerrainTriangle(
			p_start,
			p_delta,
			&p_vertices[0],
			&p_vertices[1],
			&p_vertices[2],
			p_fraction,
			p_normal,
			p_point
		)) {
		return TRUE;
	}
	return IntersectTerrainTriangle(
			   p_start,
			   p_delta,
			   &p_vertices[2],
			   &p_vertices[1],
			   &p_vertices[3],
			   p_fraction,
			   p_normal,
			   p_point
		   ) != FALSE;
}

// FUNCTION: LEGORACERS2 0x004d60e0
static LegoBool32 __cdecl VisitTerrainSegmentCell(LegoS32 p_column, LegoS32 p_row)
{
	Vector3 vertices[4];
	if (g_activeTerrainObject->GetCellVertices(p_column, p_row, vertices) && IntersectTerrainQuad(
																				 vertices,
																				 g_terrainSegmentStart,
																				 &g_terrainSegmentDelta,
																				 g_terrainSegmentFraction,
																				 g_terrainSegmentPoint,
																				 g_terrainSegmentNormal
																			 )) {
		g_terrainSegmentHit = TRUE;
		g_terrainSegmentNormal->Normalize();
		return FALSE;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004d6160
static LegoBool32 __fastcall IntersectTerrainSurfaceSegment(
	Terrain* p_terrain,
	void*,
	const Vector3* p_start,
	const Vector3* p_end,
	LegoFloat* p_fraction,
	Vector3* p_point,
	Vector3* p_normal
)
{
	g_terrainSegmentHit = FALSE;
	g_terrainSegmentStart = p_start;
	g_terrainSegmentDelta = *p_end;
	g_terrainSegmentDelta.m_x -= p_start->m_x;
	g_terrainSegmentDelta.m_y -= p_start->m_y;
	g_terrainSegmentDelta.m_z -= p_start->m_z;
	g_terrainSegmentFraction = p_fraction;
	g_terrainSegmentNormal = p_normal;
	g_terrainSegmentPoint = p_point;
	g_activeTerrainObject = p_terrain;
	LegoFloat lengthSquared = g_terrainSegmentDelta.m_y * g_terrainSegmentDelta.m_y +
							  g_terrainSegmentDelta.m_z * g_terrainSegmentDelta.m_z +
							  g_terrainSegmentDelta.m_x * g_terrainSegmentDelta.m_x;
	if (lengthSquared < 0.00001f) {
		return FALSE;
	}
	LegoFloat inverseX = 1.0f / p_terrain->m_scale.m_x;
	LegoFloat inverseZ = 1.0f / p_terrain->m_scale.m_z;
	TraverseTerrainGridRay(
		p_start->m_x * inverseX + p_terrain->m_columnOrigin,
		p_start->m_z * inverseZ + p_terrain->m_rowOrigin,
		g_terrainSegmentDelta.m_x * inverseX,
		g_terrainSegmentDelta.m_z * inverseZ,
		p_terrain->m_columns,
		p_terrain->m_rows,
		VisitTerrainSegmentCell
	);
	return g_terrainSegmentHit;
}

static LegoFloat DotZyx(const Vector3& p_first, const Vector3& p_second)
{
	return p_first.m_z * p_second.m_z + p_first.m_y * p_second.m_y + p_first.m_x * p_second.m_x;
}

// STUB: LEGORACERS2 0x004f8470
static LegoBool32 __cdecl IntersectTerrainTriangle(
	const Vector3* p_start,
	const Vector3* p_delta,
	const Vector3* p_first,
	const Vector3* p_second,
	const Vector3* p_third,
	LegoFloat* p_fraction,
	Vector3* p_normal,
	Vector3* p_point
)
{
	Vector3 firstEdge;
	firstEdge.m_x = p_second->m_x - p_first->m_x;
	firstEdge.m_y = p_second->m_y - p_first->m_y;
	firstEdge.m_z = p_second->m_z - p_first->m_z;
	Vector3 secondEdge;
	secondEdge.m_x = p_third->m_x - p_second->m_x;
	secondEdge.m_y = p_third->m_y - p_second->m_y;
	secondEdge.m_z = p_third->m_z - p_second->m_z;
	p_normal->m_z = secondEdge.m_y * firstEdge.m_x - firstEdge.m_y * secondEdge.m_x;
	p_normal->m_x = secondEdge.m_z * firstEdge.m_y - secondEdge.m_y * firstEdge.m_z;
	p_normal->m_y = firstEdge.m_z * secondEdge.m_x - secondEdge.m_z * firstEdge.m_x;
	LegoFloat firstPlane = p_normal->m_z * p_start->m_z + p_normal->m_x * p_start->m_x + p_normal->m_y * p_start->m_y;
	LegoFloat secondPlane =
		(p_normal->m_y * p_delta->m_y + p_normal->m_z * p_delta->m_z + p_normal->m_x * p_delta->m_x) * *p_fraction +
		firstPlane;
	LegoFloat plane = p_normal->m_z * p_first->m_z + p_normal->m_y * p_first->m_y + p_normal->m_x * p_first->m_x;
	if (secondPlane < firstPlane) {
		LegoFloat temporary = firstPlane;
		firstPlane = secondPlane;
		secondPlane = temporary;
	}
	if (!(firstPlane <= plane && plane <= secondPlane)) {
		return FALSE;
	}
	const Vector3* vertices[3] = {p_first, p_second, p_third};
	for (LegoS32 i = 0; i < 3; ++i) {
		const Vector3* first = vertices[i];
		const Vector3* second = vertices[(i + 1) % 3];
		const Vector3* third = vertices[(i + 2) % 3];
		Vector3 edge;
		edge.m_x = second->m_x - first->m_x;
		edge.m_y = second->m_y - first->m_y;
		edge.m_z = second->m_z - first->m_z;
		Vector3 axis;
		axis.m_z = edge.m_y * p_delta->m_x - edge.m_x * p_delta->m_y;
		axis.m_x = edge.m_z * p_delta->m_y - edge.m_y * p_delta->m_z;
		axis.m_y = edge.m_x * p_delta->m_z - edge.m_z * p_delta->m_x;
		LegoFloat origin = axis.m_x * p_start->m_x + axis.m_z * p_start->m_z + axis.m_y * p_start->m_y;
		LegoFloat minimum = axis.m_z * first->m_z + axis.m_y * first->m_y + axis.m_x * first->m_x;
		LegoFloat maximum = axis.m_z * third->m_z + axis.m_x * third->m_x + axis.m_y * third->m_y;
		if (maximum < minimum) {
			LegoFloat temporary = minimum;
			minimum = maximum;
			maximum = temporary;
		}
		if (!(minimum <= origin && origin <= maximum)) {
			return FALSE;
		}
	}
	LegoFloat originSide = DotZyx(*p_start, *p_normal);
	LegoFloat directionSide = DotZyx(*p_delta, *p_normal);
	if ((LegoFloat) fabs(directionSide) < 0.00001f) {
		return FALSE;
	}
	LegoFloat distance = (plane - originSide) / directionSide;
	*p_point = *p_start;
	p_point->m_x += distance * p_delta->m_x;
	p_point->m_y += distance * p_delta->m_y;
	p_point->m_z += distance * p_delta->m_z;
	*p_fraction = distance / *p_fraction;
	return TRUE;
}

// STUB: LEGORACERS2 0x004faf60
LegoBool32 __fastcall IntersectTerrainCollisionSegment(
	void* p_owner,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	Terrain* terrain = *(Terrain**) p_owner;
	LegoFloat fraction = 1.0f;
	Vector3 end;
	end.m_x = p_result->m_penetration * p_delta->m_x + p_start->m_x;
	end.m_y = p_result->m_penetration * p_delta->m_y + p_start->m_y;
	end.m_z = p_result->m_penetration * p_delta->m_z + p_start->m_z;
	Vector3 point;
	Vector3 normal;
	if (!IntersectTerrainSurfaceSegment(terrain, NULL, p_start, &end, &fraction, &point, &normal)) {
		return FALSE;
	}
	p_result->m_point = point;
	p_result->m_normal.m_x = normal.m_x;
	p_result->m_penetration = fraction * p_result->m_penetration;
	p_result->m_normal.m_y = normal.m_y;
	p_result->m_normal.m_z = normal.m_z;
	return TRUE;
}
