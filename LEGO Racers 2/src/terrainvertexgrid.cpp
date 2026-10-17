#include "terrainvertexgrid.h"

// FUNCTION: LEGORACERS2 0x004cb340
void TerrainVertexGrid::SelectEdges(const TerrainEdgeSelection* p_selection)
{
	if (p_selection->m_packed == m_selection.m_packed) {
		return;
	}

	if (p_selection->m_edges[0] != m_selection.m_edges[0]) {
		LegoS32 offset = m_edgeOffsets[0][0];
		if (p_selection->m_edges[0] != 0) {
			offset = m_edgeOffsets[1][0];
		}
		const LegoU16* heights = m_edgeHeights + offset;
		m_selection.m_edges[0] = p_selection->m_edges[0];
		for (LegoS32 row = 0; row < m_rows; ++row, ++heights) {
			m_vertices[row * m_columns].m_height = *heights;
		}
	}

	if (p_selection->m_edges[2] != m_selection.m_edges[2]) {
		LegoS32 offset = m_edgeOffsets[0][2];
		if (p_selection->m_edges[2] != 0) {
			offset = m_edgeOffsets[1][2];
		}
		const LegoU16* heights = m_edgeHeights + offset;
		m_selection.m_edges[2] = p_selection->m_edges[2];
		for (LegoS32 row = 0; row < m_rows; ++row) {
			m_vertices[(row + 1) * m_columns - 1].m_height = *heights++;
		}
	}

	if (p_selection->m_edges[1] != m_selection.m_edges[1]) {
		LegoS32 offset = m_edgeOffsets[0][1];
		if (p_selection->m_edges[1] != 0) {
			offset = m_edgeOffsets[1][1];
		}
		const LegoU16* heights = m_edgeHeights + offset;
		m_selection.m_edges[1] = p_selection->m_edges[1];
		for (LegoS32 column = 0; column < m_columns; ++column) {
			m_vertices[column].m_height = *heights++;
		}
	}

	if (p_selection->m_edges[3] != m_selection.m_edges[3]) {
		LegoS32 offset = m_edgeOffsets[0][3];
		if (p_selection->m_edges[3] != 0) {
			offset = m_edgeOffsets[1][3];
		}
		const LegoU16* heights = m_edgeHeights + offset;
		m_selection.m_edges[3] = p_selection->m_edges[3];
		for (LegoS32 column = 0; column < m_columns; ++column, ++heights) {
			m_vertices[(m_rows - 1) * m_columns + column].m_height = *heights;
		}
	}
}
