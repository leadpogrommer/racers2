#ifndef LR2_GLOBALOVERLAYGLYPH_H
#define LR2_GLOBALOVERLAYGLYPH_H

#include "bitmapfont.h"
#include "types.h"

#include <stddef.h>

struct GlobalOverlayRecord;
struct GameControllerName;
struct GlobalOverlayGlyphReplacementManagerView;

struct GlobalOverlayStringEntry {
	LegoU16 m_key[16];
	LegoU16 m_text[16];
};
struct GlobalOverlayGlyphManagerDispatch {
	void(__fastcall* m_initialize)(GlobalOverlayGlyphReplacementManagerView*, void*);
	void(__fastcall* m_release)(GlobalOverlayGlyphReplacementManagerView*, void*);
};
extern const GlobalOverlayGlyphManagerDispatch g_overlayGlyphManagerDispatch;

struct GlobalOverlayGlyphEntryView {
	LegoU16 m_text[16];      // 0x00, inline prefix used as the replacement key
	LegoU32 m_texture;       // 0x20
	LegoFloat m_u;           // 0x24
	LegoFloat m_v;           // 0x28
	LegoFloat m_uEnd;        // 0x2c
	LegoFloat m_vEnd;        // 0x30
	LegoFloat m_widthScale;  // 0x34
	LegoFloat m_heightScale; // 0x38
};

struct GlobalOverlayIconTexture {
	LegoS32 m_texture;
	LegoFloat m_u, m_v, m_uEnd, m_vEnd;
};

struct GlobalOverlayGlyphReplacementManagerView {
	const GlobalOverlayGlyphManagerDispatch* m_vtable;
	GlobalOverlayGlyphEntryView* m_entries[20];    // 0x04
	LegoS32 m_entryCount;                          // 0x54
	GlobalOverlayStringEntry* m_strings[16];       // 0x58
	LegoS32 m_stringCount;                         // 0x98
	GlobalOverlayGlyphEntryView* m_iconAllocation; // 0x9c
	GlobalOverlayStringEntry* m_stringAllocation;  // 0xa0
	void Initialize();
	void Release();
	void LoadIconTextures();
	void ReleaseIconTextures();
	void SetPlayerName(const GameControllerName* p_name, LegoS32 p_index);
	void RegisterIcon(GlobalOverlayGlyphEntryView* p_entry);
	void SetIconTexture(LegoU32 p_index, const GlobalOverlayIconTexture* p_texture);
	LegoS32 GetIconTexture(LegoU32 p_index);
	void RegisterString(GlobalOverlayStringEntry* p_entry);
	void SetStringSlot(LegoS32 p_index, GlobalOverlayStringEntry* p_entry);
};
DECOMP_SIZE_ASSERT(GlobalOverlayGlyphEntryView, 0x3c);
DECOMP_SIZE_ASSERT(GlobalOverlayStringEntry, 0x40);
DECOMP_SIZE_ASSERT(GlobalOverlayGlyphReplacementManagerView, 0xa4);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphReplacementManagerView, m_stringAllocation) == 0xa0);

DECOMP_SIZE_ASSERT(GlobalOverlayIconTexture, 0x14);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphEntryView, m_texture) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphEntryView, m_widthScale) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphReplacementManagerView, m_entries) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphReplacementManagerView, m_entryCount) == 0x54);

LegoBool32 __fastcall HasGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength
);
void __fastcall ApplyGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	GlobalOverlayRecord* p_record,
	BitmapGlyphQuad* p_quad,
	LegoFloat* p_advance
);
LegoFloat __fastcall MeasureGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength,
	BitmapFont* p_font
);
LegoFloat __fastcall GetGlobalOverlayGlyphReplacementAdvance(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	BitmapFont* p_font
);

#endif // LR2_GLOBALOVERLAYGLYPH_H
