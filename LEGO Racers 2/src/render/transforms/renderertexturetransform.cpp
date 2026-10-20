#include "render/core/renderer.h"

// GLOBAL: LEGORACERS2 0x0054ae10
D3DTRANSFORMSTATETYPE g_textureTransformStates[8] = {
	D3DTS_TEXTURE0,
	D3DTS_TEXTURE1,
	D3DTS_TEXTURE2,
	D3DTS_TEXTURE3,
	D3DTS_TEXTURE4,
	D3DTS_TEXTURE5,
	D3DTS_TEXTURE6,
	D3DTS_TEXTURE7
};

// GLOBAL: LEGORACERS2 0x0054ae30
D3DMATRIX g_defaultTextureTransform = {0.25f, 0, 0, 0, 0, 0.25f, 0, 0, 0, 0, 1, 0, 0.5f, 0.5f, 0, 1};

// GLOBAL: LEGORACERS2 0x0054ae70
D3DMATRIX g_scaledTextureTransform = {0.5f, 0, 0, 0, 0, 0.5f, 0, 0, 0, 0, 1, 0, 0.5f, 0.5f, 0, 1};

// STUB: LEGORACERS2 0x004ddeb0
void MultiplyTextureMatrixInPlace(D3DMATRIX* p_left, const D3DMATRIX* p_right)
{
	D3DMATRIX result;
	for (LegoS32 row = 0; row < 4; row++) {
		for (LegoS32 column = 0; column < 4; column++) {
			// Retail accumulates the fourth product before the third, and
			// stores only the completed sum before copying the temporary.
			result.m[row][column] =
				((p_right->m[0][column] * p_left->m[row][0] + p_right->m[1][column] * p_left->m[row][1]) +
				 p_right->m[3][column] * p_left->m[row][3]) +
				p_right->m[2][column] * p_left->m[row][2];
		}
	}
	*p_left = result;
}

// STUB: LEGORACERS2 0x004dfb60
void DisplayRenderer::ApplyTextureTransform(const RendererDefaultSlot* p_slot, LegoU32 p_stage)
{
	RendererTextureTransform* transform = NULL;
	if (m_textureTransformsEnabled == 1) {
		transform = &m_textureTransforms[p_slot->m_value];
		transform->m_used = 1;
	}
	if (p_slot->m_value != 1 && p_slot->m_value != 9) {
		if (transform != NULL) {
			m_device->SetTransform(g_textureTransformStates[p_stage], &transform->m_matrix);
		}
		return;
	}

	const LegoBool32 cube = (m_textureRecords[(LegoS16) p_slot->m_mask].m_flags0x9c & 8) == 8;
	if (transform != NULL) {
		D3DMATRIX matrix = cube ? m_transform0x53c : g_scaledTextureTransform;
		MultiplyTextureMatrixInPlace(&matrix, &transform->m_matrix);
		m_device->SetTransform(g_textureTransformStates[p_stage], &matrix);
	}
	else {
		m_device->SetTransform(
			g_textureTransformStates[p_stage],
			cube ? &m_transform0x53c : &g_defaultTextureTransform
		);
	}
}

// FUNCTION: LEGORACERS2 0x004dff90
LegoU32 DisplayRenderer::GetStageFactorColor()
{
	return m_stageFactorColor;
}

// FUNCTION: LEGORACERS2 0x004dffa0
LegoU32 DisplayRenderer::GetInverseStageFactorColor()
{
	return ~m_stageFactorColor & 0x00ffffff;
}
