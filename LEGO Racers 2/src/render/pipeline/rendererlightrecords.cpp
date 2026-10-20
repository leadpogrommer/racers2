#include "math/vector3.h"
#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004bf7a0
void DisplayRenderer::GetLightData(LegoS32 p_index, D3DLIGHT8** p_light)
{
	*p_light = &m_lights[p_index].m_light;
}

// STUB: LEGORACERS2 0x004bf7c0
void DisplayRenderer::UpdateLightPose(
	LegoS32 p_index,
	const Vector3* p_position,
	const Vector3* p_direction,
	LegoFloat p_radius
)
{
	RendererLightRecord* record = &m_lights[p_index];
	record->m_radius = p_radius;
	record->m_light.Position.x = p_position->m_x;
	record->m_light.Position.y = p_position->m_y;
	record->m_light.Position.z = p_position->m_z;
	record->m_light.Direction.x = p_direction->m_x;
	record->m_light.Direction.y = p_direction->m_y;
	record->m_light.Direction.z = p_direction->m_z;
	m_device->SetLight(p_index, &record->m_light);
}

// FUNCTION: LEGORACERS2 0x004bf840
RendererLightRecord* DisplayRenderer::GetLightRecord(LegoS32 p_index)
{
	return &m_lights[p_index];
}
