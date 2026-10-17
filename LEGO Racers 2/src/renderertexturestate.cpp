#include "renderer.h"

// GLOBAL: LEGORACERS2 0x0067bd70
LegoU32 g_currentRendererOperation;

// GLOBAL: LEGORACERS2 0x0067bd74
LegoU32 g_vertexUploadPassCount;

extern HRESULT g_clearBuffersResult;

// FUNCTION: LEGORACERS2 0x004ddad0
void __stdcall MultiplyMatrix4(D3DMATRIX* p_result, const D3DMATRIX* p_left, const D3DMATRIX* p_right)
{
	LegoS32 row;
	for (row = 0; row < 4; row++) {
		LegoS32 column;
		for (column = 0; column < 4; column++) {
			LegoFloat value = p_left->m[row][0] * p_right->m[0][column];
			p_result->m[row][column] = value;
			value += p_left->m[row][1] * p_right->m[1][column];
			p_result->m[row][column] = value;
			value += p_left->m[row][2] * p_right->m[2][column];
			p_result->m[row][column] = value;
			value += p_left->m[row][3] * p_right->m[3][column];
			p_result->m[row][column] = value;
		}
	}
}

// STUB: LEGORACERS2 0x004de110
void __fastcall ApplyDefaultWorldTransform(DisplayRenderer* p_renderer, void*)
{
	p_renderer->m_worldTransform = p_renderer->m_defaultWorldTransform;
	p_renderer->m_device->SetTransform(D3DTS_WORLD, &p_renderer->m_worldTransform);
	p_renderer->m_unk0x788 = 0;
}

// FUNCTION: LEGORACERS2 0x004df760
void __fastcall FlushSpecularVertexStream(DisplayRenderer* p_renderer, void*)
{
	if (p_renderer->m_streamVertexCount != 0) {
		p_renderer->m_specularVertexBuffer->Unlock();
		g_clearBuffersResult = p_renderer->m_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			p_renderer->m_streamVertexCount,
			0,
			// Retail uses SAR here, then passes the resulting bits as UINT.
			(LegoU32) ((LegoS32) p_renderer->m_streamVertexCount >> 1)
		);
		p_renderer->m_streamVertexCount = 0;
		p_renderer->m_specularVertexBuffer->Lock(0, 0x10000, &p_renderer->m_mappedStreamVertices, 0x2800);
	}
}

// STUB: LEGORACERS2 0x004e0560
void DisplayRenderer::ApplyTextureOperation(LegoU32 p_operation)
{
	LegoU32 operation = p_operation;
	if (operation == 14) {
		switch (g_currentRendererOperation) {
		case 1:
			operation = 2;
			break;
		case 3:
			operation = 4;
			break;
		case 5:
			operation = 6;
			break;
		case 7:
			operation = 8;
			break;
		case 9:
			operation = 10;
			break;
		default:
			return;
		}
	}

	switch (operation) {
	case 0:
		break;
	case 1:
		m_vtable->m_setDepthState(this, NULL, 0);
		if (m_unk0xc7ec8 == 0) {
			m_vtable->m_action0x8c(this, NULL, 0);
		}
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1b, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1d, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 0);
		BindMappedTexture(-1);
		break;
	case 2:
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 1);
		m_vtable->m_setDepthState(this, NULL, 1);
		break;
	case 3: {
		LegoU32 vertexFormat = m_quadVertexBuffer->m_storage->m_format;
		SetVertexStream(m_quadVertexBuffer->GetDeviceBuffer(), m_quadVertexBuffer->GetStride(), vertexFormat);
		m_device->SetIndices(m_quadIndexBuffer->GetDeviceBuffer(), 0);
		BindMappedTexture(-1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1d, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x0e, 0);
		if (m_unk0xc7ec8 == 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x34, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x38, 8);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x36, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x35, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x37, 7);
		}
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1b, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x0f, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x13, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x14, 2);
		break;
	}
	case 4:
		if (m_unk0xc7ec8 == 0) {
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x34, 0);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x38, 8);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x36, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x35, 1);
			SetCachedRenderState((D3DRENDERSTATETYPE) 0x37, 1);
		}
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 1);
		break;
	case 5: {
		m_vtable->m_uploadDefaultVertexConstants(this, NULL);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1d, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 0);
		SetVertexStream(m_specularVertexBuffer, 0x20, 0x1c4);
		if (m_unk0x788 == 0) {
			MultiplyMatrix4(&m_transform0x67c, &m_worldTransform, &m_viewTransform);
			MultiplyMatrix4(&m_transform0x6bc, &m_transform0x67c, &m_projectionTransform);
			MultiplyMatrix4(&m_transform0x6fc, &m_transform0x6bc, &m_viewportTransform);
			m_unk0x788 = 1;
		}
		m_device->SetIndices(m_streamIndexBuffer->GetDeviceBuffer(), 0);
		m_specularVertexBuffer->Lock(0, 0x10000, &m_mappedStreamVertices, 0x2800);
		break;
	}
	case 6:
		FlushSpecularVertexStream(this, NULL);
		m_specularVertexBuffer->Unlock();
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3 - (m_clockwiseCulling != 0));
		break;
	case 7:
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x1d, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 1);
		m_vtable->m_uploadDefaultVertexConstants(this, NULL);
		BindMappedTexture(-1);
		break;
	case 8:
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3 - (m_clockwiseCulling != 0));
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x89, 1);
		m_unk0xc7a24 = -1;
		break;
	case 9:
		m_vtable->m_setDepthState(this, NULL, 0);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3);
		break;
	case 10:
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3 - (m_clockwiseCulling != 0));
		m_vtable->m_setDepthState(this, NULL, 1);
		break;
	case 11:
		m_unk0xc7ec8 = 1;
		g_vertexUploadPassCount = 0;
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x34, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x38, 3);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x39, g_vertexUploadPassCount);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x36, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x35, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x37, 1);
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3);
		break;
	case 12:
		g_vertexUploadPassCount++;
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x39, g_vertexUploadPassCount);
		break;
	case 13:
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x16, 3 - (m_clockwiseCulling != 0));
		SetCachedRenderState((D3DRENDERSTATETYPE) 0x38, 8);
		break;
	default:
		break;
	}

	g_currentRendererOperation = operation;
}

// STUB: LEGORACERS2 0x004e0d40
void DisplayRenderer::SetVertexStream(IDirect3DVertexBuffer8* p_buffer, LegoU32 p_stride, LegoU32 p_shader)
{
	if (p_buffer == NULL) {
		m_cachedStreamBuffer = NULL;
	}
	else {
		if (m_cachedStreamBuffer == p_buffer) {
			return;
		}
		m_cachedStreamBuffer = p_buffer;
		m_device->SetStreamSource(0, p_buffer, p_stride);
	}

	if (m_cachedVertexShader != p_shader) {
		m_cachedVertexShader = p_shader;
		m_device->SetVertexShader(p_shader);
	}
}

// STUB: LEGORACERS2 0x004e1e60
void DisplayRenderer::BindMappedTexture(LegoS32 p_slot)
{
	RestoreTextureStageDefaults();

	LegoU32 textureIndex;
	if (p_slot == -1) {
		textureIndex = m_defaultTextureIndex;
	}
	else {
		textureIndex = m_textureIndexMap[p_slot];
	}

	if (m_cachedTextureHandles[0] != textureIndex) {
		m_cachedTextureHandles[0] = textureIndex;
		m_device->SetTexture(0, m_textureRecords[textureIndex].m_texture);
	}

	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 18, 2);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 13, 3);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 14, 3);
}

// STUB: LEGORACERS2 0x004e1fa0
void DisplayRenderer::RestoreTextureStageDefaults()
{
	if (m_cachedTextureHandles[0] != m_defaultTextureIndex) {
		m_cachedTextureHandles[0] = m_defaultTextureIndex;
		m_device->SetTexture(0, m_textureRecords[m_defaultTextureIndex].m_texture);
	}

	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 11, 0);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 1, 4);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 2, 2);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 3, 0);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 4, 4);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 5, 2);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 6, 0);
	SetCachedTextureStageState(0, (D3DTEXTURESTAGESTATETYPE) 18, 1);
	SetCachedTextureStageState(1, (D3DTEXTURESTAGESTATETYPE) 1, 1);
	SetCachedTextureStageState(1, (D3DTEXTURESTAGESTATETYPE) 4, 1);
}
