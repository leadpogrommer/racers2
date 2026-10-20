#include "frontend/input/menuinputbindinglabels.h"

#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "io/localization/localizedstringtable.h"

#include <vector>

// GLOBAL: LEGORACERS2 0x0055e658
MenuInputNameEntry g_menuInputMode3Names[16];

// GLOBAL: LEGORACERS2 0x0055e898
LegoFloat g_menuInputRowSpacing;

// GLOBAL: LEGORACERS2 0x0055e8a0
MenuInputNameEntry g_menuInputFallbackNames[2];

// GLOBAL: LEGORACERS2 0x0055e8e8
MenuInputTranslationEntry g_menuInputTranslations[28];

// GLOBAL: LEGORACERS2 0x0055ed50
MenuInputNameEntry g_menuInputMode1Names[256];

static const LegoU16 g_emptyMenuInputName[1] = {0};

static const LegoU16* ResolveMenuInputString(LegoS32 p_stringIndex)
{
	const LegoU16* text = g_emptyMenuInputName;
	if (p_stringIndex >= 0 && GetLocalizedStringTable() != NULL) {
		GetLocalizedStringTable()->GetString(p_stringIndex, &text);
	}
	return text;
}

// STUB: LEGORACERS2 0x004131e0
void InitializeMenuInputMode3Names()
{
	for (LegoU32 i = 0; i < 16; ++i) {
		g_menuInputMode3Names[i].m_text[0] = 0;
		g_menuInputMode3Names[i].m_stringIndex = 0x23c;
	}
}

// STUB: LEGORACERS2 0x00413310
void InitializeMenuInputMode1Names()
{
	static const struct MenuInputLiteral {
		LegoU16 m_slot;
		const char* m_text;
		LegoS32 m_stringIndex;
	} literals[] = {
		{1, "", 844},   {14, "~~BACK", -1}, {15, "", 845},  {18, "E", -1},  {19, "R", -1},   {20, "T", -1},
		{22, "U", -1},  {23, "I", -1},      {24, "O", -1},  {25, "P", -1},  {28, "~~X", -1}, {29, "", 846},
		{31, "S", -1},  {32, "D", -1},      {33, "F", -1},  {34, "G", -1},  {35, "H", -1},   {36, "J", -1},
		{37, "K", -1},  {38, "L", -1},      {42, "", 847},  {45, "X", -1},  {46, "C", -1},   {47, "V", -1},
		{48, "B", -1},  {49, "N", -1},      {54, "", 848},  {55, "", 849},  {56, "", 850},   {57, "", 851},
		{58, "", 852},  {69, "", 853},      {71, "", 854},  {72, "", 855},  {73, "", 856},   {74, "", 857},
		{75, "", 858},  {76, "", 859},      {77, "", 860},  {78, "", 861},  {79, "", 862},   {80, "", 863},
		{81, "", 864},  {82, "", 865},      {83, "", 866},  {141, "", 867}, {156, "", 868},  {157, "", 869},
		{179, "", 870}, {181, "", 871},     {184, "", 872}, {199, "", 873}, {200, "", 874},  {201, "", 875},
		{203, "", 876}, {205, "", 877},     {207, "", 878}, {208, "", 879}, {209, "", 880},  {210, "", 881},
		{211, "", 882},
	};
	LegoU32 i;
	for (i = 0; i < 256; ++i) {
		g_menuInputMode1Names[i].m_text[0] = 0;
		g_menuInputMode1Names[i].m_stringIndex = -1;
	}
	for (i = 0; i < sizeof(literals) / sizeof(literals[0]); ++i) {
		LegoU32 character;
		MenuInputNameEntry* entry = &g_menuInputMode1Names[literals[i].m_slot];
		entry->m_stringIndex = literals[i].m_stringIndex;
		for (character = 0; character < sizeof(entry->m_text) / sizeof(entry->m_text[0]); ++character) {
			entry->m_text[character] = (LegoU8) literals[i].m_text[character];
			if (literals[i].m_text[character] == 0) {
				break;
			}
		}
	}
}
// STUB: LEGORACERS2 0x00414b00
void InitializeMenuInputTranslations()
{
	g_menuInputTranslations[0]
		.InitializeInputNameTranslation(0x2, 0x31, 0x31, 0x31, 0x31, 0x26, 0x31, 0x31, 0x31, 0x31);
	g_menuInputTranslations[1]
		.InitializeInputNameTranslation(0x3, 0x32, 0x32, 0x32, 0x32, 0xe9, 0x32, 0x32, 0x32, 0x32);
	g_menuInputTranslations[2]
		.InitializeInputNameTranslation(0x4, 0x33, 0x33, 0x33, 0x33, 0x22, 0x33, 0x33, 0x33, 0x33);
	g_menuInputTranslations[3]
		.InitializeInputNameTranslation(0x5, 0x34, 0x34, 0x34, 0x34, 0x27, 0x34, 0x34, 0x34, 0x34);
	g_menuInputTranslations[4]
		.InitializeInputNameTranslation(0x6, 0x35, 0x35, 0x35, 0x35, 0x28, 0x35, 0x35, 0x35, 0x35);
	g_menuInputTranslations[5]
		.InitializeInputNameTranslation(0x7, 0x36, 0x36, 0x36, 0x36, 0x2d, 0x36, 0x36, 0x36, 0x36);
	g_menuInputTranslations[6]
		.InitializeInputNameTranslation(0x8, 0x37, 0x37, 0x37, 0x37, 0xe8, 0x37, 0x37, 0x37, 0x37);
	g_menuInputTranslations[7]
		.InitializeInputNameTranslation(0x9, 0x38, 0x38, 0x38, 0x38, 0x5f, 0x38, 0x38, 0x38, 0x38);
	g_menuInputTranslations[8]
		.InitializeInputNameTranslation(0xa, 0x39, 0x39, 0x39, 0x39, 0xe7, 0x39, 0x39, 0x39, 0x39);
	g_menuInputTranslations[9]
		.InitializeInputNameTranslation(0xb, 0x30, 0x30, 0x30, 0x30, 0xe0, 0x30, 0x30, 0x30, 0x30);
	g_menuInputTranslations[10]
		.InitializeInputNameTranslation(0xc, 0x2b, 0x2f, 0x2d, 0x2d, 0x29, 0xdf, 0x27, 0x27, 0x2b);
	g_menuInputTranslations[11]
		.InitializeInputNameTranslation(0xd, 0xb4, 0xb0, 0x3d, 0x3d, 0x3d, 0xb4, 0xec, 0xa1, 0xb4);
	g_menuInputTranslations[12]
		.InitializeInputNameTranslation(0x10, 0x71, 0x71, 0x71, 0x71, 0x61, 0x71, 0x71, 0x71, 0x71);
	g_menuInputTranslations[13]
		.InitializeInputNameTranslation(0x11, 0x77, 0x77, 0x77, 0x77, 0x7a, 0x77, 0x77, 0x77, 0x77);
	g_menuInputTranslations[14]
		.InitializeInputNameTranslation(0x15, 0x79, 0x79, 0x79, 0x79, 0x79, 0x7a, 0x79, 0x79, 0x79);
	g_menuInputTranslations[15]
		.InitializeInputNameTranslation(0x1a, 0xe5, 0xa8, 0x5b, 0x5b, 0x5e, 0xfc, 0xe8, 0x60, 0xe5);
	g_menuInputTranslations[16]
		.InitializeInputNameTranslation(0x1b, 0xa8, 0x2a, 0x5d, 0x5d, 0x24, 0x2b, 0x2b, 0x2b, 0xa8);
	g_menuInputTranslations[17]
		.InitializeInputNameTranslation(0x1e, 0x61, 0x61, 0x61, 0x61, 0x71, 0x61, 0x61, 0x61, 0x61);
	g_menuInputTranslations[18]
		.InitializeInputNameTranslation(0x27, 0xe6, 0x2b, 0x3b, 0x3b, 0x6d, 0xf6, 0xf2, 0xf1, 0xf6);
	g_menuInputTranslations[19]
		.InitializeInputNameTranslation(0x28, 0xf8, 0xb4, 0x27, 0x27, 0xf9, 0xe4, 0xe0, 0xb4, 0xe4);
	g_menuInputTranslations[20]
		.InitializeInputNameTranslation(0x29, 0xbd, 0x40, 0x60, 0x60, 0xb2, 0x5e, 0x5c, 0xba, 0xa7);
	g_menuInputTranslations[21]
		.InitializeInputNameTranslation(0x2b, 0x27, 0x3c, 0x23, 0x5c, 0x2a, 0x23, 0xf9, 0xe7, 0x27);
	g_menuInputTranslations[22]
		.InitializeInputNameTranslation(0x2c, 0x7a, 0x7a, 0x7a, 0x7a, 0x77, 0x79, 0x7a, 0x7a, 0x7a);
	g_menuInputTranslations[23]
		.InitializeInputNameTranslation(0x32, 0x6d, 0x6d, 0x6d, 0x6d, 0x2c, 0x6d, 0x6d, 0x6d, 0x6d);
	g_menuInputTranslations[24]
		.InitializeInputNameTranslation(0x33, 0x2c, 0x2c, 0x2c, 0x2c, 0x3b, 0x2c, 0x2c, 0x2c, 0x2c);
	g_menuInputTranslations[25]
		.InitializeInputNameTranslation(0x34, 0x2e, 0x2e, 0x2e, 0x2e, 0x3a, 0x2e, 0x2e, 0x2e, 0x2e);
	g_menuInputTranslations[26]
		.InitializeInputNameTranslation(0x35, 0x2d, 0x2d, 0x2f, 0x2f, 0x21, 0x2d, 0x2d, 0x2d, 0x2d);
	g_menuInputTranslations[27]
		.InitializeInputNameTranslation(0x56, 0x3c, 0x5d, 0x5c, 0x5c, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c);
}

// STUB: LEGORACERS2 0x004153d0
void MenuInputTranslationEntry::InitializeInputNameTranslation(
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
)
{
	LegoU32 languageValues[9];
	LegoU32 index;
	m_inputCode = p_inputCode;
	languageValues[0] = p_language0;
	languageValues[1] = p_language1;
	languageValues[2] = p_language2;
	languageValues[3] = p_language3;
	languageValues[4] = p_language4;
	languageValues[5] = p_language5;
	languageValues[6] = p_language6;
	languageValues[7] = p_language7;
	languageValues[8] = p_language8;
	for (index = 0; index < 9; ++index) {
		m_languageValues[index] = languageValues[index] & 0xff;
	}
}
// STUB: LEGORACERS2 0x004154f0
void InitializeMenuInputFallbackNames()
{
	static const char down[] = "~~DOWNDIR";
	static const char up[] = "~~UPDIR";
	LegoU32 i;
	for (i = 0; i < sizeof(down); ++i) {
		g_menuInputFallbackNames[0].m_text[i] = (LegoU8) down[i];
	}
	for (i = 0; i < sizeof(up); ++i) {
		g_menuInputFallbackNames[1].m_text[i] = (LegoU8) up[i];
	}
	g_menuInputFallbackNames[0].m_stringIndex = -1;
	g_menuInputFallbackNames[1].m_stringIndex = -1;
}

static int InitializeMenuInputLabelTablesAtStartup()
{
	InitializeMenuInputMode3Names();
	InitializeMenuInputMode1Names();
	InitializeMenuInputTranslations();
	InitializeMenuInputFallbackNames();
	InitializeMenuInputRowSpacing();
	return 0;
}

static int g_menuInputLabelTablesInitialized = InitializeMenuInputLabelTablesAtStartup();

// STUB: LEGORACERS2 0x004155b0
const LegoU16* GetMenuInputName(LegoS32 p_mode, const LegoS32* p_binding)
{
	const MenuInputNameEntry* entry;
	const LegoU16* text;
	LegoS32 index = p_binding[1];
	if (p_binding[0] != 0) {
		if (p_binding[0] < 1 || p_binding[0] > 2) {
			return ResolveMenuInputString(-1);
		}
		entry = &g_menuInputFallbackNames[p_binding[0] - 1];
	}
	else {
		if (index < 0) {
			// Retail reads the BSS word immediately before the selected table;
			// both observed -1 sentinels resolve to localized string zero.
			return ResolveMenuInputString(0);
		}
		const LegoU8* table = (const LegoU8*) ((p_mode == 3) ? g_menuInputMode3Names : g_menuInputMode1Names);
		entry = (const MenuInputNameEntry*) (table + index * sizeof(MenuInputNameEntry));
	}
	if (entry->m_stringIndex >= 0) {
		text = ResolveMenuInputString(entry->m_stringIndex);
		if (p_binding[0] == 0 && p_mode == 3 && (entry->m_stringIndex == 0x23c || entry->m_stringIndex == 0x390) &&
			index != -1) {
			static std::vector<LegoU16> indexedName;
			LegoU32 length = 0;
			while (text[length] != 0) {
				++length;
			}
			indexedName.resize(length + (index + 1 >= 10 ? 4 : 3));
			for (LegoU32 i = 0; i < length; ++i) {
				indexedName[i] = text[i];
			}
			indexedName[length++] = ' ';
			if (index + 1 >= 10) {
				indexedName[length++] = (LegoU16) ('0' + ((index + 1) / 10) % 10);
			}
			indexedName[length++] = (LegoU16) ('0' + (index + 1) % 10);
			indexedName[length] = 0;
			return &indexedName[0];
		}
		return text;
	}
	if (entry->m_text[0] != 0) {
		return entry->m_text;
	}
	else {
		SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
		LegoS32 language = secondary == NULL ? 0 : secondary->m_unk0x4cc;
		for (LegoU32 i = 0; i < 28; ++i) {
			if (g_menuInputTranslations[i].m_inputCode == index) {
				const LegoU8* translations = (const LegoU8*) g_menuInputTranslations;
				return (const LegoU16*) (translations + 4 + (language + (LegoS32) i * 10) * 2);
			}
		}
	}
	return entry->m_text;
}

// STUB: LEGORACERS2 0x00415740
void InitializeMenuInputRowSpacing()
{
	g_menuInputRowSpacing = 0.75f * 0.05f;
}
