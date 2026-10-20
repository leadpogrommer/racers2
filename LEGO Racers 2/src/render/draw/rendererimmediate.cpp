#include "render/core/renderer.h"
#include "render/draw/renderersprite.h"

extern HRESULT g_clearBuffersResult;

static void SelectImmediateVertexShader(DisplayRenderer* p_renderer, LegoU32 p_shader)
{
	p_renderer->m_cachedStreamBuffer = NULL;
	if (p_renderer->m_cachedVertexShader != p_shader) {
		p_renderer->m_cachedVertexShader = p_shader;
		p_renderer->m_device->SetVertexShader(p_shader);
	}
}

// STUB: LEGORACERS2 0x004e00d0
void DisplayRenderer::DrawStencilOverlay()
{
	if (m_foundPreferredDepthFormat != 1) {
		return;
	}
	RendererSpriteVertex vertices[4];
	const LegoFloat depth = (1.0f - m_nearClip * 10.0f) * m_projectionTransform.m[2][2];
	for (LegoS32 index = 0; index < 4; index++) {
		vertices[index].m_z = depth;
		vertices[index].m_rhw = 10.0f;
		vertices[index].m_diffuse = 0x30ffffff;
		vertices[index].m_specular = 0;
		vertices[index].m_u = 0.0f;
		vertices[index].m_v = 0.0f;
	}
	vertices[0].m_x = vertices[2].m_x = m_viewportX;
	vertices[1].m_x = vertices[3].m_x = m_viewportWidth + m_viewportX;
	vertices[0].m_y = vertices[1].m_y = m_viewportY;
	vertices[2].m_y = vertices[3].m_y = m_viewportHeight + m_viewportY;
	m_vtable->m_setDepthState(this, NULL, 0);
	if (m_unk0xc7ec8 == 0) {
		SetCachedRenderState(D3DRS_STENCILREF, 1);
		SetCachedRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
		SetCachedRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	}
	SetCachedRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
	SelectImmediateVertexShader(this, 0x1c4);
	g_clearBuffersResult = m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(RendererSpriteVertex));
	m_vtable->m_setDepthState(this, NULL, 1);
}

// STUB: LEGORACERS2 0x004e02d0
void DisplayRenderer::DrawUnlitLines(const Vector3* p_vertices, LegoS32 p_count)
{
	SelectImmediateVertexShader(this, D3DFVF_XYZ);
	BindMappedTexture(-1);
	SetCachedRenderState(D3DRS_ALPHABLENDENABLE, 0);
	SetCachedRenderState(D3DRS_LIGHTING, 0);
	typedef void(__fastcall * SetReference)(DisplayRenderer*, void*, LegoU32);
	SetReference setReference = *(SetReference*) ((LegoU8*) m_vtable + 0x9c);
	setReference(this, NULL, 2);
	g_clearBuffersResult = m_device->DrawPrimitiveUP(D3DPT_LINELIST, p_count / 2, p_vertices, sizeof(Vector3));
	setReference = *(SetReference*) ((LegoU8*) m_vtable + 0x9c);
	setReference(this, NULL, 0);
	SetCachedRenderState(D3DRS_LIGHTING, 1);
}

// STUB: LEGORACERS2 0x004e03d0
void DisplayRenderer::DrawUnlitTriangles(const void* p_vertices, LegoS32 p_count)
{
	SelectImmediateVertexShader(this, D3DFVF_XYZ | D3DFVF_NORMAL);
	BindMappedTexture(-1);
	// Retail material storage at +0x24528 overlaps the stage-alpha value at
	// +0x24544. Use that actual storage rather than introducing another material.
	D3DMATERIAL8* material = (D3DMATERIAL8*) m_unk0x24528;
	m_auxiliaryResources[0].CopyToDeviceMaterial(material);
	material->Ambient.r = material->Ambient.g = material->Ambient.b = material->Ambient.a = 0.0f;
	material->Diffuse.r = material->Diffuse.g = material->Diffuse.b = material->Diffuse.a = 1.0f;
	material->Emissive.r = material->Emissive.g = material->Emissive.b = material->Emissive.a = 0.0f;
	m_device->SetMaterial(material);
	SetCachedRenderState(D3DRS_SPECULARENABLE, 0);
	SetCachedRenderState(D3DRS_LIGHTING, 0);
	m_vtable->m_action0xc8(this, NULL, 0);
	g_clearBuffersResult = m_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, p_count / 3, p_vertices, 0x18);
}

// STUB: LEGORACERS2 0x004fb7f0
void RendererAuxiliaryResourceRecord::CopyToDeviceMaterial(D3DMATERIAL8* p_material) const
{
	const LegoFloat alpha = 1.0f - m_modeFactor;
	p_material->Ambient.r = m_color0[0];
	p_material->Ambient.g = m_color0[1];
	p_material->Ambient.b = m_color0[2];
	p_material->Ambient.a = alpha;
	p_material->Diffuse.r = m_color1[0];
	p_material->Diffuse.g = m_color1[1];
	const LegoFloat blue = m_color1[2];
	p_material->Diffuse.a = alpha;
	p_material->Diffuse.b = blue;
	p_material->Emissive.r = m_scaledColor1[0];
	p_material->Emissive.g = m_scaledColor1[1];
	const LegoFloat emissiveBlue = m_scaledColor1[2];
	p_material->Emissive.a = alpha;
	p_material->Emissive.b = emissiveBlue;
	p_material->Specular.r = m_color2[0];
	p_material->Specular.g = m_color2[1];
	const LegoFloat specularBlue = m_color2[2];
	p_material->Specular.a = alpha;
	p_material->Specular.b = specularBlue;
	p_material->Power = m_power;
}
