#include "rendererterrain.h"

#include "matrix3.h"
#include "renderer.h"
#include "terrainvertexgrid.h"

#include <math.h>
#include <string.h>

namespace
{

struct TerrainStreamScaleState {
	LegoFloat m_worldScaleX;   // 0x00
	LegoFloat m_worldScaleY;   // 0x04
	LegoFloat m_worldScaleZ;   // 0x08
	LegoFloat m_unk0x0c;       // 0x0c
	LegoFloat m_textureScaleX; // 0x10
	LegoFloat m_textureScaleZ; // 0x14
	LegoFloat m_unk0x18;       // 0x18
	LegoFloat m_unk0x1c;       // 0x1c
	LegoFloat m_normalScaleX;  // 0x20
	LegoFloat m_normalScaleY;  // 0x24
	LegoFloat m_normalScaleZ;  // 0x28
	LegoFloat m_unk0x2c;       // 0x2c
	LegoFloat m_vertexOffsetX; // 0x30
	LegoFloat m_vertexOffsetZ; // 0x34
};

DECOMP_SIZE_ASSERT(TerrainStreamScaleState, 0x38);

// The caller fills the per-terrain scales and per-tile offsets before it
// streams a grid. Retail stores these as a zero-initialized BSS block.
// GLOBAL: LEGORACERS2 0x006e4518
static TerrainStreamScaleState g_terrainStreamScaleState;

// Sixteen grayscale terrain colors selected by the low four flag bits.
// GLOBAL: LEGORACERS2 0x0054b0bc
static const LegoU32 g_terrainColorByFlags[16] = {
	0xff000000,
	0xff101010,
	0xff202020,
	0xff303030,
	0xff404040,
	0xff505050,
	0xff606060,
	0xff707070,
	0xff808080,
	0xff909090,
	0xffa0a0a0,
	0xffb0b0b0,
	0xffc0c0c0,
	0xffd0d0d0,
	0xffe0e0e0,
	0xfff0f0f0
};

struct TerrainStreamVertex {
	LegoFloat m_x;
	LegoFloat m_y;
	LegoFloat m_z;
	LegoFloat m_normalX;
	LegoFloat m_normalY;
	LegoFloat m_normalZ;
	LegoU32 m_diffuse;
	LegoFloat m_textureU;
	LegoFloat m_textureV;
};

DECOMP_SIZE_ASSERT(TerrainStreamVertex, 0x24);

static TerrainVertexGrid* GetTerrainGrid(TerrainTile* p_tile, LegoS32 p_level)
{
	return (TerrainVertexGrid*) ((LegoU8*) p_tile + 0x90 + p_level * 0x20);
}

static TerrainTile* GetTerrainTileByGridIndex(Terrain* p_terrain, LegoS32 p_column, LegoS32 p_row)
{
	LegoU8* bytes = (LegoU8*) p_terrain;
	LegoS32 tileRows = *(LegoS32*) (bytes + 0x1c0);
	TerrainTile* tiles = *(TerrainTile**) (bytes + 0x1d0);
	return &tiles[tileRows * p_column + p_row];
}

static LegoS32 RoundToNearestEven(LegoFloat p_value)
{
	LegoFloat integral = (LegoFloat) floor((double) p_value);
	LegoFloat fraction = p_value - integral;
	LegoS32 result = (LegoS32) integral;
	if (fraction > 0.5f || (fraction == 0.5f && (result & 1) != 0)) {
		result++;
	}
	return result;
}

static LegoS32 Clamp(LegoS32 p_value, LegoS32 p_minimum, LegoS32 p_maximum)
{
	if (p_value < p_minimum) {
		return p_minimum;
	}
	if (p_value > p_maximum) {
		return p_maximum;
	}
	return p_value;
}

static void TransformTerrainPoint(Terrain* p_terrain, Vector3* p_point)
{
	LegoU8* bytes = (LegoU8*) p_terrain;
	if (*(LegoS32*) (bytes + 0x174) == 1) {
		void* positionSource = *(void**) (bytes + 0x54);
		void** vtable = *(void***) positionSource;
		typedef Vector3*(__fastcall * GetPosition)(void*, void*);
		Vector3* sourcePosition = ((GetPosition) vtable[0x14 / 4])(positionSource, NULL);
		p_point->m_x -= sourcePosition->m_x;
		p_point->m_y -= sourcePosition->m_y;
		p_point->m_z -= sourcePosition->m_z;
		p_point->FUN_004a4e00((const Matrix3*) (bytes + 0x190));
	}
	Vector3* terrainOrigin = (Vector3*) (bytes + 0x178);
	p_point->m_x -= terrainOrigin->m_x;
	p_point->m_y -= terrainOrigin->m_y;
	p_point->m_z -= terrainOrigin->m_z;
}

static LegoS32 GetTerrainGridCoordinate(LegoFloat p_value, LegoFloat p_scale)
{
	return RoundToNearestEven(p_value * p_scale - 0.5f);
}

static LegoS32 ShiftTerrainCoordinate(LegoS32 p_coordinate, LegoS32 p_shift)
{
	return p_coordinate >> (p_shift & 0x1f);
}

static LegoS32 ComposeTerrainGridIndex(LegoS32 p_row, LegoS32 p_column, LegoS32 p_shift, LegoS32 p_rowShift)
{
	LegoU32 shiftedRow = (LegoU32) ShiftTerrainCoordinate(p_row, p_shift) << (p_rowShift & 0x1f);
	return (LegoS32) shiftedRow + ShiftTerrainCoordinate(p_column, p_shift);
}

static LegoU32 ReadTerrainCellCode(const LegoU8* p_descriptor, LegoS32 p_row, LegoS32 p_column, LegoS32 p_level)
{
	const LegoU8* data = *(const LegoU8**) p_descriptor;
	LegoS32 index = ComposeTerrainGridIndex(p_row, p_column, p_level, p_descriptor[5]);
	LegoS32 byteIndex = index >> 2;
	LegoU32 bitShift = ((LegoU32) index & 3) << 1;
	return (data[byteIndex] >> bitShift) & 3;
}

} // namespace

// STUB: LEGORACERS2 0x004bf860
LegoFloat DisplayRenderer::GetMaximumDirectionalLightIntensity()
{
	LegoFloat red = 0.0f;
	LegoFloat green = 0.0f;
	LegoFloat blue = 0.0f;

	for (LegoS32 i = 0; i < m_lightCount; i++) {
		RendererLightRecord* light = &m_lights[i];
		if ((light->m_flags & 4) != 0 && light->m_light.Type == D3DLIGHT_DIRECTIONAL) {
			red += light->m_light.Diffuse.r;
			green += light->m_light.Diffuse.g;
			blue += light->m_light.Diffuse.b;
		}
	}

	LegoFloat maximum = red;
	if (green > maximum) {
		maximum = green;
	}
	if (blue > maximum) {
		maximum = blue;
	}
	return maximum;
}

// STUB: LEGORACERS2 0x004cb530
void UpdateTerrainLodMetrics(Terrain* p_terrain)
{
	LegoU8* terrainBytes = (LegoU8*) p_terrain;
	LegoFloat maximumScale = *(LegoFloat*) (terrainBytes + 0x138);
	LegoFloat zScale = *(LegoFloat*) (terrainBytes + 0x140);
	if (maximumScale < zScale) {
		maximumScale = zScale;
	}

	LegoFloat projectedScale =
		g_displayRenderer->m_vtable->m_scaleProjectionCoordinate(g_displayRenderer, NULL, maximumScale, 1.0f);
	LegoS32 detailLevel = g_displayRenderer->m_unk0x4d0;
	LegoFloat detailScale;
	if (detailLevel < 8) {
		detailScale = 68.0f - (LegoFloat) (8 - detailLevel) * 8.0f;
	}
	else {
		detailScale = (LegoFloat) (detailLevel - 8) * 30.0f + 68.0f;
	}
	projectedScale /= 2.0f / detailScale;

	LegoFloat minimumScale = *(LegoFloat*) (terrainBytes + 0x1f0) * 0.5f * 1.1f;
	if (projectedScale < minimumScale) {
		projectedScale = minimumScale;
	}
	LegoFloat* thresholds = (LegoFloat*) (terrainBytes + 0x1dc);
	thresholds[0] = projectedScale;
	for (LegoS32 doubledIndex = 1; doubledIndex < 4; doubledIndex++) {
		thresholds[doubledIndex] = thresholds[doubledIndex - 1] + thresholds[doubledIndex - 1];
	}
	for (LegoS32 squaredIndex = 0; squaredIndex < 4; squaredIndex++) {
		thresholds[squaredIndex] *= thresholds[squaredIndex];
	}
}

// STUB: LEGORACERS2 0x004d42e0
LegoU32 TerrainVisibilityContext::GetCellState(LegoU32 p_column, LegoS32 p_row)
{
	LegoU8* context = (LegoU8*) this;
	LegoS32 baseShift = *(LegoS32*) (context + 0xf4);
	LegoS32 scaledRow = ShiftTerrainCoordinate(p_row, baseShift);
	LegoS32 scaledColumn = ShiftTerrainCoordinate((LegoS32) p_column, baseShift);
	LegoS32 level = *(LegoS32*) (context + 0xc0) - 1;
	LegoU32 cellState = p_column;

	if (level >= 0) {
		LegoU32* levelNodes = (LegoU32*) (context + 0x68) + level;
		LegoS32 nodeShift = *(LegoS32*) (context + 0xa8) - level;
		while (level >= 0) {
			LegoS32 cellIndex = ComposeTerrainGridIndex(scaledRow, scaledColumn, level, nodeShift);
			cellState = *(LegoU32*) (*(LegoU32*) levelNodes + cellIndex * 0x20 + 0x1c);
			if (cellState != 8) {
				break;
			}
			level--;
			levelNodes--;
			nodeShift++;
		}
		if (level < 0) {
			level = 0;
		}
	}

	if (cellState == 4) {
		return 4;
	}

	if (*(LegoU32*) (context + 0xb0) == *(LegoU32*) (context + 0xb4)) {
		LegoS32 descriptorCount = *(LegoS32*) (context + 0x104);
		if (descriptorCount < 1) {
			return cellState;
		}

		LegoU32 aggregate = 0;
		LegoU32** descriptors = *(LegoU32***) (context + 0x100);
		for (LegoS32 descriptorIndex = 0; descriptorIndex < descriptorCount; descriptorIndex++) {
			LegoS32 sampleLevel = level;
			const LegoU8* descriptor = (const LegoU8*) descriptors[descriptorIndex] + sampleLevel * 8;
			LegoU32 code = ReadTerrainCellCode(descriptor, scaledRow, scaledColumn, sampleLevel);
			while (code == 3) {
				sampleLevel--;
				descriptor -= 8;
				code = ReadTerrainCellCode(descriptor, scaledRow, scaledColumn, sampleLevel);
			}
			if ((code & 3) == 0) {
				return cellState;
			}
			aggregate |= code & 3;
		}
		if (aggregate == 3) {
			return cellState | 1;
		}
		return aggregate | cellState;
	}

	const LegoU8* descriptor = *(const LegoU8**) (context + 0xb0) + level * 8;
	LegoU32 code = ReadTerrainCellCode(descriptor, scaledRow, scaledColumn, level);
	while (code == 3) {
		level--;
		descriptor -= 8;
		code = ReadTerrainCellCode(descriptor, scaledRow, scaledColumn, level);
	}
	return code | cellState;
}

// STUB: LEGORACERS2 0x004dd0b0
void DisplayRenderer::ApplyAuxiliaryMaterial(LegoS32 p_index)
{
	RendererAuxiliaryResourceRecord* auxiliary = GetAuxiliaryResource(p_index);
	D3DMATERIAL8* material = (D3DMATERIAL8*) ((LegoU8*) this + 0x24528);
	auxiliary->CopyToDeviceMaterial(material);
	m_device->SetMaterial(material);

	LegoU8* rendererBytes = (LegoU8*) this;
	LegoU32 specularEnabled = *(LegoU32*) (rendererBytes + 0x24568) != 0 ? *(LegoU32*) (rendererBytes + 0xc6270) : 0;
	LegoU32* cachedSpecular = (LegoU32*) (rendererBytes + 0xc73c0);
	if (*cachedSpecular != specularEnabled) {
		*cachedSpecular = specularEnabled;
		m_device->SetRenderState((D3DRENDERSTATETYPE) 0x1d, specularEnabled);
	}
	m_vtable->m_action0xc8(this, NULL, auxiliary->m_mode);
}

// STUB: LEGORACERS2 0x004dfec0
void DisplayRenderer::SetAmbientColorFloats(const LegoFloat* p_color)
{
	m_ambientColorFloats[0] = p_color[0];
	m_ambientColorFloats[1] = p_color[1];
	m_ambientColorFloats[2] = p_color[2];
	m_ambientColorFloats[3] = p_color[3];

	LegoU8* ambientBytes = (LegoU8*) &m_ambientColor;
	volatile LegoFloat scaledChannel = p_color[0] * 255.0f;
	LegoS32 roundedChannel;
	__asm {
		fld scaledChannel
		fistp roundedChannel
	}
	ambientBytes[2] = (LegoU8) roundedChannel;
	scaledChannel = p_color[1] * 255.0f;
	__asm {
		fld scaledChannel
		fistp roundedChannel
	}
	ambientBytes[1] = (LegoU8) roundedChannel;
	scaledChannel = p_color[2] * 255.0f;
	__asm {
		fld scaledChannel
		fistp roundedChannel
	}
	ambientBytes[0] = (LegoU8) roundedChannel;
	scaledChannel = p_color[3] * 255.0f;
	__asm {
		fld scaledChannel
		fistp roundedChannel
	}
	ambientBytes[3] = (LegoU8) roundedChannel;

	LegoU32* cachedAmbient = (LegoU32*) ((LegoU8*) this + 0xc7578);
	if (*cachedAmbient != m_ambientColor) {
		*cachedAmbient = m_ambientColor;
		m_device->SetRenderState(D3DRS_AMBIENT, m_ambientColor);
	}
}

// STUB: LEGORACERS2 0x004ea550
void UpdateTerrainGridEdgeSelections(Terrain* p_terrain)
{
	LegoS32 tileColumns = p_terrain->m_tileColumns;
	LegoS32 tileRows = p_terrain->m_tileRows;
	LegoS32 maximumLevel = *(LegoS32*) ((LegoU8*) p_terrain + 0x1d4);
	TerrainTile* nextTile = p_terrain->m_tiles;

	for (LegoS32 tileColumn = 0; tileColumn < tileColumns; tileColumn++) {
		for (LegoS32 tileRow = 0; tileRow < tileRows; tileRow++) {
			TerrainTile* tile = nextTile++;
			LegoS32 level = (LegoS8) * ((LegoU8*) tile + 0x11c);
			if (level == -1 || level >= maximumLevel - 1) {
				continue;
			}

			TerrainEdgeSelection selection = {0};
			if (tileColumn > 0) {
				TerrainTile* neighbor = tile - tileRows;
				LegoS32 neighborLevel = (LegoS8) * ((LegoU8*) neighbor + 0x11c);
				if (neighborLevel != -1 && level < neighborLevel) {
					selection.m_edges[0] = 1;
				}
			}
			if (tileColumn < tileColumns - 1) {
				TerrainTile* neighbor = tile + tileRows;
				LegoS32 neighborLevel = (LegoS8) * ((LegoU8*) neighbor + 0x11c);
				if (neighborLevel != -1 && level < neighborLevel) {
					selection.m_edges[2] = 1;
				}
			}
			if (tileRow > 0) {
				TerrainTile* neighbor = tile - 1;
				LegoS32 neighborLevel = (LegoS8) * ((LegoU8*) neighbor + 0x11c);
				if (neighborLevel != -1 && level < neighborLevel) {
					selection.m_edges[1] = 1;
				}
			}
			if (tileRow < tileRows - 1) {
				TerrainTile* neighbor = tile + 1;
				LegoS32 neighborLevel = (LegoS8) * ((LegoU8*) neighbor + 0x11c);
				if (neighborLevel != -1 && level < neighborLevel) {
					selection.m_edges[3] = 1;
				}
			}

			TerrainVertexGrid* grid = (TerrainVertexGrid*) ((LegoU8*) tile + 0x90 + level * 0x20);
			grid->SelectEdges(&selection);
		}
	}
}

// STUB: LEGORACERS2 0x004ea690
void DisplayRenderer::DrawTerrain(Terrain* p_terrain)
{
	LegoU8* terrainBytes = (LegoU8*) p_terrain;
	LegoFloat savedAmbient[4];
	memcpy(savedAmbient, m_ambientColorFloats, sizeof(savedAmbient));

	LegoU32* transformObject = *(LegoU32**) (terrainBytes + 0x54);
	void** transformVtable = *(void***) transformObject;
	typedef void*(__fastcall * GetTransformValue)(void*, void*);
	void* firstTransform = ((GetTransformValue) transformVtable[0x14 / 4])(transformObject, NULL);
	transformObject = *(LegoU32**) (terrainBytes + 0x54);
	transformVtable = *(void***) transformObject;
	void* secondTransform = ((GetTransformValue) transformVtable[0x18 / 4])(transformObject, NULL);
	typedef void(__fastcall * SetTerrainTransform)(DisplayRenderer*, void*, void*, void*);
	((SetTerrainTransform) * (void**) ((LegoU8*) m_vtable + 0x30))(this, NULL, secondTransform, firstTransform);
	typedef void(__fastcall * RendererAction)(DisplayRenderer*, void*);
	((RendererAction) * (void**) ((LegoU8*) m_vtable + 0x68))(this, NULL);

	ApplyAuxiliaryMaterial(m_streamResourceId);
	LegoFloat terrainColor[4];
	const LegoU8* colorSource = terrainBytes + 0x4a60;
	for (LegoS32 channel = 0; channel < 3; channel++) {
		terrainColor[channel] = colorSource[channel] * (1.0f / 255.0f);
	}
	terrainColor[3] = 1.0f;
	SetAmbientColorFloats(terrainColor);
	m_vtable->m_setDepthState(this, NULL, 1);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x93, 1);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x91, 1);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x8d, 1);

	if (m_normalVertexBuffer == NULL) {
		m_cachedStreamBuffer = NULL;
		if (m_cachedVertexShader != 0x152) {
			m_cachedVertexShader = 0x152;
			m_device->SetVertexShader(0x152);
		}
	}
	else if (m_cachedStreamBuffer != m_normalVertexBuffer) {
		m_cachedStreamBuffer = m_normalVertexBuffer;
		m_device->SetStreamSource(0, m_normalVertexBuffer, 0x24);
		if (m_cachedVertexShader != 0x152) {
			m_cachedVertexShader = 0x152;
			m_device->SetVertexShader(0x152);
		}
	}
	for (LegoU32 stage = 0; stage < 8; stage++) {
		SetCachedTextureStageState(stage, (D3DTEXTURESTAGESTATETYPE) 0x0d, 1);
		SetCachedTextureStageState(stage, (D3DTEXTURESTAGESTATETYPE) 0x0e, 1);
	}

	LegoBool32 hardwareVertexProcessing = m_vtable->m_usesHardwareVertexProcessing(this, NULL) == 1;
	if (!hardwareVertexProcessing) {
		LegoFloat intensity = GetMaximumDirectionalLightIntensity();
		RendererAuxiliaryResourceRecord* auxiliary = GetAuxiliaryResource(m_streamResourceId);
		LegoFloat scale = intensity > 1.0f ? intensity : 1.0f;
		auxiliary->m_color1[0] = scale;
		auxiliary->m_color1[1] = scale;
		auxiliary->m_color1[2] = scale;
		auxiliary->m_color1[3] = scale;
	}

	TerrainStreamScaleState& stream = g_terrainStreamScaleState;
	stream.m_worldScaleX = p_terrain->m_scale.m_x;
	stream.m_worldScaleY = p_terrain->m_scale.m_y;
	stream.m_worldScaleZ = p_terrain->m_scale.m_z;
	stream.m_unk0x0c = 0.0f;
	stream.m_textureScaleX = 1.0f / (LegoFloat) (p_terrain->m_columns - 1);
	stream.m_textureScaleZ = 1.0f / (LegoFloat) (p_terrain->m_rows - 1);
	stream.m_unk0x18 = 0.0f;
	stream.m_unk0x1c = 0.0f;
	stream.m_normalScaleX = 1.0f / 127.0f;
	stream.m_normalScaleY = 1.0f / 127.0f;
	stream.m_normalScaleZ = 1.0f / 127.0f;
	stream.m_unk0x2c = 0.0f;

	UpdateTerrainLodMetrics(p_terrain);
	TerrainVisibilityContext* visibility = *(TerrainVisibilityContext**) (terrainBytes + 0x4a84);
	LegoS32 visibilityShift = *(LegoS32*) ((LegoU8*) visibility + 0xf0);
	LegoS32 visibilityMask = *(LegoS32*) ((LegoU8*) visibility + 0xec);
	LegoS32 maximumLevel = *(LegoS32*) (terrainBytes + 0x1d4);
	LegoS32 tileCount = *(LegoS32*) (terrainBytes + 0x1c4);
	LegoFloat* lodThresholds = (LegoFloat*) (terrainBytes + 0x1dc);
	TerrainTile* tiles = p_terrain->m_tiles;
	typedef void(__fastcall * ProjectTerrainTile)(DisplayRenderer*, void*, TerrainTile*, Vector3*);
	ProjectTerrainTile projectTile = *(ProjectTerrainTile*) ((LegoU8*) m_vtable + 0x5c);
	LegoS32 activeTileCount = 0;
	for (LegoS32 tileIndex = 0; tileIndex < tileCount; tileIndex++) {
		TerrainTile* tile = &tiles[tileIndex];
		LegoU32 cellState = visibility->GetCellState(tileIndex >> (visibilityShift & 0x1f), tileIndex & visibilityMask);
		Vector3 projected;
		projectTile(this, NULL, tile, &projected);
		LegoFloat distanceSquared =
			projected.m_x * projected.m_x + projected.m_y * projected.m_y + projected.m_z * projected.m_z;
		LegoS32 level = maximumLevel - 1;
		while (level > 0 && distanceSquared < lodThresholds[level]) {
			level--;
		}
		LegoS32 topologyLimit = (LegoS8) * ((LegoU8*) tile + 0x11e) >> 1;
		if (topologyLimit < level) {
			level = topologyLimit;
		}
		if ((cellState & 7) != 0 || projected.m_z < 0.0f || level < 0) {
			*((LegoS8*) ((LegoU8*) tile + 0x11c)) = -1;
		}
		else {
			*((LegoS8*) ((LegoU8*) tile + 0x11c)) = (LegoS8) level;
			activeTileCount++;
		}
	}
	if (activeTileCount > 0) {
		UpdateTerrainGridEdgeSelections(p_terrain);
		LegoS32 halfColumns = p_terrain->m_columns >> 1;
		LegoS32 halfRows = p_terrain->m_rows >> 1;
		for (LegoS32 tileIndex = 0; tileIndex < tileCount; tileIndex++) {
			TerrainTile* tile = &tiles[tileIndex];
			LegoS32 level = (LegoS8) * ((LegoU8*) tile + 0x11c);
			if (level < 0) {
				continue;
			}
			TerrainVertexGrid* grid = GetTerrainGrid(tile, level);
			LegoS32 vertexCount = grid->m_columns * grid->m_rows;
			LegoS32 indexCapacity = (grid->m_rows - 1) * (grid->m_columns * 2 + 1) + 8;
			LegoU8* vertexData = NULL;
			IDirect3DVertexBuffer8* vertexBuffer =
				hardwareVertexProcessing ? m_normalVertexBuffer : m_transformedVertexBuffer;
			if (vertexBuffer->Lock(0, vertexCount * sizeof(TerrainStreamVertex), &vertexData, 0x2000) != D3D_OK) {
				continue;
			}
			if (!m_streamingIndexBuffer->Lock(0, indexCapacity, 1)) {
				vertexBuffer->Unlock();
				continue;
			}
			LegoS32 halfTileColumns = *(LegoS16*) ((LegoU8*) tile + 0x0c) - halfColumns;
			LegoS32 halfTileRows = *(LegoS16*) ((LegoU8*) tile + 0x0e) - halfRows;
			stream.m_vertexOffsetX = (LegoFloat) halfTileColumns;
			stream.m_vertexOffsetZ = (LegoFloat) halfTileRows;
			LegoU8* vertexCursor = vertexData;
			LegoU32 indexOffset = 0;
			LegoU32 vertexBase = 0;
			BuildTerrainGridStream(
				&vertexCursor,
				grid,
				level,
				&indexOffset,
				&vertexBase,
				(LegoS8) * ((LegoU8*) tile + 0x11e) >> 1
			);
			m_streamingIndexBuffer->Unlock(1, 1);
			vertexBuffer->Unlock();
			m_device->SetStreamSource(0, vertexBuffer, sizeof(TerrainStreamVertex));
			m_device->SetIndices(m_streamingIndexBuffer->GetDeviceBuffer(), 0);
			m_device
				->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, vertexBase, 0, indexOffset > 2 ? indexOffset - 2 : 0);
		}
	}

	m_vtable->m_action0xc8(this, NULL, 0);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x93, 0);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x91, 0);
	SetCachedRenderState((D3DRENDERSTATETYPE) 0x8d, 0);
	SetAmbientColorFloats(savedAmbient);
}

// STUB: LEGORACERS2 0x004eb530
void DisplayRenderer::BuildTerrainGridStream(
	LegoU8** p_vertexCursor,
	TerrainVertexGrid* p_grid,
	LegoS32 p_level,
	LegoU32* p_indexOffset,
	LegoU32* p_vertexBase,
	LegoS32 p_topology
)
{
	LegoS32 columns = p_grid->m_columns;
	LegoS32 rows = p_grid->m_rows;
	LegoS32 step = 1 << (p_level & 0x1f);
	LegoS32 scaledColumns = columns << (p_level & 0x1f);
	LegoS32 scaledRows = rows << (p_level & 0x1f);
	LegoU16 firstVertex = (LegoU16) *p_vertexBase;
	LegoU8* vertexCursor = *p_vertexCursor;
	TerrainVertex* sourceVertex = p_grid->m_vertices;

	LegoBool32 hardwareVertexProcessing = m_vtable->m_usesHardwareVertexProcessing(this, NULL) == 1;
	for (LegoS32 streamRow = 0; streamRow < scaledRows; streamRow += step) {
		LegoFloat worldZ =
			(streamRow + g_terrainStreamScaleState.m_vertexOffsetZ) * g_terrainStreamScaleState.m_worldScaleZ;
		LegoFloat textureV = streamRow * g_terrainStreamScaleState.m_textureScaleZ;
		for (LegoS32 streamColumn = 0; streamColumn < scaledColumns; streamColumn += step) {
			LegoS32 normalX = (LegoS8) sourceVertex->m_unk0x02[0];
			LegoS32 normalY = (LegoS8) sourceVertex->m_unk0x02[1];
			LegoS32 normalZ = (LegoS8) sourceVertex->m_unk0x02[2];
			LegoU16 colorFlags = *(LegoU16*) ((LegoU8*) sourceVertex + 6);
			TerrainStreamVertex* output = (TerrainStreamVertex*) vertexCursor;
			output->m_x =
				(streamColumn + g_terrainStreamScaleState.m_vertexOffsetX) * g_terrainStreamScaleState.m_worldScaleX;
			output->m_y = sourceVertex->m_height * g_terrainStreamScaleState.m_worldScaleY;
			output->m_z = worldZ;
			output->m_normalX = normalZ * g_terrainStreamScaleState.m_normalScaleZ;
			output->m_normalY = normalY * g_terrainStreamScaleState.m_normalScaleY;
			output->m_normalZ = normalX * g_terrainStreamScaleState.m_normalScaleX;
			if (hardwareVertexProcessing) {
				output->m_diffuse = g_terrainColorByFlags[colorFlags & 0x0f];
			}
			else {
				output->m_diffuse = ((LegoU32) colorFlags << 28) | 0x00ffffff;
			}
			output->m_textureU = streamColumn * g_terrainStreamScaleState.m_textureScaleX;
			output->m_textureV = textureV;
			vertexCursor += sizeof(TerrainStreamVertex);
			sourceVertex++;
		}
	}
	*p_vertexCursor = vertexCursor;

	RendererIndexBuffer* indexBuffer = m_streamingIndexBuffer;
	LegoU16* indices = (LegoU16*) (indexBuffer->m_data + *p_indexOffset * sizeof(LegoU16));
	LegoU16* firstNewIndex = indices;
	LegoU32 prefixCount = 0;
	if (*p_indexOffset != 0) {
		LegoU16 previousIndex = indices[-1];
		*indices++ = previousIndex;
		*indices++ = firstVertex;
		prefixCount = 2;
	}

	if (p_topology == 7) {
		for (LegoS32 indexRow = 0; indexRow < rows - 1; indexRow++) {
			if ((indexRow & 1) == 0) {
				for (LegoS32 indexColumn = 0; indexColumn < columns; indexColumn++) {
					*indices++ = firstVertex + indexRow * columns + indexColumn;
					*indices++ = firstVertex + (indexRow + 1) * columns + indexColumn;
				}
			}
			else {
				for (LegoS32 reverseColumn = columns - 1; reverseColumn >= 0; reverseColumn--) {
					*indices++ = firstVertex + indexRow * columns + reverseColumn;
					*indices++ = firstVertex + (indexRow + 1) * columns + reverseColumn;
				}
			}
			*indices = indices[-1];
			indices++;
		}
		*p_indexOffset += (LegoU32) (indices - firstNewIndex);
		*p_vertexBase += (LegoU32) (columns * rows);
		return;
	}

	LegoU16* segmentStart = indices - prefixCount;
	LegoU16* generatedStart = indices;
	LegoU32 nextTop = firstVertex;
	LegoU32 nextBottom = (LegoU16) (firstVertex + columns);
	LegoBool32 inHole = (p_grid->m_vertices[columns + 1].m_flags & 0x80) != 0;
	for (LegoS32 holeRow = 0; holeRow < rows - 1; holeRow++) {
		for (LegoS32 holeColumn = 0; holeColumn < columns - 1; holeColumn++) {
			LegoS32 flagIndex = (holeRow + 1) * columns + holeColumn + 1;
			LegoBool32 cellIsHole = (p_grid->m_vertices[flagIndex].m_flags & 0x80) != 0;
			if (cellIsHole != inHole) {
				inHole = cellIsHole;
				if (cellIsHole) {
					*indices++ = (LegoU16) nextTop;
					*indices++ = (LegoU16) nextBottom;
				}
				else if (indices > segmentStart) {
					LegoU16 previous = indices[-1];
					*indices++ = previous;
					*indices++ = (LegoU16) nextTop;
					*indices++ = (LegoU16) nextTop;
					*indices++ = (LegoU16) nextBottom;
				}
			}
			if (!inHole) {
				*indices++ = (LegoU16) nextTop;
				*indices++ = (LegoU16) nextBottom;
			}
			nextTop++;
			nextBottom++;
		}
		if (!inHole) {
			*indices++ = (LegoU16) nextTop;
			*indices++ = (LegoU16) nextBottom;
		}
		nextTop++;
		nextBottom++;
		inHole = TRUE;
	}
	if (indices > generatedStart) {
		LegoU16 previousIndex = indices[-1];
		indices[0] = previousIndex;
		indices[1] = previousIndex;
		indices += 2;
		*p_indexOffset += (LegoU32) (indices - firstNewIndex);
	}
	*p_vertexBase += (LegoU32) (columns * rows);
}

// STUB: LEGORACERS2 0x004eb9d0
void __stdcall BuildTerrainGeometry(
	Terrain* p_terrain,
	Vector3* p_outputPoint,
	const Vector3* p_inputPoints,
	LegoS32 p_inputPointCount,
	RendererTerrainGeometryPacket* p_packet,
	LegoS32 p_mode
)
{
	LegoU8* terrainBytes = (LegoU8*) p_terrain;
	RendererVertexBuffer* vertexBuffer = p_packet->m_vertexBuffer;
	RendererIndexBuffer* indexBuffer = p_packet->m_indexBuffer;
	LegoU8* indexOutput = indexBuffer->m_data;
	LegoS32 maxVertices = vertexBuffer->GetCount();
	LegoS32 vertexStride = (LegoS32) vertexBuffer->GetStride();
	p_packet->m_triangleCount = 0;
	p_packet->m_vertexCount = 0;
	LegoS32 emittedVertices = 0;
	LegoS32 emittedIndices = 0;

	Vector3 scale = p_terrain->m_scale;
	LegoS32 halfColumns = p_terrain->m_columns >> 1;
	LegoS32 halfRows = p_terrain->m_rows >> 1;
	Vector3 minimum = {2147483648.0f, 2147483648.0f, 2147483648.0f};
	Vector3 maximum = {-2147483648.0f, -2147483648.0f, -2147483648.0f};

	for (LegoS32 i = 0; i < p_inputPointCount; i++) {
		Vector3 point = p_inputPoints[i];
		TransformTerrainPoint(p_terrain, &point);
		if (point.m_x < minimum.m_x) {
			minimum.m_x = point.m_x;
		}
		if (point.m_y < minimum.m_y) {
			minimum.m_y = point.m_y;
		}
		if (point.m_z < minimum.m_z) {
			minimum.m_z = point.m_z;
		}
		if (point.m_x > maximum.m_x) {
			maximum.m_x = point.m_x;
		}
		if (point.m_y > maximum.m_y) {
			maximum.m_y = point.m_y;
		}
		if (point.m_z > maximum.m_z) {
			maximum.m_z = point.m_z;
		}
	}

	LegoFloat xScale = *(LegoFloat*) (terrainBytes + 0x1b4);
	LegoFloat zScale = *(LegoFloat*) (terrainBytes + 0x1b8);
	LegoS32 tileColumns = *(LegoS32*) (terrainBytes + 0x1bc);
	LegoS32 tileRows = *(LegoS32*) (terrainBytes + 0x1c0);
	LegoS32 tileMinColumn = GetTerrainGridCoordinate(minimum.m_x, xScale);
	LegoS32 tileMinRow = GetTerrainGridCoordinate(minimum.m_z, zScale);
	LegoS32 tileMaxColumn = RoundToNearestEven(maximum.m_x * xScale + 0.5f);
	LegoS32 tileMaxRow = RoundToNearestEven(maximum.m_z * zScale + 0.5f);
	LegoBool32 minimumInside =
		tileMinColumn >= 0 && tileMinColumn < tileColumns && tileMinRow >= 0 && tileMinRow < tileRows;
	LegoBool32 maximumInside =
		tileMaxColumn >= 0 && tileMaxColumn < tileColumns && tileMaxRow >= 0 && tileMaxRow < tileRows;
	tileMinColumn = Clamp(tileMinColumn, 0, tileColumns - 1);
	tileMinRow = Clamp(tileMinRow, 0, tileRows - 1);
	tileMaxColumn = Clamp(tileMaxColumn, 0, tileColumns - 1);
	tileMaxRow = Clamp(tileMaxRow, 0, tileRows - 1);

	if (minimumInside || maximumInside) {
		LegoFloat halfX = (maximum.m_x + minimum.m_x) * 0.5f;
		LegoFloat halfZ = (maximum.m_z + minimum.m_z) * 0.5f;
		LegoFloat tileWidth = 1.0f;
		TerrainTile* firstTile = GetTerrainTileByGridIndex(p_terrain, 0, 0);
		LegoFloat gridScaleX =
			tileWidth / (*(LegoFloat*) ((LegoU8*) firstTile + 0x30) - *(LegoFloat*) ((LegoU8*) firstTile + 0x10));
		LegoFloat gridScaleZ =
			tileWidth / (*(LegoFloat*) ((LegoU8*) firstTile + 0x58) - *(LegoFloat*) ((LegoU8*) firstTile + 0x18));
		LegoFloat verticalBias = 0.01f;

		for (LegoS32 tileRow = tileMinRow; tileRow <= tileMaxRow; tileRow++) {
			for (LegoS32 tileColumn = tileMinColumn; tileColumn <= tileMaxColumn; tileColumn++) {
				TerrainTile* tile = GetTerrainTileByGridIndex(p_terrain, tileColumn, tileRow);
				LegoS32 level = p_mode == 1 ? 0 : (LegoS8) * ((LegoU8*) tile + 0x11c);
				if (level < 0) {
					continue;
				}

				TerrainVertexGrid* grid = GetTerrainGrid(tile, level);
				LegoS32 lastColumn = grid->m_columns - 1;
				LegoS32 lastRow = grid->m_rows - 1;
				LegoFloat originX = *(LegoFloat*) ((LegoU8*) tile + 0x10);
				LegoFloat originZ = *(LegoFloat*) ((LegoU8*) tile + 0x18);
				LegoS32 firstGridColumn = RoundToNearestEven(
					((minimum.m_x - *(LegoFloat*) (terrainBytes + 0x184)) - originX) * gridScaleX * lastColumn - 0.5f
				);
				LegoS32 firstGridRow = RoundToNearestEven(
					((minimum.m_z - *(LegoFloat*) (terrainBytes + 0x18c)) - originZ) * gridScaleZ * lastRow - 0.5f
				);
				LegoS32 lastGridColumn = RoundToNearestEven(
					((maximum.m_x - *(LegoFloat*) (terrainBytes + 0x184)) - originX) * gridScaleX * lastColumn + 0.5f
				);
				LegoS32 lastGridRow = RoundToNearestEven(
					((maximum.m_z - *(LegoFloat*) (terrainBytes + 0x18c)) - originZ) * gridScaleZ * lastRow + 0.5f
				);
				firstGridColumn = firstGridColumn < 0 ? 0 : firstGridColumn;
				firstGridRow = firstGridRow < 0 ? 0 : firstGridRow;
				lastGridColumn = lastGridColumn > lastColumn ? lastColumn : lastGridColumn;
				lastGridRow = lastGridRow > lastRow ? lastRow : lastGridRow;

				LegoS32 patchColumns = lastGridColumn - firstGridColumn + 1;
				LegoS32 patchRows = lastGridRow - firstGridRow + 1;
				LegoS32 patchVertexCount = patchColumns * patchRows;
				if (patchVertexCount <= 0 || emittedVertices + patchVertexCount > maxVertices) {
					continue;
				}

				LegoU8* output = vertexBuffer->GetVertex(emittedVertices);
				vertexBuffer->SetCursor(emittedVertices);
				LegoS32 step = 1 << level;
				for (LegoS32 vertexRow = firstGridRow; vertexRow <= lastGridRow; vertexRow++) {
					for (LegoS32 column = firstGridColumn; column <= lastGridColumn; column++) {
						TerrainVertex* source = &grid->m_vertices[grid->m_columns * vertexRow + column];
						LegoFloat x =
							((LegoFloat) (column * step + ((LegoS16*) ((LegoU8*) tile + 0x0c))[0] - halfColumns)) *
							scale.m_x;
						LegoFloat y = (LegoFloat) (LegoS16) source->m_height * scale.m_y + verticalBias;
						LegoFloat z =
							((LegoFloat) (vertexRow * step + ((LegoS16*) ((LegoU8*) tile + 0x0c))[1] - halfRows)) *
							scale.m_z;
						*(LegoFloat*) (output + 0) = x;
						*(LegoFloat*) (output + 4) = y;
						*(LegoFloat*) (output + 8) = z;
						output += vertexStride;
					}
				}

				LegoS32 baseVertex = emittedVertices;
				LegoU8 tileTopology = *((LegoU8*) tile + 0x11e);
				for (LegoS32 topologyRow = 0; topologyRow + 1 < patchRows; topologyRow++) {
					for (LegoS32 column = 0; column + 1 < patchColumns; column++) {
						TerrainVertex* lower =
							&grid->m_vertices
								 [grid->m_columns * (firstGridRow + topologyRow + 1) + firstGridColumn + column];
						if ((tileTopology & 0xfe) != 0x0e && (lower->m_flags & 0x80) != 0) {
							continue;
						}
						LegoS16 a = (LegoS16) (baseVertex + topologyRow * patchColumns + column);
						LegoS16 b = (LegoS16) (a + 1);
						LegoS16 c = (LegoS16) (a + patchColumns);
						LegoS16 d = (LegoS16) (c + 1);
						LegoS16* indices = (LegoS16*) indexOutput + emittedIndices;
						if ((topologyRow & 1) == 0) {
							indices[0] = a;
							indices[1] = c;
							indices[2] = b;
							indices[3] = b;
							indices[4] = c;
							indices[5] = d;
						}
						else {
							indices[0] = a;
							indices[1] = c;
							indices[2] = d;
							indices[3] = a;
							indices[4] = d;
							indices[5] = b;
						}
						emittedIndices += 6;
					}
				}
				emittedVertices += patchVertexCount;
			}
		}

		p_packet->m_triangleCount = (LegoS16) (emittedIndices / 3);
		p_packet->m_vertexCount = (LegoS16) emittedVertices;

		LegoS32 centerColumn = Clamp(RoundToNearestEven(halfX * xScale - 0.5f), 0, tileColumns - 1);
		LegoS32 centerRow = Clamp(RoundToNearestEven(halfZ * zScale - 0.5f), 0, tileRows - 1);
		TerrainTile* centerTile = GetTerrainTileByGridIndex(p_terrain, centerColumn, centerRow);
		TerrainVertexGrid* centerGrid = GetTerrainGrid(centerTile, 0);
		LegoS32 centerX = Clamp(
			RoundToNearestEven(
				(halfX - *(LegoFloat*) (terrainBytes + 0x184) - *(LegoFloat*) ((LegoU8*) centerTile + 0x10)) *
				gridScaleX * (centerGrid->m_columns - 1)
			),
			0,
			centerGrid->m_columns - 1
		);
		LegoS32 centerZ = Clamp(
			RoundToNearestEven(
				(halfZ - *(LegoFloat*) (terrainBytes + 0x18c) - *(LegoFloat*) ((LegoU8*) centerTile + 0x18)) *
				gridScaleZ * (centerGrid->m_rows - 1)
			),
			0,
			centerGrid->m_rows - 1
		);
		TerrainVertex* centerVertex = &centerGrid->m_vertices[centerGrid->m_columns * centerZ + centerX];
		Vector3* result = (Vector3*) p_outputPoint;
		result->m_x = ((LegoFloat) (centerX + ((LegoS16*) ((LegoU8*) centerTile + 0x0c))[0] - halfColumns)) * scale.m_x;
		result->m_y = (LegoFloat) (LegoS16) centerVertex->m_height * scale.m_y + verticalBias;
		result->m_z = ((LegoFloat) (centerZ + ((LegoS16*) ((LegoU8*) centerTile + 0x0c))[1] - halfRows)) * scale.m_z;
	}
}
