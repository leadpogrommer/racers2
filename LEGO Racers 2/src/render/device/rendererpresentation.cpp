#include "render/core/renderer.h"

#include <string.h>

namespace
{

struct ClearQuadVertex {
	LegoFloat m_x;
	LegoFloat m_y;
	LegoFloat m_z;
	LegoU32 m_diffuse;
	LegoFloat m_u;
	LegoFloat m_v;
};

struct ClearQuadPacket {
	LegoU32 m_primitiveType;
	LegoU32 m_primitiveCount;
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	ClearQuadVertex m_vertices[4];
};

DECOMP_SIZE_ASSERT(ClearQuadVertex, 0x18);
DECOMP_SIZE_ASSERT(ClearQuadPacket, 0x70);

static LegoS32 RoundStoredFloatUsingCurrentFpuMode(LegoFloat p_value)
{
	volatile LegoFloat storedValue = p_value;
	LegoS32 result;
	// The retail path stores the scaled float before FISTP, which observes the
	// active x87 rounding mode rather than truncating with a C++ cast.
	__asm {
		fld storedValue
		fistp result
	}
	return result;
}

} // namespace

// GLOBAL: LEGORACERS2 0x0067bd58
HRESULT g_clearBuffersResult;

// STUB: LEGORACERS2 0x004ad130
LegoU32 WINAPI GetChannelMaskBitCount(LegoU32 p_mask)
{
	switch (p_mask) {
	case 1:
		return 1;
	case 3:
		return 2;
	case 7:
		return 3;
	case 15:
		return 4;
	case 31:
		return 5;
	case 63:
		return 6;
	case 127:
		return 7;
	case 255:
		return 8;
	default:
		return 0;
	}
}

// STUB: LEGORACERS2 0x004ae5c0
void DisplayRenderer::SetMaterialColor(LegoU32 p_color)
{
	memset(&m_material, 0, sizeof(m_material));
	LegoFloat red = (LegoFloat) (p_color & 0xff) * (1.0f / 255.0f);
	LegoFloat green = (LegoFloat) ((p_color >> 8) & 0xff) * (1.0f / 255.0f);
	LegoFloat blue = (LegoFloat) ((p_color >> 16) & 0xff) * (1.0f / 255.0f);

	m_material.Diffuse.r = red;
	m_material.Diffuse.g = green;
	m_material.Diffuse.b = blue;
	m_material.Diffuse.a = 1.0f;
	m_material.Ambient.r = red;
	m_material.Ambient.g = green;
	m_material.Ambient.b = blue;
	m_material.Ambient.a = 1.0f;
	m_material.Specular.r = red;
	m_material.Specular.g = green;
	m_material.Specular.b = blue;
	m_material.Specular.a = 1.0f;
	m_material.Emissive.r = 1.0f;
	m_material.Emissive.g = 1.0f;
	m_material.Emissive.b = 1.0f;
	m_material.Emissive.a = 1.0f;
	m_material.Power = 5.0f;
}

// STUB: LEGORACERS2 0x004dc9b0
LegoBool32 DisplayRenderer::ClearBuffers(LegoU32 p_flags)
{
	if ((p_flags & 8) != 0) {
		IDirect3DSurface8* renderTarget;
		D3DSURFACE_DESC surfaceDescription;
		ClearQuadPacket packet;
		LegoU32 depthWrite = (p_flags >> 1) & 1;

		memset(&packet, 0, sizeof(packet));
		m_device->GetRenderTarget(&renderTarget);
		renderTarget->GetDesc(&surfaceDescription);

		m_vtable->m_action0x124(this, NULL, 7);
		m_vtable->m_setDepthState(this, NULL, 0);

		if (m_cachedRenderStates[14] != depthWrite) {
			m_cachedRenderStates[14] = depthWrite;
			m_device->SetRenderState(D3DRS_ZWRITEENABLE, depthWrite);
		}

		if ((p_flags & 4) != 0 && m_foundPreferredDepthFormat == 1) {
			if (m_cachedRenderStates[52] != 1) {
				m_cachedRenderStates[52] = 1;
				m_device->SetRenderState(D3DRS_STENCILENABLE, 1);
			}
			if (m_cachedRenderStates[56] != 8) {
				m_cachedRenderStates[56] = 8;
				m_device->SetRenderState(D3DRS_STENCILFUNC, 8);
			}
			if (m_cachedRenderStates[55] != 2) {
				m_cachedRenderStates[55] = 2;
				m_device->SetRenderState(D3DRS_STENCILPASS, 2);
			}
		}
		else if (m_cachedRenderStates[52] != 0) {
			m_cachedRenderStates[52] = 0;
			m_device->SetRenderState(D3DRS_STENCILENABLE, 0);
		}

		packet.m_primitiveType = 6;
		packet.m_unk0x0c = 0xffffffff;
		packet.m_vertices[0].m_z = m_clearQuadDepth;
		packet.m_vertices[1].m_z = m_clearQuadDepth;
		packet.m_vertices[2].m_z = m_clearQuadDepth;
		packet.m_vertices[3].m_z = m_clearQuadDepth;
		packet.m_vertices[0].m_diffuse = 0xff000000;
		packet.m_vertices[1].m_diffuse = 0xff000000;
		packet.m_vertices[2].m_diffuse = 0xff000000;
		packet.m_vertices[3].m_diffuse = 0xff000000;
		packet.m_vertices[1].m_x = (LegoFloat) (LegoS32) surfaceDescription.Width;
		packet.m_vertices[2].m_y = (LegoFloat) (LegoS32) surfaceDescription.Height;
		packet.m_vertices[3].m_x = (LegoFloat) (LegoS32) surfaceDescription.Width;
		packet.m_vertices[3].m_y = (LegoFloat) (LegoS32) surfaceDescription.Height;

		m_vtable->m_drawQuad(this, NULL, &packet);
		m_vtable->m_setDepthState(this, NULL, 1);
		m_vtable->m_action0x124(this, NULL, 8);
		return TRUE;
	}

	LegoU32 clearFlags = 0;
	if ((p_flags & 1) != 0) {
		clearFlags |= D3DCLEAR_TARGET;
	}
	if ((p_flags & 2) != 0) {
		clearFlags |= D3DCLEAR_ZBUFFER;
	}
	if ((p_flags & 4) != 0 && m_foundPreferredDepthFormat == 1) {
		clearFlags |= D3DCLEAR_STENCIL;
	}
	if (clearFlags == 0) {
		return TRUE;
	}

	m_vtable->m_setRenderTarget(this, NULL, m_clearRenderTarget);

	LegoS32 blue = RoundStoredFloatUsingCurrentFpuMode(m_material.Ambient.b * 255.0f);
	LegoS32 green = RoundStoredFloatUsingCurrentFpuMode(m_material.Ambient.g * 255.0f);
	LegoS32 red = RoundStoredFloatUsingCurrentFpuMode(m_material.Ambient.r * 255.0f);
	LegoS32 alpha = RoundStoredFloatUsingCurrentFpuMode(m_material.Ambient.a * 255.0f);
	LegoU32 color = ((LegoU32) alpha << 24) | ((LegoU32) red << 16) | ((LegoU32) green << 8) | (LegoU32) blue;

	g_clearBuffersResult = m_device->Clear(0, NULL, clearFlags, color, 1.0f, 0);
	return g_clearBuffersResult == D3D_OK;
}

// FUNCTION: LEGORACERS2 0x004dfb20
LegoBool32 DisplayRenderer::Present()
{
	m_device->Present(NULL, NULL, NULL, NULL);
	for (LegoU32 i = 0; i < sizeOfArray(m_cachedTextureHandles); i++) {
		m_cachedTextureHandles[i] = 0x12345678;
	}
	return TRUE;
}
