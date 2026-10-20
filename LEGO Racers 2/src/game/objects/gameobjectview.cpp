#include "game/objects/gameobjectrenderprep.h"
#include "math/matrix3.h"
#include "math/vector3.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"

#include <math.h>
#include <stddef.h>

namespace
{

struct GameObjectViewState {
	undefined m_opaque0x000[0x188];
	Vector3 m_position;         // 0x188
	LegoFloat m_orientation[4]; // 0x194
	LegoFloat m_viewScale;      // 0x1a4
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
	Vector3 m_frontPlaneNormal;      // 0x1d8
	Vector3 m_frustumNormals[4];     // 0x1e4
	Vector3 m_backPlaneNormal;       // 0x214
	Vector3 m_frontPlanePoint;       // 0x220
	Vector3 m_frustumPoints[4];      // 0x22c
	Vector3 m_backPlanePoint;        // 0x25c
	undefined m_opaque0x268[0x08];
	Vector3 m_cullingNormals[5]; // 0x270
	undefined m_opaque0x2ac[0x0c];
	Vector3 m_cullingPoints[5]; // 0x2b8
	undefined m_opaque0x2f4[0x0c];
	LegoU32 m_cullingPlanePointer; // 0x300
	LegoU32 m_cullingPlaneCount;   // 0x304
	undefined m_opaque0x308[0x0c];
	LegoU32 m_projectionMode;             // 0x314
	LegoFloat m_alternateProjectionScale; // 0x318
	LegoFloat m_orthographicScale;        // 0x31c
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_position) == 0x188);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_projectionWidth) == 0x1b8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_frontPlaneNormal) == 0x1d8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_frustumNormals) == 0x1e4);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_frontPlanePoint) == 0x220);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_cullingNormals) == 0x270);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_cullingPoints) == 0x2b8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectViewState, m_projectionMode) == 0x314);

const LegoFloat kPi = 3.1415927410125732f;
const LegoFloat kDegreesToRadians = 0.0055555556900799274f;

} // namespace

// STUB: LEGORACERS2 0x004ba3b0
void __fastcall UpdateGameObjectRenderGeometry(void* p_object, void*)
{
	GameObjectViewState* view = (GameObjectViewState*) p_object;
	DisplayRenderer* renderer = g_displayRenderer;
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	LegoFloat halfAngle;
	LegoFloat horizontalAspect;
	LegoFloat verticalAspect;
	Matrix3 orientation;

	view->m_geometryUsable = 1;
	if (view->m_projectionHeight < view->m_projectionWidth || view->m_alternateProjectionScale == 0.0f) {
		view->m_horizontalFov = kPi * view->m_viewScale * kDegreesToRadians;
		halfAngle = tan(view->m_horizontalFov * 0.5f);
		verticalAspect = (LegoFloat) view->m_projectionHeight / (LegoFloat) view->m_projectionWidth;
		view->m_verticalFov = (LegoFloat) (atan(verticalAspect * halfAngle) * 2.0);
	}
	else {
		view->m_verticalFov = kPi * view->m_viewScale * kDegreesToRadians;
		halfAngle = tan(view->m_verticalFov * 0.5f);
		horizontalAspect = (LegoFloat) view->m_projectionWidth / (LegoFloat) view->m_projectionHeight;
		view->m_horizontalFov = (LegoFloat) (atan(horizontalAspect * halfAngle) * 2.0);
	}

	dispatch->m_selectViewport(renderer, NULL, view->m_viewportIndex);
	orientation.FUN_004a4d30(view->m_orientation);

	view->m_frontPlaneNormal.m_x = 0.0f;
	view->m_frontPlaneNormal.m_y = 0.0f;
	view->m_frontPlaneNormal.m_z = 1.0f;
	view->m_backPlaneNormal.m_x = 0.0f;
	view->m_backPlaneNormal.m_y = 0.0f;
	view->m_backPlaneNormal.m_z = -1.0f;
	view->m_frontPlanePoint.m_x = 0.0f;
	view->m_frontPlanePoint.m_y = 0.0f;
	view->m_frontPlanePoint.m_z = view->m_nearPlane;
	view->m_backPlanePoint.m_x = 0.0f;
	view->m_backPlanePoint.m_y = 0.0f;
	view->m_backPlanePoint.m_z = view->m_farPlane;

	if (view->m_projectionMode != 0) {
		LegoFloat h = view->m_verticalFov * 0.5f;
		LegoFloat v = view->m_horizontalFov * 0.5f;
		LegoFloat cosH = (LegoFloat) cos(h);
		LegoFloat sinH = (LegoFloat) sin(h);
		LegoFloat cosV = (LegoFloat) cos(v);
		LegoFloat sinV = (LegoFloat) sin(v);

		view->m_frustumNormals[0].m_x = cosH;
		view->m_frustumNormals[0].m_y = 0.0f;
		view->m_frustumNormals[0].m_z = sinH;
		view->m_frustumNormals[1].m_x = -cosH;
		view->m_frustumNormals[1].m_y = 0.0f;
		view->m_frustumNormals[1].m_z = sinH;
		view->m_frustumNormals[2].m_x = 0.0f;
		view->m_frustumNormals[2].m_y = cosV;
		view->m_frustumNormals[2].m_z = sinV;
		view->m_frustumNormals[3].m_x = 0.0f;
		view->m_frustumNormals[3].m_y = -cosV;
		view->m_frustumNormals[3].m_z = sinV;
		for (LegoU32 i = 0; i < 4; ++i) {
			view->m_frustumPoints[i].m_x = 0.0f;
			view->m_frustumPoints[i].m_y = 0.0f;
			view->m_frustumPoints[i].m_z = 0.0f;
		}
	}
	else {
		for (LegoU32 i = 0; i < 4; ++i) {
			view->m_frustumNormals[i].m_x = 0.0f;
			view->m_frustumNormals[i].m_y = 0.0f;
			view->m_frustumNormals[i].m_z = 1.0f;
			view->m_frustumPoints[i].m_x = 0.0f;
			view->m_frustumPoints[i].m_y = 0.0f;
			view->m_frustumPoints[i].m_z = view->m_nearPlane;
		}
	}

	view->m_cullingPlanePointer = (LegoU32) ((LegoU8*) view + 0x2ac);
	view->m_cullingPlaneCount = 5;
	for (LegoU32 i = 0; i < 5; ++i) {
		Vector3 normal;
		Vector3 point;
		if (i == 0) {
			normal.m_x = view->m_frontPlaneNormal.m_x;
			normal.m_y = view->m_frontPlaneNormal.m_y;
			normal.m_z = view->m_frontPlaneNormal.m_z;
			point.m_x = view->m_frontPlanePoint.m_x;
			point.m_y = view->m_frontPlanePoint.m_y;
			point.m_z = view->m_frontPlanePoint.m_z;
		}
		else {
			normal.m_x = view->m_frustumNormals[i - 1].m_x;
			normal.m_y = view->m_frustumNormals[i - 1].m_y;
			normal.m_z = view->m_frustumNormals[i - 1].m_z;
			point.m_x = view->m_frustumPoints[i - 1].m_x;
			point.m_y = view->m_frustumPoints[i - 1].m_y;
			point.m_z = view->m_frustumPoints[i - 1].m_z;
		}
		normal.FUN_004a4e00(&orientation);
		point.FUN_004a4e00(&orientation);
		point.m_x += view->m_position.m_x;
		point.m_y += view->m_position.m_y;
		point.m_z += view->m_position.m_z;
		view->m_cullingNormals[i] = normal;
		view->m_cullingPoints[i] = point;
	}
}
