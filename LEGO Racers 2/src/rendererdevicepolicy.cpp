#include "displayconfig.h"
#include "renderer.h"
#include "rendererquaddraw.h"

namespace
{
void ApplySelectedDisplayMode(DisplayRenderer* p_renderer)
{
	p_renderer->m_presentationParameters.BackBufferWidth = p_renderer->m_modeWidth;
	p_renderer->m_presentationParameters.BackBufferHeight = p_renderer->m_modeHeight;
	p_renderer->m_presentationParameters.BackBufferFormat = p_renderer->m_modeFormat;
	p_renderer->SelectDepthStencilFormat();
	g_displayConfiguration->m_width = p_renderer->m_modeWidth;
	g_displayConfiguration->m_height = p_renderer->m_modeHeight;
	g_displayConfiguration->m_bitDepth = p_renderer->m_requestedBitDepth;
	// Preserve the fullscreen flag across the dimensions update.
	g_displayConfiguration->m_heightToWidthRatio =
		(LegoFloat) (LegoS32) p_renderer->m_modeHeight / (LegoFloat) (LegoS32) p_renderer->m_modeWidth;
	p_renderer->m_vtable->m_updateViewport(
		p_renderer,
		NULL,
		p_renderer->m_clearRenderTarget,
		0,
		0,
		p_renderer->m_modeWidth,
		p_renderer->m_modeHeight
	);
}

LegoBool32 InvokeDeviceFallback(DisplayRenderer* p_renderer)
{
	RendererDispatchView::Policy fallback = *(RendererDispatchView::Policy*) ((LegoU8*) p_renderer->m_vtable + 0x18c);
	return fallback(p_renderer, NULL);
}
} // namespace

// STUB: LEGORACERS2 0x004ad640
LegoBool32 DisplayRenderer::ReconfigureDisplayMode(
	LegoU32 p_width,
	LegoU32 p_height,
	LegoU32 p_bitDepth,
	LegoS32 p_refreshRate
)
{
	if (g_displayConfiguration->m_fullscreen != 1) {
		return TRUE;
	}
	while (p_refreshRate != -1) {
		if (FindBestDisplayMode(&p_width, &p_height, &p_bitDepth, (LegoU32*) &p_refreshRate) == 1) {
			ApplySelectedDisplayMode(this);
			break;
		}
		if (p_refreshRate == 0) {
			p_refreshRate = -1;
			break;
		}
		if (p_refreshRate > 0) {
			p_refreshRate = 0;
		}
	}
	ReleaseDeviceResources();
	for (;;) {
		if (m_device->TestCooperativeLevel() == D3DERR_DEVICELOST) {
			return TRUE;
		}
		m_device->Reset(&m_presentationParameters);
		m_device->TestCooperativeLevel();
		InitializeDefaultRenderStates();

		RendererGuiRectangle rectangle;
		rectangle.m_left = 0.375f;
		rectangle.m_top = 0.47f;
		rectangle.m_right = 0.625f;
		rectangle.m_bottom = 0.53f;
		rectangle.m_clipLeft = 0.0f;
		rectangle.m_clipTop = 0.0f;
		rectangle.m_clipRight = 1.0f;
		rectangle.m_clipBottom = 1.0f;
		rectangle.m_color = 0xffffffff;
		rectangle.m_textureIndex = m_unk0x810;
		rectangle.m_textureLeft = 0.0f;
		rectangle.m_textureTop = 1.0f;
		rectangle.m_textureRight = 1.0f;
		rectangle.m_textureBottom = 0.0f;
		rectangle.m_rotation = 0.0f;
		rectangle.m_blendMode = 0;
		for (LegoU32 index = 0; index < sizeOfArray(rectangle.m_opaque0x48); index++) {
			rectangle.m_opaque0x48[index] = 0;
		}
		RendererDefaultRecord defaults;
		defaults.m_unk0x00 = 1;
		defaults.m_unk0x04 = 1;
		defaults.m_unk0x06 = 0;
		defaults.m_unk0x07 = 1;
		defaults.m_slots[0].m_value = 0;
		defaults.m_slots[0].m_mask = (LegoU16) m_unk0x810;
		defaults.m_slots[0].m_kind = 0;
		defaults.m_slots[0].m_flags = 0;

		if (m_unk0x810 != -1 && (m_textureRecords[m_unk0x810].m_flags0x06 & 1) != 0) {
			CreateSecondaryTexture(&m_textureRecords[m_unk0x810]);
			m_textureRecords[m_unk0x810].UpdateGpuTexture();
		}
		for (LegoU32 frame = 0; frame < 4; frame++) {
			m_vtable->m_clearBuffers(this, NULL, 7);
			if (m_unk0x810 != -1) {
				m_vtable->m_action0x124(this, NULL, 9);
				m_vtable->m_drawGuiRectangle(this, NULL, &rectangle, &defaults);
				m_vtable->m_action0x124(this, NULL, 10);
			}
			m_vtable->m_present(this, NULL);
		}
		if (m_unk0x810 != -1) {
			RendererTextureRecord& record = m_textureRecords[m_unk0x810];
			if ((record.m_flags0x06 & 1) != 0 && record.m_texture != NULL) {
				record.m_texture->Release();
				record.m_texture = NULL;
			}
		}
		ReapplyLights();
		CreateVertexBuffers();
		if (RestoreDeviceResources() == 1) {
			break;
		}
		ReleaseDeviceResources();
		if (InvokeDeviceFallback(this) == 0) {
			break;
		}
	}
	InitializeDefaultRenderStates();
	return m_vtable->m_textureQuality(this, NULL) != 8;
}

// FUNCTION: LEGORACERS2 0x004c06d0
RendererAuxiliaryResourceRecord* DisplayRenderer::GetAuxiliaryResource(LegoS32 p_index)
{
	return m_auxiliaryResources + p_index;
}

// STUB: LEGORACERS2 0x004dbab0
LegoBool32 DisplayRenderer::SetDeviceResourceMode(LegoU32 p_mode)
{
	if (m_createDeviceBuffers == 0) {
		if (p_mode == 0) {
			ReleaseDeviceResources();
			m_unk0xc7ecc = 1;
			m_bufferStoragePolicy = 0;
		}
		else if (p_mode == 1) {
			m_unk0xc7ecc = 1;
			m_bufferStoragePolicy = 1;
			return TRUE;
		}
		else if (p_mode == 2) {
			if (m_bufferStoragePolicy == 1) {
				ReleaseDeviceResources();
			}
			RefreshTextureFormats();
			CreateVertexBuffers();
			LegoBool32 restored = RestoreDeviceResources();
			for (;;) {
				if (restored != FALSE) {
					m_bufferStoragePolicy = 1;
					m_unk0xc7ecc = 0;
					return TRUE;
				}
				ReleaseDeviceResources();
				if (InvokeDeviceFallback(this) == 1) {
					if (m_device->TestCooperativeLevel() == D3DERR_DEVICELOST) {
						return TRUE;
					}
					m_device->Reset(&m_presentationParameters);
					m_device->TestCooperativeLevel();
					ReapplyLights();
					InitializeDefaultRenderStates();
				}
				if (m_vtable->m_textureQuality(this, NULL) == 8) {
					return FALSE;
				}
				CreateVertexBuffers();
				restored = RestoreDeviceResources();
			}
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004dc250
LegoBool32 DisplayRenderer::AdvanceDeviceFallback()
{
	switch (m_textureQuality) {
	case 0:
		m_textureQuality = 1;
	case 1:
		m_textureQuality = 2;
	case 2:
		if (m_presentationParameters.MultiSampleType != D3DMULTISAMPLE_NONE) {
			m_presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			return TRUE;
		}
		m_textureQuality = 3;
	case 3:
		if (m_presentationParameters.BackBufferCount > 1) {
			m_presentationParameters.BackBufferCount = 1;
			return TRUE;
		}
		m_textureQuality = 4;
	case 4:
		SetTextureQuality(5);
		return FALSE;
	case 5:
		m_textureQuality = 6;
	case 6:
		break;
	case 7:
		m_textureQuality = 8;
	default:
		return FALSE;
	}

	if (g_displayConfiguration->m_fullscreen == 1) {
		LegoU32 bitDepth = m_requestedBitDepth;
		LegoU32 refresh = m_modeRefreshRate;
		LegoU32 width = m_modeWidth - 1;
		LegoU32 height = m_modeHeight - 1;
		if ((LegoS32) width > 640 && (LegoS32) height > 480) {
			while ((LegoS32) refresh != -1) {
				if (FindBestDisplayMode(&width, &height, &bitDepth, &refresh) == 1) {
					ApplySelectedDisplayMode(this);
					return TRUE;
				}
				if (refresh == 0) {
					break;
				}
				if ((LegoS32) refresh > 0) {
					refresh = 0;
				}
			}
		}
	}
	m_textureQuality = 7;
	for (LegoS32 index = 0; index < 800; index++) {
		if ((m_textureRecords[index].m_flags0x06 & 1) != 0) {
			m_textureRecords[index].DropTopMip();
		}
	}
	return FALSE;
}
