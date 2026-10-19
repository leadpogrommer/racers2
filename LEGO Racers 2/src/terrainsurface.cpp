#include "physicallifecycle.h"
#include "terrain.h"
#include "vector3.h"

namespace
{

static LegoS32 RoundTerrainGridCoordinate(LegoFloat p_value)
{
	volatile LegoFloat value = p_value;
	LegoS32 result;
	__asm {
		fld value
		fistp result
	}
	return result;
}

struct TerrainLifecycleOwner {
	LegoU8 m_unk0x00[0x54];
	PhysicalLifecycleState* m_lifecycle;
};

static PhysicalLifecycleState* GetTerrainLifecycle(const Terrain* p_terrain)
{
	return ((const TerrainLifecycleOwner*) p_terrain)->m_lifecycle;
}

static const LifecycleTransformDispatch* GetTerrainDispatch(const Terrain* p_terrain)
{
	return GetTerrainLifecycle(p_terrain)->GetTransformDispatch();
}

static Vector3 GetTerrainWorldPosition(const Terrain* p_terrain)
{
	const LifecycleTransformDispatch* dispatch = GetTerrainDispatch(p_terrain);
	return *dispatch->m_position(GetTerrainLifecycle(p_terrain), NULL);
}

static const Matrix3* GetTerrainWorldRotation(const Terrain* p_terrain)
{
	const LifecycleTransformDispatch* dispatch = GetTerrainDispatch(p_terrain);
	return dispatch->m_rotation(GetTerrainLifecycle(p_terrain), NULL);
}

static Vector3 MakeTerrainVertex(const Terrain* p_terrain, LegoS32 p_column, LegoS32 p_row)
{
	TerrainVertex* vertex = p_terrain->GetVertex(p_column, p_row);
	Vector3 result;
	result.m_x = ((LegoFloat) p_column - p_terrain->m_columnOrigin) * p_terrain->m_scale.m_x;
	result.m_y = (LegoFloat) vertex->m_height * p_terrain->m_scale.m_y;
	result.m_z = ((LegoFloat) p_row - p_terrain->m_rowOrigin) * p_terrain->m_scale.m_z;
	return result;
}

static void CopyTerrainFallback(
	const Terrain* p_terrain,
	const Vector3& p_localPosition,
	Vector3* p_worldPosition,
	Vector3* p_worldNormal
)
{
	*p_worldPosition = p_localPosition;
	p_worldNormal->SetValues(0.0f, 1.0f, 0.0f);
	const Matrix3* rotation = GetTerrainWorldRotation(p_terrain);
	p_worldPosition->FUN_004a4e00(rotation);
	p_worldNormal->FUN_004a4e00(rotation);
	Vector3 origin = GetTerrainWorldPosition(p_terrain);
	p_worldPosition->Add(&origin);
}

} // namespace

// STUB: LEGORACERS2 0x004c4200
LegoBool32 Terrain::ProjectToSurface(
	const Vector3* p_worldPosition,
	Vector3* p_surfacePosition,
	Vector3* p_worldNormal
) const
{
	Vector3 terrainOrigin = GetTerrainWorldPosition(this);
	Vector3 localPosition = *p_worldPosition;
	localPosition.Subtract(&terrainOrigin);
	localPosition.FUN_004a4e50(GetTerrainWorldRotation(this));

	LegoS32 minColumn;
	LegoS32 minRow;
	LegoS32 maxColumn;
	LegoS32 maxRow;
	if (!GetCellRange(&localPosition, 0.0f, &minColumn, &minRow, &maxColumn, &maxRow)) {
		CopyTerrainFallback(this, localPosition, p_surfacePosition, p_worldNormal);
		return FALSE;
	}

	// A set low bit marks a tile cell whose surface is treated as flat by this
	// projection routine. The unset case is the height-field triangle path.
	if ((GetVertex(minColumn + 1, minRow + 1)->m_flags & 1) != 0) {
		CopyTerrainFallback(this, localPosition, p_surfacePosition, p_worldNormal);
		return FALSE;
	}

	Vector3 corners[4];
	corners[0] = MakeTerrainVertex(this, minColumn, minRow);
	corners[1] = MakeTerrainVertex(this, minColumn, minRow + 1);
	corners[2] = MakeTerrainVertex(this, minColumn + 1, minRow);
	corners[3] = MakeTerrainVertex(this, minColumn + 1, minRow + 1);
	LegoFloat dx = localPosition.m_x - corners[0].m_x;
	LegoFloat dz = localPosition.m_z - corners[0].m_z;
	LegoFloat splitLeft = corners[3].m_z - corners[0].m_z;
	LegoFloat splitRight = corners[3].m_x - corners[0].m_x;
	LegoBool32 firstTriangle = dx <= 0.0001f || dz / dx <= splitLeft / splitRight;
	const Vector3* origin;
	const Vector3* edgeA;
	const Vector3* edgeB;
	if (firstTriangle) {
		origin = &corners[0];
		edgeA = &corners[3];
		edgeB = &corners[2];
	}
	else {
		origin = &corners[0];
		edgeA = &corners[1];
		edgeB = &corners[3];
	}
	Vector3 firstEdge = *edgeA;
	firstEdge.Subtract(origin);
	Vector3 secondEdge = *edgeB;
	secondEdge.Subtract(origin);
	Vector3 localNormal;
	localNormal.Cross(&firstEdge, &secondEdge);
	LegoFloat verticalNormal = localNormal.m_y;
	if (verticalNormal < 0.0f) {
		verticalNormal = -verticalNormal;
	}
	if (verticalNormal < 0.00001f) {
		return FALSE;
	}

	*p_surfacePosition = localPosition;
	p_surfacePosition->m_y +=
		(localNormal.m_x * origin->m_x + localNormal.m_y * origin->m_y + localNormal.m_z * origin->m_z -
		 (localNormal.m_x * localPosition.m_x + localNormal.m_z * localPosition.m_z +
		  localNormal.m_y * localPosition.m_y)) /
		localNormal.m_y;
	p_surfacePosition->FUN_004a4e00(GetTerrainWorldRotation(this));
	*p_worldNormal = localNormal;
	p_worldNormal->Normalize();
	p_worldNormal->FUN_004a4e00(GetTerrainWorldRotation(this));
	Vector3 currentOrigin = GetTerrainWorldPosition(this);
	p_surfacePosition->Add(&currentOrigin);
	return TRUE;
}

// STUB: LEGORACERS2 0x004c4d30
LegoBool32 Terrain::GetCellRange(
	const Vector3* p_position,
	LegoFloat p_radius,
	LegoS32* p_minColumn,
	LegoS32* p_minRow,
	LegoS32* p_maxColumn,
	LegoS32* p_maxRow
) const
{
	LegoFloat centerColumn = p_position->m_x * m_inverseScaleX + m_columnOrigin;
	LegoFloat centerRow = p_position->m_z * m_inverseScaleZ + m_rowOrigin;
	LegoFloat halfColumn = p_radius * m_inverseScaleX;
	LegoFloat halfRow = p_radius * m_inverseScaleZ;
	*p_minColumn = RoundTerrainGridCoordinate((centerColumn - halfColumn) - 0.5f);
	*p_minRow = RoundTerrainGridCoordinate((centerRow - halfRow) - 0.5f);
	*p_maxColumn = RoundTerrainGridCoordinate((centerColumn + halfColumn) + 0.5f);
	*p_maxRow = RoundTerrainGridCoordinate((centerRow + halfRow) + 0.5f);
	LegoBool32 wasEmpty = *p_maxColumn < 1 || *p_maxRow < 1;
	if (*p_minColumn < 0) {
		*p_minColumn = 0;
	}
	if (*p_minRow < 0) {
		*p_minRow = 0;
	}
	if (m_columns <= *p_maxColumn) {
		*p_maxColumn = m_columns - 1;
	}
	if (m_rows <= *p_maxRow) {
		*p_maxRow = m_rows - 1;
	}
	return !wasEmpty && *p_minColumn < m_columns - 1 && *p_minRow < m_rows - 1;
}
