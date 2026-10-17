#include "terrain.h"

// GLOBAL: LEGORACERS2 0x0067a688
TerrainEdgeSelection g_terrainCellEdges = {0};

// GLOBAL: LEGORACERS2 0x0067b68c
TerrainEdgeSelection g_terrainLookupEdges = {0};

// STUB: LEGORACERS2 0x004c3f40
LegoBool32 Terrain::GetCellVertices(LegoS32 p_column, LegoS32 p_row, Vector3* p_vertices) const
{
	if (p_column < 0) {
		return FALSE;
	}
	if (p_row < 0) {
		return FALSE;
	}
	if (p_column >= m_columns - 1) {
		return FALSE;
	}
	if (p_row >= m_rows - 1) {
		return FALSE;
	}

	TerrainTile* tile = GetTile(p_column, p_row);
	if (p_column - tile->m_column + 1 >= m_tileColumnSpan || p_row - tile->m_row + 1 >= m_tileRowSpan) {
		tile = NULL;
	}

	LegoFloat x = (p_column - m_columnOrigin) * m_scale.m_x;
	LegoFloat nextX = x + m_scale.m_x;
	LegoFloat z = (p_row - m_rowOrigin) * m_scale.m_z;
	LegoFloat nextZ = z + m_scale.m_z;

	TerrainVertex* vertex00;
	TerrainVertex* vertex01;
	TerrainVertex* vertex10;
	TerrainVertex* vertex11;
	if (tile == NULL) {
		vertex00 = GetVertex(p_column, p_row);
		vertex01 = GetVertex(p_column, p_row + 1);
		vertex10 = GetVertex(p_column + 1, p_row);
		vertex11 = GetVertex(p_column + 1, p_row + 1);
	}
	else {
		p_column -= tile->m_column;
		p_row -= tile->m_row;
		TerrainVertexGrid* grid = &tile->m_vertexGrid;
		grid->SelectEdges(&g_terrainCellEdges);
		vertex00 = &grid->m_vertices[grid->m_columns * p_row + p_column];
		vertex10 = vertex00 + 1;
		vertex01 = &grid->m_vertices[grid->m_columns * (p_row + 1) + p_column];
		vertex11 = vertex01 + 1;
	}

	p_vertices[0].m_x = x;
	p_vertices[0].m_z = z;
	p_vertices[0].m_y = vertex00->m_height * m_scale.m_y;
	p_vertices[1].m_z = nextZ;
	p_vertices[1].m_x = x;
	p_vertices[1].m_y = vertex01->m_height * m_scale.m_y;
	p_vertices[2].m_x = nextX;
	p_vertices[2].m_z = z;
	p_vertices[2].m_y = vertex10->m_height * m_scale.m_y;
	p_vertices[3].m_x = nextX;
	p_vertices[3].m_z = nextZ;
	p_vertices[3].m_y = vertex11->m_height * m_scale.m_y;
	return (~vertex11->m_flags) & 1;
}

// FUNCTION: LEGORACERS2 0x004c4140
Vector3* Terrain::GetMinimumBounds(Vector3* p_result) const
{
	LegoFloat z = -((m_rows + 1) / 2) * m_scale.m_z;
	LegoFloat y = m_minHeight * m_scale.m_y;
	LegoFloat x = -((m_columns + 1) / 2) * m_scale.m_x;
	p_result->m_x = x;
	p_result->m_y = y;
	p_result->m_z = z;
	return p_result;
}

// FUNCTION: LEGORACERS2 0x004c41a0
Vector3* Terrain::GetMaximumBounds(Vector3* p_result) const
{
	LegoFloat z = ((m_rows + 1) / 2) * m_scale.m_z;
	LegoFloat y = m_maxHeight * m_scale.m_y;
	LegoFloat x = ((m_columns + 1) / 2) * m_scale.m_x;
	p_result->m_x = x;
	p_result->m_y = y;
	p_result->m_z = z;
	return p_result;
}

// FUNCTION: LEGORACERS2 0x004cb630
TerrainVertex* Terrain::GetVertex(LegoS32 p_column, LegoS32 p_row) const
{
	TerrainTile* tile = GetTile(p_column, p_row);
	TerrainVertexGrid* grid = &tile->m_vertexGrid;
	LegoS32 column = p_column - tile->m_column;
	LegoS32 row = p_row - tile->m_row;
	grid->SelectEdges(&g_terrainLookupEdges);
	return &grid->m_vertices[grid->m_columns * row + column];
}

// FUNCTION: LEGORACERS2 0x004cb680
TerrainTile* Terrain::GetTile(LegoS32 p_column, LegoS32 p_row) const
{
	LegoS32 tileColumn = m_tileColumns * p_column / m_columns;
	LegoS32 tileRow = m_tileRows * p_row / m_rows;
	return &m_tiles[tileColumn * m_tileRows + tileRow];
}
