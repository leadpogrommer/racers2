#include "math/matrix3.h"
#include "math/vector3.h"
#include "render/core/renderer.h"
#include "render/device/displayconfig.h"

#include <math.h>
#include <string.h>

// STUB: LEGORACERS2 0x004de080
void DisplayRenderer::SetWorldTransform(const Matrix3* p_rotation, const Vector3* p_position)
{
	for (LegoU32 row = 0; row < 3; row++) {
		for (LegoU32 column = 0; column < 3; column++) {
			m_worldTransform.m[row][column] = p_rotation->m_values[column][row];
		}
		m_worldTransform.m[row][3] = 0.0f;
	}

	m_worldTransform.m[3][0] = p_position->m_x;
	m_worldTransform.m[3][1] = p_position->m_y;
	m_worldTransform.m[3][2] = p_position->m_z;
	m_worldTransform.m[3][3] = 1.0f;
	m_device->SetTransform(D3DTS_WORLD, &m_worldTransform);
	m_unk0x788 = 0;
}

// STUB: LEGORACERS2 0x004de160
void DisplayRenderer::SetViewTransform(const Matrix3* p_rotation, const Vector3* p_position)
{
	LegoS32 row;
	LegoS32 column;
	Matrix3 rotation;
	rotation.FUN_004a5040(p_rotation);

	Vector3 inversePosition;
	inversePosition.m_x = -p_position->m_x;
	inversePosition.m_y = -p_position->m_y;
	inversePosition.m_z = -p_position->m_z;
	inversePosition.FUN_004a4e00(&rotation);

	memset(&m_viewTransform, 0, sizeof(m_viewTransform));
	for (row = 0; row < 3; row++) {
		for (column = 0; column < 3; column++) {
			m_viewTransform.m[row][column] = rotation.m_values[column][row];
		}
	}
	m_viewTransform.m[3][0] = inversePosition.m_x;
	m_viewTransform.m[3][1] = inversePosition.m_y;
	m_viewTransform.m[3][2] = inversePosition.m_z;
	m_viewTransform.m[3][3] = 1.0f;
	m_device->SetTransform(D3DTS_VIEW, &m_viewTransform);

	memset(&m_transform0x53c, 0, sizeof(m_transform0x53c));
	for (row = 0; row < 3; row++) {
		for (column = 0; column < 3; column++) {
			m_transform0x53c.m[row][column] = p_rotation->m_values[column][row];
		}
	}
	m_transform0x53c.m[3][3] = 1.0f;

	const LegoFloat halfPi = 3.1415927410125732f * 0.5f;
	const LegoFloat angles[3] = {0.0f, halfPi, 0.0f};
	Matrix3 eulerRotation;
	eulerRotation.FUN_004a4f60(angles);
	D3DMATRIX eulerTransform;
	memset(&eulerTransform, 0, sizeof(eulerTransform));
	for (row = 0; row < 3; row++) {
		for (column = 0; column < 3; column++) {
			eulerTransform.m[row][column] = eulerRotation.m_values[row][column];
		}
	}
	eulerTransform.m[3][3] = 1.0f;
	D3DMATRIX product;
	MultiplyMatrix4(&product, &m_transform0x53c, &eulerTransform);
	m_transform0x53c = product;

	D3DMATRIX yReflection;
	memset(&yReflection, 0, sizeof(yReflection));
	yReflection.m[0][0] = 1.0f;
	yReflection.m[1][1] = -1.0f;
	yReflection.m[2][2] = 1.0f;
	yReflection.m[3][3] = 1.0f;
	MultiplyMatrix4(&product, &m_transform0x53c, &yReflection);
	m_transform0x53c = product;
}

// STUB: LEGORACERS2 0x004de2e0
void DisplayRenderer::SetProjection(
	LegoFloat p_orthographicScale,
	LegoFloat p_horizontalFov,
	LegoFloat p_verticalFov,
	LegoFloat p_nearPlane,
	LegoFloat p_farPlane,
	LegoU32 p_mode
)
{
	memset(&m_projectionTransform, 0, sizeof(m_projectionTransform));
	m_nearClip = p_nearPlane;
	m_clearQuadDepth = p_farPlane;

	if (p_mode == 0) {
		m_projectionTransform.m[0][0] = g_displayConfiguration->m_heightToWidthRatio * p_orthographicScale;
		m_projectionTransform.m[1][1] = p_orthographicScale;
		m_projectionTransform.m[2][2] = 0.0001f;
		m_projectionTransform.m[3][3] = 1.0f;
	}
	else {
		const LegoFloat depthRange = p_farPlane / (p_farPlane - p_nearPlane);
		const LegoFloat horizontalHalfAngle = p_horizontalFov * 0.5f;
		const LegoFloat verticalHalfAngle = p_verticalFov * 0.5f;
		m_projectionTransform.m[0][0] = (LegoFloat) (cos(horizontalHalfAngle) / sin(horizontalHalfAngle));
		m_projectionTransform.m[1][1] = (LegoFloat) (cos(verticalHalfAngle) / sin(verticalHalfAngle));
		m_projectionTransform.m[2][2] = depthRange;
		m_projectionTransform.m[2][3] = 1.0f;
		m_projectionTransform.m[3][2] = -(depthRange * p_nearPlane);
	}

	SetCachedRenderState(D3DRS_CULLMODE, 3 - (m_clockwiseCulling != 0));
	m_projectionPixelScale = (LegoFloat) (LegoS32) g_displayConfiguration->m_width * 0.0015625000232830644f * 400.0f *
							 m_projectionTransform.m[0][0];
	if (m_clockwiseCulling == 1) {
		m_projectionTransform.m[0][0] *= -1.0f;
	}
	m_device->SetTransform(D3DTS_PROJECTION, &m_projectionTransform);
}
