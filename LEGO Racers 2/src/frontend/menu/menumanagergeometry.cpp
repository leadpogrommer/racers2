#include "frontend/menu/menumanager.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/widgets/menutextpair.h"
#include "frontend/widgets/menuwidget.h"

// FUNCTION: LEGORACERS2 0x00423090
LegoBool32 MenuManager::GetPanelRectangle(LegoFloat* p_topLeft, LegoFloat* p_bottomRight)
{
	if (m_unk0x1a8 == 1) {
		return FALSE;
	}
	p_topLeft[0] = m_panelTopLeft[0];
	p_topLeft[1] = m_panelTopLeft[1];
	p_bottomRight[0] = m_panelBottomRight[0];
	p_bottomRight[1] = m_panelBottomRight[1];
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004230d0
void MenuManager::GetSecondaryPanelRectangle(LegoFloat* p_topLeft, LegoFloat* p_bottomRight)
{
	p_topLeft[0] = m_secondaryTopLeft[0];
	p_topLeft[1] = m_secondaryTopLeft[1];
	p_bottomRight[0] = m_secondaryBottomRight[0];
	p_bottomRight[1] = m_secondaryBottomRight[1];
}

// STUB: LEGORACERS2 0x004236a0
void MenuManager::SetPanelClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	LegoU32 i;
	for (i = 0; i < 8; ++i) {
		((GlobalOverlayRecord*) m_overlayRecords[i])->SetClipRectangle(p_topLeft, p_bottomRight);
	}
	for (i = 0; i < 18; ++i) {
		MenuWidgetBase* widget = m_widgets[i];
		if (widget != NULL && widget->m_state != 0 && widget->m_unk0x24 == 0 && widget->m_unk0x1c == 0 &&
			widget->m_unk0x20 == 0) {
			widget->m_vtable->m_setClipRectangle(widget, NULL, p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x00423710
void MenuManager::SetSecondaryPanelClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	for (LegoU32 i = 9; i < 14; ++i) {
		((GlobalOverlayRecord*) m_overlayRecords[i])->SetClipRectangle(p_topLeft, p_bottomRight);
	}
	m_transitionCover->SetClipRectangle(p_topLeft, p_bottomRight);
	MenuWidgetBase* widget = m_widgets[17];
	if (widget != NULL && widget->m_state != 0) {
		widget->m_vtable->m_setClipRectangle(widget, NULL, p_topLeft, p_bottomRight);
	}
}

// FUNCTION: LEGORACERS2 0x00423d00
void MenuTextPair::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	for (LegoU32 i = 0; i < 2; ++i) {
		if (m_records[i] != NULL) {
			m_records[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x00423d30
void MenuTextPair::GetRectangle(LegoFloat* p_topLeft, LegoFloat* p_bottomRight)
{
	if (m_records[0] == NULL) {
		p_topLeft[0] = p_topLeft[1] = 1.0f;
		p_bottomRight[0] = p_bottomRight[1] = 0;
		return;
	}
	for (LegoU32 i = 0; i < 2; ++i) {
		if (m_records[i] != NULL && m_records[i]->m_text[0] != 0) {
			LegoFloat topLeft[2], bottomRight[2];
			m_records[i]->GetRectangle(topLeft, bottomRight);
			if (topLeft[0] < p_topLeft[0]) {
				p_topLeft[0] = topLeft[0];
			}
			if (topLeft[1] < p_topLeft[1]) {
				p_topLeft[1] = topLeft[1];
			}
			if (p_bottomRight[0] < bottomRight[0]) {
				p_bottomRight[0] = bottomRight[0];
			}
			if (p_bottomRight[1] < bottomRight[1]) {
				p_bottomRight[1] = bottomRight[1];
			}
		}
	}
}
