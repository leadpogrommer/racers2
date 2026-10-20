#include "render/draw/rendererquaddraw.h"

#include "render/core/renderer.h"
#include "render/draw/renderersprite.h"

#include <math.h>

extern HRESULT g_clearBuffersResult;

// STUB: LEGORACERS2 0x004df260
void DisplayRenderer::DrawGuiRectangle(const RendererGuiRectangle* p_rectangle, RendererDefaultRecord* p_defaults)
{
	RendererGuiRectangle clipped;
	const RendererGuiRectangle* selected = p_rectangle;
	LegoBool32 needsClipping = FALSE;
	if (p_rectangle->m_left < p_rectangle->m_clipLeft || p_rectangle->m_left != p_rectangle->m_left ||
		p_rectangle->m_clipLeft != p_rectangle->m_clipLeft || p_rectangle->m_top < p_rectangle->m_clipTop ||
		p_rectangle->m_top != p_rectangle->m_top || p_rectangle->m_clipTop != p_rectangle->m_clipTop ||
		p_rectangle->m_right > p_rectangle->m_clipRight || p_rectangle->m_bottom > p_rectangle->m_clipBottom) {
		needsClipping = TRUE;
	}
	if (needsClipping) {
		clipped = *p_rectangle;
		if (!ClipGuiRectangle(&clipped)) {
			return;
		}
		selected = &clipped;
	}
	const RendererGuiRectangle& rectangle = *selected;

	LegoU32 blendMode = rectangle.m_blendMode;
	if (blendMode == 7 && (rectangle.m_color >> 24) == 0xff) {
		blendMode = 0;
	}
	m_vtable->m_action0xc8(this, NULL, blendMode);

	if (m_cachedRenderStates[14] != 0) {
		m_cachedRenderStates[14] = 0;
		m_device->SetRenderState(D3DRS_ZWRITEENABLE, 0);
	}
	m_textureStageManager.ApplyStagePlanSubrecord(p_defaults, 0);

	RendererSpriteVertex vertices[4];
	LegoFloat screenX[4];
	LegoFloat screenY[4];
	union RotationBits {
		LegoFloat m_float;
		LegoU32 m_bits;
	} rotationBits;
	rotationBits.m_float = rectangle.m_rotation;
	if (rotationBits.m_bits == 0) {
		screenX[0] = rectangle.m_left * m_viewportWidth + m_viewportX;
		screenY[0] = rectangle.m_top * m_viewportHeight + m_viewportY;
		screenX[1] = rectangle.m_right * m_viewportWidth + m_viewportX;
		screenY[1] = screenY[0];
		screenX[2] = screenX[0];
		screenY[2] = rectangle.m_bottom * m_viewportHeight + m_viewportY;
		screenX[3] = screenX[1];
		screenY[3] = screenY[2];
	}
	else {
		const LegoFloat halfWidth = (rectangle.m_right - rectangle.m_left) * 0.5f;
		const LegoFloat halfHeight = (rectangle.m_bottom - rectangle.m_top) * 0.5f;
		const LegoFloat centerX = (rectangle.m_left + halfWidth) * m_viewportWidth + m_viewportX;
		const LegoFloat centerY = (rectangle.m_top + halfHeight) * m_viewportHeight + m_viewportY;
		const LegoFloat scaledHalfWidth = halfWidth * m_viewportWidth;
		const LegoFloat scaledHalfHeight = halfHeight * m_viewportHeight;
		const LegoFloat sine = (LegoFloat) sin(-(double) rectangle.m_rotation);
		const LegoFloat cosine = (LegoFloat) cos(-(double) rectangle.m_rotation);
		const LegoFloat xCosine = scaledHalfWidth * cosine;
		const LegoFloat ySine = scaledHalfHeight * sine;
		const LegoFloat xSine = scaledHalfWidth * sine;
		const LegoFloat yCosine = scaledHalfHeight * cosine;

		screenX[0] = centerX - xCosine + ySine;
		screenY[0] = centerY - xSine - yCosine;
		screenX[1] = centerX + xCosine + ySine;
		screenY[1] = centerY + xSine - yCosine;
		screenX[2] = centerX - xCosine - ySine;
		screenY[2] = centerY - xSine + yCosine;
		screenX[3] = centerX + xCosine - ySine;
		screenY[3] = centerY + xSine + yCosine;
	}

	for (LegoU32 i = 0; i < 4; i++) {
		vertices[i].m_x = screenX[i];
		vertices[i].m_y = screenY[i];
		vertices[i].m_z = 1.0f;
		vertices[i].m_rhw = 1.0f;
		vertices[i].m_diffuse = rectangle.m_color;
		vertices[i].m_specular = 0;
	}
	vertices[0].m_u = vertices[2].m_u = rectangle.m_textureLeft;
	vertices[1].m_u = vertices[3].m_u = rectangle.m_textureRight;
	vertices[0].m_v = vertices[1].m_v = rectangle.m_textureTop;
	vertices[2].m_v = vertices[3].m_v = rectangle.m_textureBottom;

	m_cachedStreamBuffer = NULL;
	if (m_cachedVertexShader != 0x1c4) {
		m_cachedVertexShader = 0x1c4;
		m_device->SetVertexShader(0x1c4);
	}
	g_clearBuffersResult = m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(RendererSpriteVertex));
}

// STUB: LEGORACERS2 0x004fb880
LegoBool32 ClipGuiRectangle(RendererGuiRectangle* p_rectangle)
{
	LegoFloat width = p_rectangle->m_right - p_rectangle->m_left;
	LegoFloat height = p_rectangle->m_bottom - p_rectangle->m_top;
	LegoFloat horizontalScale;
	LegoFloat verticalScale;

	if (p_rectangle->m_rotation != 0.0f && p_rectangle->m_rotation == p_rectangle->m_rotation) {
		return FALSE;
	}

	if (width > 0.0f) {
		horizontalScale = 1.0f / width;
	}
	else {
		horizontalScale = 0.0f;
	}

	if (height > 0.0f) {
		verticalScale = 1.0f / height;
	}
	else {
		verticalScale = 0.0f;
	}

	// The original unordered compare sets the carry bit for these lower-edge
	// checks. Preserve that by treating NaN on either side as a clipping case;
	// the resulting NaN fraction then follows the same rejected path.
	if (p_rectangle->m_left < p_rectangle->m_clipLeft || p_rectangle->m_left != p_rectangle->m_left ||
		p_rectangle->m_clipLeft != p_rectangle->m_clipLeft) {
		LegoFloat fraction = (p_rectangle->m_clipLeft - p_rectangle->m_left) * horizontalScale;
		if (!(fraction > 0.0f)) {
			return FALSE;
		}
		p_rectangle->m_left = p_rectangle->m_clipLeft;
		p_rectangle->m_textureLeft += (p_rectangle->m_textureRight - p_rectangle->m_textureLeft) * fraction;
	}

	if (p_rectangle->m_right > p_rectangle->m_clipRight) {
		LegoFloat fraction = (p_rectangle->m_right - p_rectangle->m_clipRight) * horizontalScale;
		if (!(fraction > 0.0f)) {
			return FALSE;
		}
		p_rectangle->m_right = p_rectangle->m_clipRight;
		p_rectangle->m_textureRight -= (p_rectangle->m_textureRight - p_rectangle->m_textureLeft) * fraction;
	}

	if (p_rectangle->m_top < p_rectangle->m_clipTop || p_rectangle->m_top != p_rectangle->m_top ||
		p_rectangle->m_clipTop != p_rectangle->m_clipTop) {
		LegoFloat fraction = (p_rectangle->m_clipTop - p_rectangle->m_top) * verticalScale;
		if (!(fraction > 0.0f)) {
			return FALSE;
		}
		p_rectangle->m_top = p_rectangle->m_clipTop;
		p_rectangle->m_textureTop += (p_rectangle->m_textureBottom - p_rectangle->m_textureTop) * fraction;
	}

	if (p_rectangle->m_bottom > p_rectangle->m_clipBottom) {
		LegoFloat fraction = (p_rectangle->m_bottom - p_rectangle->m_clipBottom) * verticalScale;
		if (!(fraction > 0.0f)) {
			return FALSE;
		}
		p_rectangle->m_bottom = p_rectangle->m_clipBottom;
		p_rectangle->m_textureBottom -= (p_rectangle->m_textureBottom - p_rectangle->m_textureTop) * fraction;
	}

	return TRUE;
}
