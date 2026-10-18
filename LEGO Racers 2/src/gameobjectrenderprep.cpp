#include "gameobjectrenderprep.h"

#include "gameobjectterrainvisibility.h"
#include "matrix3.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "vector3.h"

#include <stddef.h>

namespace
{

struct RenderableGameObjectView {
	undefined m_opaque0x000[0x188];
	Vector3 m_position;         // 0x188
	LegoFloat m_orientation[4]; // 0x194
	LegoFloat m_primaryScale;   // 0x1a4
	LegoFloat m_nearPlane;      // 0x1a8
	LegoFloat m_farPlane;       // 0x1ac
	undefined m_opaque0x1b0[0x08];
	LegoS32 m_projectionWidth;       // 0x1b8
	LegoS32 m_projectionHeight;      // 0x1bc
	LegoU32 m_geometryUsable;        // 0x1c0
	LegoU32 m_viewportIndex;         // 0x1c4
	LegoS32 m_shadowState;           // 0x1c8
	LegoU32 m_updateProjectionRatio; // 0x1cc
	LegoFloat m_horizontalFov;       // 0x1d0
	LegoFloat m_verticalFov;         // 0x1d4
	undefined m_opaque0x1d8[0x13c];
	LegoU32 m_projectionMode;             // 0x314
	LegoFloat m_alternateProjectionScale; // 0x318
	LegoFloat m_orthographicScale;        // 0x31c
};

DECOMP_STATIC_ASSERT(offsetof(RenderableGameObjectView, m_position) == 0x188);
DECOMP_STATIC_ASSERT(offsetof(RenderableGameObjectView, m_orientation) == 0x194);
DECOMP_STATIC_ASSERT(offsetof(RenderableGameObjectView, m_projectionWidth) == 0x1b8);
DECOMP_STATIC_ASSERT(offsetof(RenderableGameObjectView, m_projectionMode) == 0x314);
DECOMP_STATIC_ASSERT(offsetof(RenderableGameObjectView, m_orthographicScale) == 0x31c);

} // namespace

// GLOBAL: LEGORACERS2 0x00549c7c
LegoFloat g_renderProjectionRatio = 1.0f;

// GLOBAL: LEGORACERS2 0x0067bafc
void* g_unk0x0067bafc = NULL;

// STUB: LEGORACERS2 0x004aa3c0
void __cdecl SetRenderProjectionRatio(LegoFloat p_scale, LegoFloat p_limit)
{
	if (!(p_scale >= p_limit)) {
		LegoFloat ratio = p_limit / p_scale;
		g_renderProjectionRatio = ratio * ratio;
	}
	else {
		g_renderProjectionRatio = 1.0f;
	}
}

// STUB: LEGORACERS2 0x004ba280
void __fastcall PrepareGameObjectForRendering(void* p_object, void*)
{
	RenderableGameObjectView* object = (RenderableGameObjectView*) p_object;
	Matrix3 orientation;
	DisplayRenderer* renderer = g_displayRenderer;
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	LegoFloat projectionScale;

	g_unk0x00679ef0 = p_object;
	UpdateGameObjectRenderGeometry(p_object, NULL);
	orientation.FUN_004a4d30(object->m_orientation);
	dispatch->m_setViewMatrix(renderer, NULL, &orientation, &object->m_position);

	if (object->m_projectionMode == 0) {
		projectionScale = object->m_orthographicScale;
	}
	else {
		projectionScale = object->m_primaryScale;
	}
	dispatch->m_setProjection(
		renderer,
		NULL,
		projectionScale,
		object->m_horizontalFov,
		object->m_verticalFov,
		object->m_nearPlane,
		object->m_farPlane,
		object->m_projectionMode
	);

	if (object->m_updateProjectionRatio != 0) {
		if (g_unk0x0067bafc != 0) {
			UpdateTerrainRegionVisibility(g_unk0x0067bafc, NULL, p_object);
		}
		if (object->m_projectionMode == 1) {
			SetRenderProjectionRatio(90.0f, 90.0f);
		}
		else {
			SetRenderProjectionRatio(object->m_primaryScale, 90.0f);
		}
	}
}
