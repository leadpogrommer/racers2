#include "application.h"
#include "nativewindow.h"
#include "renderer.h"

// STUB: LEGORACERS2 0x004bf6e0
void DisplayRenderer::ReapplyLights()
{
	m_device->LightEnable(31, TRUE);
	m_device->LightEnable(31, FALSE);

	for (LegoS32 index = 0; index < m_lightCount; index++) {
		RendererLightRecord& light = m_lights[index];
		LegoU32 enabled = 0;
		if ((light.m_flags & 1) != 0) {
			m_device->SetLight(light.m_deviceIndex, &light.m_light);
			if ((light.m_flags & 2) != 0) {
				light.m_flags |= 4;
				if ((light.m_flags & 8) == 0) {
					GetStaticDisplayRenderer()->m_device->LightEnable(light.m_deviceIndex, TRUE);
					light.m_flags |= 8;
				}
				enabled = (light.m_flags >> 1) & 1;
			}
		}
		m_device->LightEnable(light.m_deviceIndex, enabled);
	}
}

// STUB: LEGORACERS2 0x004dd130
LegoBool32 DisplayRenderer::BeginScene()
{
	if (m_bufferStoragePolicy == 0) {
		return FALSE;
	}

	Application* application = GetApplication();
	if (application->m_vtable->m_value0x94(application, NULL) == 0) {
		if (GetActiveWindow() != g_mainWindow || IsIconic(g_mainWindow) != FALSE) {
			return FALSE;
		}
	}

	HRESULT cooperativeResult = m_device->TestCooperativeLevel();
	if (cooperativeResult == D3DERR_DEVICELOST) {
		return FALSE;
	}
	if (cooperativeResult == D3DERR_DEVICENOTRESET) {
		ReleaseDeviceResources();
		if (m_device->Reset(&m_presentationParameters) == D3D_OK) {
			ReapplyLights();
			CreateVertexBuffers();
			RestoreDeviceResources();
		}
		m_device->TestCooperativeLevel();
		InitializeDefaultRenderStates();
	}

	if (m_device->BeginScene() != D3D_OK) {
		return FALSE;
	}

	if (m_unk0x4d0 < 4 && m_vtable->m_usesHardwareVertexProcessing(this, NULL) != 1) {
		m_unk0xc6270 = 0;
	}
	else {
		m_unk0xc6270 = 1;
	}
	RestoreTextureStageDefaults();
	return TRUE;
}
