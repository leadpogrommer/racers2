#include "frontend/widgets/menutrackselector.h"

#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/session/gamecontrollersession.h"

#include <math.h>
#include <stddef.h>

extern LegoU32 g_menuChoiceColorA;

struct MenuTrackCatalogView {
	undefined m_prefix[0x1a6c];
	LegoS32 m_trackCount; // +0x1a6c
	undefined m_gap[8];
	LegoS32 m_modes[10];          // +0x1a78
	LegoS32 m_trackIndices[10];   // +0x1aa0
	LegoS32 m_displayNumbers[10]; // +0x1ac8
};

DECOMP_STATIC_ASSERT(offsetof(MenuTrackCatalogView, m_trackCount) == 0x1a6c);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackCatalogView, m_modes) == 0x1a78);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackCatalogView, m_trackIndices) == 0x1aa0);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackCatalogView, m_displayNumbers) == 0x1ac8);
DECOMP_SIZE_ASSERT(MenuTrackCatalogView, 0x1af0);

static LegoS32 GetTrackNameStringIndex(LegoS32 p_mode, LegoS32 p_trackIndex)
{
	static const LegoS32 kModeStringOffsets[5] = {0x91, 0x8c, 0x96, 0x9b, 0xa0};
	if (p_mode < 0 || p_mode >= 5) {
		return -1;
	}
	return p_trackIndex + kModeStringOffsets[p_mode];
}

static void __fastcall MenuTrackSelectorUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTrackSelector*) p_widget)->UpdateBounds();
}

static void __fastcall MenuTrackSelectorResetBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTrackSelector*) p_widget)->Reset();
}

static LegoU32 __fastcall MenuTrackSelectorInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuTrackSelector*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall MenuTrackSelectorClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuTrackSelector*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

const MenuWidgetDispatch g_menuTrackSelectorDispatch = {
	&MenuTrackSelectorUpdateBoundsBridge,
	&SetMenuWidgetFont,
	&MenuWidgetDerivedNoOp,
	&MenuTrackSelectorResetBridge,
	&MenuTrackSelectorInputBridge,
	&MenuTrackSelectorClipBridge
};

// STUB: LEGORACERS2 0x00416b20
MenuTrackSelector::MenuTrackSelector() : MenuWidgetBase(-1)
{
	m_active = 0;
	m_selection = -1;
	m_layoutProbe = NULL;
	m_pointerX = -1.0f;
	m_pointerY = -1.0f;
	m_hoveredRow = -1;
	for (LegoU32 row = 0; row < 10; ++row) {
		m_trackLabels[row] = NULL;
		m_trackNumbers[row] = NULL;
	}
	m_vtable = &g_menuTrackSelectorDispatch;
}

// STUB: LEGORACERS2 0x00416b90
void MenuTrackSelector::Initialize(LegoS32 p_initialValue, const LegoFloat* p_position, LegoU32 p_wrapMode)
{
	BeginState();
	m_initialValue = p_initialValue;
	m_unk0x14 = p_wrapMode;
	m_selection = -1;
	m_positionX = p_position[0];
	m_positionY = p_position[1];

	// This record is temporary in retail: it measures translated track names
	// and the two-character number label before being returned to the pool.
	// Retail leaves parameter words 2 and 3 untouched on the stack before
	// passing this record to the overlay allocator. Preserve that behavior;
	// their values are not recoverable from the initializer's stores.
	LegoU32 parameters[13];
	parameters[0] = (LegoU32) m_font;
	parameters[1] = 0xffffffff;
	parameters[4] = 0;
	parameters[5] = 0;
	parameters[6] = 0x3f800000;
	parameters[7] = 0x3f800000;
	parameters[8] = 0;
	parameters[9] = 0;
	parameters[10] = 0x3f800000;
	parameters[11] = 0;
	parameters[12] = 0;
	m_layoutProbe = g_globalOverlayManager.AllocateRecord(parameters, -1);
	m_leftLabelExtent = 0.0f;
	m_numberLabelExtent = 0.0f;
	if (m_layoutProbe != NULL) {
		// The original helper at 0x00423100 checks localized labels for all five
		// modes when p_mode is -1, retaining the maximum measured width.
		LegoFloat maximumWidth =
			MeasureLocalizedTrackLabelWidth(g_gameController.m_menuManager, NULL, -1, m_layoutProbe);
		m_leftLabelExtent = maximumWidth * 0.4f;
		m_layoutProbe->InitializeNarrowText(" 99", 4, 0.0f);
		m_numberLabelExtent = m_layoutProbe->m_measuredWidth * 0.4f;
		m_layoutProbe->Release();
	}

	// Retail changes word 9 to 9 only after the temporary measurement record.
	parameters[9] = 9;
	for (LegoU32 recordIndex = 0; recordIndex < 10; ++recordIndex) {
		m_trackLabels[recordIndex] = g_globalOverlayManager.AllocateRecord(parameters, -1);
		m_trackNumbers[recordIndex] = g_globalOverlayManager.AllocateRecord(parameters, -1);
	}

	LegoFloat labelPosition[2];
	labelPosition[0] = m_positionX - m_numberLabelExtent * 0.5f;
	labelPosition[1] = m_positionY;
	LegoFloat numberPosition[2];
	numberPosition[0] = m_leftLabelExtent * 0.5f + m_positionX;
	numberPosition[1] = m_positionY;
	for (LegoU32 positionIndex = 0; positionIndex < 10; ++positionIndex) {
		m_trackLabels[positionIndex]->SetPosition(labelPosition, 0xffffffff);
		m_trackNumbers[positionIndex]->SetPosition(numberPosition, 0xffffffff);
		labelPosition[1] += 0.01f;
		numberPosition[1] += 0.01f;
	}

	PopulateRows();
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	MenuNavigationState* navigation = (MenuNavigationState*) secondary->m_unk0x7c;
	if (navigation != NULL && navigation->m_active != 0) {
		navigation->GetPointerPosition(&m_pointerX);
		FindHoveredRow(&m_hoveredRow, navigation);
	}
	m_vtable->m_updateBounds(this, NULL);
}

// FUNCTION: LEGORACERS2 0x00416da0
void MenuTrackSelector::Reset()
{
	for (LegoS32 row = 9; row >= 0; --row) {
		if (m_trackNumbers[row] != NULL) {
			m_trackNumbers[row]->Release();
			m_trackNumbers[row] = NULL;
		}
		if (m_trackLabels[row] != NULL) {
			m_trackLabels[row]->Release();
			m_trackLabels[row] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x00416df0
LegoU32 MenuTrackSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoU32 result = 0;
	LegoS32 selected = m_active;
	if (m_layoutInitialized != 0) {
		if (p_input[1] == 0) {
			m_layoutInitialized = 0;
		}
		else {
			selected = 0;
		}
	}
	LegoS32 desired = m_selection;
	if (selected != 0) {
		MenuTrackCatalogView* catalog = (MenuTrackCatalogView*) GetGameControllerSession();
		LegoS32 count = catalog->m_trackCount;
		if ((p_input[3] & 4) != 0) {
			--desired;
		}
		else if ((p_input[3] & 8) != 0) {
			++desired;
		}

		SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
		MenuNavigationState* navigation = (MenuNavigationState*) secondary->m_unk0x7c;
		LegoBool32 pointerSelect = FALSE;
		if (navigation != NULL && navigation->m_active != 0) {
			pointerSelect = (navigation->m_verticalFlags & 1) != 0;
			FindHoveredRow(&m_hoveredRow, navigation);
			if (m_hoveredRow != -1) {
				navigation->ReadDirectionInput(1);
			}
			LegoFloat pointerPosition[2];
			navigation->GetPointerPosition(pointerPosition);
			if (fabs(m_pointerX - pointerPosition[0]) > 0.0001f || fabs(m_pointerY - pointerPosition[1]) > 0.0001f) {
				if (m_hoveredRow != -1) {
					desired = m_hoveredRow;
				}
				m_pointerX = pointerPosition[0];
				m_pointerY = pointerPosition[1];
			}
			if (pointerSelect && m_hoveredRow != -1) {
				desired = m_hoveredRow;
			}
		}

		if (desired < 0) {
			desired = m_unk0x14 == 0 ? 0 : count - 1;
		}
		else if (count <= desired) {
			desired = m_unk0x14 == 0 ? count - 1 : 0;
		}

		if ((p_input[1] & 1) != 0 || (pointerSelect && m_hoveredRow == desired)) {
			SetActive(0);
			result = 2;
			*p_result = 0;
		}
		else if ((p_input[1] & 0x202) != 0) {
			SetActive(0);
			desired = -1;
			*p_result = 1;
		}
	}

	if (desired == m_selection) {
		if (m_active != 0) {
			return result;
		}
	}
	else {
		m_selection = desired;
		if (desired != -1) {
			*p_result = 3;
		}
	}
	PopulateRows();
	return result;
}

// FUNCTION: LEGORACERS2 0x00416fe0
void MenuTrackSelector::SetActive(LegoU32 p_active)
{
	m_active = p_active;
	if (p_active != 0) {
		m_selection = 0;
		PopulateRows();
		m_layoutInitialized = 1;
	}
}

// STUB: LEGORACERS2 0x00417010
void MenuTrackSelector::PopulateRows()
{
	MenuTrackCatalogView* catalog = (MenuTrackCatalogView*) GetGameControllerSession();
	LegoS32 count = catalog->m_trackCount;
	LegoS32 highlighted = m_active != 0 ? m_selection : -1;
	LegoFloat labelX = m_positionX - m_numberLabelExtent * 0.5f;
	LegoFloat numberX = m_leftLabelExtent * 0.5f + m_positionX;
	LegoFloat y = m_positionY;
	LegoFloat labelPosition[2] = {labelX, y};
	LegoFloat numberPosition[2] = {numberX, y};
	for (LegoS32 row = 0; row < 10; ++row) {
		GlobalOverlayRecord* label = m_trackLabels[row];
		GlobalOverlayRecord* number = m_trackNumbers[row];
		if (row < count) {
			label->InitializeTextByIndex(
				GetTrackNameStringIndex(catalog->m_modes[row], catalog->m_trackIndices[row]),
				1,
				0.0f
			);
			LegoS32 displayNumber = catalog->m_displayNumbers[row];
			LegoChar text[4];
			text[0] = ' ';
			text[1] = (LegoChar) ((displayNumber / 10) % 10 + '0');
			text[2] = (LegoChar) (displayNumber % 10 + '0');
			text[3] = 0;
			number->InitializeNarrowText(text, 4, 0.0f);
		}
		else {
			label->InitializeNarrowText("-------", 1, 0.0f);
			number->InitializeTextByIndex(-1, 0, 0.0f);
		}
		label->SetScale(0.4f);
		number->SetScale(0.4f);
		label->SetPosition(labelPosition, 0xffffffff);
		number->SetPosition(numberPosition, 0xffffffff);
		labelPosition[1] += 0.01f;
		numberPosition[1] += 0.01f;
		UpdateRowSelectionVisual(row, row == highlighted);
	}
}

// STUB: LEGORACERS2 0x00417230
void MenuTrackSelector::UpdateRowSelectionVisual(LegoS32 p_row, LegoU32 p_selected)
{
	GlobalOverlayRecord* label = m_trackLabels[p_row];
	GlobalOverlayRecord* number = m_trackNumbers[p_row];
	LegoU32 color =
		p_selected
			? g_menuChoiceColorA
			: (p_row < ((MenuTrackCatalogView*) GetGameControllerSession())->m_trackCount ? 0xffffffff : 0xc8ffffff);
	label->SetColor(&color);
	number->SetColor(&color);
	if (p_selected) {
		label->m_animationFlags |= 0x40;
		number->m_animationFlags |= 0x40;
	}
	else {
		label->m_animationFlags &= ~0x40;
		number->m_animationFlags &= ~0x40;
	}
}

// FUNCTION: LEGORACERS2 0x00417300
void MenuTrackSelector::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	for (LegoU32 row = 0; row < 10; ++row) {
		if (m_trackLabels[row] != NULL) {
			m_trackLabels[row]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
		if (m_trackNumbers[row] != NULL) {
			m_trackNumbers[row]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x00417350
void MenuTrackSelector::UpdateBounds()
{
	m_bounds[0] = 1.0f;
	m_bounds[1] = 1.0f;
	m_bounds[2] = 0.0f;
	m_bounds[3] = 0.0f;
	for (LegoU32 row = 0; row < 10; ++row) {
		GlobalOverlayRecord* label = m_trackLabels[row];
		if (label->m_text[0] != 0) {
			LegoFloat topLeft[2];
			LegoFloat bottomRight[2];
			label->GetRectangle(topLeft, bottomRight);
			if (topLeft[1] < m_bounds[1]) {
				m_bounds[1] = topLeft[1];
			}
			if (m_bounds[3] < bottomRight[1]) {
				m_bounds[3] = bottomRight[1];
			}
		}
	}
	LegoFloat extent = (m_numberLabelExtent + m_leftLabelExtent) * 0.5f;
	m_bounds[0] = m_positionX - extent;
	m_bounds[2] = extent + m_positionX;
}

// FUNCTION: LEGORACERS2 0x00417410
void MenuTrackSelector::FindHoveredRow(LegoS32* p_row, MenuNavigationState* p_navigation)
{
	*p_row = -1;
	for (LegoS32 row = 0; row < 10; ++row) {
		if (p_navigation->IsRecordUnderPointer(m_trackLabels[row])) {
			*p_row = row;
			return;
		}
	}
}

// STUB: LEGORACERS2 0x00417450
void MenuTrackSelector::SetLayoutPosition(const LegoFloat* p_position)
{
	if (fabs(m_positionX - p_position[0]) <= 0.0001f && fabs(m_positionY - p_position[1]) <= 0.0001f) {
		return;
	}
	m_positionX = p_position[0];
	m_positionY = p_position[1];
	LegoFloat labelX = m_positionX - m_numberLabelExtent * 0.5f;
	LegoFloat numberX = m_leftLabelExtent * 0.5f + m_positionX;
	LegoFloat y = m_positionY;
	LegoFloat labelPosition[2] = {labelX, y};
	LegoFloat numberPosition[2] = {numberX, y};
	for (LegoU32 row = 0; row < 10; ++row) {
		m_trackLabels[row]->SetPosition(labelPosition, 0xffffffff);
		m_trackNumbers[row]->SetPosition(numberPosition, 0xffffffff);
		labelPosition[1] += 0.01f;
		numberPosition[1] += 0.01f;
	}
	PopulateRows();
	m_vtable->m_updateBounds(this, NULL);
}

// STUB: LEGORACERS2 0x00423100
LegoFloat __fastcall MeasureLocalizedTrackLabelWidth(void*, void*, LegoS32 p_mode, GlobalOverlayRecord* p_record)
{
	static const LegoS32 kModeBase[5] = {0x92, 0x8d, 0x97, 0x9c, 0xa1};
	LegoFloat maximumWidth = 0.0f;
	for (LegoS32 mode = 0; mode < 5; ++mode) {
		if (p_mode != -1 && p_mode != mode) {
			continue;
		}
		LegoS32 variantCount = mode == 0 ? 4 : 5;
		for (LegoS32 variant = 0; variant < variantCount; ++variant) {
			p_record->InitializeTextByIndex(kModeBase[mode] + variant, 0, 0.0f);
			if (maximumWidth < p_record->m_measuredWidth) {
				maximumWidth = p_record->m_measuredWidth;
			}
		}
	}
	return maximumWidth;
}
