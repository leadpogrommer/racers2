#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/resources/controllereventrecords.h"
#include "frontend/secondary/secondarymenuinteraction.h"
#include "frontend/widgets/menutrackselector.h"
#include "game/controller/gamecontroller.h"
#include "game/controller/gamecontrollerconfig.h"
#include "game/session/gamecontrollersession.h"
#include "game/session/gamecontrollersessionqueries.h"

#include <string.h>

struct MenuTimeText {
	LegoChar m_text[9];
	MenuTimeText(LegoFloat, LegoChar);
	void FillDigits(LegoChar);
	void SetSeconds(LegoFloat);
};
DECOMP_SIZE_ASSERT(MenuTimeText, 9);

static LegoS32 RoundMenuClockValue(LegoFloat p_value)
{
	LegoS32 result;
	__asm {
  fld p_value
  fistp result
	}
	return result;
}

static GlobalOverlayRecord* GetMenuDetailRecord(MenuManager* p_menu, LegoU32 p_index)
{
	return (GlobalOverlayRecord*) p_menu->m_overlayRecords[p_index];
}

static LegoS32 GetMenuRaceModeString(LegoS32 p_mode, LegoBool32 p_raceSummary)
{
	switch (p_mode) {
	case -1:
		return 0x69;
	case 0:
		return 0x68;
	case 2:
		return 0x78;
	case 3:
		return p_raceSummary ? 0x79 : -1;
	case 4:
		return p_raceSummary ? 0x141 : -1;
	case 5:
		return 0x6a;
	default:
		return -1;
	}
}

// STUB: LEGORACERS2 0x0041d360
void __fastcall LayoutMenuOptionWidgets(
	MenuManager* p_menu,
	void*,
	LegoU32 p_apply,
	LegoU32,
	LegoS32 p_option,
	LegoFloat* p_origin,
	LegoFloat* p_size
)
{
	if (p_menu->m_unk0x1dc == 0) {
		return;
	}
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	if (p_apply == 0) {
		p_size[0] = p_size[1] = 0.0f;
	}
	LegoFloat bottom = 0.0f;
	LegoFloat spriteSize[2] = {0.0f, 0.0f};
	LegoFloat rectangleTop[2] = {0.0f, 0.0f};
	LegoFloat rectangleBottom[2] = {0.0f, 0.0f};
	LegoFloat position[2];
	LegoFloat topOffset = 0.0f;
	LegoU32 recordEnd = 9;
	ControllerEventRecord texture;
	GameControllerSession* session = GetGameControllerSession();
	GameControllerSettings* settings = GetGameControllerSettings();
	switch (p_menu->m_unk0x19c) {
	case 13:
	case 14:
		rectangleBottom[0] = spriteSize[0] = 0.125f;
		rectangleBottom[1] = spriteSize[1] = 0.166666672f;
		texture = resources->m_records[(0xd4c - 4) / 0x14];
		break;
	case 26: {
		MenuTrackSelector* tracks = (MenuTrackSelector*) p_menu->m_widgets[17];
		rectangleBottom[0] = tracks->m_bounds[0];
		rectangleBottom[1] = tracks->m_bounds[1];
		p_size[0] = tracks->m_bounds[2] - rectangleBottom[0];
		p_size[1] = tracks->m_bounds[3] - rectangleBottom[1];
		bottom = p_size[1] + p_origin[1];
		position[0] = p_size[0] * 0.5f + p_origin[0];
		position[1] = p_origin[1];
		tracks->SetLayoutPosition(position);
		goto measure_records;
	}
	case 28: {
		position[0] = p_size[0] * 0.5f + p_origin[0];
		position[1] = p_origin[1];
		recordEnd = 10;
		GlobalOverlayRecord* title = GetMenuDetailRecord(p_menu, 9);
		title->InitializeTextByIndex(GetMenuRaceModeString(session->m_mode, FALSE), 9, 0.0f);
		title->SetScale(0.5f);
		title->SetPosition(position, -1);
		LegoS32 world = session->m_selectedMode;
		if (session->m_mode == 2) {
			world = *(LegoS32*) ((LegoU8*) session + 0x1a78 + *(LegoS32*) ((LegoU8*) session + 0x1a70) * 4);
		}
		rectangleBottom[0] = spriteSize[0] = 0.125f;
		rectangleBottom[1] = spriteSize[1] = 0.166666672f;
		topOffset = 0.02f;
		texture = resources->m_records[(0x147c - 4) / 0x14 + world];
		if ((session->m_mode == 5 || session->m_mode == -1) && p_option >= 0 && p_option < 5) {
			position[0] = p_size[0] * 0.5f + p_origin[0];
			position[1] = p_origin[1] + 0.186666667f;
			GlobalOverlayRecord* label = GetMenuDetailRecord(p_menu, 10);
			label->InitializeTextByIndex(0xa6, 5, 0.0f);
			label->SetScale(0.5f);
			label->SetPosition(position, -1);
			position[1] += 0.02f;
			GameControllerScore score =
				*GetControllerTrackScore((LegoU8*) settings + 0x244, NULL, session->m_selectedMode, p_option + 1);
			MenuTimeText time(score.m_value, ':');
			GlobalOverlayRecord* value = GetMenuDetailRecord(p_menu, 11);
			value->InitializeNarrowText(time.m_text, 5, 0.0f);
			value->SetScale(0.5f);
			value->SetPosition(position, -1);
			position[1] += 0.03f;
			GlobalOverlayRecord* name = GetMenuDetailRecord(p_menu, 12);
			name->InitializeText(score.m_name.m_text, 1, 0.0f);
			name->SetScale(0.5f);
			name->SetPosition(position, -1);
			recordEnd = 13;
		}
		else {
			for (LegoS32 i = 10; i < 14; ++i) {
				GetMenuDetailRecord(p_menu, i)->InitializeTextByIndex(-1, 0, 0.0f);
			}
		}
		break;
	}
	case 30: {
		rectangleBottom[0] = spriteSize[0] = 0.125f;
		rectangleBottom[1] = spriteSize[1] = 0.166666672f;
		texture = resources->m_records[(0x147c - 4) / 0x14 + session->m_selectedMode];
		if (p_option >= 0 && p_option < 5) {
			position[0] = p_size[0] * 0.5f + p_origin[0];
			position[1] = p_origin[1] + 0.166666672f;
			GameControllerScore score =
				*GetControllerWorldScore((LegoU8*) settings + 0x244, NULL, session->m_selectedMode, p_option);
			MenuTimeText time(score.m_value, ':');
			GlobalOverlayRecord* label = GetMenuDetailRecord(p_menu, 9);
			label->InitializeTextByIndex(0xaf, 5, 0.0f);
			label->AppendNarrowText(time.m_text, 1);
			label->SetScale(0.5f);
			label->SetPosition(position, -1);
			recordEnd = 10;
		}
		else {
			GetMenuDetailRecord(p_menu, 9)->InitializeTextByIndex(-1, 0, 0.0f);
		}
		break;
	}
	case 34: {
		position[0] = p_size[0] * 0.5f + p_origin[0];
		position[1] = p_origin[1];
		recordEnd = 10;
		GlobalOverlayRecord* title = GetMenuDetailRecord(p_menu, 9);
		title->InitializeTextByIndex(GetMenuRaceModeString(session->m_mode, TRUE), 9, 0.0f);
		title->SetScale(0.5f);
		title->SetPosition(position, -1);
		rectangleBottom[0] = spriteSize[0] = 0.125f;
		rectangleBottom[1] = spriteSize[1] = 0.166666672f;
		topOffset = 0.02f;
		texture = resources->m_records[(0x147c - 4) / 0x14 + session->m_selectedMode];
		position[0] = p_size[0] * 0.5f + p_origin[0];
		position[1] = p_origin[1] + 0.186666667f;
		if (session->m_mode != 3) {
			LegoChar count[3] = {0};
			LegoS32 stringIndex;
			if (session->m_mode == 4) {
				stringIndex = (session->m_track != 1) + 0xad;
			}
			else {
				switch (session->m_selectedMode) {
				case 1:
					stringIndex = 0x8d;
					break;
				case 2:
					stringIndex = 0x97;
					break;
				case 3:
					stringIndex = 0x9c;
					break;
				case 4:
					stringIndex = 0xa1;
					break;
				default:
					stringIndex = 0x92;
				}
				stringIndex += session->m_track - 1;
				if (session->m_mode != 5) {
					LegoS32 value =
						*(LegoS32*) ((LegoU8*) settings + (session->m_track + (session->m_selectedMode + 2) * 5) * 4);
					count[0] = (value / 10) % 10 + '0';
					count[1] = value % 10 + '0';
				}
			}
			GlobalOverlayRecord* detail = GetMenuDetailRecord(p_menu, 10);
			detail->InitializeTextByIndex(stringIndex, 5, 0.0f);
			if (count[0] != 0) {
				detail->AppendNarrowText(count, 1);
			}
			detail->SetScale(0.5f);
			detail->SetPosition(position, -1);
			recordEnd = 11;
		}
		break;
	}
	default:
		return;
	}
	if (texture.m_index != -1) {
		position[0] = p_origin[0];
		position[1] = p_origin[1];
		if (p_apply == 0) {
			if (p_size[0] < spriteSize[0]) {
				p_size[0] = spriteSize[0];
			}
			LegoFloat candidateBottom = position[1] + topOffset + spriteSize[1];
			if (candidateBottom > 0.0f) {
				bottom = candidateBottom;
			}
		}
		else {
			position[0] += p_size[0] * 0.5f;
			position[1] = spriteSize[1] * 0.5f + position[1] + topOffset;
			p_menu->m_transitionCover->SetPosition(position);
			p_menu->m_transitionCover->SetSize(spriteSize);
			memcpy(&p_menu->m_transitionCover->m_texture, &texture, sizeof(texture));
			p_menu->m_transitionCover->SetVisible(1);
		}
	}
measure_records:
	for (LegoU32 index = 9; index < recordEnd; ++index) {
		GlobalOverlayRecord* record = GetMenuDetailRecord(p_menu, index);
		if (record->m_text[0] != 0) {
			record->GetRectangle(rectangleTop, rectangleBottom);
			if (p_size[0] < rectangleBottom[0] - rectangleTop[0]) {
				p_size[0] = rectangleBottom[0] - rectangleTop[0];
			}
			if (bottom < rectangleBottom[1]) {
				bottom = rectangleBottom[1];
			}
		}
	}
	p_size[1] = bottom - p_origin[1];
}

// STUB: LEGORACERS2 0x004c1650
MenuTimeText::MenuTimeText(LegoFloat p_seconds, LegoChar p_separator)
{
	m_text[5] = m_text[2] = p_separator;
	m_text[8] = 0;
	SetSeconds(p_seconds);
}

// STUB: LEGORACERS2 0x004c1680
void MenuTimeText::FillDigits(LegoChar p_value)
{
	m_text[0] = m_text[1] = m_text[3] = m_text[4] = m_text[6] = m_text[7] = p_value;
}

// STUB: LEGORACERS2 0x004c16a0
void MenuTimeText::SetSeconds(LegoFloat p_seconds)
{
	if (p_seconds < 0.0f) {
		FillDigits('-');
		return;
	}
	LegoS32 seconds = RoundMenuClockValue(p_seconds - 0.49f);
	LegoS32 minutes = seconds / 60;
	seconds -= minutes * 60;
	LegoS32 hundredths = RoundMenuClockValue(p_seconds * 100.0f) % 100;
	m_text[0] = (minutes / 10) % 10 + '0';
	m_text[1] = minutes % 10 + '0';
	m_text[3] = (seconds / 10) % 10 + '0';
	m_text[4] = seconds % 10 + '0';
	m_text[6] = (hundredths / 10) % 10 + '0';
	m_text[7] = hundredths % 10 + '0';
}
