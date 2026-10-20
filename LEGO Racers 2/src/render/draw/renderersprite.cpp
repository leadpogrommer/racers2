#include "render/draw/renderersprite.h"

#include "math/scalarutil.h"
#include "render/core/renderer.h"

// GLOBAL: LEGORACERS2 0x00578cc0
LegoFloat g_spriteSineTable[256];
// GLOBAL: LEGORACERS2 0x005790c0
LegoU32 g_spriteSineTableInitialized;

void __fastcall FlushSpecularVertexStream(DisplayRenderer*, void*);

static LegoS32 SignedFloatBits(LegoFloat p_value)
{
	union FloatBits {
		LegoFloat m_float;
		LegoS32 m_bits;
	} bits;
	bits.m_float = p_value;
	return bits.m_bits;
}

// STUB: LEGORACERS2 0x004df6b0
void DisplayRenderer::BeginSpriteBatch(LegoS32 p_textureId, LegoU32 p_blendMode, LegoFloat, LegoFloat)
{
	FlushSpecularVertexStream(this, NULL);
	m_vtable->m_action0xc8(this, NULL, p_blendMode);
	BindMappedTexture(p_textureId);
}

// STUB: LEGORACERS2 0x004df6e0
void DisplayRenderer::DrawSprite(RendererSpriteRecord* p_sprite)
{
	typedef void(__fastcall * BeginBatch)(DisplayRenderer*, void*, LegoS32, LegoU32, LegoFloat, LegoFloat);
	typedef void(__fastcall * DrawGeometry)(DisplayRenderer*, void*, RendererSpriteRecord*);
	BeginBatch begin = *(BeginBatch*) ((LegoU8*) m_vtable + 0x144);
	begin(
		this,
		NULL,
		p_sprite->m_textureId,
		p_sprite->m_blendMode,
		p_sprite->m_halfWidth,
		p_sprite->m_maximumWidthFraction
	);
	DrawGeometry draw = *(DrawGeometry*) ((LegoU8*) m_vtable + 0x148);
	draw(this, NULL, p_sprite);
}

// STUB: LEGORACERS2 0x004df720
RendererSpriteVertex* DisplayRenderer::AllocateSpriteVertices()
{
	if ((LegoS32) (m_streamVertexCount + 4) > 0x800) {
		FlushSpecularVertexStream(this, NULL);
	}
	const LegoU32 start = m_streamVertexCount;
	m_streamVertexCount += 4;
	return (RendererSpriteVertex*) (m_mappedStreamVertices + start * 0x20);
}

// STUB: LEGORACERS2 0x004df7d0
void DisplayRenderer::DrawSpriteGeometry(RendererSpriteRecord* p_sprite)
{
	const D3DMATRIX& matrix = GetStaticDisplayRenderer()->m_transform0x6fc;
	const Vector3& position = p_sprite->m_position;
	LegoFloat depth = position.m_x * matrix.m[0][2] + matrix.m[3][2] + position.m_y * matrix.m[1][2] +
					  position.m_z * matrix.m[2][2] + p_sprite->m_depthBias;
	if (SignedFloatBits(depth) < SignedFloatBits(m_nearClip)) {
		return;
	}
	LegoFloat reciprocalW = 1.0f / (position.m_z * matrix.m[2][3] + position.m_y * matrix.m[1][3] +
									position.m_x * matrix.m[0][3] + matrix.m[3][3]);
	LegoFloat screenX = reciprocalW * (position.m_z * matrix.m[2][0] + position.m_y * matrix.m[1][0] +
									   position.m_x * matrix.m[0][0] + matrix.m[3][0]);
	LegoFloat screenY = reciprocalW * (position.m_z * matrix.m[2][1] + position.m_y * matrix.m[1][1] +
									   position.m_x * matrix.m[0][1] + matrix.m[3][1]);
	depth *= reciprocalW;
	LegoFloat scale = reciprocalW * m_projectionPixelScale;
	LegoFloat halfWidth = p_sprite->m_halfWidth * scale;
	LegoFloat halfHeight = p_sprite->m_halfHeight * scale;
	if (SignedFloatBits(halfWidth) <= 0x3b03126e) {
		return;
	}
	const LegoFloat maximumWidth = p_sprite->m_maximumWidthFraction * m_viewportHalfWidth;
	if (SignedFloatBits(maximumWidth) < SignedFloatBits(halfWidth)) {
		scale = maximumWidth / halfWidth;
		halfWidth *= scale;
		halfHeight *= scale;
	}
	const LegoFloat right = screenX + halfWidth;
	const LegoFloat bottom = screenY + halfHeight;
	const LegoFloat left = screenX - halfWidth;
	const LegoFloat top = screenY - halfHeight;
	if (!(SignedFloatBits(m_viewportX) < SignedFloatBits(right) &&
		  SignedFloatBits(m_viewportY) < SignedFloatBits(bottom) &&
		  SignedFloatBits(left) < SignedFloatBits(m_viewportRight) &&
		  SignedFloatBits(top) < SignedFloatBits(m_viewportBottom))) {
		return;
	}
	RendererSpriteVertex* vertices = AllocateSpriteVertices();
	if (p_sprite->m_angleIndex == 0) {
		vertices[0].m_x = left;
		vertices[0].m_y = top;
		vertices[1].m_x = right;
		vertices[1].m_y = top;
		vertices[2].m_x = left;
		vertices[2].m_y = bottom;
		vertices[3].m_x = right;
		vertices[3].m_y = bottom;
	}
	else {
		LegoS32 angle = p_sprite->m_angleIndex;
		if (g_spriteSineTableInitialized == 0) {
			BuildSineTable(g_spriteSineTable, 256);
			g_spriteSineTableInitialized = 1;
		}
		LegoFloat sine = g_spriteSineTable[angle];
		LegoS32 cosineIndex = angle + 0x40;
		if (cosineIndex > 0xff) {
			cosineIndex = angle - 0xc0;
		}
		// Retail checks the lazy-init flag again before looking up cosine.
		if (g_spriteSineTableInitialized == 0) {
			BuildSineTable(g_spriteSineTable, 256);
			g_spriteSineTableInitialized = 1;
		}
		LegoFloat cosine = g_spriteSineTable[cosineIndex];
		LegoFloat cosineWidth = cosine * halfWidth;
		LegoFloat sineWidth = sine * halfWidth;
		LegoFloat cosineHeight = cosine * halfHeight;
		LegoFloat sineHeight = sine * halfHeight;
		vertices[0].m_x = (sineHeight - cosineWidth) + screenX;
		vertices[0].m_y = (-sineWidth - cosineHeight) + screenY;
		vertices[1].m_x = sineHeight + cosineWidth + screenX;
		vertices[1].m_y = (sineWidth - cosineHeight) + screenY;
		vertices[2].m_x = (-cosineWidth - sineHeight) + screenX;
		vertices[2].m_y = (cosineHeight - sineWidth) + screenY;
		vertices[3].m_x = (cosineWidth - sineHeight) + screenX;
		vertices[3].m_y = cosineHeight + sineWidth + screenY;
	}
	vertices[2].m_u = p_sprite->m_u0;
	vertices[0].m_u = p_sprite->m_u0;
	vertices[1].m_v = p_sprite->m_v0;
	vertices[0].m_v = p_sprite->m_v0;
	vertices[3].m_u = p_sprite->m_u1;
	vertices[1].m_u = p_sprite->m_u1;
	vertices[3].m_v = p_sprite->m_v1;
	vertices[2].m_v = p_sprite->m_v1;
	const LegoFloat reciprocalDepth = 1.0f / depth;
	for (LegoS32 index = 3; index >= 0; index--) {
		vertices[index].m_diffuse = p_sprite->m_color;
		vertices[index].m_z = depth;
		vertices[index].m_rhw = reciprocalDepth;
	}
}
