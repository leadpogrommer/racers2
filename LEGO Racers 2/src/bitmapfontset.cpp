#include "bitmapfontset.h"

#include "memoryheap.h"
#include "renderer.h"

namespace
{

struct BitmapFontProfile {
	LegoU32 m_resourceKind;
	LegoS32 m_glyphWidth;
	LegoU32 m_caseMode;
	LegoFloat m_spacing;
	LegoFloat m_scale;
	LegoS32 m_nextCopySource;
};

const BitmapFontProfile g_bitmapFontProfiles[4] = {
	{0, 30, 1, 1.0f, 0.7f, -1},
	{1, 60, 1, 1.0f, 0.7f, 0},
	{(LegoU32) -1, 30, 1, 1.0f, 1.0f, 1},
	{(LegoU32) -1, 60, 1, -1.0f, 0.3f, 0},
};

const LegoChar g_smallFontPath[] = "game data\\fonts\\small_font.tga";
const LegoChar g_normalFontPath[] = "game data\\fonts\\normal_font.tga";
const LegoChar g_smallFontCharacterMap[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!\"?,.+=()[]#*\\'\x60;-/:"
	"\xc0\xc1\xc2\xc4\xc5\xc6\xdf\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd1\xd2\xd3\xd4\xd6\xd8\x8c"
	"\xd9\xda\xdb\xdc\xa1\xbf\xba\xbd\xb4\xa8^<$&\xb2_@\xa7";
const LegoChar g_normalFontCharacterMap[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!\"?,.+=()[]#*\\'\x60;-/:"
	"\xc0\xc1\xc2\xc4\xc5\xc6\xdf\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd1\xd2\xd3\xd4\xd6\xd8\x8c"
	"\xd9\xda\xdb\xdc\xa1\xbf\xba";

static void ReleaseFont(BitmapFont* p_font)
{
	if (p_font == NULL) {
		return;
	}

	for (LegoS32 index = p_font->m_textureCount - 1; index >= 0; index--) {
		g_displayRenderer->ReleaseTextureResourceReference(p_font->m_textureIds[index], 1);
	}
	FreeHeapMemory(p_font);
}

static void GetFontModeRange(LegoU32 p_mode, LegoS32* p_first, LegoS32* p_last)
{
	if (p_mode == 0) {
		*p_first = 0;
		*p_last = 3;
	}
	else if (p_mode == 2) {
		*p_first = 1;
		*p_last = 1;
	}
	else {
		*p_first = 0;
		*p_last = 2;
	}
}

} // namespace

// FUNCTION: LEGORACERS2 0x0042ce60
BitmapFontSet::BitmapFontSet()
{
	for (LegoU32 index = 0; index < 4; index++) {
		m_fonts[index] = NULL;
	}
}

// STUB: LEGORACERS2 0x0042ce80
BitmapFontSet::~BitmapFontSet()
{
	ReleaseAll();
}

// STUB: LEGORACERS2 0x0042cef0
void BitmapFontSet::Initialize(LegoU32 p_mode)
{
	LegoS32 first;
	LegoS32 last;
	GetFontModeRange(p_mode, &first, &last);

	for (LegoS32 index = first; index <= last; index++) {
		BitmapFont* font = m_fonts[index];
		if (font == NULL) {
			font = (BitmapFont*) AllocateHeapMemory(sizeof(BitmapFont));
			m_fonts[index] = font;

			LegoS32 copySource = (index == 0) ? -1 : g_bitmapFontProfiles[index - 1].m_nextCopySource;
			if (copySource == -1) {
				const BitmapFontProfile* profile = &g_bitmapFontProfiles[index];
				const LegoChar* path = (index == 0) ? g_smallFontPath : g_normalFontPath;
				font->Load(
					(LegoU32) index,
					path,
					profile->m_resourceKind == 0 ? g_smallFontCharacterMap : g_normalFontCharacterMap,
					profile->m_glyphWidth,
					profile->m_caseMode
				);
			}
			else {
				font->CopyFrom(2, m_fonts[copySource]);
			}
			font->m_unk0x10 = (LegoS32) g_bitmapFontProfiles[index].m_resourceKind;
		}

		const BitmapFontProfile* profile = &g_bitmapFontProfiles[index];
		font->m_scaleX = profile->m_scale * 0.0015625f;
		font->m_scaleY = profile->m_scale * 0.0020833334419876337f;
		font->m_spacing = profile->m_spacing;
	}
}

// STUB: LEGORACERS2 0x0042d020
void BitmapFontSet::Release(LegoU32 p_mode)
{
	LegoS32 first;
	LegoS32 last;
	GetFontModeRange(p_mode, &first, &last);

	for (LegoS32 index = last; index >= first; index--) {
		BitmapFont* font = m_fonts[index];
		if (font != NULL) {
			ReleaseFont(font);
			m_fonts[index] = NULL;
		}
	}
}

void BitmapFontSet::ReleaseAll()
{
	for (LegoS32 index = 3; index >= 0; index--) {
		BitmapFont* font = m_fonts[index];
		if (font != NULL) {
			ReleaseFont(font);
			m_fonts[index] = NULL;
		}
	}
}
