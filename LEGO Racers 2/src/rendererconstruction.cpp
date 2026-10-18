#include "application.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"

#include <string.h>

// STUB: LEGORACERS2 0x004ab9e0
DisplayRenderer::DisplayRenderer() : m_vtable((const RendererDispatchView*) &g_rendererBaseDispatch)
{
	LegoU32 i;
	m_bucketedDraws.m_vtable = &g_rendererBucketedLookupDispatch;
	m_orderedDraws.m_vtable = &g_rendererOrderedLookupDispatch;
	m_deferredDraws.m_vtable = &g_rendererDeferredLookupDispatch;

	m_direct3D = NULL;
	m_device = NULL;
	m_textureQuality = 0;
	m_clockwiseCulling = 0;
	m_unk0x810 = -1;
	m_deviceMemory = 0;

	for (i = 0; i < sizeOfArray(m_viewports); ++i) {
		m_viewports[i].m_inUse = 0;
	}
	m_vertexBufferUsage = 0;
	m_foundPreferredDepthFormat = 0;
	memset(m_unk0x245e4, 0, sizeof(m_unk0x245e4));
	m_timingStatistics.m_counter0 = 0;
	m_timingStatistics.m_counter1 = 0;
	for (i = 0; i < sizeOfArray(m_timingStatistics.m_pairCounters); ++i) {
		m_timingStatistics.m_pairCounters[i][0] = 0;
		m_timingStatistics.m_pairCounters[i][1] = 0;
	}
	for (i = 0; i < sizeOfArray(m_timingStatistics.m_counterHistory) / 2; ++i) {
		m_timingStatistics.m_counterHistory[i * 2] = 0;
		m_timingStatistics.m_counterHistory[i * 2 + 1] = 0;
	}
	m_timingStatistics.m_tailCounters[0] = 0;
	m_timingStatistics.m_tailCounters[1] = 0;
	for (i = 0; i < sizeOfArray(m_timingStatistics.m_counterHistorySnapshot) / 2; ++i) {
		m_timingStatistics.m_counterHistorySnapshot[i * 2] = 0;
		m_timingStatistics.m_counterHistorySnapshot[i * 2 + 1] = 0;
	}
	for (i = 0; i < sizeOfArray(m_timingStatistics.m_pairCounterSnapshot); ++i) {
		m_timingStatistics.m_pairCounterSnapshot[i][0] = 0;
		m_timingStatistics.m_pairCounterSnapshot[i][1] = 0;
	}

	m_textureStageManager.m_unk0x3480 = 0;
	m_textureStageManager.m_unk0x3484 = 0;
	for (i = 0; i < sizeOfArray(m_vertexStorage); ++i) {
		m_vertexStorage[i].m_deviceBuffer = NULL;
		m_vertexStorage[i].m_flags &= 0xfffffff2;
		m_vertexStorage[i].m_base = NULL;
	}
	for (i = 0; i < sizeOfArray(m_indexStorage); ++i) {
		m_indexStorage[i].m_capacity = 0;
	}

	m_unk0xc6268 = 0;
	m_fillMode = 3;
	m_unk0xc6270 = 1;
	m_colorOverrideEnabled = 0;
	m_useBlendModeOverride = 0;
	for (i = 0; i < sizeOfArray(m_lights); ++i) {
		m_lights[i].m_flags = 0;
	}
	m_lightCount = 0;
	m_lightDirection[0] = 0.0f;
	m_lightDirection[1] = -1.0f;
	m_lightDirection[2] = 0.0f;
	m_textureTransformsEnabled = 0;
	for (i = 0; i < sizeOfArray(m_textureTransforms); ++i) {
		m_textureTransforms[i].m_used = 0;
	}

	m_cachedStreamBuffer = NULL;
	m_cachedVertexShader = 0;
	m_unk0xc7a24 = -1;
	m_currentDrawFlags = 0;
	m_stageBlend = 1.0f;
	for (i = 0; i < sizeOfArray(m_animationResources); ++i) {
		m_animationResources[i].Initialize();
	}
	m_unk0xc7ec0 = 0;
	m_unk0xc7ec8 = 0;
	m_unk0xc7ecc = 1;
	m_bufferStoragePolicy = 0;
	m_createDeviceBuffers = 0;
	m_streamResourceId = -1;
	m_vertexBufferCapacity = 0;
	m_normalVertexBuffer = NULL;
	m_transformedVertexBuffer = NULL;
	m_streamingIndexBuffer = NULL;
	m_quadVertexBuffer = NULL;
	m_quadIndexBuffer = NULL;
	m_streamVertexCount = 0;
	m_specularVertexBuffer = NULL;
	m_streamIndexBuffer = NULL;
	m_mappedStreamVertices = NULL;

	// The derived table is installed after the member records are initialized,
	// matching the point where retail begins using concrete renderer callbacks.
	m_vtable = (const RendererDispatchView*) &g_rendererDerivedDispatch;
	for (i = 0; i < sizeOfArray(m_lights); ++i) {
		m_lights[i].m_deviceIndex = (LegoU16) i;
	}
	for (i = 0; i < sizeOfArray(m_textureIndexMap); ++i) {
		m_textureIndexMap[i] = i;
	}
	for (i = 0; i < sizeOfArray(m_textureAddressModes); ++i) {
		m_textureAddressModes[i] = 3;
	}
	m_textureAddressModes[1] = 1;
	m_textureAddressModes[2] = 1;
	m_textureAddressModes[3] = 2;
	m_textureAddressModes[7] = 2;

	if (GetApplication()->IsDevelopmentMode()) {
		m_createDeviceBuffers = 1;
		m_unk0xc7ecc = 1;
		m_bufferStoragePolicy = 1;
	}
}
