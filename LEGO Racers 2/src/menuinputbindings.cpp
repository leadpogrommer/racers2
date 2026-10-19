#include "menuinputbindings.h"

#include "gamecontroller.h"
#include "globaloverlaymanager.h"
#include "inputdeviceservice.h"
#include "localizedstringtable.h"
#include "memoryheap.h"
#include "menuinputbindinglabels.h"
#include "menulabelselector.h"
#include "menumanager.h"

#include <new.h>

extern LegoU32 g_menuChoiceColorA;
extern LegoU32 g_menuChoiceColorB;
extern LegoFloat g_menuInputRowSpacing;

static LegoFloat __fastcall GetMenuActionHeadingTextWidth(MenuInputBindingWidget* p_widget, void*);
static LegoFloat __fastcall GetMenuInputLabelTextWidth(MenuInputBindingWidget* p_widget, void*);
static LegoBool32 __fastcall HasDuplicateMenuInputBinding(
	const MenuInputBindingWidget* p_widget,
	void*,
	LegoS32 p_code,
	LegoS32 p_device,
	LegoS32 p_mode
);

static const LegoS32 g_menuInputActionStringIds[15] =
	{0xa9, 0x33d, 0xe8, 0xe9, 0x1e4, 0x1e5, 0xea, 0xeb, 0xec, 0xed, 0x338, 0x33e, 0x337, 0x39c, 0x6};
static const LegoS32 g_menuInputBindingStringIds[9] = {0xea, 0xeb, 0xec, 0xed, 0x338, 0x33e, 0x337, 0x1e4, 0x1e5};

static LegoS32 GetDefaultMenuInputCode(LegoS32 p_mode)
{
	// Terminal values returned by the retail tables at 0x005305e0/0x0053060c.
	return p_mode == 1 ? 1 : 8;
}

static void __fastcall MenuInputBindingsUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuInputBindingWidget*) p_widget)->UpdateBounds();
}

static void __fastcall MenuInputBindingsSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	((MenuInputBindingWidget*) p_widget)->SetFontAndCreateSelectors(p_font);
}

static void __fastcall MenuInputBindingsDestroySelectorsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuInputBindingWidget*) p_widget)->DestroySelectors();
}

static void __fastcall MenuInputBindingsResetBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuInputBindingWidget*) p_widget)->Reset();
}

static LegoU32 __fastcall MenuInputBindingsHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuInputBindingWidget*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall MenuInputBindingsSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuInputBindingWidget*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

const MenuWidgetDispatch g_menuInputBindingWidgetDispatch = {
	&MenuInputBindingsUpdateBoundsBridge,
	&MenuInputBindingsSetFontBridge,
	&MenuInputBindingsDestroySelectorsBridge,
	&MenuInputBindingsResetBridge,
	&MenuInputBindingsHandleInputBridge,
	&MenuInputBindingsSetClipBridge
};

// STUB: LEGORACERS2 0x00415760
MenuInputBindingWidget::MenuInputBindingWidget() : MenuWidgetBase(-1)
{
	LegoU32 index;
	LegoU32 pair;
	for (pair = 3; pair < 7; ++pair) {
		m_bindingData[pair] = 0;
	}
	for (pair = 0; pair < 9; ++pair) {
		m_bindingData[7 + pair * 2] = 0;
		m_bindingData[8 + pair * 2] = 0;
		m_bindingData[25 + pair * 2] = 0;
		m_bindingData[26 + pair * 2] = 0;
	}
	m_bindingMode = 0;
	m_primarySelector = NULL;
	m_secondarySelector = NULL;
	for (index = 0; index < 15; ++index) {
		m_actionRows[index] = NULL;
	}
	for (index = 0; index < 11; ++index) {
		m_inputRows[index] = NULL;
	}
	m_vtable = &g_menuInputBindingWidgetDispatch;
}

// STUB: LEGORACERS2 0x004157f0
void MenuInputBindingWidget::SetFontAndCreateSelectors(BitmapFont* p_font)
{
	MenuLabelSelector* selector;
	m_font = p_font;
	selector = (MenuLabelSelector*) AllocateHeapMemory(sizeof(MenuLabelSelector));
	if (selector != NULL) {
		new (selector) MenuLabelSelector();
	}
	m_primarySelector = selector;
	selector->m_vtable->m_setFont(selector, NULL, p_font);

	selector = (MenuLabelSelector*) AllocateHeapMemory(sizeof(MenuLabelSelector));
	if (selector != NULL) {
		new (selector) MenuLabelSelector();
	}
	m_secondarySelector = selector;
	selector->m_vtable->m_setFont(selector, NULL, p_font);
}

// STUB: LEGORACERS2 0x004158b0
void MenuInputBindingWidget::DestroySelectors()
{
	MenuLabelSelector* selector = m_secondarySelector;
	if (selector != NULL) {
		selector->m_vtable->m_unk0x08(selector, NULL);
		selector->RestoreBaseDispatch();
		FreeHeapMemory(selector);
		m_secondarySelector = NULL;
	}
	selector = m_primarySelector;
	if (selector != NULL) {
		selector->m_vtable->m_unk0x08(selector, NULL);
		selector->RestoreBaseDispatch();
		FreeHeapMemory(selector);
		m_primarySelector = NULL;
	}
}

// STUB: LEGORACERS2 0x00415930
void MenuInputBindingWidget::Initialize(
	LegoS32 p_initialValue,
	const LegoFloat* p_position,
	LegoS32 p_bindingMode,
	const LegoU32* p_bindingData,
	void* p_bindingSource,
	LegoU32 p_wrapMode
)
{
	LegoU32 index;
	BeginState();
	m_initialValue = p_initialValue;
	m_unk0x14 = p_wrapMode;
	m_positionX = p_position[0];
	m_positionY = p_position[1];
	m_bindingMode = p_bindingMode;
	for (index = 0; index < 43; ++index) {
		m_bindingData[index] = p_bindingData[index];
	}
	m_bindingSource = p_bindingSource;
	m_pendingAction = -1;
	m_inputLatch = 0;
	m_selectedAction = 0;

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
	recordParameters[10] = 0x3f400000; // PE value at 0x0052fa80 is 0.75f.
	recordParameters[11] = 0;
	recordParameters[12] = 0;

	for (index = 0; index < 15; ++index) {
		if (m_actionRows[index] == NULL) {
			m_actionRows[index] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
		}
	}
	for (index = 0; index < 11; ++index) {
		if (m_inputRows[index] == NULL) {
			m_inputRows[index] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
		}
	}
	m_vtable->m_updateBounds(this, NULL);
	RefreshRows();
}

// STUB: LEGORACERS2 0x00415a60
void MenuInputBindingWidget::Reset()
{
	LegoS32 index;
	if (m_secondarySelector != NULL) {
		m_secondarySelector->m_vtable->m_reset(m_secondarySelector, NULL);
	}
	if (m_primarySelector != NULL) {
		m_primarySelector->m_vtable->m_reset(m_primarySelector, NULL);
	}
	for (index = 10; index >= 0; --index) {
		if (m_inputRows[index] != NULL) {
			m_inputRows[index]->Release();
			m_inputRows[index] = NULL;
		}
	}
	for (index = 14; index >= 0; --index) {
		if (m_actionRows[index] != NULL) {
			m_actionRows[index]->Release();
			m_actionRows[index] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x00415ae0
LegoU32 MenuInputBindingWidget::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	LegoS32 mode;
	LegoS32 deviceIndex;
	LegoS32 capturedMode = 0;
	LegoS32 capturedCode;
	LegoU32 limit;
	LegoU32 index;
	InputDeviceRecord* device;
	const LegoU16* label;
	LegoS32* target;
	LegoS32 targetSlot;
	LegoS32 firstSlot;
	LegoS32 slot;
	LegoS32 pairBase;
	*p_result = 0xffffffff;
	if (m_inputLatch != 0) {
		if ((p_input[0] & 1) != 0) {
			return 0;
		}
		m_inputLatch = 0;
	}
	if (m_pendingAction >= 0) {
		mode = (LegoS32) m_bindingData[1];
		deviceIndex = mode == 1 ? 0 : (LegoS32) m_bindingData[0];
		limit = mode == 1 ? 0x100 : 0x10;
		device = g_inputService->FindDeviceByType(deviceIndex, mode);
		if (device == NULL) {
			m_pendingAction = -1;
			return 0;
		}
		capturedCode = (LegoS32) limit;
		for (index = 0; index < limit; ++index) {
			if (device->m_vtable->isButtonPressed(device, NULL, index)) {
				capturedCode = (LegoS32) index;
				break;
			}
		}
		if ((LegoU32) capturedCode == limit) {
			LegoU32 axisCount = 0;
			if (device->m_vtable->VTable0x14 != NULL) {
				typedef LegoU32(__fastcall * AxisCount)(InputDeviceRecord*, void*);
				// The retail dispatch slot at +0x14 returns the number of axes.
				axisCount = ((AxisCount) device->m_vtable->VTable0x14)(device, NULL);
			}
			for (index = 1; index < axisCount; ++index) {
				LegoFloat value = device->m_vtable->getAxisValue(device, NULL, index);
				if (value < -0.5f) {
					capturedMode = 2;
					capturedCode = (LegoS32) index;
					break;
				}
				if (value > 0.5f) {
					capturedMode = 1;
					capturedCode = (LegoS32) index;
					break;
				}
			}
		}
		if (HasDuplicateMenuInputBinding(this, NULL, capturedCode, capturedMode, mode) ||
			(LegoU32) capturedCode >= limit || capturedCode == GetDefaultMenuInputCode(mode)) {
			if (p_input[1] == 0 && p_input[3] == 0) {
				return 0;
			}
			*p_result = 2;
			return 0;
		}
		if (mode == 1) {
			if (m_pendingAction < 2) {
				pairBase = 0x64;
				targetSlot = m_pendingAction;
			}
			else {
				pairBase = 0x74;
				targetSlot = m_pendingAction - 2;
			}
		}
		else {
			pairBase = 0xbc;
			targetSlot = m_pendingAction - 2;
		}
		target = (LegoS32*) ((LegoU8*) this + pairBase + targetSlot * 8);
		firstSlot = mode == 1 ? 0 : 2;
		for (slot = firstSlot; slot < 11; ++slot) {
			LegoS32* candidate;
			if (slot == m_pendingAction) {
				continue;
			}
			if (mode == 1) {
				candidate = (LegoS32*) ((LegoU8*) this + (slot < 2 ? 0x64 : 0x74) + (slot < 2 ? slot : slot - 2) * 8);
			}
			else {
				candidate = (LegoS32*) ((LegoU8*) this + 0xbc + (slot - 2) * 8);
			}
			if (candidate[0] == capturedMode && candidate[1] == capturedCode) {
				candidate[0] = target[0];
				candidate[1] = target[1];
				label = GetMenuInputName(mode, candidate);
				if (label != NULL && label[0] != 0) {
					m_inputRows[slot]->InitializeText(label, 0xd, 0.0f);
				}
				break;
			}
		}
		target[0] = capturedMode;
		target[1] = capturedCode;
		label = GetMenuInputName(mode, target);
		if (label != NULL) {
			m_inputRows[m_pendingAction + 2]->InitializeText(label, 0xd, 0.0f);
		}
		m_pendingAction = -1;
		*p_result = 0;
		return 0;
	}
	if (m_selectedAction == 0 && m_primarySelector != NULL) {
		m_primarySelector->m_vtable->m_unk0x10(m_primarySelector, NULL, p_result, p_input);
		if (*p_result != 0xffffffff) {
			LegoS32 selection = m_primarySelector->m_selection;
			if (selection == 0) {
				if (m_bindingData[1] != 3 || m_bindingData[0] != 0) {
					m_bindingData[1] = 3;
					m_bindingData[0] = 0;
				}
			}
			else if (selection == 1) {
				if (m_bindingData[1] != 3 || m_bindingData[0] != 1) {
					m_bindingData[1] = 3;
					m_bindingData[0] = 1;
				}
			}
			else {
				m_bindingData[1] = 1;
				m_bindingData[0] = 0;
			}
			RefreshRows();
			return 0;
		}
	}
	else if (m_selectedAction == 1 && m_secondarySelector != NULL) {
		m_secondarySelector->m_vtable->m_unk0x10(m_secondarySelector, NULL, p_result, p_input);
		if (*p_result != 0xffffffff) {
			m_bindingData[2] = m_secondarySelector->m_selection == 0;
		}
	}
	{
		LegoS32 direction = (p_input[3] & 4) != 0 ? -1 : ((p_input[3] & 8) != 0 ? 1 : 0);
		LegoS32 oldSelection = m_selectedAction;
		LegoS32 next = oldSelection;
		do {
			next += direction;
			if (next < 0) {
				next = 14;
			}
			else if (next > 14) {
				next = 0;
			}
		} while (m_actionRows[next]->m_text[0] == 0);
		if (next != oldSelection) {
			SetActionRowSelected(oldSelection, FALSE);
			SetActionRowSelected(next, TRUE);
			m_selectedAction = next;
			*p_result = 3;
		}
	}
	if ((p_input[1] & 1) != 0) {
		if (m_selectedAction >= 2 && m_selectedAction <= 12) {
			deviceIndex = m_bindingData[1] == 3 ? (LegoS32) m_bindingData[0] : 0;
			device = g_inputService->FindDeviceByType(deviceIndex, m_bindingData[1]);
			if (device != NULL) {
				m_pendingAction = m_selectedAction - 2;
				m_inputRows[m_pendingAction]->InitializeTextByIndex(-1, 0, 0.0f);
				m_inputLatch = 1;
				*p_result = 0;
			}
		}
		else if (m_selectedAction == 13) {
			LegoU32 resetMode = m_bindingData[1];
			LegoU32 resetDevice = resetMode == 3 ? m_bindingData[0] : m_bindingMode;
			m_bindingData[0] = resetDevice;
			m_bindingData[1] = resetMode;
			m_bindingData[2] = 1;
			m_bindingData[3] = 0;
			m_bindingData[5] = 0;
			for (index = 0; index < 9; ++index) {
				m_bindingData[7 + index * 2] = 0;
				m_bindingData[25 + index * 2] = 0;
			}
			m_bindingData[0x24] = 1;
			m_bindingData[0x28] = 1;
			m_bindingData[0x2a] = 1;
			m_bindingData[0x29] = 1;
			m_bindingData[0x1a] = 0;
			m_bindingData[0x1c] = 3;
			m_bindingData[0x1e] = 4;
			m_bindingData[0x20] = 6;
			m_bindingData[0x22] = 7;
			m_bindingData[0x26] = 2;
			m_bindingData[0x27] = 2;
			if (resetDevice == 0) {
				m_bindingData[4] = 0xcb;
				m_bindingData[6] = 0xcd;
				m_bindingData[8] = 200;
				m_bindingData[10] = 0x39;
				m_bindingData[12] = 0xd0;
				for (index = 0; index < 7; ++index) {
					m_bindingData[14 + index * 2] = index + 2;
				}
			}
			else {
				m_bindingData[4] = 0x4b;
				m_bindingData[6] = 0x4d;
				m_bindingData[8] = 0x48;
				m_bindingData[10] = 0x4c;
				m_bindingData[12] = 0x50;
				m_bindingData[14] = 0xd2;
				m_bindingData[16] = 199;
				m_bindingData[18] = 0xc9;
				m_bindingData[20] = 0xd3;
				m_bindingData[22] = 0xcf;
				m_bindingData[24] = 0xd1;
			}
			RefreshRows();
		}
		else if (m_selectedAction == 14) {
			*p_result = 0;
			return 2;
		}
		*p_result = 0;
	}
	if ((p_input[1] & 0x202) != 0 && m_pendingAction == -1) {
		*p_result = 1;
		return 1;
	}
	if (*p_result == 0xffffffff && (p_input[1] != 0 || p_input[3] != 0)) {
		*p_result = 2;
	}
	return 0;
}

// STUB: LEGORACERS2 0x00416090
void MenuInputBindingWidget::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	LegoU32 index;
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	for (index = 0; index < 15; ++index) {
		if (m_actionRows[index] != NULL) {
			m_actionRows[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
	for (index = 0; index < 11; ++index) {
		if (m_inputRows[index] != NULL) {
			m_inputRows[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
	if (m_primarySelector != NULL && m_primarySelector->m_state != 0) {
		m_primarySelector->m_vtable->m_setClipRectangle(m_primarySelector, NULL, p_topLeft, p_bottomRight);
	}
	if (m_secondarySelector != NULL && m_secondarySelector->m_state != 0) {
		m_secondarySelector->m_vtable->m_setClipRectangle(m_secondarySelector, NULL, p_topLeft, p_bottomRight);
	}
}

// STUB: LEGORACERS2 0x00416130
void MenuInputBindingWidget::UpdateBounds()
{
	LegoFloat actionWidth;
	LegoFloat inputWidth;
	LegoFloat halfGap = 0.1f * 0.5f;
	LegoFloat halfDifference;
	m_bounds[0] = 1.0f;
	m_bounds[1] = 1.0f;
	m_bounds[2] = 0.0f;
	m_bounds[3] = 0.0f;
	actionWidth = GetMenuActionHeadingTextWidth(this, NULL);
	inputWidth = GetMenuInputLabelTextWidth(this, NULL);
	halfDifference = (actionWidth - inputWidth) * 0.5f;
	m_unk0x50 = actionWidth * 0.5f + halfGap - halfDifference;
	m_unk0x54 = inputWidth * 0.5f + halfGap + halfDifference;
	m_bounds[0] = m_positionX - (halfGap + actionWidth - halfDifference);
	m_bounds[1] = m_positionY;
	m_bounds[2] = halfGap + halfDifference + inputWidth + m_positionX;
	m_bounds[3] = m_positionY + g_menuInputRowSpacing * 15.0f;
	m_bounds[3] += 0.02f;
}

// STUB: LEGORACERS2 0x00416210
void MenuInputBindingWidget::RefreshRows()
{
	static const LegoS32 primaryStrings[3] = {0xe4, 0xe4, 0xe5};
	static const LegoS32 primaryNumbers[3] = {1, 2, -1};
	static const LegoS32 secondaryStrings[2] = {0xe6, 0xe7};
	LegoU16 titleSuffix[2];
	LegoFloat position[2];
	LegoFloat y = m_positionY + 0.02f + g_menuInputRowSpacing;
	LegoS32 index;
	for (index = 0; index < 15; ++index) {
		m_actionRows[index]->InitializeTextByIndex(-1, 0, 0.0f);
	}
	for (index = 0; index < 11; ++index) {
		m_inputRows[index]->InitializeTextByIndex(-1, 0, 0.0f);
	}
	position[0] = m_positionX - m_unk0x54;
	position[1] = y;
	m_actionRows[0]->InitializeTextByIndex(0xa9, 0xd, 0.0f);
	m_actionRows[0]->SetPosition(position, 0xffffffff);
	titleSuffix[0] = (LegoU16) (m_bindingMode + 0x31);
	titleSuffix[1] = 0;
	m_actionRows[0]->AppendText(titleSuffix, 1);
	m_actionRows[0]->SetPosition(position, 0xffffffff);
	position[0] = m_positionX + m_unk0x50;
	position[1] = y;
	if (m_primarySelector != NULL) {
		if (m_primarySelector->m_state != 0) {
			m_primarySelector->m_vtable->m_reset(m_primarySelector, NULL);
		}
		m_primarySelector->Initialize(
			-1,
			position,
			primaryStrings,
			3,
			m_bindingData[1] == 1 ? 2 : (m_bindingData[0] == 0 ? 0 : 1),
			primaryNumbers,
			TRUE,
			0.75f,
			m_unk0x184,
			1
		);
	}
	y += g_menuInputRowSpacing;
	if (m_bindingData[1] == 3) {
		position[0] = m_positionX - m_unk0x54;
		position[1] = y;
		m_actionRows[1]->InitializeTextByIndex(0x33d, 9, 0.0f);
		m_actionRows[1]->SetPosition(position, 0xffffffff);
		position[0] = m_positionX + m_unk0x50;
		if (m_secondarySelector != NULL && m_secondarySelector->m_state != 0) {
			m_secondarySelector->m_vtable->m_reset(m_secondarySelector, NULL);
		}
		m_secondarySelector
			->Initialize(-1, position, secondaryStrings, 2, m_bindingData[2] == 0, NULL, TRUE, 0.75f, m_unk0x184, 1);
	}
	else {
		if (m_secondarySelector != NULL && m_secondarySelector->m_state != 0) {
			m_secondarySelector->m_vtable->m_reset(m_secondarySelector, NULL);
		}
		for (index = 0; index < 2; ++index) {
			if (index != 0) {
				y += g_menuInputRowSpacing;
			}
			position[0] = m_positionX - m_unk0x54;
			position[1] = y;
			m_actionRows[index + 2]->InitializeTextByIndex(index == 0 ? 0xe8 : 0xe9, 9, 0.0f);
			m_actionRows[index + 2]->SetPosition(position, 0xffffffff);
			position[0] = m_positionX + m_unk0x50;
			position[1] = y;
			m_inputRows[index]->InitializeText(GetMenuInputName(1, (LegoS32*) &m_bindingData[3 + index * 2]), 9, 0.0f);
			m_inputRows[index]->SetPosition(position, 0xffffffff);
		}
	}
	for (index = 0; index < 9; ++index) {
		y += g_menuInputRowSpacing;
		LegoS32 pairIndex = m_bindingData[1] == 1 ? 7 + index * 2 : 25 + index * 2;
		position[0] = m_positionX - m_unk0x54;
		position[1] = y;
		m_actionRows[index + 4]->InitializeTextByIndex(g_menuInputBindingStringIds[index], 9, 0.0f);
		m_actionRows[index + 4]->SetPosition(position, 0xffffffff);
		position[0] = m_positionX + m_unk0x50;
		m_inputRows[index + 2]
			->InitializeText(GetMenuInputName(m_bindingData[1], (LegoS32*) &m_bindingData[pairIndex]), 0xd, 0.0f);
		m_inputRows[index + 2]->SetPosition(position, 0xffffffff);
	}
	y += g_menuInputRowSpacing;
	position[0] = m_positionX - m_unk0x54;
	position[1] = y;
	m_actionRows[13]->InitializeTextByIndex(0x39c, 9, 0.0f);
	m_actionRows[13]->SetPosition(position, 0xffffffff);
	y += g_menuInputRowSpacing;
	position[1] = y;
	m_actionRows[14]->InitializeTextByIndex(6, 9, 0.0f);
	m_actionRows[14]->SetPosition(position, 0xffffffff);
	index = m_selectedAction;
	while (m_actionRows[index]->m_text[0] == 0) {
		index = (index + 1) % 15;
	}
	m_selectedAction = index;
	for (index = 0; index < 15; ++index) {
		if (m_actionRows[index]->m_text[0] != 0) {
			SetActionRowSelected(index, index == m_selectedAction);
		}
	}
}

// STUB: LEGORACERS2 0x00416700
void MenuInputBindingWidget::SetActionRowSelected(LegoS32 p_index, LegoBool32 p_selected)
{
	GlobalOverlayRecord* row = m_actionRows[p_index];
	if (p_selected != FALSE) {
		MenuManager* manager = (MenuManager*) g_gameController.m_menuManager;
		LegoU32 color = ((~manager->m_unk0x218 & 1) == 0) ? g_menuChoiceColorA : g_menuChoiceColorB;
		row->SetColor(&color);
		row->m_animationFlags |= 0x40;
	}
	else {
		LegoU32 color = 0xffffffff;
		row->SetColor(&color);
		row->m_animationFlags &= ~0x40;
	}
}

// STUB: LEGORACERS2 0x00416780
static LegoFloat __fastcall GetMenuActionHeadingTextWidth(MenuInputBindingWidget* p_widget, void*)
{
	static const LegoU16 oneSuffix[2] = {'1', 0};
	LegoFloat width = 0.0f;
	LegoS32 index;
	GlobalOverlayRecord* probe = p_widget->m_actionRows[14];
	if (probe == NULL) {
		return width;
	}
	for (index = 0; index < 15; ++index) {
		probe->InitializeTextByIndex(g_menuInputActionStringIds[index], 0xc, 0.0f);
		if (index == 0) {
			probe->AppendText(oneSuffix, 1);
		}
		if (probe->m_measuredWidth > width) {
			width = probe->m_measuredWidth;
		}
	}
	return width;
}

static const LegoU16* GetMenuInputEntryNameForSizing(const MenuInputNameEntry* p_entry, LegoS32 p_mode, LegoS32 p_index)
{
	if (p_entry->m_stringIndex == -1) {
		return p_entry->m_text;
	}
	if (p_mode == 3 && (p_entry->m_stringIndex == 0x23c || p_entry->m_stringIndex == 0x390)) {
		LegoS32 binding[2] = {0, p_index};
		return GetMenuInputName(p_mode, binding);
	}
	{
		const LegoU16* text = NULL;
		LocalizedStringTable* strings = GetLocalizedStringTable();
		if (strings != NULL) {
			strings->GetString(p_entry->m_stringIndex, &text);
		}
		return text;
	}
}

// STUB: LEGORACERS2 0x004167e0
static LegoFloat __fastcall GetMenuInputLabelTextWidth(MenuInputBindingWidget* p_widget, void*)
{
	static const LegoS32 extraStringIds[4] = {0xe5, 0xe4, 0xe6, 0xe7};
	static const LegoU16 oneSuffix[2] = {'1', 0};
	LegoFloat width = 0.0f;
	LegoFloat extraWidth = 0.0f;
	LegoU32 index;
	GlobalOverlayRecord* probe = p_widget->m_inputRows[10];
	if (probe == NULL) {
		return width;
	}
	for (index = 0; index < 16; ++index) {
		const MenuInputNameEntry* entry = &g_menuInputMode3Names[index];
		const LegoU16* name = GetMenuInputEntryNameForSizing(entry, 3, (LegoS32) index);
		if (name != NULL && name[0] != 0) {
			probe->InitializeText(name, 0xc, 0.0f);
			if (probe->m_measuredWidth > width) {
				width = probe->m_measuredWidth;
			}
		}
	}
	for (index = 0; index < 256; ++index) {
		const MenuInputNameEntry* entry = &g_menuInputMode1Names[index];
		const LegoU16* name = GetMenuInputEntryNameForSizing(entry, 1, (LegoS32) index);
		if (name != NULL && name[0] != 0) {
			probe->InitializeText(name, 8, 0.0f);
			if (probe->m_measuredWidth > width) {
				width = probe->m_measuredWidth;
			}
		}
	}
	for (index = 0; index < 2; ++index) {
		const MenuInputNameEntry* entry = &g_menuInputFallbackNames[index];
		const LegoU16* name = GetMenuInputEntryNameForSizing(entry, 1, (LegoS32) index);
		if (name != NULL && name[0] != 0) {
			probe->InitializeText(name, 0xc, 0.0f);
			if (probe->m_measuredWidth > width) {
				width = probe->m_measuredWidth;
			}
		}
	}
	p_widget->m_unk0x184 = 0.0f;
	for (index = 0; index < 4; ++index) {
		probe->InitializeTextByIndex(extraStringIds[index], 0xc, 0.0f);
		if (index == 1) {
			probe->AppendText(oneSuffix, 1);
		}
		if (probe->m_measuredWidth > p_widget->m_unk0x184) {
			p_widget->m_unk0x184 = probe->m_measuredWidth;
		}
		if (probe->m_measuredWidth > extraWidth) {
			extraWidth = probe->m_measuredWidth;
		}
	}
	extraWidth += 0.75f * 0.06f * 2.0f;
	return width > extraWidth ? width : extraWidth;
}

// STUB: LEGORACERS2 0x00416aa0
static LegoBool32 __fastcall HasDuplicateMenuInputBinding(
	const MenuInputBindingWidget* p_widget,
	void*,
	LegoS32 p_code,
	LegoS32 p_device,
	LegoS32 p_mode
)
{
	const LegoS32* pairs;
	LegoU32 i;
	if (p_mode == 3) {
		return FALSE;
	}
	pairs = (const LegoS32*) ((const LegoU8*) p_widget->m_bindingSource + 0x0c);
	for (i = 0; i < 2; ++i) {
		if (pairs[i * 2 + 1] == p_code && pairs[i * 2] == p_device) {
			return TRUE;
		}
	}
	for (i = 0; i < 9; ++i) {
		if (pairs[i * 2 + 5] == p_code && pairs[i * 2] == p_device) {
			return TRUE;
		}
	}
	return FALSE;
}
