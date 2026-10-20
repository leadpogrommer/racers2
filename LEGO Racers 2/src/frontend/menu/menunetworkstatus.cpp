#include "frontend/menu/menunetworkstatus.h"

#include "frontend/menu/menulayouttuning.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/widgets/menutrackselector.h"
#include "game/controller/gamecontroller.h"

#include <math.h>

// GLOBAL: LEGORACERS2 0x00561a08
LegoFloat g_networkStatusHeadingOffsetX;
// GLOBAL: LEGORACERS2 0x00561a0c
LegoFloat g_networkStatusHeadingOffsetY;
// GLOBAL: LEGORACERS2 0x00561a10
LegoFloat g_networkStatusFirstLabelOffsetX;
// GLOBAL: LEGORACERS2 0x00561a14
LegoFloat g_networkStatusFirstLabelOffsetY;
// GLOBAL: LEGORACERS2 0x00561a20
LegoFloat g_networkStatusSecondLabelOffsetX;
// GLOBAL: LEGORACERS2 0x00561a24
LegoFloat g_networkStatusSecondLabelOffsetY;

static LegoU32 s_networkStatusProviderLabelIds[3] = {104, 120, 121};
static const LegoChar s_unavailableProviderLabel[] = "---";
extern LegoU32 g_menuChoiceColorA;

struct ProviderSpriteVariantsView {
	undefined m_prefix[0x147c];
	LegoU32 m_entries[5][5];
};
DECOMP_STATIC_ASSERT(offsetof(ProviderSpriteVariantsView, m_entries) == 0x147c);

static void __fastcall MenuNetworkStatusUpdateBoundsBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuNetworkStatusSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font);
static void __fastcall MenuNetworkStatusBareReturnBridge(MenuWidgetBase*, void*);
static void __fastcall MenuNetworkStatusResetBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuNetworkStatusBaseSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
);
static LegoU32 __fastcall MenuNetworkStatusHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
);
const MenuWidgetDispatch g_menuNetworkStatusDispatch = {
	&MenuNetworkStatusUpdateBoundsBridge,
	&MenuNetworkStatusSetFontBridge,
	&MenuNetworkStatusBareReturnBridge,
	&MenuNetworkStatusResetBridge,
	&MenuNetworkStatusHandleInputBridge,
	&MenuNetworkStatusBaseSetClipBridge
};

struct InitializeNetworkStatusLayout {
	InitializeNetworkStatusLayout()
	{
		InitializeNetworkStatusHeadingOffset();
		InitializeNetworkStatusFirstLabelOffset();
		InitializeNetworkStatusSecondLabelOffset();
	}
};

static InitializeNetworkStatusLayout s_initializeNetworkStatusLayout;

// FUNCTION: LEGORACERS2 0x00425850
void InitializeNetworkStatusFirstLabelOffset()
{
	g_networkStatusFirstLabelOffsetX = 0.18f;
	g_networkStatusFirstLabelOffsetY = 0.1f;
}

// FUNCTION: LEGORACERS2 0x00425880
void InitializeNetworkStatusSecondLabelOffset()
{
	g_networkStatusSecondLabelOffsetX = 0.18f;
	g_networkStatusSecondLabelOffsetY = 0.17f;
}

// FUNCTION: LEGORACERS2 0x004258b0
void InitializeNetworkStatusHeadingOffset()
{
	g_networkStatusHeadingOffsetX = 0.18f;
	g_networkStatusHeadingOffsetY = 0.0f;
}

// STUB: LEGORACERS2 0x004258d0
MenuNetworkStatusWidget::MenuNetworkStatusWidget() : MenuWidgetBase(-1)
{
	m_vtable = &g_menuNetworkStatusDispatch;
	for (LegoU32 index = 0; index < 12; ++index) {
		m_records[index] = NULL;
	}
	m_statusSprite = NULL;
}

// STUB: LEGORACERS2 0x00425900
void MenuNetworkStatusWidget::Initialize(LegoS32 p_initialValue, const LegoFloat* p_position, LegoU32 p_mode)
{
	BeginState();
	m_initialValue = p_initialValue;
	m_mode = p_mode;
	m_position[0] = p_position[0];
	m_position[1] = p_position[1];

	// Retail leaves parameter words 2 and 3 untouched before passing this
	// 13-word record description to the overlay pool.
	LegoU32 recordParameters[13];
	recordParameters[0] = (LegoU32) m_font;
	recordParameters[1] = 0xffffffff;
	recordParameters[4] = 0;
	recordParameters[5] = 0;
	recordParameters[6] = 0x3f800000;
	recordParameters[7] = 0x3f800000;
	recordParameters[8] = 0;
	recordParameters[9] = 9;
	recordParameters[10] = 0x3f800000;
	recordParameters[11] = 0;
	recordParameters[12] = 0;

	for (LegoU32 recordIndex = 0; recordIndex < 12; ++recordIndex) {
		m_records[recordIndex] = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
	}

	// The original local sprite description also leaves words 8, 9, and 11
	// through 15 untouched. GlobalOverlaySprite::Initialize copies all 21.
	LegoU32 spriteParameters[21];
	spriteParameters[0] = 0;
	spriteParameters[1] = 0;
	spriteParameters[2] = 0;
	spriteParameters[3] = 0;
	spriteParameters[4] = 0;
	spriteParameters[5] = 0;
	spriteParameters[6] = 0x3f800000;
	spriteParameters[7] = 0x3f800000;
	spriteParameters[10] = 0xffffffff;
	spriteParameters[16] = 0;
	spriteParameters[17] = 0;
	spriteParameters[18] = 1;
	spriteParameters[19] = 0;
	spriteParameters[20] = 0;
	m_statusSprite = g_globalOverlayManager.AllocateSprite(spriteParameters);
	m_statusSprite->SetVisible(0);
	m_statusSprite->SetList(8);

	if (m_mode == 0) {
		LegoFloat position[2];
		position[0] = m_position[0] + 0.05f;
		position[1] = m_position[1];
		for (LegoU32 providerIndex = 0; providerIndex < 8; ++providerIndex) {
			m_records[providerIndex]->SetPosition(position, 0xffffffff);
			position[1] = position[1] + 0.05f;
		}

		LegoFloat spritePosition[2];
		spritePosition[0] = m_position[0] + g_networkStatusFirstLabelOffsetX + g_menuLayoutMarginX;
		spritePosition[1] = m_position[1] + g_networkStatusFirstLabelOffsetY + g_menuLayoutMarginY;
		m_statusSprite->SetPosition(spritePosition);
		LegoFloat spriteSize[2];
		spriteSize[0] = 0.125f;
		spriteSize[1] = 0.16666667f;
		m_statusSprite->SetSize(spriteSize);

		position[0] = m_position[0] + g_networkStatusSecondLabelOffsetX + g_menuLayoutMarginX;
		position[1] = m_position[1] + g_networkStatusSecondLabelOffsetY + g_menuLayoutMarginY;
		m_records[8]->SetPosition(position, 0xffffffff);
		m_records[8]->SetScale(0.5f);

		position[0] = m_position[0] + g_networkStatusHeadingOffsetX + g_menuLayoutMarginX;
		position[1] = m_position[1] + g_networkStatusHeadingOffsetY + g_menuLayoutMarginY;
		m_records[9]->SetPosition(position, 0xffffffff);
		m_records[9]->SetScale(0.5f);

		m_vtable->m_updateBounds(this, NULL);
		spritePosition[0] = (m_providerBounds[2] - m_providerBounds[0]) * 0.5f + m_bounds[2] + g_menuLayoutMarginX;
		spritePosition[1] = spriteSize[1] * 0.5f + m_statusSprite->m_top;
		m_statusSprite->SetPosition(spritePosition);

		for (LegoU32 statusLabelIndex = 8; statusLabelIndex < 10; ++statusLabelIndex) {
			position[0] = spritePosition[0];
			position[1] = m_records[statusLabelIndex]->m_positionY;
			m_records[statusLabelIndex]->SetPosition(position, 0xffffffff);
		}
	}
	else if (m_mode == 1) {
		LegoU32 color = g_menuChoiceColorA;
		for (LegoU32 statusRecordIndex = 0; statusRecordIndex < 2; ++statusRecordIndex) {
			m_records[statusRecordIndex]->SetPosition(m_position, 0xffffffff);
			m_records[statusRecordIndex]->SetColor(&color);
			m_records[statusRecordIndex]->SetScale(0.75f);
		}
		m_vtable->m_updateBounds(this, NULL);
	}
	else if (m_mode == 2) {
		LegoFloat position[2] = {m_position[0], m_position[1]};
		for (LegoU32 rowIndex = 0; rowIndex < 10; ++rowIndex) {
			m_records[rowIndex]->SetScale(0.65f);
			m_records[rowIndex]->SetPosition(position, 0xffffffff);
			position[1] = position[1] + 0.05f;
		}
		m_vtable->m_updateBounds(this, NULL);
	}
	else {
		m_vtable->m_updateBounds(this, NULL);
	}
}

// FUNCTION: LEGORACERS2 0x00425d70
void MenuNetworkStatusWidget::Reset()
{
	if (m_statusSprite != NULL) {
		m_statusSprite->Release();
		m_statusSprite = NULL;
	}
	for (LegoS32 index = 11; index >= 0; --index) {
		if (m_records[index] != NULL) {
			m_records[index]->Release();
			m_records[index] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x00425dc0
LegoU32 MenuNetworkStatusWidget::HandleInput(LegoU32* p_result, const LegoU32*)
{
	*p_result = 0xffffffff;
	NetworkApplicationMenuView* application = (NetworkApplicationMenuView*) g_gameController.m_networkApp;
	if (m_mode == 0) {
		for (LegoS32 index = 0; index < 8; ++index) {
			const LegoU16* label = application->GetProviderLabel(index);
			if (label == NULL) {
				m_records[index]->InitializeNarrowText(s_unavailableProviderLabel, 1, 0.0f);
			}
			else {
				m_records[index]->InitializeText(label, 1, 0.0f);
			}
		}

		const LegoS32 provider = application->m_selectedProvider;
		const LegoS32 providerCount = application->m_providerCount;
		if (provider >= 0 && provider < 5 && providerCount > 0 && providerCount < 6) {
			ProviderSpriteVariantsView* variants = (ProviderSpriteVariantsView*) g_gameController.m_unk0x90;
			LegoU32* spriteTextureData = (LegoU32*) &m_statusSprite->m_texture;
			for (LegoU32 word = 0; word < 5; ++word) {
				spriteTextureData[word] = variants->m_entries[provider][word];
			}
			m_statusSprite->SetVisible(1);

			static const LegoS32 providerStringBases[5] = {0x92, 0x8d, 0x97, 0x9c, 0xa1};
			m_records[8]->InitializeTextByIndex(providerStringBases[provider] + providerCount - 1, 9, 0.0f);
			switch (application->m_lastProviderLabel.m_mode) {
			case 0:
				m_records[9]->InitializeTextByIndex(0x68, 9, 0.0f);
				break;
			case 2:
				m_records[9]->InitializeTextByIndex(0x78, 9, 0.0f);
				break;
			case 3:
				m_records[9]->InitializeTextByIndex(0x79, 9, 0.0f);
				break;
			default:
				m_records[9]->InitializeTextByIndex(-1, 9, 0.0f);
				break;
			}
		}
		else {
			m_statusSprite->SetVisible(0);
			m_records[8]->InitializeTextByIndex(-1, 0, 0.0f);
			m_records[9]->InitializeTextByIndex(-1, 0, 0.0f);
		}
	}
	else if (m_mode == 1) {
		LegoBool32 isReady;
		const LegoU16* status = application->m_lobbyState.GetLobbyStatus(&isReady);
		m_records[0]->InitializeText(status, 8, 0.0f);
		if (isReady != 0 && (application->m_connectionMode == 2 || application->m_lobbyMode == 4) &&
			application->m_hasLobbyData != 0) {
			GlobalOverlayRecord* target = m_records[1];
			GlobalOverlayRecord* source = m_records[0];
			LegoFloat targetX = target->m_positionX;
			LegoFloat targetY = target->m_positionY;
			LegoFloat sourceX = source->m_positionX;
			LegoFloat sourceY = source->m_positionY;
			LegoFloat sourceRight = source->m_measuredWidth + source->m_positionX;
			if (target->m_text[0] == '-' && fabs(targetX - sourceRight) <= 0.0001f &&
				fabs(targetY - sourceY) <= 0.0001f) {
				return 0;
			}
			LegoFloat position[2] = {sourceRight, sourceY};
			target->SetPosition(position, 0xffffffff);
			target->InitializeNarrowText("-", 0x48, 0.0f);
		}
		else {
			m_records[1]->InitializeTextByIndex(-1, 0, 0.0f);
		}
	}
	else if (m_mode == 2) {
		for (LegoS32 index = 0; index < 10; ++index) {
			m_records[index]->InitializeText(application->m_lobbyState.GetLobbyText(index), 8, 0.0f);
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x00426130
void MenuNetworkStatusWidget::UpdateProviderBounds(LegoFloat* p_topLeft, LegoFloat* p_bottomRight)
{
	m_statusSprite->GetRectangle(p_topLeft, p_bottomRight);

	GlobalOverlayRecord* label = m_records[8];
	for (LegoU32 index = 0; index < 3; ++index) {
		label->InitializeTextByIndex(s_networkStatusProviderLabelIds[index], 9, 0.0f);
		LegoFloat labelTopLeft[2];
		LegoFloat labelBottomRight[2];
		label->GetRectangle(labelTopLeft, labelBottomRight);
		if (labelTopLeft[0] < p_topLeft[0]) {
			p_topLeft[0] = labelTopLeft[0];
		}
		if (labelTopLeft[1] < p_topLeft[1]) {
			p_topLeft[1] = labelTopLeft[1];
		}
		if (p_bottomRight[0] < labelBottomRight[0]) {
			p_bottomRight[0] = labelBottomRight[0];
		}
		if (p_bottomRight[1] < labelBottomRight[1]) {
			p_bottomRight[1] = labelBottomRight[1];
		}
	}
	label->InitializeTextByIndex(-1, 0, 0.0f);
	label = m_records[9];
	label->InitializeTextByIndex(0x92, 9, 0.0f);
	LegoFloat labelTopLeft[2];
	LegoFloat labelBottomRight[2];
	label->GetRectangle(labelTopLeft, labelBottomRight);
	if (labelTopLeft[0] < p_topLeft[0]) {
		p_topLeft[0] = labelTopLeft[0];
	}
	if (labelTopLeft[1] < p_topLeft[1]) {
		p_topLeft[1] = labelTopLeft[1];
	}
	if (p_bottomRight[0] < labelBottomRight[0]) {
		p_bottomRight[0] = labelBottomRight[0];
	}
	if (p_bottomRight[1] < labelBottomRight[1]) {
		p_bottomRight[1] = labelBottomRight[1];
	}

	LegoFloat labelWidth = MeasureLocalizedTrackLabelWidth(g_gameController.m_menuManager, NULL, -1, label);
	LegoFloat labelHalfExtent = labelWidth * 0.5f;
	LegoFloat labelLeft = label->m_positionX - labelHalfExtent * 0.5f;
	LegoFloat labelRight = labelLeft + labelHalfExtent;
	if (labelLeft < p_topLeft[0]) {
		p_topLeft[0] = labelLeft;
	}
	if (p_bottomRight[0] < labelRight) {
		p_bottomRight[0] = labelRight;
	}
	label->InitializeTextByIndex(-1, 0, 0.0f);
}

// STUB: LEGORACERS2 0x00426330
void MenuNetworkStatusWidget::UpdateBounds()
{
	if (m_mode == 0) {
		m_bounds[0] = m_position[0];
		m_bounds[1] = m_position[1];
		m_bounds[2] = m_position[0];
		m_bounds[3] = m_position[1];
		m_bounds[2] = 0.05f + 0.05f + m_bounds[2];
		m_bounds[3] = 0.05f * 8.0f + m_bounds[3];
		UpdateProviderBounds(&m_providerBounds[0], &m_providerBounds[2]);
	}
	else if (m_mode == 1) {
		m_bounds[0] = m_position[0];
		m_bounds[1] = m_position[1];
		m_bounds[2] = m_position[0];
		m_bounds[3] = m_position[1];
		m_bounds[2] = 0.5f + m_bounds[2];
		m_bounds[3] = m_records[0]->GetLineHeight() + m_bounds[3];
	}
	else if (m_mode == 2) {
		m_bounds[0] = m_position[0];
		m_bounds[1] = m_position[1];
		m_bounds[2] = m_position[0];
		m_bounds[3] = m_position[1];
		m_bounds[2] = 0.5f + m_bounds[2];
		m_bounds[3] = (0.05f * 9.0f) + m_bounds[3];
		m_bounds[3] = m_records[0]->GetLineHeight() + m_bounds[3];
	}
}

// STUB: LEGORACERS2 0x00494cd0
const LegoU16* NetworkApplicationMenuView::GetProviderLabel(LegoS32 p_index) const
{
	const NetworkProviderLabelView* entries = (const NetworkProviderLabelView*) &m_providerLabels[0];
	const LegoU8* entry = (const LegoU8*) entries + p_index * 0x334;
	if ((entry[0x1c] & 1) != 0 && p_index < m_availableProviderCount) {
		return (const LegoU16*) entry;
	}
	return NULL;
}

// FUNCTION: LEGORACERS2 0x00495100
const LegoU16* NetworkLobbyStateView::GetLobbyText(LegoS32 p_index) const
{
	LegoS32 row = m_currentRow + 1 + p_index;
	if (row >= 10) {
		row -= 10;
	}
	return (const LegoU16*) &m_textRows[row];
}

// FUNCTION: LEGORACERS2 0x00495130
const LegoU16* NetworkLobbyStateView::GetLobbyStatus(LegoBool32* p_isReady) const
{
	*p_isReady = m_statusCode < 0x12;
	return m_statusText;
}

static void __fastcall MenuNetworkStatusUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuNetworkStatusWidget*) p_widget)->UpdateBounds();
}

static void __fastcall MenuNetworkStatusSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	SetMenuWidgetFont(p_widget, NULL, p_font);
}

static void __fastcall MenuNetworkStatusBareReturnBridge(MenuWidgetBase*, void*)
{
}

static void __fastcall MenuNetworkStatusResetBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuNetworkStatusWidget*) p_widget)->Reset();
}

static LegoU32 __fastcall MenuNetworkStatusHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuNetworkStatusWidget*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall MenuNetworkStatusBaseSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuWidgetBase*) p_widget)->SetChildClipRectangle(p_topLeft, p_bottomRight);
}
