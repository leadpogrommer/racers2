#include "bitmapfont.h"

#include "bitmaputil.h"
#include "renderer.h"
#include "rendererrecords.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x004c3db0
LegoBool32 BitmapFont::GetGlyphMetrics(LegoU16 p_character, BitmapGlyphQuad* p_quad, LegoFloat* p_advance)
{
	BitmapFontGlyph* glyph = &m_glyphs[m_characterMap[p_character]];
	p_quad->m_texture = glyph->m_texture;
	p_quad->m_u = glyph->m_u;
	p_quad->m_v = glyph->m_v;
	p_quad->m_uEnd = glyph->m_uExtent + glyph->m_u;
	p_quad->m_vEnd = glyph->m_vExtent + glyph->m_v;
	*p_advance = (m_spacing + glyph->m_width) * m_scaleX;
	p_quad->m_right = glyph->m_width * m_scaleX + p_quad->m_left;
	p_quad->m_bottom = glyph->m_height * m_scaleY + p_quad->m_top;
	return TRUE;
}

// STUB: LEGORACERS2 0x004c3ed0
void BitmapFont::CopyFrom(LegoU32 p_resourceType, const BitmapFont* p_source)
{
	memcpy(this, p_source, sizeof(BitmapFont));
	m_fontIndex = p_resourceType;

	for (LegoS32 index = 0; index < m_textureCount; index++) {
		RendererResourceRef* reference = g_displayRenderer->FindTextureReferenceById(m_textureIds[index], 1);
		if (reference != NULL) {
			reference->m_referenceCount++;
		}
	}
}
