#include "render/pipeline/renderertexturemanager.h"

#include "render/core/renderer.h"

#include <string.h>

static LegoBool32 SupportsTextureOperation(const D3DCAPS8& p_caps, LegoU32 p_operation)
{
	LegoU32 operationMask = 0;
	if (p_operation >= 1 && p_operation <= 24) {
		operationMask = 1 << (p_operation - 1);
	}
	return (p_caps.TextureOpCaps & operationMask) != 0 ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004e1960
void RendererTextureStageManager::Initialize()
{
	memset(m_recordIndexCache, 0xff, sizeof(m_recordIndexCache));

	for (LegoU32 i = 0; i < sizeOfArray(m_stageRoutes); i++) {
		m_stageRoutes[i].m_enabled = 0;
		m_stageRoutes[i].m_priority = 1;
	}

	m_stageRoutes[0].m_priority = 1;
	m_stageRoutes[5].m_priority = 2;
	m_stageRoutes[7].m_priority = 3;
	m_stageRoutes[9].m_priority = 4;
	m_stageRoutes[4].m_priority = 5;
	m_stageRoutes[1].m_priority = 6;
	m_stageRoutes[6].m_priority = 7;
	m_stageRoutes[2].m_priority = 8;
	m_stageRoutes[3].m_priority = 9;
	m_stageRoutes[0].m_enabled = 1;

	m_operationProfiles[0].m_colorOperation = 4;
	m_operationProfiles[0].m_colorArgument1 = 2;
	m_operationProfiles[0].m_colorArgument2 = 0;
	m_operationProfiles[0].m_alphaOperation = 4;
	m_operationProfiles[0].m_alphaArgument1 = 2;
	m_operationProfiles[0].m_alphaArgument2 = 0;
	m_operationProfiles[0].m_unk0x18 = 0;

	DisplayRenderer* renderer = (DisplayRenderer*) ((BYTE*) this - offsetof(DisplayRenderer, m_textureStageManager));
	if ((renderer->m_selectedAdapter->m_caps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) == 0) {
		m_operationProfiles[4].m_alphaOperation = 3;
		m_operationProfiles[4].m_colorOperation = 7;
		m_operationProfiles[4].m_colorArgument1 = 2;
		m_operationProfiles[4].m_colorArgument2 = 1;
		m_operationProfiles[4].m_alphaArgument1 = 2;
		m_operationProfiles[4].m_alphaArgument2 = 1;
		m_operationProfiles[4].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(4)) {
			m_stageRoutes[4].m_enabled = 1;
		}

		m_operationProfiles[8].m_colorOperation = 10;
		m_operationProfiles[8].m_colorArgument1 = 18;
		m_operationProfiles[8].m_colorArgument2 = 17;
		m_operationProfiles[8].m_alphaOperation = 3;
		m_operationProfiles[8].m_alphaArgument1 = 2;
		m_operationProfiles[8].m_alphaArgument2 = 1;
		m_operationProfiles[8].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(8)) {
			m_stageRoutes[8].m_enabled = 1;
		}

		m_operationProfiles[1].m_colorOperation = 11;
		m_operationProfiles[1].m_colorArgument1 = 2;
		m_operationProfiles[1].m_colorArgument2 = 1;
		m_operationProfiles[1].m_alphaOperation = 3;
		m_operationProfiles[1].m_alphaArgument1 = 2;
		m_operationProfiles[1].m_alphaArgument2 = 1;
		m_operationProfiles[1].m_unk0x18 = 2;
		if (!AreTextureOperationsSupported(1)) {
			m_operationProfiles[1].m_colorOperation = 7;
		}
		if (AreTextureOperationsSupported(1)) {
			m_stageRoutes[1].m_enabled = 1;
		}

		m_operationProfiles[2].m_colorOperation = 18;
		m_operationProfiles[2].m_colorArgument1 = 2;
		m_operationProfiles[2].m_colorArgument2 = 1;
		m_operationProfiles[2].m_alphaOperation = 3;
		m_operationProfiles[2].m_alphaArgument1 = 2;
		m_operationProfiles[2].m_alphaArgument2 = 1;
		m_operationProfiles[2].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(2)) {
			m_stageRoutes[2].m_enabled = 1;
		}

		m_operationProfiles[5].m_colorOperation = 4;
		m_operationProfiles[5].m_colorArgument1 = 2;
		m_operationProfiles[5].m_colorArgument2 = 1;
		m_operationProfiles[5].m_alphaOperation = 3;
		m_operationProfiles[5].m_alphaArgument1 = 2;
		m_operationProfiles[5].m_alphaArgument2 = 1;
		m_operationProfiles[5].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(5)) {
			m_stageRoutes[5].m_enabled = 1;
		}

		m_operationProfiles[6].m_colorOperation = 13;
		m_operationProfiles[6].m_colorArgument1 = 2;
		m_operationProfiles[6].m_colorArgument2 = 1;
		m_operationProfiles[6].m_alphaOperation = 2;
		m_operationProfiles[6].m_alphaArgument1 = 2;
		m_operationProfiles[6].m_alphaArgument2 = 1;
		m_operationProfiles[6].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(6)) {
			m_stageRoutes[6].m_enabled = 1;
		}

		m_operationProfiles[7].m_colorOperation = 14;
		m_operationProfiles[7].m_colorArgument1 = 2;
		m_operationProfiles[7].m_colorArgument2 = 1;
		m_operationProfiles[7].m_alphaOperation = 3;
		m_operationProfiles[7].m_alphaArgument1 = 2;
		m_operationProfiles[7].m_alphaArgument2 = 1;
		m_operationProfiles[7].m_unk0x18 = 0;
		if (AreTextureOperationsSupported(7)) {
			m_stageRoutes[7].m_enabled = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004e1dd0
LegoBool32 RendererTextureStageManager::AreTextureOperationsSupported(LegoU32 p_profileIndex) const
{
	DisplayRenderer* renderer = (DisplayRenderer*) ((BYTE*) this - offsetof(DisplayRenderer, m_textureStageManager));
	const D3DCAPS8& caps = renderer->m_selectedAdapter->m_caps;
	const RendererTextureOperationProfile* profile = &m_operationProfiles[p_profileIndex];
	if (!SupportsTextureOperation(caps, profile->m_colorOperation)) {
		return FALSE;
	}
	return SupportsTextureOperation(caps, profile->m_alphaOperation);
}
