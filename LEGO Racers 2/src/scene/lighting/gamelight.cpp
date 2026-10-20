#include "scene/lighting/gamelight.h"

#include "render/core/renderer.h"

#include <string.h>

// STUB: LEGORACERS2 0x004bed30
void GameLightProperties::SetEnabled(LegoS32 p_enabled)
{
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	RendererLightRecord* record = renderer->GetLightRecord(m_lightIndex);
	if (p_enabled == 1) {
		if ((record->m_flags & 8) == 0) {
			renderer->m_device->LightEnable(record->m_deviceIndex, TRUE);
			record->m_flags |= 8;
		}
		record->m_flags |= 2;
		m_enabled = 1;
	}
	else {
		if ((record->m_flags & 8) != 0) {
			renderer->m_device->LightEnable(record->m_deviceIndex, FALSE);
			record->m_flags &= ~8;
		}
		record->m_flags &= ~2;
		m_enabled = p_enabled;
	}
}

// STUB: LEGORACERS2 0x004bedb0
void GameAmbientLight::ApplyToRenderer()
{
	LegoFloat color[4];
	for (LegoU32 index = 0; index < 4; ++index) {
		color[index] = (LegoFloat) m_color[index] * 0.003921568859368563f;
	}
	GetStaticDisplayRenderer()->SetAmbientColorFloats(color);
}

// STUB: LEGORACERS2 0x004beef0
void __cdecl CopyGameLightProperties(D3DLIGHT8* p_light, const GameLightProperties* p_properties)
{
	memset(p_light, 0, sizeof(*p_light));
	p_light->Ambient.r = p_properties->m_ambient.r;
	p_light->Ambient.g = p_properties->m_ambient.g;
	p_light->Ambient.b = p_properties->m_ambient.b;
	p_light->Diffuse.r = p_properties->m_diffuse.r;
	p_light->Diffuse.g = p_properties->m_diffuse.g;
	p_light->Diffuse.b = p_properties->m_diffuse.b;
	p_light->Specular.r = p_properties->m_specular.r;
	p_light->Specular.g = p_properties->m_specular.g;
	p_light->Specular.b = p_properties->m_specular.b;
}

// STUB: LEGORACERS2 0x004bef60
void GameDirectionalLight::UpdateRendererLight()
{
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	D3DLIGHT8* light;
	renderer->GetLightData(m_properties.m_lightIndex, &light);
	CopyGameLightProperties(light, this == NULL ? NULL : &m_properties);
	light->Type = D3DLIGHT_DIRECTIONAL;
	Vector3 position = {0, 0, 0};
	renderer->UpdateLightPose(m_properties.m_lightIndex, &position, &m_direction, 2147483648.0f);
}
