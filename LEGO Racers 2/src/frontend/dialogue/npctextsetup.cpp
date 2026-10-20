#include "audio/service/audioservice.h"
#include "core/strings/charbuffer24.h"
#include "frontend/dialogue/npctextmanager.h"
#include "frontend/input/menuinputbindinglabels.h"
#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/overlay/overlayrecords.h"
#include "frontend/overlay/textboxmanager.h"
#include "frontend/resources/controllereventrecords.h"
#include "game/controller/gamecontroller.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/session/gamecontrollerplayers.h"
#include "game/session/gamecontrollersession.h"
#include "render/fonts/bitmapfont.h"
#include "render/fonts/bitmapfontset.h"

#include <string.h>

extern LegoU32 g_menuChoiceColorA;

// GLOBAL: LEGORACERS2 0x00543728
const NpcTextSpeaker g_npcTextSpeakers[63] = {
	{333, 81, 2, "sparky", -2},
	{487, -1, 2, "mr cunningham", -2},
	{488, 52, 1, "pippin", -2},
	{489, -1, 2, "baron rosso", -2},
	{226, 56, 2, "johnny thunder", -2},
	{490, -1, 3, "cassiopeia", -2},
	{491, -1, 2, "mac", -2},
	{492, -1, 2, "frankie solido", -2},
	{210, 65, 2, "workman2", 6},
	{205, 63, 2, "postman", 5},
	{206, 62, 2, "fireman", 4},
	{207, 64, 2, "policeman", 0},
	{204, 67, 2, "workman1", 3},
	{211, 69, 2, "workman3", 3},
	{203, 66, 2, "professor", -1},
	{212, 68, 2, "foreman", -1},
	{208, 60, 2, "doctor", -1},
	{209, 61, 1, "nurse", -1},
	{213, 59, 2, "lighthouse keeper", -1},
	{214, 71, 1, "Jan", -1},
	{215, 72, 1, "Gillian", -1},
	{216, 73, 1, "Suzie", -1},
	{217, 70, 2, "Steve", 1},
	{218, 78, 2, "generic male1", -1},
	{219, 79, 2, "generic male2", -1},
	{220, 80, 2, "generic male3", -1},
	{221, 75, 1, "generic female1", -1},
	{222, 76, 1, "generic female2", -1},
	{223, 77, 1, "generic female3", -1},
	{224, 58, 2, "coastguard", 2},
	{493, 74, 2, "fisherman", -1},
	{494, 57, 2, "sam sanister", 15},
	{495, 53, 2, "slyboots", 16},
	{496, 51, 2, "achu", 18},
	{497, 54, 2, "tribesman1", 21},
	{498, 55, 2, "tribesman2", 19},
	{225, 50, 1, "alexis sanister", 20},
	{499, 99, 2, "mike", 17},
	{500, 89, 0, "", -1},
	{501, 86, 2, "eskimo1", 7},
	{502, 87, 2, "eskimo2", 8},
	{503, 85, 2, "ross", 11},
	{504, 84, 2, "crystal", 13},
	{505, 83, 2, "cosmo", 9},
	{506, 88, 2, "scooter", 12},
	{263, 82, 2, "arctic doc", 10},
	{265, 94, 3, "rigel", 23},
	{262, 93, 2, "bb", 24},
	{507, 92, 2, "doc", 25},
	{264, 90, 3, "vega", 30},
	{266, 91, 3, "altair", 27},
	{508, 97, 3, "scientist", 28},
	{509, 98, 3, "antares", 29},
	{510, 95, 2, "rocket racer", 37},
	{511, -1, 4, "lightor", 42},
	{512, 96, 4, "warrior", 44},
	{513, -1, 4, "halloween", 41},
	{514, -1, 4, "xmas2", 38},
	{515, -1, 4, "beige racer", 39},
	{516, -1, 4, "xmas1", 40},
	{517, -1, 4, "green racer", 43},
	{-1, -1, 2, "", -1},
	{-1, -1, 2, "", -1},
};

// GLOBAL: LEGORACERS2 0x0056bfc8
static LegoFloat g_npcTextContinueOffset[2];
// GLOBAL: LEGORACERS2 0x0056bfd0
static LegoFloat g_npcTextContinueSize[2];
// GLOBAL: LEGORACERS2 0x0056bfd8
static LegoU16 g_npcTextWord[36];
static LegoFloat* GetNpcTextLineOffset()
{
	return &g_textBoxFormattingState.m_rightOffset;
}
static LegoFloat* GetNpcTextPortraitSize()
{
	return &g_textBoxFormattingState.m_leftOffset;
}

static void InitializeNpcTextBorderSize();
static void InitializeNpcTextPortraitSize();
static void InitializeNpcTextPosition();
static void InitializeNpcTextLineOffset();
static void InitializeNpcTextContinueSize();
static void InitializeNpcTextContinueOffset();
static LegoFloat __fastcall WrapNpcTextLines(NestedTextBoxManagerPrefix*, void*, const LegoU16*, const LegoFloat*);
static void __stdcall NormalizeNpcTextChunk(const LegoU16*, LegoU16*, LegoS32, LegoS32*, LegoS32*);
static LegoS32 __fastcall ExpandNpcTextInputBinding(
	NestedTextBoxManagerPrefix*,
	void*,
	const LegoU16*,
	const LegoU16**
);
static void __fastcall StartNpcTextVoice(NestedTextBoxManagerPrefix*, void*);

static LegoFloat* TextBoxFormattingFloats()
{
	return (LegoFloat*) &g_textBoxFormattingState;
}

static void InitializeNpcTextTuning()
{
	static LegoBool32 initialized;
	if (!initialized) {
		InitializeNpcTextBorderSize();
		InitializeNpcTextPortraitSize();
		InitializeNpcTextPosition();
		InitializeNpcTextLineOffset();
		InitializeNpcTextContinueSize();
		InitializeNpcTextContinueOffset();
		initialized = TRUE;
	}
}

// STUB: LEGORACERS2 0x00415570
static const LegoU16* __stdcall GetNpcTextBindingName(const GameControllerPairGroups* p_bindings, LegoS32 p_index)
{
	const LegoS32* pair =
		(const LegoS32*) ((const LegoU8*) p_bindings + (p_bindings->m_enabled == 3 ? 0x64 : 0x1c) + p_index * 8);
	return GetMenuInputName(p_bindings->m_enabled, pair);
}

// STUB: LEGORACERS2 0x004359a0
LegoS32 __stdcall GetNpcTextPlayerPortrait(const LegoU32* p_parts)
{
	static const LegoS32 rowOffsets[8] = {7, 6, 4, 3, 2, 5, 1, 8};
	const LegoU8* parts = (const LegoU8*) g_gameController.m_bodypartManager;
	const LegoS32* values = (const LegoS32*) p_parts;
	LegoS32 first = values[0] - *(const LegoS32*) (parts + 0x458) + 8;
	LegoS32 second = values[1] - *(const LegoS32*) (parts + 0x470) + 8;
	if (first < 0 || first >= 8) {
		first = 0;
	}
	if (second < 0 || second >= 8) {
		second = 0;
	}
	return second + rowOffsets[first] * 8 + 0x5c;
}

// STUB: LEGORACERS2 0x00437860
static void InitializeNpcTextBorderSize()
{
	TextBoxFormattingFloats()[2] = 0.0225000009f;
	TextBoxFormattingFloats()[3] = 0.0298999995f;
}

// STUB: LEGORACERS2 0x00437890
static void InitializeNpcTextPortraitSize()
{
	GetNpcTextPortraitSize()[0] = 0.110399999f;
	GetNpcTextPortraitSize()[1] = 0.122400001f;
}
// STUB: LEGORACERS2 0x004378c0
static void InitializeNpcTextPosition()
{
	TextBoxFormattingFloats()[0] = 0.0146000003f;
	TextBoxFormattingFloats()[1] = 0.0182000007f;
}
// STUB: LEGORACERS2 0x004378f0
static void InitializeNpcTextLineOffset()
{
	GetNpcTextLineOffset()[0] = 0.01f;
	GetNpcTextLineOffset()[1] = 0.02f;
}
// FUNCTION: LEGORACERS2 0x00437920
static void InitializeNpcTextContinueSize()
{
	g_npcTextContinueSize[0] = 0.0186000001f;
	g_npcTextContinueSize[1] = 0.0220999997f;
}
// FUNCTION: LEGORACERS2 0x00437950
static void InitializeNpcTextContinueOffset()
{
	g_npcTextContinueOffset[0] = 0.0f;
	g_npcTextContinueOffset[1] = 0.01f;
}

// STUB: LEGORACERS2 0x00437dc0
void __fastcall InitializeNestedNpcTextBox(
	NestedTextBoxManagerPrefix* p_nested,
	void*,
	const LegoU16* p_text,
	const LegoU32* p_originWords,
	const LegoU32* p_sizeWords,
	LegoU32 p_mode,
	LegoU32 p_textStyle,
	LegoU32 p_visibleLines,
	LegoU32 p_style
)
{
	InitializeNpcTextTuning();
	LegoU8* bytes = (LegoU8*) p_nested;
	const LegoFloat* origin = (const LegoFloat*) p_originWords;
	LegoFloat size[2] = {((const LegoFloat*) p_sizeWords)[0], ((const LegoFloat*) p_sizeWords)[1]};
	p_nested->m_selected = p_mode;
	p_nested->m_active = 1;
	p_nested->m_unk0x60 = 0;
	p_nested->m_unk0x64 = p_textStyle;
	p_nested->m_unk0x68 = p_style;
	*(LegoFloat*) (bytes + 0x18d4) = 0.0f;
	*(LegoFloat*) (bytes + 0x18d8) = 0.0f;
	*(LegoFloat*) (bytes + 0x18cc) = origin[0];
	*(LegoFloat*) (bytes + 0x18d0) = origin[1];
	*(LegoU32*) (bytes + 0x18c0) = p_visibleLines;
	*(LegoU32*) (bytes + 0x18c8) = 0;
	*(LegoS32*) (bytes + 0xbc) = -1;
	p_nested->m_selectedResource = -1;

	LegoU32 spriteParameters[21] = {0};
	LegoFloat* spriteFloats = (LegoFloat*) spriteParameters;
	spriteFloats[2] = TextBoxFormattingFloats()[2];
	spriteFloats[3] = TextBoxFormattingFloats()[3];
	spriteFloats[6] = spriteFloats[7] = 1.0f;
	spriteParameters[10] = -1;
	spriteParameters[18] = 7;
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	for (LegoS32 i = 0; i < 9; ++i) {
		memcpy(spriteParameters + 11, &resources->m_records[40 + i], sizeof(ControllerEventRecord));
		if (p_nested->m_sprites0x7c[i] == NULL) {
			p_nested->m_sprites0x7c[i] = g_globalOverlayManager.AllocateSprite(spriteParameters);
		}
		p_nested->m_sprites0x7c[i]->SetType(5);
	}
	if (p_nested->m_sprites0x7c[9] == NULL) {
		p_nested->m_sprites0x7c[9] = g_globalOverlayManager.AllocateSprite(spriteParameters);
	}
	p_nested->m_sprites0x7c[9]->SetType(6);
	memcpy(spriteParameters + 11, &resources->m_records[49], sizeof(ControllerEventRecord));
	if (p_nested->m_sprites0x7c[10] == NULL) {
		p_nested->m_sprites0x7c[10] = g_globalOverlayManager.AllocateSprite(spriteParameters);
	}
	p_nested->m_sprites0x7c[10]->SetType(6);

	LegoU32 textParameters[13] = {0};
	textParameters[0] = (LegoU32) g_gameController.m_fontSet->m_fonts[0];
	textParameters[1] = -1;
	textParameters[6] = textParameters[7] = textParameters[10] = 0x3f800000;
	textParameters[9] = 6;
	if (p_nested->m_record0xa8 == NULL) {
		p_nested->m_record0xa8 = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	}
	for (LegoS32 j = 0; j < 4; ++j) {
		if (p_nested->m_records0xac[j] == NULL) {
			p_nested->m_records0xac[j] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
		}
	}
	LegoFloat height =
		g_npcTextContinueOffset[1] + g_npcTextContinueSize[1] + WrapNpcTextLines(p_nested, NULL, p_text, size);
	LegoFloat minimumHeight = TextBoxFormattingFloats()[1] + TextBoxFormattingFloats()[1] + GetNpcTextPortraitSize()[1];
	if (height < minimumHeight) {
		height = minimumHeight;
	}
	if (size[1] <= 0.0f) {
		size[1] = height;
	}
	LegoFloat namePosition[2] = {
		TextBoxFormattingFloats()[0] + GetNpcTextPortraitSize()[0] + *(LegoFloat*) (bytes + 0x18cc) +
			GetNpcTextLineOffset()[0],
		*(LegoFloat*) (bytes + 0x18d0) + GetNpcTextLineOffset()[1]
	};
	if (p_mode == 0x3d) {
		p_nested->m_record0xa8->InitializeNarrowText("~~1UP", 0, 0.0f);
	}
	else if (p_mode == 0x3e) {
		p_nested->m_record0xa8->InitializeNarrowText("~~2UP", 0, 0.0f);
	}
	else {
		p_nested->m_record0xa8->InitializeTextByIndex(g_npcTextSpeakers[p_mode].m_stringIndex, 0, 0.0f);
	}
	p_nested->m_record0xa8->SetPosition(namePosition, -1);
	p_nested->m_record0xa8->SetColor(&g_menuChoiceColorA);
	LegoS32 portrait;
	if (p_mode == 0x3d || p_mode == 0x3e) {
		const LegoU32* parts =
			(const LegoU32*) ((LegoU8*) GetGameControllerSession() + 0x370 + (p_mode - 0x3d) * 0x344);
		portrait = GetNpcTextPlayerPortrait(parts);
	}
	else {
		portrait = g_npcTextSpeakers[p_mode].m_portrait;
	}
	GlobalOverlaySprite* portraitSprite = p_nested->m_sprites0x7c[9];
	if (portrait == -1) {
		portraitSprite->SetVisible(0);
	}
	else {
		LegoFloat portraitPosition[2] = {
			origin[0] + TextBoxFormattingFloats()[0],
			origin[1] + TextBoxFormattingFloats()[1]
		};
		memcpy(&portraitSprite->m_texture, &resources->m_records[portrait], sizeof(ControllerEventRecord));
		portraitSprite->SetPosition(portraitPosition);
		portraitSprite->SetSize(GetNpcTextPortraitSize());
	}
	LegoFloat position[2] = {origin[0], origin[1]};
	p_nested->m_sprites0x7c[0]->SetPosition(position);
	position[0] += size[0] - TextBoxFormattingFloats()[2];
	p_nested->m_sprites0x7c[1]->SetPosition(position);
	position[1] += size[1] - TextBoxFormattingFloats()[3];
	p_nested->m_sprites0x7c[2]->SetPosition(position);
	position[0] = origin[0];
	p_nested->m_sprites0x7c[3]->SetPosition(position);
	position[1] = origin[1];
	LegoFloat stripSize[2] = {
		size[0] - (TextBoxFormattingFloats()[2] + TextBoxFormattingFloats()[2]),
		TextBoxFormattingFloats()[3]
	};
	position[0] = origin[0] + TextBoxFormattingFloats()[2];
	p_nested->m_sprites0x7c[4]->SetPosition(position);
	p_nested->m_sprites0x7c[4]->SetSize(stripSize);
	position[1] += size[1] - TextBoxFormattingFloats()[3];
	p_nested->m_sprites0x7c[5]->SetPosition(position);
	p_nested->m_sprites0x7c[5]->SetSize(stripSize);
	position[0] = origin[0];
	stripSize[1] = size[1] - (TextBoxFormattingFloats()[3] + TextBoxFormattingFloats()[3]);
	stripSize[0] = TextBoxFormattingFloats()[2];
	position[1] = origin[1] + TextBoxFormattingFloats()[3];
	p_nested->m_sprites0x7c[6]->SetPosition(position);
	p_nested->m_sprites0x7c[6]->SetSize(stripSize);
	position[0] += size[0] - TextBoxFormattingFloats()[2];
	p_nested->m_sprites0x7c[7]->SetPosition(position);
	p_nested->m_sprites0x7c[7]->SetSize(stripSize);
	stripSize[0] = size[0] - (TextBoxFormattingFloats()[2] + TextBoxFormattingFloats()[2]);
	stripSize[1] = size[1] - (TextBoxFormattingFloats()[3] + TextBoxFormattingFloats()[3]);
	position[0] = origin[0] + TextBoxFormattingFloats()[2];
	position[1] = origin[1] + TextBoxFormattingFloats()[3];
	p_nested->m_sprites0x7c[8]->SetPosition(position);
	p_nested->m_sprites0x7c[8]->SetSize(stripSize);
	LegoFloat continuePosition[2] = {
		(size[0] - g_npcTextContinueSize[0]) * 0.5f + origin[0] + g_npcTextContinueOffset[0],
		(size[1] - g_npcTextContinueSize[1] - g_npcTextContinueOffset[1]) + origin[1]
	};
	p_nested->m_sprites0x7c[10]->SetPosition(continuePosition);
	p_nested->m_sprites0x7c[10]->SetSize(g_npcTextContinueSize);
	StartNpcTextVoice(p_nested, NULL);
}

// STUB: LEGORACERS2 0x004385c0
static LegoFloat __fastcall WrapNpcTextLines(
	NestedTextBoxManagerPrefix* p_nested,
	void*,
	const LegoU16* p_text,
	const LegoFloat* p_size
)
{
	if (p_text == NULL) {
		return 0.0f;
	}
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[0];
	LegoFloat maximumWidth =
		((p_size[0] - (GetNpcTextLineOffset()[0] + GetNpcTextLineOffset()[0])) - TextBoxFormattingFloats()[0]) -
		GetNpcTextPortraitSize()[0];
	LegoFloat lineHeight = font->m_unk0x20 * font->m_scaleY;
	LegoFloat spaceWidth = font->m_unk0x28 * font->m_scaleX;
	LegoU16* normalized = g_textBoxFormattingState.m_normalizedText;
	LegoS32 consumed, normalizedLength;
	NormalizeNpcTextChunk(p_text, normalized, 0x100, &consumed, &normalizedLength);
	LegoU8* bytes = (LegoU8*) p_nested;
	*(LegoS32*) (bytes + 0x18c4) = 0;
	const LegoU16* nextChunk = p_text + consumed;
	LegoU16* output = (LegoU16*) (bytes + 0xc0);
	LegoU16* wordOutput = g_npcTextWord;
	const LegoU16* cursor = normalized;
	const LegoU16* resume = NULL;
	LegoFloat lineWidth = 0.0f;
	LegoFloat wordWidth = 0.0f;
	LegoS32 words = 0;
	LegoBool32 replacement = FALSE;
	LegoBool32 newLine = FALSE;
	LegoBool32 finished = FALSE;
	for (;;) {
		if (cursor[0] == 'X' && cursor[1] == 'X' && cursor[2] == 'X' && !replacement) {
			resume = cursor + 3;
			replacement = TRUE;
			cursor = GetGameControllerSettings()->m_names[p_nested->m_unk0x68].m_text;
		}
		else {
			if (*cursor == '~' && !replacement) {
				const LegoU16* expanded = NULL;
				LegoS32 matched = ExpandNpcTextInputBinding(p_nested, NULL, cursor, &expanded);
				if (matched != 0) {
					resume = cursor + matched;
					replacement = TRUE;
					cursor = expanded;
					continue;
				}
			}
			const LegoU16* current = cursor;
			if (*cursor == 0) {
				if (replacement) {
					replacement = FALSE;
					cursor = resume;
					continue;
				}
				finished = TRUE;
				newLine = TRUE;
			}
			else if (*cursor == '\\' && cursor[1] == 'n') {
				newLine = TRUE;
				current = cursor + 2;
			}
			else if (*cursor == '\n') {
				current = cursor + 1;
			}
			if (*current == ' ' || newLine) {
				if (words > 0) {
					*output++ = ' ';
				}
				*wordOutput = 0;
				for (cursor = g_npcTextWord; *cursor != 0; ++cursor) {
					*output++ = *cursor;
				}
				lineWidth += spaceWidth;
				wordOutput = g_npcTextWord;
				++words;
				wordWidth = 0.0f;
			}
			else {
				if (*current == '_' || *current == '\n') {
					*(LegoU16*) current = ' ';
				}
				LegoU32 count = 1;
				LegoFloat advance;
				if (font->GetTextAdvance(current, &count, &advance)) {
					wordWidth += advance;
					lineWidth += advance;
					if (maximumWidth < lineWidth && wordWidth <= maximumWidth) {
						newLine = TRUE;
					}
				}
				for (LegoU32 i = 0; i < count; ++i) {
					*wordOutput++ = *current++;
				}
				--current;
			}
			if (newLine) {
				LegoS32* lineCount = (LegoS32*) (bytes + 0x18c4);
				if (*lineCount < 24) {
					*output = 0;
					++*lineCount;
					lineWidth = wordWidth;
					words = 0;
					newLine = FALSE;
					output = (LegoU16*) (bytes + 0xc0 + *lineCount * 0x100);
				}
				else {
					finished = TRUE;
				}
			}
			cursor = current + 1;
			LegoS32 consumedNormalized = ((LegoS32) current - (LegoS32) normalized) / 2 + 1;
			if (normalizedLength <= consumedNormalized) {
				if (finished) {
					break;
				}
				if (!replacement) {
					NormalizeNpcTextChunk(nextChunk, normalized, 0x100, &consumed, &normalizedLength);
					cursor = normalized;
					nextChunk += consumed;
				}
			}
		}
		if (finished) {
			break;
		}
	}
	LegoS32 lines = *(LegoS32*) (bytes + 0x18c4);
	if (*(LegoS32*) (bytes + 0x18c0) < lines) {
		lines = *(LegoS32*) (bytes + 0x18c0);
	}
	return (LegoFloat) (lines + 1) * lineHeight + GetNpcTextLineOffset()[1];
}

// STUB: LEGORACERS2 0x00438940
static void __stdcall NormalizeNpcTextChunk(
	const LegoU16* p_text,
	LegoU16* p_output,
	LegoS32 p_capacity,
	LegoS32* p_consumed,
	LegoS32* p_length
)
{
	LegoS32 threshold = p_capacity - (p_capacity >> 2);
	LegoS32 end = threshold + 16;
	if (p_capacity <= end) {
		end = p_capacity - 1;
	}
	LegoBool32 replacement = FALSE;
	LegoBool32 tail = FALSE;
	LegoS32 replacementRemaining = 0;
	const LegoU16* cursor = p_text;
	const LegoU16* resume = NULL;
	LegoU16* output = p_output;
	GlobalOverlayGlyphReplacementManagerView* glyphs =
		(GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c;
	*p_consumed = 0;
	*p_length = 0;
	for (;;) {
		if (!replacement) {
			if (*cursor == 0) {
				break;
			}
		}
		else if (replacementRemaining < 1) {
			if (*cursor == 0) {
				replacement = FALSE;
				cursor = resume;
			}
			else {
				goto copy_character;
			}
		}
		else {
			--replacementRemaining;
			if (replacementRemaining != 0) {
				goto copy_character;
			}
			replacement = FALSE;
		}
		if (glyphs != NULL) {
			LegoU32 matched;
			LegoU32 count;
			if (HasGlobalOverlayGlyphReplacement(glyphs, NULL, cursor, &matched)) {
				count = matched;
			}
			else {
				LegoU16* expansionEnd = g_textBoxFormattingState.m_expandedSequence;
				if (!ExpandOverlayGlyphSequence(glyphs, NULL, cursor, &matched, &expansionEnd)) {
					goto copy_character;
				}
				*expansionEnd = 0;
				resume = cursor + matched;
				cursor = g_textBoxFormattingState.m_expandedSequence;
				count = replacementRemaining;
			}
			replacementRemaining = count;
			replacement = TRUE;
			if (!tail) {
				*p_consumed += matched;
			}
		}
	copy_character:
		*output++ = *cursor++;
		if (!tail) {
			if (!replacement) {
				++*p_consumed;
			}
			++*p_length;
			if (!replacement) {
				if (threshold < *p_length) {
					tail = TRUE;
				}
			}
		}
		else if (!replacement && output - p_output > end) {
			break;
		}
	}
	*output = 0;
}

// STUB: LEGORACERS2 0x00438ac0
static LegoS32 __fastcall ExpandNpcTextInputBinding(
	NestedTextBoxManagerPrefix* p_nested,
	void*,
	const LegoU16* p_text,
	const LegoU16** p_expansion
)
{
	// GLOBAL: LEGORACERS2 0x00541b1c
	static const LegoChar* tokens[3] = {"~~ACC", "~~BRK", "~~REV"};
	for (LegoS32 i = 0; i < 3; ++i) {
		const LegoChar* token = tokens[i];
		LegoS32 count = 0;
		while (token[count] != 0 && (LegoU16) token[count] == p_text[count]) {
			++count;
		}
		if (token[count] == 0) {
			const GameControllerPairGroups* bindings =
				(const GameControllerPairGroups*) ((LegoU8*) GetGameControllerSettings() + 0xb4 +
												   p_nested->m_unk0x68 * 0xac);
			*p_expansion = GetNpcTextBindingName(bindings, i);
			return count;
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x00438c40
static void __fastcall StartNpcTextVoice(NestedTextBoxManagerPrefix* p_nested, void*)
{
	ReleaseTextBoxManagerResource((GameObject*) p_nested, NULL);
	if (p_nested->m_unk0x64 != 1 && p_nested->m_selected > -1 &&
		g_npcTextSpeakers[p_nested->m_selected].m_voiceCategory != -1) {
		AudioSourceRecord* voice =
			SelectTextBoxVoice(p_nested, NULL, p_nested->m_selected, &p_nested->m_selectedResource);
		p_nested->m_unk0x70 = (LegoU32) voice;
		voice->m_vtable->m_setMaximumGain(voice, NULL, p_nested->m_scale);
		voice->m_vtable->m_resetPlaybackState(voice, NULL);
	}
}
