#ifndef LR2_MENUINPUTBINDINGLABELS_H
#define LR2_MENUINPUTBINDINGLABELS_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// The editor stores a UTF-16 label followed by its localized string index.
// The 36-byte stride and field offsets are observed in 0x004155b0 and
// 0x004167e0.
struct MenuInputNameEntry {
	LegoU16 m_text[16];
	LegoS32 m_stringIndex;
};
DECOMP_SIZE_ASSERT(MenuInputNameEntry, 0x24);
DECOMP_STATIC_ASSERT(offsetof(MenuInputNameEntry, m_stringIndex) == 0x20);

// The ten words per row are written by the input-name setup at 0x00414b00.
struct MenuInputTranslationEntry {
	void InitializeInputNameTranslation(
		LegoS32 p_inputCode,
		LegoU32 p_language0,
		LegoU32 p_language1,
		LegoU32 p_language2,
		LegoU32 p_language3,
		LegoU32 p_language4,
		LegoU32 p_language5,
		LegoU32 p_language6,
		LegoU32 p_language7,
		LegoU32 p_language8
	);

	LegoS32 m_inputCode;
	LegoU32 m_languageValues[9];
};
DECOMP_SIZE_ASSERT(MenuInputTranslationEntry, 0x28);

// Mode 3 reads the first table; the other observed capture mode reads the
// second. Names deliberately describe the mode split rather than guessing
// which physical device class each mode represents.
extern MenuInputNameEntry g_menuInputMode3Names[16];
extern MenuInputNameEntry g_menuInputMode1Names[256];
extern MenuInputTranslationEntry g_menuInputTranslations[28];
extern MenuInputNameEntry g_menuInputFallbackNames[2];

void InitializeMenuInputMode3Names();
void InitializeMenuInputMode1Names();
void InitializeMenuInputTranslations();
void InitializeMenuInputFallbackNames();
void InitializeMenuInputRowSpacing();

const LegoU16* GetMenuInputName(LegoS32 p_mode, const LegoS32* p_binding);

#endif
