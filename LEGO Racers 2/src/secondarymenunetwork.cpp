#include "gamecontroller.h"
#include "globaloverlaymanager.h"
#include "secondarymenuinteraction.h"

LegoBool32 __fastcall IsMenuOptionEnabled(MenuManager*, void*, LegoS32);
void __fastcall HighlightMenuOption(MenuManager*, void*, LegoS32, LegoU32);

// GLOBAL: LEGORACERS2 0x00561700
LegoU32 g_menuDisabledColor;

// STUB: LEGORACERS2 0x0041c270
void InitializeMenuDisabledColor()
{
	g_menuDisabledColor = 0x80ffffff;
}

struct MenuDisabledColorInitializer {
	MenuDisabledColorInitializer() { InitializeMenuDisabledColor(); }
};
static MenuDisabledColorInitializer g_menuDisabledColorInitializer;

// STUB: LEGORACERS2 0x00422d60
void __fastcall RefreshMenuNetworkList(MenuManager* p_menu, void*)
{
	LegoS32 previous =
		p_menu->m_unk0x198 == 0 ? -1 : *(LegoS32*) ((LegoU8*) p_menu + (p_menu->m_unk0x198 * 5 + 0x14) * 4);
	LegoS32 selected = -1;
	LegoU16 oldName[24];
	if (previous < 0 || previous > 4) {
		oldName[0] = 0;
		selected = previous;
	}
	else {
		const LegoU16* from = ((GlobalOverlayRecord*) p_menu->m_overlayRecords[previous])->m_text;
		LegoU16* to = oldName;
		do {
			*to++ = *from;
		} while (*from++ != 0);
	}
	LegoBool32 found = FALSE;
	const LegoU8* network = (const LegoU8*) g_gameController.m_networkApp;
	static const LegoChar emptyName[] = "----------";
	for (LegoS32 i = 0; i < 5; ++i) {
		const LegoChar* name = (const LegoChar*) network + 0x89c + i * 0x8c;
		if (*(const LegoS32*) (network + 0xb58) <= i || name == NULL) {
			name = emptyName;
		}
		LegoU16 wideName[24];
		const LegoU8* source = (const LegoU8*) name;
		LegoU16* destination = wideName;
		do {
			*destination++ = *source;
		} while (*source++ != 0);
		LegoBool32 unchanged = TRUE;
		if (!found && name != emptyName) {
			const LegoU16* a = oldName;
			const LegoU16* b = wideName;
			while (*a == *b) {
				if (*a == 0) {
					found = TRUE;
					selected = i;
					break;
				}
				++a;
				++b;
			}
			if (!found) {
				unchanged = FALSE;
			}
		}
		LegoU32 color;
		if (name == emptyName) {
			*(LegoU32*) ((LegoU8*) p_menu + 0x1f0 + i * 4) = 1;
			color = g_menuDisabledColor;
		}
		else {
			*(LegoU32*) ((LegoU8*) p_menu + 0x1f0 + i * 4) = 0;
			color = -1;
		}
		if (!unchanged || previous != i) {
			GlobalOverlayRecord* record = (GlobalOverlayRecord*) p_menu->m_overlayRecords[i];
			record->InitializeText(wideName, 1, 0.0f);
			record->SetColor(&color);
		}
	}
	if (selected == -1) {
		for (LegoS32 j = 0; j < 6; ++j) {
			if (IsMenuOptionEnabled(p_menu, NULL, j)) {
				selected = j;
				break;
			}
		}
	}
	*(LegoS32*) ((LegoU8*) p_menu + (p_menu->m_unk0x198 + 4) * 0x14) = selected;
	HighlightMenuOption(p_menu, NULL, previous, 0);
	HighlightMenuOption(p_menu, NULL, selected, 1);
}
