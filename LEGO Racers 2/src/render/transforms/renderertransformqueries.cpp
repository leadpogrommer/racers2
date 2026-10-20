#include "render/transforms/renderertransformqueries.h"

#include "math/vector3.h"
#include "render/core/renderer.h"
#include "render/device/displayconfig.h"
#include "render/device/renderertimingstatistics.h"

#include <mmsystem.h>

// GLOBAL: LEGORACERS2 0x0067bc78
LegoFloat g_rendererTimingMetrics[6];

// GLOBAL: LEGORACERS2 0x0067bd5c
LegoU32 g_rendererTimingMetricsSampleTime;

// STUB: LEGORACERS2 0x004abf00
void DisplayRenderer::UpdateTransformCache()
{
	if (m_unk0x788 == 0) {
		MultiplyMatrix4(&m_transform0x67c, &m_worldTransform, &m_viewTransform);
		MultiplyMatrix4(&m_transform0x6bc, &m_transform0x67c, &m_projectionTransform);
		MultiplyMatrix4(&m_transform0x6fc, &m_transform0x6bc, &m_viewportTransform);
		m_unk0x788 = 1;
	}
}

// FUNCTION: LEGORACERS2 0x004ac080
void DisplayRenderer::GetTimingMetrics(LegoFloat p_metrics[6])
{
	m_timingStatistics.GetMetrics(p_metrics);
}

// FUNCTION: LEGORACERS2 0x004dd230
void DisplayRenderer::EndScene()
{
	m_device->EndScene();
}

// FUNCTION length: 0x21d bytes (0x004dd240-0x004dd45c).
// STUB: LEGORACERS2 0x004dd240
void RendererTimingStatisticsState::GetMetrics(LegoFloat p_metrics[6])
{
	DWORD now = timeGetTime();
	if (g_rendererTimingMetricsSampleTime == 0) {
		for (LegoS32 metric = 0; metric < 6; metric++) {
			g_rendererTimingMetrics[metric] = 0.0f;
		}
	}

	LegoU32 elapsedMilliseconds = now - g_rendererTimingMetricsSampleTime;
	if (elapsedMilliseconds > 1000) {
		if (g_rendererTimingMetricsSampleTime != 0) {
			LegoS32 elapsedCount = (LegoS32) (m_counter1 - m_previousCounter1);
			LegoS32 xDelta = (LegoS32) ((LegoU32) m_counter0 - (LegoU32) m_previousCounter0);
			g_rendererTimingMetrics[0] = (LegoFloat) elapsedCount;
			g_rendererTimingMetrics[1] = (LegoFloat) xDelta;
			g_rendererTimingMetrics[2] = (LegoFloat) xDelta;
			if (g_rendererTimingMetrics[0] > 1.0f) {
				g_rendererTimingMetrics[2] /= g_rendererTimingMetrics[0];
			}

			LegoU32 pairDelta = 0;
			for (LegoS32 pair = 0; pair < 8; pair++) {
				pairDelta += (LegoU32) m_pairCounters[pair][0] - (LegoU32) m_pairCounterSnapshot[pair][0];
			}
			if (g_rendererTimingMetrics[0] > 1.0f) {
				g_rendererTimingMetrics[3] = (LegoFloat) pairDelta / g_rendererTimingMetrics[0];
				g_rendererTimingMetrics[5] =
					(LegoFloat) ((LegoU32) m_tailCounters[0] - (LegoU32) m_tailCounterSnapshot[0]) /
					g_rendererTimingMetrics[0];
			}
			else {
				g_rendererTimingMetrics[3] = (LegoFloat) pairDelta;
				g_rendererTimingMetrics[5] =
					(LegoFloat) ((LegoU32) m_tailCounters[0] - (LegoU32) m_tailCounterSnapshot[0]);
			}
		}

		m_previousCounter0 = m_counter0;
		m_previousCounter1 = m_counter1;
		for (LegoS32 index = 0; index < 0x158; index++) {
			m_counterHistorySnapshot[index] = m_counterHistory[index];
		}
		for (LegoS32 pair = 0; pair < 8; pair++) {
			m_pairCounterSnapshot[pair][0] = m_pairCounters[pair][0];
			m_pairCounterSnapshot[pair][1] = m_pairCounters[pair][1];
		}
		m_tailCounterSnapshot[0] = m_tailCounters[0];
		m_tailCounterSnapshot[1] = m_tailCounters[1];
		g_rendererTimingMetricsSampleTime = now;
	}

	for (LegoS32 metric = 0; metric < 6; metric++) {
		p_metrics[metric] = g_rendererTimingMetrics[metric];
	}
}

// STUB: LEGORACERS2 0x004dd9f0
LegoBool32 DisplayRenderer::IsPointWithinViewport(const Vector3* p_source, Vector3* p_result)
{
	UpdateTransformCache();
	if (!(m_vtable->m_transformViewport(this, NULL, p_source, p_result) > 0.0f)) {
		return FALSE;
	}
	if (!(p_result->m_x >= 0.0f) || !(p_result->m_y >= 0.0f)) {
		return FALSE;
	}
	if (!(p_result->m_x < (LegoFloat) g_displayConfiguration->m_width) ||
		!(p_result->m_y < (LegoFloat) g_displayConfiguration->m_height)) {
		return FALSE;
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004ddb30
LegoFloat DisplayRenderer::ScaleProjectionCoordinate(LegoFloat p_scale, LegoFloat p_depth)
{
	if (!(p_depth >= m_nearClip)) {
		p_depth = m_nearClip;
	}

	LegoFloat projectionScale = m_projectionTransform.m[0][0];
	if (m_clockwiseCulling == 1) {
		projectionScale = -projectionScale;
	}

	return (projectionScale / p_depth) * p_scale;
}

// STUB: LEGORACERS2 0x004ddb70
void __stdcall TransformStaticWorldViewPoint(const Vector3* p_source, Vector3* p_result)
{
	const D3DMATRIX* matrix = &GetStaticDisplayRenderer()->m_transform0x67c;
	LegoFloat x = p_source->m_x * matrix->m[0][0] + p_source->m_y * matrix->m[1][0] + p_source->m_z * matrix->m[2][0] +
				  matrix->m[3][0];
	LegoFloat y = p_source->m_x * matrix->m[0][1] + p_source->m_y * matrix->m[1][1] + p_source->m_z * matrix->m[2][1] +
				  matrix->m[3][1];
	LegoFloat z = p_source->m_x * matrix->m[0][2] + p_source->m_y * matrix->m[1][2] + p_source->m_z * matrix->m[2][2] +
				  matrix->m[3][2];

	p_result->m_x = x;
	p_result->m_z = z;
	p_result->m_y = y;
}

static LegoFloat TransformPointByMatrix(const D3DMATRIX* p_matrix, const Vector3* p_source, Vector3* p_result)
{
	LegoFloat x = p_matrix->m[0][0] * p_source->m_x;
	x += p_matrix->m[2][0] * p_source->m_z;
	x += p_matrix->m[1][0] * p_source->m_y;
	p_result->m_x = x;

	LegoFloat y = p_matrix->m[0][1] * p_source->m_x;
	y += p_matrix->m[2][1] * p_source->m_z;
	y += p_matrix->m[1][1] * p_source->m_y;
	p_result->m_y = y;

	LegoFloat z = p_matrix->m[0][2] * p_source->m_x;
	z += p_matrix->m[2][2] * p_source->m_z;
	z += p_matrix->m[1][2] * p_source->m_y;
	p_result->m_z = z;

	p_result->m_x += p_matrix->m[3][0];
	p_result->m_y += p_matrix->m[3][1];
	p_result->m_z += p_matrix->m[3][2];

	LegoFloat homogeneous = p_matrix->m[0][3] * p_source->m_x;
	homogeneous += p_matrix->m[2][3] * p_source->m_z;
	homogeneous += p_matrix->m[1][3] * p_source->m_y;
	homogeneous += p_matrix->m[3][3];

	if (homogeneous > 0.0f) {
		LegoFloat reciprocal = 1.0f / homogeneous;
		p_result->m_x *= reciprocal;
		p_result->m_y *= reciprocal;
		p_result->m_z *= reciprocal;
		return reciprocal;
	}
	return homogeneous;
}

// STUB: LEGORACERS2 0x004ddc00
LegoFloat DisplayRenderer::TransformProjectedPoint(const Vector3* p_source, Vector3* p_result)
{
	return TransformPointByMatrix(&m_transform0x6bc, p_source, p_result);
}

// STUB: LEGORACERS2 0x004ddce0
LegoFloat DisplayRenderer::TransformViewportPoint(const Vector3* p_source, Vector3* p_result)
{
	return TransformPointByMatrix(&m_transform0x6fc, p_source, p_result);
}
