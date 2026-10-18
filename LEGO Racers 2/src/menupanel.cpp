#include "menupanel.h"

#include "gamecontroller.h"
#include "menumanager.h"
#include "menuwidget.h"

// GLOBAL: LEGORACERS2 0x00561160
MenuPanelTuning g_menuPanelTuning = {0};

// Retail initializes these BSS values through the ordered static initializers
// at 417a30..417cf0, before any panel is constructed.
struct InitializeMenuPanelTuning {
	InitializeMenuPanelTuning()
	{
		g_menuPanelTuning.m_closeDuration = 0.5f * 0.15f;
		g_menuPanelTuning.m_resizeDuration = 0.5f * 0.35f;
		g_menuPanelTuning.m_duration = 0.5f;
		g_menuPanelTuning.m_closeEnd = g_menuPanelTuning.m_duration - g_menuPanelTuning.m_closeDuration;
		g_menuPanelTuning.m_openStart = g_menuPanelTuning.m_closeEnd - g_menuPanelTuning.m_resizeDuration;
		g_menuPanelTuning.m_openEnd = g_menuPanelTuning.m_openStart - g_menuPanelTuning.m_resizeDuration;
		g_menuPanelTuning.m_border[0] = 0.0625f;
		g_menuPanelTuning.m_border[1] = 0.083333336f;
		g_menuPanelTuning.m_padding[0] = g_menuPanelTuning.m_padding[1] = 0.04f;
		g_menuPanelTuning.m_largeBorder[0] = 0.125f;
		g_menuPanelTuning.m_largeBorder[1] = 0.16666667f;
		g_menuPanelTuning.m_largePadding[0] = g_menuPanelTuning.m_largePadding[1] = 0.1f;
		g_menuPanelTuning.m_smallBorder[0] = 0.03125f;
		g_menuPanelTuning.m_smallBorder[1] = 0.041666668f;
		g_menuPanelTuning.m_smallPadding[0] = g_menuPanelTuning.m_smallPadding[1] = 0.01f;
		g_menuPanelTuning.m_secondaryBorder[0] = 0.0625f;
		g_menuPanelTuning.m_secondaryBorder[1] = 0.083333336f;
		g_menuPanelTuning.m_secondaryInset[0] = g_menuPanelTuning.m_secondaryInset[1] = 0.02f;
		g_menuPanelTuning.m_secondaryOffset[0] = -0.01f;
		g_menuPanelTuning.m_secondaryOffset[1] = 0;
		g_menuPanelTuning.m_shadowOffset[0] = g_menuPanelTuning.m_shadowOffset[1] = 0.015f;
	}
};
static InitializeMenuPanelTuning g_initializeMenuPanelTuning;

// FUNCTION: LEGORACERS2 0x00417d10
MenuPanel::MenuPanel()
{
	m_listId = 6;
	for (LegoU32 i = 0; i < 9; ++i) {
		m_sprites[i] = NULL;
		m_shadows[i] = NULL;
		m_secondarySprites[i] = NULL;
		m_secondaryShadows[i] = NULL;
	}
}

// STUB: LEGORACERS2 0x00417d40
void MenuPanel::Initialize()
{
	m_contextType = m_selection = m_style = m_targetStyle = -1;
	m_velocity[0] = m_velocity[1] = 0;
	m_position[0] = m_position[1] = 0;
	m_targetPosition[0] = m_targetPosition[1] = 0;
	m_size[0] = m_size[1] = 0;
	m_targetSize[0] = m_targetSize[1] = 0;
	m_secondaryPosition[0] = m_secondaryPosition[1] = 0;
	m_targetSecondaryPosition[0] = m_targetSecondaryPosition[1] = 0;
	m_secondarySize[0] = m_secondarySize[1] = 0;
	m_targetSecondarySize[0] = m_targetSecondarySize[1] = 0;
	m_context = NULL;
	m_transition = 0;
	m_remaining = 0;
	m_translucent = m_targetTranslucent = 0;
	m_hasSecondary = m_targetHasSecondary = 0;
}

// FUNCTION: LEGORACERS2 0x00417e30
void MenuPanel::Release()
{
	ReleaseSprites();
}

// STUB: LEGORACERS2 0x00417e40
void MenuPanel::Activate(LegoS32 p_contextType, void* p_context)
{
	m_contextType = p_contextType;
	m_context = p_context;
	LegoU32 primary = 0;
	LegoU32 secondary = 0;
	if (p_contextType == 1 || p_contextType == 2 || p_contextType == 3) {
		m_targetStyle = 0;
		m_targetHasSecondary = 0;
		if (p_contextType == 1) {
			MenuWidgetBase* widget = (MenuWidgetBase*) p_context;
			if (widget->m_unk0x24 == 0) {
				m_targetStyle = 3;
			}
			else {
				MenuManager* manager = (MenuManager*) g_gameController.m_menuManager;
				m_targetStyle = (~manager->m_unk0x218 & 1) == 0 ? 0 : 1;
			}
			m_targetHasSecondary = widget->m_unk0x20;
			m_listId = widget->m_unk0x24 == 0 ? 7 : 6;
		}
		else if (p_contextType == 2) {
			m_listId = 10;
		}
		QueryTargetGeometry(
			m_targetStyle,
			m_targetPosition,
			m_targetSize,
			m_targetSecondaryPosition,
			m_targetSecondarySize,
			&m_targetTranslucent
		);
		m_remaining = g_menuPanelTuning.m_openStart;
		m_transition = 3;
		SetTransitionVelocity(3);
		primary = 1;
		secondary = m_targetHasSecondary;
	}
	AllocateSprites(primary, secondary);
	UpdateClipping();
}

// STUB: LEGORACERS2 0x00417fd0
void MenuPanel::AllocateSprites(LegoU32 p_primary, LegoU32 p_secondary)
{
	if (p_primary == 0 && p_secondary == 0) {
		return;
	}
	LegoU32 parameters[21];
	parameters[0] = parameters[1] = parameters[2] = parameters[3] = 0;
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	// Tile size and UV words are uninitialized until geometry is assigned,
	// exactly as in the retail parameter constructor (409190 is a bare RET).
	parameters[10] = 0xc8ffffff;
	parameters[11] = 0;
	parameters[16] = parameters[18] = parameters[19] = parameters[20] = 0;
	parameters[17] = 7;
	LegoU32 shadowColor = 0x80000000;
	if (p_primary != 0) {
		for (LegoU32 i = 0; i < 9; ++i) {
			if (m_sprites[i] == NULL) {
				m_sprites[i] = g_globalOverlayManager.AllocateSprite(parameters);
				m_sprites[i]->SetVisible(0);
				m_sprites[i]->SetList(m_listId);
			}
			if (m_shadows[i] == NULL) {
				m_shadows[i] = g_globalOverlayManager.AllocateSprite(parameters);
				m_shadows[i]->SetVisible(0);
				m_shadows[i]->SetList(5);
				m_shadows[i]->SetColor(&shadowColor);
			}
		}
	}
	if (p_secondary != 0) {
		for (LegoU32 i = 0; i < 9; ++i) {
			if (m_secondarySprites[i] == NULL) {
				m_secondarySprites[i] = g_globalOverlayManager.AllocateSprite(parameters);
				m_secondarySprites[i]->SetVisible(0);
				m_secondarySprites[i]->SetList(7);
			}
			if (m_secondaryShadows[i] == NULL) {
				m_secondaryShadows[i] = g_globalOverlayManager.AllocateSprite(parameters);
				m_secondaryShadows[i]->SetVisible(0);
				m_secondaryShadows[i]->SetList(5);
				m_secondaryShadows[i]->SetColor(&shadowColor);
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x004181c0
void MenuPanel::ReleaseSprites()
{
	LegoS32 i;
	for (i = 8; i >= 0; --i) {
		if (m_secondaryShadows[i] != NULL) {
			m_secondaryShadows[i]->Release();
			m_secondaryShadows[i] = NULL;
		}
		if (m_secondarySprites[i] != NULL) {
			m_secondarySprites[i]->Release();
			m_secondarySprites[i] = NULL;
		}
	}
	for (i = 8; i >= 0; --i) {
		if (m_shadows[i] != NULL) {
			m_shadows[i]->Release();
			m_shadows[i] = NULL;
		}
		if (m_sprites[i] != NULL) {
			m_sprites[i]->Release();
			m_sprites[i] = NULL;
		}
	}
}

// FUNCTION: LEGORACERS2 0x00418230
void MenuPanel::BeginClosing()
{
	if (m_contextType != -1) {
		m_transition = 1;
		if (m_contextType != 0) {
			m_transition = 1;
			m_remaining = g_menuPanelTuning.m_duration;
			SetTransitionVelocity(1);
		}
	}
}

// FUNCTION: LEGORACERS2 0x00418260
void MenuPanel::Deactivate()
{
	m_contextType = -1;
	m_transition = 0;
	ResetGeometry();
}

// STUB: LEGORACERS2 0x004189a0
void MenuPanel::ResetGeometry()
{
	ReleaseSprites();
	m_selection = m_style = -1;
	m_size[0] = m_size[1] = 0;
	m_secondarySize[0] = m_secondarySize[1] = 0;
	m_hasSecondary = 0;
}
