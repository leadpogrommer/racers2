#include "renderer.h"

#include <stdlib.h>
#include <string.h>

struct RendererQuadVertex28 {
	LegoU32 m_words[7];
};

struct RendererQuadVertex36 {
	LegoU32 m_words[9];
};

DECOMP_SIZE_ASSERT(RendererQuadVertex28, 0x1c);
DECOMP_SIZE_ASSERT(RendererQuadVertex36, 0x24);

// GLOBAL: LEGORACERS2 0x0067bb70
LegoU32 g_quadPacketCleanupFlags;

// GLOBAL: LEGORACERS2 0x0067bb78
RendererQuadVertex36 g_quadPacketFourVertexScratch[4];

// GLOBAL: LEGORACERS2 0x0067bc08
RendererQuadVertex36 g_quadPacketThreeVertexScratch[3];

// GLOBAL: LEGORACERS2 0x0067bc90
RendererQuadVertex28 g_quadPacketTriangleListScratch[4];

// GLOBAL: LEGORACERS2 0x0067bd00
RendererQuadVertex28 g_quadPacketTriangleStripScratch[3];

namespace
{

void __cdecl QuadPacketExitCallback()
{
}

LegoU32 ReadPacketWord(const BYTE* p_packet, LegoU32 p_offset)
{
	LegoU32 value;
	memcpy(&value, p_packet + p_offset, sizeof(value));
	return value;
}

LegoS16 ReadPacketSignedWord(const BYTE* p_packet, LegoU32 p_offset)
{
	LegoS16 value;
	memcpy(&value, p_packet + p_offset, sizeof(value));
	return value;
}

void StoreFloatWord(LegoU32* p_destination, LegoFloat p_value)
{
	memcpy(p_destination, &p_value, sizeof(p_value));
}

void SelectPacketTexture(DisplayRenderer* p_renderer, LegoS32 p_index, LegoU32 p_stage)
{
	const LegoU32 textureRecordIndex = p_renderer->m_textureIndexMap[p_index];
	if (p_renderer->m_cachedTextureHandles[p_stage] != textureRecordIndex) {
		p_renderer->m_cachedTextureHandles[p_stage] = textureRecordIndex;
		p_renderer->m_device->SetTexture(p_stage, p_renderer->m_textureRecords[textureRecordIndex].m_texture);
	}
}

void SelectPacketTexturePair(DisplayRenderer* p_renderer, const BYTE* p_packet)
{
	SelectPacketTexture(p_renderer, ReadPacketSignedWord(p_packet, 0x0c), 0);
	SelectPacketTexture(p_renderer, ReadPacketSignedWord(p_packet, 0x0e), 1);
}

void EnterQuadVertexMode(DisplayRenderer* p_renderer, LegoU32 p_mode, LegoU32 p_fvf)
{
	if (p_renderer->m_unk0xc7a24 != p_mode) {
		p_renderer->m_unk0xc7a24 = p_mode;
		p_renderer->m_cachedStreamBuffer = NULL;
		if (p_renderer->m_cachedVertexShader != p_fvf) {
			p_renderer->m_cachedVertexShader = p_fvf;
			p_renderer->m_device->SetVertexShader(p_fvf);
		}
	}
}

void InitializeDrawDefaults(DisplayRenderer* p_renderer, const BYTE* p_packet)
{
	p_renderer->m_vtable->m_action0xc8(p_renderer, NULL, ReadPacketWord(p_packet, 4));
	p_renderer->m_drawDefaults.m_slots[0].m_mask = ReadPacketSignedWord(p_packet, 0x0c);
	p_renderer->m_drawDefaults.m_slots[1].m_value = ReadPacketWord(p_packet, 0x10);
	p_renderer->m_drawDefaults.m_slots[1].m_mask = ReadPacketSignedWord(p_packet, 0x0e);
}

void DrawStagePlanPacket(
	DisplayRenderer* p_renderer,
	const void* p_vertices,
	LegoU32 p_primitiveCount,
	LegoU32 p_stride
)
{
	LegoS32 index = 0;
	while (p_renderer->m_textureStageManager.ApplyStagePlanSubrecord(&p_renderer->m_drawDefaults, index) == 1) {
		p_renderer->m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, p_primitiveCount, p_vertices, p_stride);
		++index;
	}
}

void InitializeGeneratedVertex(
	LegoU32* p_vertex,
	const LegoFloat* p_input,
	LegoFloat p_nearClip,
	LegoFloat p_projectionScale
)
{
	memcpy(&p_vertex[0], &p_input[0], sizeof(LegoU32));
	memcpy(&p_vertex[1], &p_input[1], sizeof(LegoU32));
	StoreFloatWord(&p_vertex[2], (1.0f - p_nearClip / p_input[2]) * p_projectionScale);
	StoreFloatWord(&p_vertex[3], 1.0f / p_input[2]);
	memcpy(&p_vertex[4], &p_input[3], 3 * sizeof(LegoU32));
}

void DrawGeneratedVertices(
	DisplayRenderer* p_renderer,
	const BYTE* p_packet,
	RendererQuadVertex28* p_vertices,
	LegoU32 p_vertexCount,
	LegoU32 p_primitiveCount
)
{
	const LegoFloat projectionScale = p_renderer->m_projectionTransform.m[2][2];
	const LegoFloat* input = (const LegoFloat*) (p_packet + 0x10);
	for (LegoU32 i = 0; i < p_vertexCount; ++i) {
		InitializeGeneratedVertex(p_vertices[i].m_words, input, p_renderer->m_nearClip, projectionScale);
		input += 6;
	}
	p_renderer->m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, p_primitiveCount, p_vertices, 0x1c);
}

void DrawGeneratedTextureVertices(
	DisplayRenderer* p_renderer,
	const BYTE* p_packet,
	RendererQuadVertex36* p_vertices,
	LegoU32 p_vertexCount
)
{
	const LegoFloat projectionScale = p_renderer->m_projectionTransform.m[2][2];
	const LegoFloat* input = (const LegoFloat*) (p_packet + 0x1c);
	for (LegoU32 i = 0; i < p_vertexCount; ++i) {
		LegoU32* output = p_vertices[i].m_words;
		memcpy(&output[0], &input[-2], sizeof(LegoU32));
		memcpy(&output[1], &input[-1], sizeof(LegoU32));
		StoreFloatWord(&output[2], (1.0f - p_renderer->m_nearClip / input[0]) * projectionScale);
		StoreFloatWord(&output[3], 1.0f / input[0]);
		memcpy(&output[4], &input[1], 5 * sizeof(LegoU32));
		input += 8;
	}
}

void RegisterGeneratedVertexCleanup(LegoU32 p_flag)
{
	if ((g_quadPacketCleanupFlags & p_flag) == 0) {
		g_quadPacketCleanupFlags |= p_flag;
		atexit(QuadPacketExitCallback);
	}
}

} // namespace

// STUB: LEGORACERS2 0x004e0db0
void DisplayRenderer::DrawQuadPacket(const void* p_packetData)
{
	const BYTE* packet = (const BYTE*) p_packetData;
	const LegoU32 kind = ReadPacketWord(packet, 0);
	if (kind > 7) {
		return;
	}
	const LegoU32 flags = ReadPacketWord(packet, 8);

	switch (kind) {
	case 0:
	case 2: {
		const LegoS32 textureIndex = (LegoS32) ReadPacketWord(packet, 0x0c);
		if ((flags & 0x80) != 0) {
			SelectPacketTexture(this, textureIndex, 0);
		}
		else {
			m_vtable->m_action0xc8(this, NULL, ReadPacketWord(packet, 4));
			BindMappedTexture(textureIndex);
		}
		EnterQuadVertexMode(this, 4, 0x142);
		m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, kind == 0 ? 1 : 2, packet + 0x10, 0x18);
		break;
	}
	case 1:
	case 3: {
		EnterQuadVertexMode(this, 5, 0x242);
		if ((flags & 0x80) != 0) {
			SelectPacketTexturePair(this, packet);
			m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, kind == 1 ? 1 : 2, packet + 0x14, 0x20);
		}
		else {
			InitializeDrawDefaults(this, packet);
			DrawStagePlanPacket(this, packet + 0x14, kind == 1 ? 1 : 2, 0x20);
		}
		break;
	}
	case 4:
	case 6: {
		RegisterGeneratedVertexCleanup(kind == 4 ? 1 : 4);
		const LegoS32 textureIndex = (LegoS32) ReadPacketWord(packet, 0x0c);
		if ((flags & 0x80) != 0) {
			SelectPacketTexture(this, textureIndex, 0);
		}
		else {
			m_vtable->m_action0xc8(this, NULL, ReadPacketWord(packet, 4));
			BindMappedTexture(textureIndex);
		}
		EnterQuadVertexMode(this, 0, 0x144);
		if (kind == 4) {
			DrawGeneratedVertices(this, packet, g_quadPacketTriangleStripScratch, 3, 1);
		}
		else {
			DrawGeneratedVertices(this, packet, g_quadPacketTriangleListScratch, 4, 2);
		}
		break;
	}
	case 5:
	case 7: {
		RegisterGeneratedVertexCleanup(kind == 5 ? 2 : 8);
		EnterQuadVertexMode(this, 1, 0x244);
		RendererQuadVertex36* vertices = kind == 5 ? g_quadPacketThreeVertexScratch : g_quadPacketFourVertexScratch;
		const LegoU32 primitiveCount = kind == 5 ? 1 : 2;
		DrawGeneratedTextureVertices(this, packet, vertices, kind == 5 ? 3 : 4);
		if ((flags & 0x80) != 0) {
			SelectPacketTexturePair(this, packet);
			m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, primitiveCount, vertices, 0x24);
		}
		else {
			InitializeDrawDefaults(this, packet);
			DrawStagePlanPacket(this, vertices, primitiveCount, 0x24);
		}
		break;
	}
	default:
		break;
	}
}
