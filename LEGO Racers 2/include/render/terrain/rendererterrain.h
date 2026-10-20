#ifndef LR2_RENDERERTERRAIN_H
#define LR2_RENDERERTERRAIN_H

#include "math/vector3.h"
#include "render/buffers/rendererbufferobjects.h"
#include "scene/terrain/terrain.h"
#include "types.h"

#include <stddef.h>

// Receiver view for the terrain visibility/culling data passed through the
// renderer. Only the member ABI is claimed; its owning allocation is not yet
// reconstructed.
class TerrainVisibilityContext {
public:
	LegoU32 GetCellState(LegoU32 p_column, LegoS32 p_row);
};

// Output packet consumed and filled by the terrain geometry callback. The
// callback reads the two render-buffer pointers after the eight-byte gap.
struct RendererTerrainGeometryPacket {
	LegoS16 m_triangleCount; // 0x00
	LegoS16 m_vertexCount;   // 0x02
	LegoU8 m_unk0x04[8];
	RendererVertexBuffer* m_vertexBuffer; // 0x0c
	RendererIndexBuffer* m_indexBuffer;   // 0x10
};

DECOMP_SIZE_ASSERT(RendererTerrainGeometryPacket, 0x14);
DECOMP_STATIC_ASSERT(offsetof(RendererTerrainGeometryPacket, m_vertexBuffer) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(RendererTerrainGeometryPacket, m_indexBuffer) == 0x10);

// Builds the vertex and index ranges for a terrain draw. This is the free
// six-stack-argument callback at 0x004eb9d0 (callee pops 0x18 bytes).
void __stdcall BuildTerrainGeometry(
	Terrain* p_terrain,
	Vector3* p_outputPoint,
	const Vector3* p_inputPoints,
	LegoS32 p_inputPointCount,
	RendererTerrainGeometryPacket* p_packet,
	LegoS32 p_mode
);

// Re-selects the edge height samples of each active tile grid from the LOD of
// its four neighbors. This is the terrain helper at 0x004ea550.
void UpdateTerrainGridEdgeSelections(Terrain* p_terrain);

// Recomputes the four squared terrain LOD thresholds from renderer projection
// scale and the renderer's current detail level. This is 0x004cb530.
void UpdateTerrainLodMetrics(Terrain* p_terrain);

#endif // LR2_RENDERERTERRAIN_H
