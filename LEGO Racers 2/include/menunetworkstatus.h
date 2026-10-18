#ifndef LR2_MENUNETWORKSTATUS_H
#define LR2_MENUNETWORKSTATUS_H

#include "menuwidget.h"

#include <stddef.h>

struct NetworkProviderLabelView {
	undefined m_prefix[0x1c];
	LegoU8 m_available;
	undefined m_gap[0x334 - 0x1d];
};
DECOMP_SIZE_ASSERT(NetworkProviderLabelView, 0x334);
DECOMP_STATIC_ASSERT(offsetof(NetworkProviderLabelView, m_available) == 0x1c);

struct NetworkLobbyTextRowView {
	LegoU16 m_text[0x1a];
};
DECOMP_SIZE_ASSERT(NetworkLobbyTextRowView, 0x34);

struct NetworkLobbyStateView {
	LegoU16 m_statusText[0x18];
	LegoS32 m_statusCode;
	NetworkLobbyTextRowView m_textRows[10];
	LegoS32 m_currentRow;

	const LegoU16* GetLobbyText(LegoS32 p_index) const;
	const LegoU16* GetLobbyStatus(LegoBool32* p_isReady) const;
};
DECOMP_STATIC_ASSERT(offsetof(NetworkLobbyStateView, m_statusCode) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(NetworkLobbyStateView, m_textRows) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(NetworkLobbyStateView, m_currentRow) == 0x23c);

struct NetworkApplicationMenuView {
	undefined m_prefix[0x60];
	LegoS32 m_connectionMode; // +0x60
	LegoS32 m_lobbyMode;      // +0x64
	undefined m_gapToLobbyState[0xb74 - 0x68];
	NetworkLobbyStateView m_lobbyState;
	undefined m_gapToLabels[0xe54 - 0xb74 - sizeof(NetworkLobbyStateView)];
	NetworkProviderLabelView m_providerLabels[7];
	struct {
		undefined m_prefix[0x330];
		LegoS32 m_mode;
	} m_lastProviderLabel;
	LegoS32 m_selectedProvider; // +0x27f4
	LegoS32 m_providerCount;    // +0x27f8
	LegoU32 m_unk0x27fc;
	LegoS32 m_availableProviderCount; // +0x2800
	undefined m_gapToLobbyData[0x2a84 - 0x2804];
	LegoU32 m_hasLobbyData; // +0x2a84

	const LegoU16* GetProviderLabel(LegoS32 p_index) const;
};
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_connectionMode) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_lobbyMode) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_providerLabels) == 0xe54);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_lastProviderLabel) == 0x24c0);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_selectedProvider) == 0x27f4);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_providerCount) == 0x27f8);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_availableProviderCount) == 0x2800);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_hasLobbyData) == 0x2a84);
DECOMP_STATIC_ASSERT(offsetof(NetworkApplicationMenuView, m_lobbyState) == 0xb74);

struct MenuNetworkStatusWidget : MenuWidgetBase {
	LegoFloat m_providerBounds[4];       // +0x38, written by mode-0 bounds update
	LegoU32 m_mode;                      // +0x48
	LegoFloat m_position[2];             // +0x4c
	GlobalOverlayRecord* m_records[12];  // +0x54
	GlobalOverlaySprite* m_statusSprite; // +0x84

	MenuNetworkStatusWidget();
	void Initialize(LegoS32 p_initialValue, const LegoFloat* p_position, LegoU32 p_mode);
	void Reset();
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void UpdateBounds();
	void UpdateProviderBounds(LegoFloat* p_topLeft, LegoFloat* p_bottomRight);
};
DECOMP_SIZE_ASSERT(MenuNetworkStatusWidget, 0x88);
DECOMP_STATIC_ASSERT(offsetof(MenuNetworkStatusWidget, m_mode) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(MenuNetworkStatusWidget, m_providerBounds) == 0x38);
DECOMP_STATIC_ASSERT(offsetof(MenuNetworkStatusWidget, m_position) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuNetworkStatusWidget, m_records) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(MenuNetworkStatusWidget, m_statusSprite) == 0x84);

extern const MenuWidgetDispatch g_menuNetworkStatusDispatch;

// The three dynamic-initializer values are BSS in the original image.
extern LegoFloat g_networkStatusHeadingOffsetX;
extern LegoFloat g_networkStatusHeadingOffsetY;
extern LegoFloat g_networkStatusFirstLabelOffsetX;
extern LegoFloat g_networkStatusFirstLabelOffsetY;
extern LegoFloat g_networkStatusSecondLabelOffsetX;
extern LegoFloat g_networkStatusSecondLabelOffsetY;

void InitializeNetworkStatusFirstLabelOffset();
void InitializeNetworkStatusSecondLabelOffset();
void InitializeNetworkStatusHeadingOffset();

#endif // LR2_MENUNETWORKSTATUS_H
