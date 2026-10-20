#include "math/quaternion.h"
#include "math/vector3.h"
#include "render/core/renderer.h"

#include <math.h>
#include <string.h>

// FUNCTION: LEGORACERS2 0x004ac040
RendererTextureRecord* DisplayRenderer::GetTextureRecord(LegoU32 p_index)
{
	return &m_textureRecords[p_index];
}

// STUB: LEGORACERS2 0x004ae360
LegoS32 DisplayRenderer::AddViewport(LegoU32 p_x, LegoU32 p_y, LegoU32 p_width, LegoU32 p_height)
{
	LegoU32 index;
	for (index = 0; index < sizeOfArray(m_viewports); index++) {
		if (m_viewports[index].m_inUse == 0) {
			break;
		}
	}

	if (index == sizeOfArray(m_viewports)) {
		return -1;
	}

	m_viewports[index].m_inUse = 1;
	m_vtable->m_updateViewport(this, NULL, index, p_x, p_y, p_width, p_height);
	return index;
}

// FUNCTION: LEGORACERS2 0x004ae3d0
void DisplayRenderer::UpdateViewport(LegoU32 p_index, LegoU32 p_x, LegoU32 p_y, LegoU32 p_width, LegoU32 p_height)
{
	RendererViewportRecord& record = m_viewports[p_index];
	memset(&record.m_viewport, 0, sizeof(record.m_viewport));
	record.m_viewport.X = p_x;
	record.m_viewport.Y = p_y;
	record.m_viewport.Width = p_width;
	record.m_viewport.Height = p_height;
	record.m_viewport.MinZ = 0.0f;
	record.m_viewport.MaxZ = 1.0f;
}

// FUNCTION: LEGORACERS2 0x004ae430
void DisplayRenderer::SelectViewport(LegoU32 p_index)
{
	RendererViewportRecord& record = m_viewports[p_index];
	g_lastDirect3DResult = m_device->SetViewport(&record.m_viewport);
	m_currentViewportIndex = p_index;
	SetProjectionFromViewport(&record.m_viewport);
}

// STUB: LEGORACERS2 0x004ae480
void DisplayRenderer::SetProjectionFromViewport(const D3DVIEWPORT8* p_viewport)
{
	m_viewportX = (LegoFloat) (LegoS32) p_viewport->X;
	m_viewportY = (LegoFloat) (LegoS32) p_viewport->Y;
	m_viewportWidth = (LegoFloat) (LegoS32) p_viewport->Width;
	m_viewportHeight = (LegoFloat) (LegoS32) p_viewport->Height;
	m_viewportHalfWidth = m_viewportWidth * 0.5f;
	m_viewportHalfHeight = m_viewportHeight * 0.5f;
	m_viewportRight = m_viewportWidth + m_viewportX;
	m_viewportBottom = m_viewportHeight + m_viewportY;

	memset(&m_viewportTransform, 0, sizeof(m_viewportTransform));
	m_viewportTransform.m[0][0] = m_viewportHalfWidth;
	m_viewportTransform.m[1][1] = -m_viewportHalfHeight;
	m_viewportTransform.m[2][2] = 1.0f;
	m_viewportTransform.m[3][0] = m_viewportX + m_viewportHalfWidth;
	m_viewportTransform.m[3][1] = m_viewportY + m_viewportHalfHeight;
	m_viewportTransform.m[3][3] = 1.0f;
}

static void SetRendererLightEnabled(RendererLightRecord& p_light, IDirect3DDevice8* p_device, LegoBool32 p_enabled)
{
	if (p_enabled != FALSE) {
		if ((p_light.m_flags & 8) == 0) {
			p_device->LightEnable(p_light.m_deviceIndex, TRUE);
			p_light.m_flags |= 8;
		}
	}
	else if ((p_light.m_flags & 8) != 0) {
		p_device->LightEnable(p_light.m_deviceIndex, FALSE);
		p_light.m_flags &= (LegoU16) ~8;
	}
}

// STUB: LEGORACERS2 0x004bf150
void DisplayRenderer::SelectLights(
	const Vector3* p_center,
	LegoFloat p_radius,
	LegoU32 p_mode,
	LegoU32* p_maskAndDirection
)
{
	if (p_maskAndDirection[0] != 0xffffffff) {
		for (LegoS32 index = 0; index < m_lightCount; index++) {
			RendererLightRecord& light = m_lights[index];
			LegoBool32 enabled = (p_maskAndDirection[0] & (1u << (index & 31))) != 0 ? TRUE : FALSE;
			if (enabled != FALSE) {
				light.m_flags |= 4;
			}
			else {
				light.m_flags &= (LegoU16) ~4;
			}
			SetRendererLightEnabled(light, m_device, enabled);
		}
		memcpy(m_lightDirection, p_maskAndDirection + 1, sizeof(m_lightDirection));
		return;
	}

	p_maskAndDirection[0] = 0;
	volatile LegoFloat accumulatedX = 0.0f;
	volatile LegoFloat accumulatedY = 0.0f;
	volatile LegoFloat accumulatedZ = 0.0f;
	LegoS32 selectedCount = 0;
	LegoS32 candidateCount = 0;
	LegoS32 candidates[32];

	for (LegoS32 index = 0; index < m_lightCount; index++) {
		RendererLightRecord& light = m_lights[index];
		if ((light.m_flags & 2) == 0) {
			continue;
		}

		if (light.m_radius == 2147483648.0f) {
			if (p_mode == 1) {
				accumulatedX = accumulatedX + light.m_light.Direction.x;
				accumulatedY = accumulatedY + light.m_light.Direction.y;
				accumulatedZ = accumulatedZ + light.m_light.Direction.z;
			}
			p_maskAndDirection[0] |= 1u << (index & 31);
			light.m_flags |= 4;
			SetRendererLightEnabled(light, m_device, TRUE);
			selectedCount++;
			continue;
		}

		Vector3 delta;
		delta.m_x = p_center->m_x - light.m_light.Position.x;
		delta.m_y = p_center->m_y - light.m_light.Position.y;
		delta.m_z = p_center->m_z - light.m_light.Position.z;
		LegoFloat distanceSquared = delta.m_x * delta.m_x + delta.m_y * delta.m_y + delta.m_z * delta.m_z;
		LegoFloat expandedRadius = light.m_radius + p_radius;
		LegoFloat expandedRadiusSquared = expandedRadius * expandedRadius;

		if (expandedRadiusSquared <= distanceSquared) {
			light.m_flags &= (LegoU16) ~4;
			SetRendererLightEnabled(light, m_device, FALSE);
		}
		else {
			if (p_mode == 1 && distanceSquared > 0.001f) {
				LegoFloat directionScale = 1.0f - distanceSquared / expandedRadiusSquared;
				if (light.m_light.Type == D3DLIGHT_SPOT) {
					directionScale *= 0.25f;
				}
				delta.Normalize();
				accumulatedX = accumulatedX + delta.m_x * directionScale;
				accumulatedY = accumulatedY + delta.m_y * directionScale;
				accumulatedZ = accumulatedZ + delta.m_z * directionScale;
			}

			if (candidateCount < sizeOfArray(candidates)) {
				candidates[candidateCount++] = index;
				p_maskAndDirection[0] |= 1u << (index & 31);
				light.m_flags |= 4;
				SetRendererLightEnabled(light, m_device, TRUE);
				selectedCount++;
			}
			else {
				light.m_flags &= (LegoU16) ~4;
				SetRendererLightEnabled(light, m_device, FALSE);
			}
		}
	}

	if (p_mode == 1) {
		if (fabsf(accumulatedX) <= 0.0001f && fabsf(accumulatedY) <= 0.0001f && fabsf(accumulatedZ) <= 0.0001f) {
			accumulatedX = 0.35f;
			accumulatedY = -1.0f;
			accumulatedZ = 0.35f;
		}

		Vector3 direction;
		direction.m_x = accumulatedX;
		direction.m_y = accumulatedY;
		direction.m_z = accumulatedZ;
		direction.Normalize();
		memcpy(p_maskAndDirection + 1, &direction, sizeof(direction));
		memcpy(m_lightDirection, &direction, sizeof(direction));
	}

	while (selectedCount > 4) {
		LegoS32 farthestPosition = -1;
		LegoFloat farthestDistanceSquared = 0.0f;
		for (LegoS32 candidatePosition = 0; candidatePosition < candidateCount; candidatePosition++) {
			const LegoS32 lightIndex = candidates[candidatePosition];
			const D3DVECTOR& lightPosition = m_lights[lightIndex].m_light.Position;
			const LegoFloat dx = p_center->m_x - lightPosition.x;
			const LegoFloat dy = p_center->m_y - lightPosition.y;
			const LegoFloat dz = p_center->m_z - lightPosition.z;
			const LegoFloat distanceSquared = dx * dx + dy * dy + dz * dz;
			if (farthestDistanceSquared < distanceSquared) {
				farthestPosition = candidatePosition;
				farthestDistanceSquared = distanceSquared;
			}
		}

		if (farthestPosition == -1) {
			return;
		}

		const LegoS32 lightIndex = candidates[farthestPosition];
		p_maskAndDirection[0] &= ~(1u << (lightIndex & 31));
		RendererLightRecord& light = m_lights[lightIndex];
		light.m_flags &= (LegoU16) ~4;
		SetRendererLightEnabled(light, m_device, FALSE);
		selectedCount--;
		candidateCount--;
		for (LegoS32 moveIndex = farthestPosition; moveIndex < candidateCount; moveIndex++) {
			candidates[moveIndex] = candidates[moveIndex + 1];
		}
	}
}

// STUB: LEGORACERS2 0x004ddf30
void DisplayRenderer::SetWorldTransform(const Quaternion* p_rotation, const Vector3* p_position)
{
	LegoFloat x = p_rotation->m_x;
	LegoFloat y = p_rotation->m_y;
	LegoFloat z = p_rotation->m_z;
	LegoFloat w = p_rotation->m_w;
	// Retail spills these values to float temporaries before building the
	// matrix. Keep those 32-bit rounding points explicit; the unspilled 2xy and
	// 2wz terms below follow the x87 expression path directly.
	volatile LegoFloat twiceXX = (x * x) + (x * x);
	volatile LegoFloat twiceYY = (y * y) + (y * y);
	volatile LegoFloat twiceZZ = (z * z) + (z * z);
	volatile LegoFloat twiceWX = (w + w) * x;
	volatile LegoFloat twiceWY = (w + w) * y;
	volatile LegoFloat twiceXZ = (x * z) + (x * z);
	volatile LegoFloat twiceYZ = (y * z) + (y * z);
	LegoFloat twiceXY = (x * y) + (x * y);
	LegoFloat twiceWZ = (w + w) * z;

	m_worldTransform.m[0][0] = 1.0f - twiceYY - twiceZZ;
	m_worldTransform.m[0][1] = twiceXY + twiceWZ;
	m_worldTransform.m[0][2] = twiceXZ - twiceWY;
	m_worldTransform.m[0][3] = 0.0f;
	m_worldTransform.m[1][0] = twiceXY - twiceWZ;
	m_worldTransform.m[1][1] = 1.0f - twiceXX - twiceZZ;
	m_worldTransform.m[1][2] = twiceYZ + twiceWX;
	m_worldTransform.m[1][3] = 0.0f;
	m_worldTransform.m[2][0] = twiceXZ + twiceWY;
	m_worldTransform.m[2][1] = twiceYZ - twiceWX;
	m_worldTransform.m[2][2] = 1.0f - twiceXX - twiceYY;
	m_worldTransform.m[2][3] = 0.0f;
	m_worldTransform.m[3][0] = p_position->m_x;
	m_worldTransform.m[3][1] = p_position->m_y;
	m_worldTransform.m[3][2] = p_position->m_z;
	m_worldTransform.m[3][3] = 1.0f;

	m_device->SetTransform(D3DTS_WORLD, &m_worldTransform);
	m_unk0x788 = 0;
}
