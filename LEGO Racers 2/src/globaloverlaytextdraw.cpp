#include "bitmapfont.h"
#include "globaloverlayglyph.h"
#include "globaloverlaymanager.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererquaddraw.h"

#include <string.h>

namespace
{

LegoFloat EvaluateFpuSine(LegoFloat p_angle)
{
	LegoFloat result;
	__asm {
		fld p_angle
		fsin
		fstp result
	}
	return result;
}

LegoS32 ConvertFpuRounded(LegoFloat p_value)
{
	LegoS32 result;
	__asm {
		fld p_value
		fistp result
	}
	return result;
}

void DrawGlyphQuad(
	const BitmapGlyphQuad* p_glyph,
	LegoFloat p_left,
	LegoFloat p_top,
	LegoFloat p_right,
	LegoFloat p_bottom,
	LegoFloat p_clipLeft,
	LegoFloat p_clipTop,
	LegoFloat p_clipRight,
	LegoFloat p_clipBottom,
	LegoU32 p_color,
	RendererDefaultRecord* p_defaults
)
{
	if (p_right < p_clipLeft || p_bottom < p_clipTop || p_left > p_clipRight || p_top > p_clipBottom) {
		return;
	}

	RendererGuiRectangle rectangle;
	LegoU32 rotationBits = 7;
	rectangle.m_left = p_left;
	rectangle.m_top = p_top;
	rectangle.m_right = p_right;
	rectangle.m_bottom = p_bottom;
	rectangle.m_clipLeft = p_clipLeft;
	rectangle.m_clipTop = p_clipTop;
	rectangle.m_clipRight = p_clipRight;
	rectangle.m_clipBottom = p_clipBottom;
	rectangle.m_opaque0x20[0] = 0;
	rectangle.m_opaque0x20[1] = 0;
	rectangle.m_color = p_color;
	rectangle.m_textureIndex = p_glyph->m_texture;
	rectangle.m_textureLeft = p_glyph->m_u;
	rectangle.m_textureTop = p_glyph->m_v;
	rectangle.m_textureRight = p_glyph->m_uEnd;
	rectangle.m_textureBottom = p_glyph->m_vEnd;
	memcpy(&rectangle.m_rotation, &rotationBits, sizeof(rotationBits));
	rectangle.m_blendMode = 0;
	rectangle.m_opaque0x48[0] = 0;
	rectangle.m_opaque0x48[1] = 0;
	rectangle.m_opaque0x48[2] = 0;
	p_defaults->m_slots[0].m_mask = (LegoU16) p_color;

	RendererMainDispatch* dispatch = (RendererMainDispatch*) g_displayRenderer->m_vtable;
	dispatch->m_drawGuiRectangle(g_displayRenderer, NULL, &rectangle, p_defaults);
}

} // namespace

// STUB: LEGORACERS2 0x004b3fd0
void __fastcall DrawGlobalOverlayText(GlobalOverlayRecord* p_record, void*, RendererDefaultRecord* p_defaults)
{
	BitmapFont* font = (BitmapFont*) p_record->m_font;
	LegoU32 flags = p_record->m_animationFlags;
	LegoFloat localScaleX = (flags & 8) != 0 ? p_record->m_scale : 1.0f;
	LegoFloat localScaleY = localScaleX;
	LegoBool32 scaleGlyphs = (flags & 8) != 0 ? TRUE : FALSE;
	LegoFloat x = p_record->m_positionX;
	LegoFloat y = p_record->m_positionY;
	LegoFloat baseX = x;
	LegoFloat lineAdvance = *(LegoFloat*) ((LegoU8*) font + 0x20) * *(LegoFloat*) ((LegoU8*) font + 0x18);
	LegoFloat spaceAdvance = *(LegoFloat*) ((LegoU8*) font + 0x28) * font->m_scaleX;
	LegoFloat digitAdvance = (font->m_spacing + *(LegoFloat*) ((LegoU8*) font + 0x24)) * font->m_scaleX;
	if (p_record->m_type != 0) {
		if (g_globalOverlayManager.m_unk0x24944 == 0) {
			localScaleX += localScaleX;
		}
		else {
			localScaleY += localScaleY;
		}
		scaleGlyphs = TRUE;
	}
	lineAdvance *= localScaleY;
	spaceAdvance *= localScaleX;
	digitAdvance *= localScaleX;

	LegoFloat clipLeft = p_record->m_clipLeft;
	LegoFloat clipTop = p_record->m_clipTop;
	LegoFloat clipRight = p_record->m_clipRight;
	LegoFloat clipBottom = p_record->m_clipBottom;
	LegoU32 color = p_record->m_color;
	if ((flags & 0x40) != 0) {
		LegoFloat alpha = EvaluateFpuSine(3.0f * 3.14159265358979323846f * p_record->m_animationTime) * 95.0f + 160.0f;
		LegoS32 roundedAlpha = ConvertFpuRounded(alpha);
		color = (color & 0x00ffffff) | (((LegoU32) roundedAlpha & 0xff) << 24);
	}

	LegoU32 characterIndex = 0;
	LegoU32 scheduleIndex = 0;
	LegoU32 scheduledCharacter = p_record->m_textLength == 0 ? 0xffffffff : p_record->m_characterAnimationIndices[0];
	while (p_record->m_text[characterIndex] != 0) {
		LegoU16 character = p_record->m_text[characterIndex];
		if (character == 0x0a) {
			y += lineAdvance;
			x = baseX;
		}
		else if (character == 0x0d) {
			x = baseX;
		}
		else if (character == 0x20) {
			x += spaceAdvance;
		}
		else {
			BitmapGlyphQuad glyph;
			glyph.m_left = 0.0f;
			glyph.m_top = 0.0f;
			LegoFloat advance = 0.0f;
			LegoBool32 hasReplacement = FALSE;
			if (p_record->m_textLength != 0 && characterIndex == scheduledCharacter) {
				++scheduleIndex;
				if (scheduleIndex < (LegoU32) p_record->m_textLength) {
					scheduledCharacter = p_record->m_characterAnimationIndices[scheduleIndex];
				}
				else {
					scheduledCharacter = 0xffffffff;
				}

				GlobalOverlayGlyphReplacementManagerView* replacements =
					(GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c;
				if (replacements != NULL) {
					ApplyGlobalOverlayGlyphReplacement(
						replacements,
						NULL,
						(LegoU32) character - 0xd903,
						p_record,
						&glyph,
						&advance
					);
					hasReplacement = TRUE;
				}
			}
			if (hasReplacement == FALSE) {
				font->GetGlyphMetrics(character, &glyph, &advance);
			}
			LegoFloat glyphLeft = x;
			LegoFloat glyphTop = y;
			LegoFloat glyphRight = x + glyph.m_right;
			LegoFloat glyphBottom = y + glyph.m_bottom;
			if ((flags & 0x20) != 0) {
				LegoFloat waveInput =
					p_record->m_animationTime * p_record->m_elapsed + (LegoFloat) characterIndex * 4.0f;
				LegoFloat wave = EvaluateFpuSine(waveInput) * 0.01f;
				if (scaleGlyphs != FALSE) {
					wave *= localScaleY;
				}
				glyphTop += wave;
			}
			if (scaleGlyphs != FALSE) {
				advance *= localScaleX;
				glyphRight = glyphLeft + (glyphRight - glyphLeft) * localScaleX;
				glyphBottom = glyphTop + (glyphBottom - glyphTop) * localScaleY;
			}

			if ((flags & 4) != 0 && character >= '0' && character <= '9') {
				LegoFloat halfDifference = (digitAdvance - advance) * 0.5f;
				glyphLeft += halfDifference;
				glyphRight += halfDifference;
				advance = digitAdvance;
			}

			DrawGlyphQuad(
				&glyph,
				glyphLeft,
				glyphTop,
				glyphRight,
				glyphBottom,
				clipLeft,
				clipTop,
				clipRight,
				clipBottom,
				color,
				p_defaults
			);
			x += advance;
		}
		++characterIndex;
	}
}
