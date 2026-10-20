#include "frontend/widgets/menutextentry.h"

#include "frontend/input/menuinputbindinglabels.h"
#include "frontend/menu/menumanager.h"
#include "frontend/resources/controllereventrecords.h"
#include "frontend/secondary/secondarymanager.h"
#include "frontend/widgets/menutextentrytuning.h"
#include "game/controller/gamecontroller.h"
#include "input/service/inputdeviceservice.h"
#include "io/localization/localizedstringtable.h"

#include <math.h>
#include <string.h>

extern LegoU32 g_menuChoiceColorA;
extern LegoU32 g_menuChoiceColorB;

static void __fastcall MenuTextEntryUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTextEntry*) p_widget)->UpdateBounds();
}

static void __fastcall MenuTextEntryResetBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTextEntry*) p_widget)->Reset();
}

static LegoU32 __fastcall MenuTextEntryInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuTextEntry*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall MenuTextEntryClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuTextEntry*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

const MenuWidgetDispatch g_menuTextEntryDispatch = {
	&MenuTextEntryUpdateBoundsBridge,
	&SetMenuWidgetFont,
	&MenuWidgetDerivedNoOp,
	&MenuTextEntryResetBridge,
	&MenuTextEntryInputBridge,
	&MenuTextEntryClipBridge
};

// STUB: LEGORACERS2 0x004243b0
MenuTextEntry::MenuTextEntry() : MenuWidgetBase(-1)
{
	LegoS32 i;
	m_vtable = &g_menuTextEntryDispatch;
	m_pointerX = -1.0f;
	m_pointerY = -1.0f;
	m_pointerRow = -1;
	m_pointerColumn = -1;
	m_lastKeyboardScanCode = -1;
	for (i = 0; i < 8; ++i) {
		m_textRecords[i] = NULL;
		m_textCursorRecords[i] = NULL;
	}
	for (i = 0; i < 50; ++i) {
		((GlobalOverlayRecord**) m_keyboardRecords)[i] = NULL;
	}
	for (i = 0; i < 3; ++i) {
		m_actionSprites[i] = NULL;
	}
}

// STUB: LEGORACERS2 0x00424430
void MenuTextEntry::Initialize(
	LegoS32 p_initialValue,
	const LegoFloat* p_center,
	const LegoU16* p_initialText,
	LegoU32 p_mode
)
{
	LegoS32 i, row, column;
	BeginState();
	m_initialValue = p_initialValue;
	m_unk0x14 = p_mode;

	LegoU32 recordParameters[13];
	recordParameters[0] = (LegoU32) m_font;
	recordParameters[1] = 0xffffffff;
	recordParameters[2] = 0;
	recordParameters[3] = 0;
	recordParameters[4] = 0;
	recordParameters[5] = 0;
	recordParameters[6] = 0x3f800000;
	recordParameters[7] = 0x3f800000;
	recordParameters[8] = 0;
	recordParameters[9] = 9;
	recordParameters[10] = 0x3f800000;
	recordParameters[11] = 0;
	recordParameters[12] = 0;
	for (i = 0; i < 8; ++i) {
		m_textRecords[i] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
		m_textCursorRecords[i] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
	}
	for (row = 0; row < 5; ++row) {
		for (column = 0; column < 10; ++column) {
			m_keyboardRecords[row][column] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
		}
	}

	const LegoU16* localizedKeys = NULL;
	if (!g_globalOverlayManager.m_localizedStrings->GetString(0xb5, &localizedKeys)) {
		return;
	}
	LegoS32 keyCount = 0;
	while (localizedKeys[keyCount] != 0) {
		++keyCount;
	}
	m_keyboardRowCount = (keyCount + 9) / 10;
	LegoS32 keysPerRow = keyCount / m_keyboardRowCount;
	LegoS32 extraKeys = keyCount - keysPerRow * m_keyboardRowCount;
	LegoS32 extraAccumulator = 0;
	LegoFloat keyX = p_center[0] - g_menuTextEntryTuning[2] * 4.5f;
	LegoFloat keyY = p_center[1] + 0.08f;
	LegoS32 keyIndex = 0;
	for (row = 0; row < 4; ++row) {
		LegoS32 rowLength = 0;
		if (row < m_keyboardRowCount) {
			rowLength = keysPerRow;
			extraAccumulator += extraKeys;
			if (extraAccumulator >= m_keyboardRowCount) {
				++rowLength;
				extraAccumulator -= m_keyboardRowCount;
			}
		}
		LegoS32 leftPadding = (10 - keysPerRow) / 2;
		LegoFloat rowX = keyX;
		if (rowLength != 0 && (keysPerRow & 1) != 0) {
			rowX += g_menuTextEntryTuning[2] * 0.5f;
		}
		if (rowLength > keysPerRow) {
			rowX -= g_menuTextEntryTuning[2] * 0.5f;
		}
		for (column = 0; column < 10; ++column) {
			GlobalOverlayRecord* record = m_keyboardRecords[row][column];
			LegoFloat position[2] = {rowX, keyY};
			if (column < leftPadding || column >= leftPadding + rowLength) {
				m_keyboardCharacters[row][column] = 0;
				record->InitializeTextByIndex(-1, 0, 0.0f);
			}
			else {
				LegoU16 oneCharacter[2] = {localizedKeys[keyIndex++], 0};
				m_keyboardCharacters[row][column] = oneCharacter[0];
				record->SetPosition(position, 0xffffffff);
				record->InitializeText(oneCharacter, 1, 0.0f);
			}
			rowX += g_menuTextEntryTuning[2];
		}
		if (row < m_keyboardRowCount - 1) {
			keyY += g_menuTextEntryTuning[3];
		}
	}
	keyY += g_menuTextEntryTuning[3];
	LegoFloat digitX = keyX;
	for (column = 0; column < 10; ++column) {
		LegoS32 row = m_keyboardRowCount;
		LegoU16 digit[2] = {(LegoU16) ('0' + column), 0};
		m_keyboardCharacters[row][column] = digit[0];
		LegoFloat position[2] = {digitX, keyY};
		m_keyboardRecords[row][column]->SetPosition(position, 0xffffffff);
		m_keyboardRecords[row][column]->InitializeText(digit, 1, 0.0f);
		digitX += g_menuTextEntryTuning[2];
	}

	ControllerEventRecordTable* spriteTable = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	LegoU32 spriteParameters[21];
	spriteParameters[0] = 0;
	spriteParameters[1] = 0;
	spriteParameters[2] = 0;
	spriteParameters[3] = 0;
	spriteParameters[4] = 0;
	spriteParameters[5] = 0;
	spriteParameters[6] = 0x3f800000;
	spriteParameters[7] = 0x3f800000;
	// Retail leaves tile dimensions and UV words untouched before copying the
	// controller texture record over UVs; keep those stack words unspecified.
	spriteParameters[10] = 0xffffffff;
	spriteParameters[11] = 0;
	spriteParameters[16] = 0;
	spriteParameters[17] = 7;
	spriteParameters[18] = 1;
	spriteParameters[19] = 0;
	spriteParameters[20] = 0;
	for (i = 0; i < 3; ++i) {
		m_actionSprites[i] = g_globalOverlayManager.AllocateSprite(spriteParameters);
		memcpy(&m_actionSprites[i]->m_texture, &spriteTable->m_records[175 + i], 5 * sizeof(LegoU32));
		m_actionSprites[i]->SetList(9);
		m_actionSprites[i]->SetVisible(1);
		if (i == 2) {
			m_actionSprites[i]->SetAlpha(0x7f);
		}
	}
	LegoFloat actionPosition[2] = {p_center[0], keyY + 0.1f};
	for (i = 0; i < 3; ++i) {
		m_actionSprites[i]->SetPosition(actionPosition);
		m_actionSprites[i]->SetSize(i == 0 ? &g_menuTextEntryTuning[0] : &g_menuTextEntryTuning[6]);
		if (i == 0) {
			actionPosition[0] += g_menuTextEntryTuning[4];
		}
		else if (i == 1) {
			actionPosition[0] += g_menuTextEntryTuning[5];
		}
	}

	m_textLength = 0;
	while (p_initialText[m_textLength] != 0) {
		m_text[m_textLength] = p_initialText[m_textLength];
		++m_textLength;
	}
	m_text[m_textLength] = 0;
	LegoFloat textX = p_center[0] - 0.07f * 3.5f;
	LegoFloat textY = actionPosition[1] + 0.04f;
	for (i = 0; i < 8; ++i) {
		LegoU16 character[2] = {i < m_textLength ? m_text[i] : 0, 0};
		m_textRecords[i]->InitializeText(character, 1, 0.0f);
		m_textRecords[i]->SetScale(1.6f);
		LegoFloat position[2] = {textX, textY};
		m_textRecords[i]->SetPosition(position, 0xffffffff);
		textX += 0.07f;
	}
	textY += 0.04f;
	textX = p_center[0] - 0.07f * 3.5f;
	const LegoChar cursorCharacter[] = "-";
	for (i = 0; i < 8; ++i) {
		m_textCursorRecords[i]->InitializeNarrowText(cursorCharacter, 1, 0.0f);
		m_textCursorRecords[i]->SetScale(1.8f);
		LegoFloat position[2] = {textX, textY};
		m_textCursorRecords[i]->SetPosition(position, 0xffffffff);
		textX += 0.07f;
	}
	m_selectedRow = 5;
	m_selectedColumn = 2;
	m_textAccepted = 0;
	SetKeyboardSelectionColor(TRUE);
	SetTextCursorColor(TRUE);
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	MenuNavigationState* navigation = (MenuNavigationState*) secondary->m_unk0x7c;
	if (navigation != NULL && navigation->m_active != 0) {
		navigation->GetPointerPosition(&m_pointerX);
		FindPointerSelection(&m_pointerRow, &m_pointerColumn, navigation);
	}
	m_vtable->m_updateBounds(this, NULL);
}

// FUNCTION: LEGORACERS2 0x00424b10
void MenuTextEntry::Reset()
{
	LegoS32 i, row, column;
	for (i = 2; i >= 0; --i) {
		if (m_actionSprites[i] != NULL) {
			m_actionSprites[i]->Release();
			m_actionSprites[i] = NULL;
		}
	}
	for (row = 4; row >= 0; --row) {
		for (column = 9; column >= 0; --column) {
			GlobalOverlayRecord*& record = m_keyboardRecords[row][column];
			if (record != NULL) {
				record->Release();
				record = NULL;
			}
		}
	}
	for (i = 7; i >= 0; --i) {
		if (m_textCursorRecords[i] != NULL) {
			m_textCursorRecords[i]->Release();
			m_textCursorRecords[i] = NULL;
		}
		if (m_textRecords[i] != NULL) {
			m_textRecords[i]->Release();
			m_textRecords[i] = NULL;
		}
	}
	MenuWidgetBase::ResetState();
}

// STUB: LEGORACERS2 0x00424bb0
void MenuTextEntry::AppendCharacter(LegoS32 p_row, LegoS32 p_column)
{
	SetTextCursorColor(FALSE);
	LegoU16 character = (p_row == 5) ? 0x20 : m_keyboardCharacters[p_row][p_column];
	LegoU16 terminatedCharacter[2] = {character, 0};
	m_text[m_textLength] = character;
	m_text[m_textLength + 1] = 0;
	m_textRecords[m_textLength]->InitializeText(terminatedCharacter, 9, 0.0f);
	++m_textLength;
	SetTextCursorColor(TRUE);
}

// FUNCTION: LEGORACERS2 0x00424c40
void MenuTextEntry::RemoveLastCharacter()
{
	SetTextCursorColor(FALSE);
	--m_textLength;
	m_text[m_textLength] = 0;
	m_textRecords[m_textLength]->InitializeTextByIndex(-1, 0, 0.0f);
	SetTextCursorColor(TRUE);
}

// STUB: LEGORACERS2 0x00424c80
void MenuTextEntry::SetKeyboardSelectionColor(LegoBool32 p_selected)
{
	LegoU32 color = 0xffffffff;
	if (p_selected != FALSE) {
		MenuManager* manager = (MenuManager*) g_gameController.m_menuManager;
		color = ((~manager->m_unk0x218 & 1) != 0) ? g_menuChoiceColorB : g_menuChoiceColorA;
	}
	if (m_selectedRow != 5) {
		m_keyboardRecords[m_selectedRow][m_selectedColumn]->SetColor(&color);
	}
	else if (m_selectedColumn == 2) {
		m_actionSprites[m_selectedColumn]->SetAlpha(p_selected ? 0xff : 0x7f);
	}
	else {
		m_actionSprites[m_selectedColumn]->SetColor(&color);
	}
}

// STUB: LEGORACERS2 0x00424d10
void MenuTextEntry::SetTextCursorColor(LegoBool32 p_selected)
{
	if (m_textLength >= 8) {
		return;
	}
	LegoU32 color = 0xffffffff;
	if (p_selected != FALSE) {
		MenuManager* manager = (MenuManager*) g_gameController.m_menuManager;
		color = ((~manager->m_unk0x218 & 1) == 0) ? g_menuChoiceColorA : g_menuChoiceColorB;
	}
	m_textCursorRecords[m_textLength]->SetColor(&color);
}

// STUB: LEGORACERS2 0x00424d70
LegoU32 MenuTextEntry::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoS32 row = m_selectedRow;
	LegoS32 column = m_selectedColumn;
	LegoU32 mouseClick = 0;
	LegoU32 mouseAction = 0;
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	MenuNavigationState* navigation = (MenuNavigationState*) secondary->m_unk0x7c;
	if (navigation != NULL && navigation->m_active != 0) {
		mouseClick = navigation->m_verticalFlags & 1;
		mouseAction = navigation->m_verticalFlags & 2;
		FindPointerSelection(&m_pointerRow, &m_pointerColumn, navigation);
		if (m_pointerRow != -1) {
			navigation->ReadDirectionInput(1);
		}
		LegoFloat pointer[2];
		navigation->GetPointerPosition(pointer);
		if (fabs(m_pointerX - pointer[0]) > 0.0001f || fabs(m_pointerY - pointer[1]) > 0.0001f || mouseClick != 0) {
			if (m_pointerRow != -1 && (m_pointerRow != m_selectedRow || m_pointerColumn != m_selectedColumn)) {
				SetKeyboardSelectionColor(FALSE);
				m_selectedRow = m_pointerRow;
				m_selectedColumn = m_pointerColumn;
				row = m_selectedRow;
				column = m_selectedColumn;
				SetKeyboardSelectionColor(TRUE);
				*p_result = 3;
			}
			m_pointerX = pointer[0];
			m_pointerY = pointer[1];
		}
	}

	if ((p_input[3] & 0x0c) != 0) {
		LegoS32 nextRow = row + ((p_input[3] & 4) != 0 ? -1 : 1);
		if (nextRow < 0) {
			row = m_unk0x14 != 0 ? 5 : 0;
		}
		else if (nextRow > m_keyboardRowCount) {
			if (row == 5) {
				row = m_keyboardRowCount;
				if (nextRow > 4) {
					row = m_unk0x14 != 0 ? 0 : 5;
				}
			}
			else {
				row = 5;
			}
		}
		else {
			row = nextRow;
		}
		if (row != m_selectedRow) {
			if (row == 5) {
				*p_result = 3;
				column = 0;
			}
			else {
				if (m_selectedRow == 5) {
					static const LegoS32 actionColumnMap[3] = {4, 8, 9};
					column = actionColumnMap[m_selectedColumn];
				}
				if (m_keyboardCharacters[row][column] == 0) {
					LegoS32 step = column < 6 ? 1 : -1;
					do {
						column += step;
					} while (m_keyboardCharacters[row][column] == 0);
				}
				*p_result = 3;
			}
		}
	}
	else if ((p_input[3] & 3) != 0) {
		do {
			LegoS32 columnCount = 10;
			LegoS32 wrap = m_unk0x14;
			if (row == 5) {
				columnCount = 3;
				if (m_textLength == 0) {
					wrap = 0;
					columnCount = 2;
				}
			}
			column += (p_input[3] & 1) != 0 ? -1 : 1;
			if (column < 0) {
				column = wrap ? columnCount - 1 : m_selectedColumn;
			}
			else if (column >= columnCount) {
				column = wrap ? 0 : m_selectedColumn;
			}
		} while (row != 5 && m_keyboardCharacters[row][column] == 0);
		if (column != m_selectedColumn) {
			*p_result = 3;
		}
	}
	else if (
		(p_input[1] & 1) == 0 &&
		((mouseClick == 0 || m_pointerRow != m_selectedRow || m_pointerColumn != m_selectedColumn))
	) {
		if ((p_input[1] & 0x202) != 0 || mouseAction != 0) {
			*p_result = 1;
			return 1;
		}
		HandleKeyboardInput(p_result, &row, &column);
	}
	else if (row == 5) {
		if (column == 1) {
			if (m_textLength != 0) {
				RemoveLastCharacter();
				*p_result = 1;
			}
			else {
				*p_result = 2;
			}
		}
		else if (column == 2) {
			if (m_textLength != 0) {
				m_textAccepted = 1;
				*p_result = 0;
			}
			else {
				*p_result = 2;
			}
		}
		else {
			*p_result = 0;
			AppendCharacter(row, column);
			if (m_textLength > 7) {
				row = 5;
				column = 2;
			}
		}
	}
	else {
		*p_result = 0;
		AppendCharacter(row, column);
		if (m_textLength > 7) {
			row = 5;
			column = 2;
		}
	}

	if (*p_result != 0xffffffff) {
		if (m_textLength < 8 || (row == 5 && column != 0)) {
			SetKeyboardSelectionColor(FALSE);
			m_selectedRow = row;
			m_selectedColumn = column;
			SetKeyboardSelectionColor(TRUE);
		}
		else {
			*p_result = 2;
		}
	}
	return m_textAccepted != 0 ? 2 : 0;
}

// STUB: LEGORACERS2 0x004251e0
void MenuTextEntry::HandleKeyboardInput(LegoU32* p_result, LegoS32* p_row, LegoS32* p_column)
{
	InputDeviceRecord* keyboard = g_inputService->m_vtable->findDevice(g_inputService, NULL, 0, 1);
	if (keyboard == NULL) {
		return;
	}
	LegoS32 scanCode;
	for (scanCode = 0; scanCode < 0x100; ++scanCode) {
		if (keyboard->m_vtable->isButtonPressed(keyboard, NULL, scanCode) != FALSE) {
			break;
		}
	}
	if (scanCode == 0x100) {
		m_lastKeyboardScanCode = -1;
		return;
	}
	if (scanCode == m_lastKeyboardScanCode) {
		return;
	}
	m_lastKeyboardScanCode = scanCode;
	if (scanCode == 0x0e) {
		if (m_textLength <= 0) {
			*p_result = 2;
			return;
		}
		*p_result = 0;
		RemoveLastCharacter();
		if (m_textLength == 0) {
			*p_row = 5;
			*p_column = 1;
		}
		else {
			*p_row = 5;
			*p_column = 2;
		}
		return;
	}

	LegoS32 targetColumn = -1;
	LegoS32 targetRow = m_keyboardRowCount;
	switch (scanCode) {
	case 0x39:
		targetColumn = 10;
		break;
	case 0x47:
		targetColumn = 7;
		break;
	case 0x48:
		targetColumn = 8;
		break;
	case 0x49:
		targetColumn = 9;
		break;
	case 0x4b:
		targetColumn = 4;
		break;
	case 0x4c:
		targetColumn = 5;
		break;
	case 0x4d:
		targetColumn = 6;
		break;
	case 0x4f:
		targetColumn = 1;
		break;
	case 0x50:
		targetColumn = 2;
		break;
	case 0x51:
		targetColumn = 3;
		break;
	case 0x52:
		targetColumn = 0;
		break;
	default:
		break;
	}
	if (targetColumn == 10) {
		targetRow = 5;
		targetColumn = 0;
	}
	else if (targetColumn == -1) {
		const LegoU16* keyName = GetMenuInputName(1, &scanCode);
		if (keyName == NULL || keyName[0] == 0 || keyName[1] != 0) {
			return;
		}
		LegoU16 character = keyName[0];
		if ((character < 0x100 && character >= 'a' && character <= 'z') || (character >= 0xe0 && character <= 0xff)) {
			character = (LegoU16) (character - 0x20);
		}
		if (m_keyboardRowCount < 0) {
			return;
		}
		for (targetRow = 0; targetRow <= m_keyboardRowCount; ++targetRow) {
			for (targetColumn = 0; targetColumn < 10; ++targetColumn) {
				if (m_keyboardCharacters[targetRow][targetColumn] == character) {
					goto found_character;
				}
			}
		}
		return;
	}

found_character:
	if (m_textLength >= 8) {
		*p_result = 2;
		return;
	}
	*p_result = 0;
	AppendCharacter(targetRow, targetColumn);
	*p_row = 5;
	*p_column = 2;
}

// FUNCTION: LEGORACERS2 0x00425460
LegoU16* MenuTextEntry::GetText()
{
	return m_textAccepted != 0 ? m_text : NULL;
}

// STUB: LEGORACERS2 0x00425480
LegoBool32 MenuTextEntry::FindPointerSelection(LegoS32* p_row, LegoS32* p_column, MenuNavigationState* p_navigation)
{
	LegoS32 row, column, index;
	*p_row = -1;
	*p_column = -1;
	if (m_textLength < 8) {
		for (row = 0; row < 5; ++row) {
			for (column = 0; column < 10; ++column) {
				GlobalOverlayRecord* record = m_keyboardRecords[row][column];
				if (record != NULL && record->m_text[0] != 0 && p_navigation->IsRecordUnderPointer(record)) {
					*p_row = row;
					*p_column = column;
					return TRUE;
				}
			}
		}
	}
	for (index = 0; index < 3; ++index) {
		GlobalOverlaySprite* sprite = m_actionSprites[index];
		if (sprite == NULL || (index == 0 && m_textLength >= 8) || (index == 2 && m_textLength == 0)) {
			continue;
		}
		if (p_navigation->IsSpriteUnderPointer(sprite)) {
			*p_row = 5;
			*p_column = index;
			return TRUE;
		}
	}
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x00425560
void MenuTextEntry::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	LegoS32 i, row, column;
	MenuWidgetBase::SetChildClipRectangle(p_topLeft, p_bottomRight);
	for (i = 0; i < 8; ++i) {
		if (m_textRecords[i] != NULL) {
			m_textRecords[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
		if (m_textCursorRecords[i] != NULL) {
			m_textCursorRecords[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
	for (row = 0; row < 5; ++row) {
		for (column = 0; column < 10; ++column) {
			if (m_keyboardRecords[row][column] != NULL) {
				m_keyboardRecords[row][column]->SetClipRectangle(p_topLeft, p_bottomRight);
			}
		}
	}
	for (i = 0; i < 3; ++i) {
		if (m_actionSprites[i] != NULL) {
			m_actionSprites[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x00425610
void MenuTextEntry::UpdateBounds()
{
	LegoS32 i, j, row, column;
	m_bounds[0] = 1.0f;
	m_bounds[1] = 1.0f;
	m_bounds[2] = 0.0f;
	m_bounds[3] = 0.0f;
	for (i = 0; i < 8; ++i) {
		GlobalOverlayRecord* records[2] = {m_textCursorRecords[i], m_textRecords[i]};
		for (j = 0; j < 2; ++j) {
			if (records[j] == NULL || records[j]->m_text[0] == 0) {
				continue;
			}
			LegoFloat topLeft[2], bottomRight[2];
			records[j]->GetRectangle(topLeft, bottomRight);
			if (topLeft[0] < m_bounds[0]) {
				m_bounds[0] = topLeft[0];
			}
			if (topLeft[1] < m_bounds[1]) {
				m_bounds[1] = topLeft[1];
			}
			if (m_bounds[2] < bottomRight[0]) {
				m_bounds[2] = bottomRight[0];
			}
			if (m_bounds[3] < bottomRight[1]) {
				m_bounds[3] = bottomRight[1];
			}
		}
	}
	for (row = 0; row < 5; ++row) {
		for (column = 0; column < 10; ++column) {
			GlobalOverlayRecord* record = m_keyboardRecords[row][column];
			if (record == NULL || record->m_text[0] == 0) {
				continue;
			}
			LegoFloat topLeft[2], bottomRight[2];
			record->GetRectangle(topLeft, bottomRight);
			if (topLeft[0] < m_bounds[0]) {
				m_bounds[0] = topLeft[0];
			}
			if (topLeft[1] < m_bounds[1]) {
				m_bounds[1] = topLeft[1];
			}
			if (m_bounds[2] < bottomRight[0]) {
				m_bounds[2] = bottomRight[0];
			}
			if (m_bounds[3] < bottomRight[1]) {
				m_bounds[3] = bottomRight[1];
			}
		}
	}
	for (i = 0; i < 3; ++i) {
		GlobalOverlaySprite* sprite = m_actionSprites[i];
		if (sprite == NULL) {
			continue;
		}
		LegoFloat topLeft[2], bottomRight[2];
		sprite->GetRectangle(topLeft, bottomRight);
		if (topLeft[0] < m_bounds[0]) {
			m_bounds[0] = topLeft[0];
		}
		if (topLeft[1] < m_bounds[1]) {
			m_bounds[1] = topLeft[1];
		}
		if (m_bounds[2] < bottomRight[0]) {
			m_bounds[2] = bottomRight[0];
		}
		if (m_bounds[3] < bottomRight[1]) {
			m_bounds[3] = bottomRight[1];
		}
	}
}
