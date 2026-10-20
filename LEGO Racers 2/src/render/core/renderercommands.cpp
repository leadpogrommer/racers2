#include "math/matrix3.h"
#include "math/vector3.h"
#include "render/buffers/rendererbufferdispatch.h"
#include "render/core/renderer.h"
#include "render/pipeline/rendererstageplan.h"

extern HRESULT g_clearBuffersResult;

// GLOBAL: LEGORACERS2 0x0067bd68
IDirect3DSurface8* g_savedRendererTarget;
// GLOBAL: LEGORACERS2 0x0067bd6c
D3DVIEWPORT8* g_savedRendererViewport;

// Retail 0x00406cd0 shares its rebuilt eight-byte body with the annotated
// exception filter at 0x004c32b0. Keep one comparison mapping for this ICF body.
LegoBool32 DisplayRenderer::AcceptUnusedRendererPolicy(LegoU32)
{
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004abdd0
LegoBool32 DisplayRenderer::RejectUnknownDrawRequest(LegoU32, LegoU32, LegoU32)
{
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x004ac0e0
LegoBool32 DisplayRenderer::SupportsUnknownDrawFeature(LegoU32, LegoU32, LegoU32, LegoU32, LegoU32, LegoU32)
{
	return FALSE;
}

// STUB: LEGORACERS2 0x004dfd80
void DisplayRenderer::SetTextureRenderTarget(RendererTextureRecord* p_record)
{
	IDirect3DSurface8* depthStencil;
	m_device->GetDepthStencilSurface(&depthStencil);
	if (p_record == NULL) {
		m_device->SetRenderTarget(g_savedRendererTarget, depthStencil);
		m_device->SetViewport(g_savedRendererViewport);
		SetProjectionFromViewport(g_savedRendererViewport);
		g_savedRendererTarget = NULL;
		g_savedRendererViewport = NULL;
		return;
	}
	if (g_savedRendererTarget == NULL) {
		m_device->GetRenderTarget(&g_savedRendererTarget);
	}
	if (g_savedRendererViewport == NULL) {
		g_savedRendererViewport = &m_viewports[m_currentViewportIndex].m_viewport;
	}
	IDirect3DSurface8* target;
	((IDirect3DTexture8*) p_record->m_texture)->GetSurfaceLevel(0, &target);
	m_device->SetRenderTarget(target, depthStencil);
	D3DVIEWPORT8 viewport;
	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = (LegoS32) p_record->m_width;
	viewport.Height = (LegoS32) p_record->m_height;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	m_device->SetViewport(&viewport);
	SetProjectionFromViewport(&viewport);
}

// STUB: LEGORACERS2 0x004dffb0
void DisplayRenderer::DrawStencilVolumes(LegoS32 p_vertexCount)
{
	if (m_foundPreferredDepthFormat == 1) {
		g_clearBuffersResult =
			m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, p_vertexCount, 0, p_vertexCount >> 1);
		SetCachedRenderState(D3DRS_CULLMODE, (m_clockwiseCulling != 0) + 2);
		if (m_unk0xc7ec8 == 0) {
			SetCachedRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
		}
		g_clearBuffersResult =
			m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, p_vertexCount, 0, p_vertexCount >> 1);
		SetCachedRenderState(D3DRS_CULLMODE, 3 - (m_clockwiseCulling != 0));
		if (m_unk0xc7ec8 == 0) {
			SetCachedRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		}
	}
}

// STUB: LEGORACERS2 0x004e18a0
void DisplayRenderer::SetTextureTransform(LegoS32 p_index, const Matrix3* p_rotation, const Vector3* p_translation)
{
	RendererTextureTransform& transform = m_textureTransforms[p_index];
	for (LegoS32 row = 0; row < 3; row++) {
		for (LegoS32 column = 0; column < 3; column++) {
			transform.m_matrix.m[row][column] = p_rotation->m_values[column][row];
		}
	}
	transform.m_matrix.m[3][0] = p_translation->m_x;
	transform.m_matrix.m[3][1] = p_translation->m_y;
	transform.m_matrix.m[3][2] = p_translation->m_z;
	transform.m_matrix.m[2][3] = 0.0f;
	transform.m_matrix.m[1][3] = 0.0f;
	transform.m_matrix.m[0][3] = 0.0f;
	transform.m_matrix.m[3][3] = 1.0f;
	transform.m_used = 1;
	m_textureTransformsEnabled = 1;
}

// STUB: LEGORACERS2 0x004e9db0
void DisplayRenderer::PrepareDrawPlan(RendererStagePlanRequest* p_request)
{
	typedef void(__fastcall * PlanCallback)(DisplayRenderer*, void*, RendererStagePlanRequest*);
	if ((p_request->m_defaults.m_unk0x00 & 8) != 0) {
		PlanCallback prepare = *(PlanCallback*) ((LegoU8*) m_vtable + 0x84);
		prepare(this, NULL, p_request);
	}
	RendererVertexBuffer* vertexBuffer = (RendererVertexBuffer*) p_request->m_unk0x0c;
	const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) vertexBuffer->m_vtable;
	if ((dispatch->m_getAttributes(vertexBuffer, NULL) & 4) != 0) {
		p_request->m_unk0x06 |= 0x200;
	}
	PlanCallback resolve = *(PlanCallback*) ((LegoU8*) m_vtable + 0x80);
	resolve(this, NULL, p_request);
}
