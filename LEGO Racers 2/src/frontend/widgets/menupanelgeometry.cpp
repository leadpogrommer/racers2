#include "frontend/menu/menumanager.h"
#include "frontend/resources/controllereventrecords.h"
#include "frontend/widgets/menupanel.h"
#include "frontend/widgets/menutextpair.h"
#include "frontend/widgets/menuwidget.h"
#include "game/controller/gamecontroller.h"

#include <string.h>

static void GetPanelPadding(LegoS32 p_style, LegoFloat* p_padding)
{
	const LegoFloat* padding = p_style == 2   ? g_menuPanelTuning.m_largePadding
							   : p_style == 3 ? g_menuPanelTuning.m_smallPadding
											  : g_menuPanelTuning.m_padding;
	p_padding[0] = padding[0];
	p_padding[1] = padding[1];
}

// STUB: LEGORACERS2 0x004187f0
void MenuPanel::UpdateClipping()
{
	LegoFloat origin[2] = {m_size[0] * -0.5f + m_position[0], m_size[1] * -0.5f + m_position[1]};
	LegoFloat padding[2];
	GetPanelPadding(m_style, padding);
	LegoFloat topLeft[2] = {padding[0] + origin[0], origin[1] + padding[1]};
	LegoFloat bottomRight[2] = {(origin[0] + m_size[0]) - padding[0], (origin[1] + m_size[1]) - padding[1]};
	switch (m_contextType) {
	case 0: {
		MenuManager* manager = (MenuManager*) m_context;
		manager->SetPanelClipRectangle(topLeft, bottomRight);
		if (m_hasSecondary != 0) {
			LegoFloat secondaryTopLeft[2] = {m_secondaryPosition[0], m_secondaryPosition[1]};
			LegoFloat secondaryBottomRight[2] = {
				m_secondaryPosition[0] + m_secondarySize[0],
				m_secondaryPosition[1] + m_secondarySize[1]
			};
			manager->SetSecondaryPanelClipRectangle(secondaryTopLeft, secondaryBottomRight);
		}
		break;
	}
	case 1:
	case 2: {
		MenuWidgetBase* widget = (MenuWidgetBase*) m_context;
		widget->m_vtable->m_setClipRectangle(widget, NULL, topLeft, bottomRight);
		break;
	}
	case 3:
		((MenuTextPair*) m_context)->SetClipRectangle(topLeft, bottomRight);
	}
}

// STUB: LEGORACERS2 0x00418a10
void MenuPanel::SetTransitionVelocity(LegoU32 p_transition)
{
	LegoFloat current[2];
	LegoFloat target[2];
	LegoFloat factor;
	switch (p_transition) {
	case 1:
		current[0] = m_secondarySize[0];
		current[1] = m_secondarySize[1];
		target[0] = 0;
		target[1] = m_secondarySize[1];
		factor = 1.0f / g_menuPanelTuning.m_closeDuration;
		break;
	case 2:
		current[0] = m_size[0];
		current[1] = m_size[1];
		target[0] = m_size[0];
		target[1] = 0;
		factor = 1.0f / g_menuPanelTuning.m_resizeDuration;
		break;
	case 3:
		if (m_size[1] < 0.01f) {
			m_size[0] = m_targetSize[0];
		}
		current[0] = m_size[0];
		current[1] = m_size[1];
		target[0] = m_targetSize[0];
		target[1] = m_targetSize[1];
		factor = 1.0f / g_menuPanelTuning.m_resizeDuration;
		m_style = m_targetStyle;
		m_position[0] = m_targetPosition[0];
		m_position[1] = m_targetPosition[1];
		m_translucent = m_targetTranslucent;
		m_hasSecondary = m_targetHasSecondary;
		m_secondaryPosition[0] = m_targetSecondaryPosition[0];
		m_secondaryPosition[1] = m_targetSecondaryPosition[1];
		break;
	case 4:
		if (m_secondarySize[0] < 0.01f) {
			m_secondarySize[1] = m_targetSecondarySize[1];
		}
		current[0] = m_secondarySize[0];
		current[1] = m_secondarySize[1];
		target[0] = m_targetSecondarySize[0];
		target[1] = m_targetSecondarySize[1];
		factor = 1.0f / g_menuPanelTuning.m_closeDuration;
		break;
	default:
		return;
	}
	m_velocity[0] = (target[0] - current[0]) * factor;
	m_velocity[1] = (target[1] - current[1]) * factor;
}

// STUB: LEGORACERS2 0x00418ba0
void MenuPanel::UpdateSpriteGeometry(
	LegoS32 p_style,
	const LegoFloat* p_position,
	const LegoFloat* p_size,
	LegoU32 p_secondary
)
{
	LegoFloat border[2], origin[2];
	GlobalOverlaySprite** sprites;
	GlobalOverlaySprite** shadows;
	LegoS32 recordIndex;
	if (p_secondary == 0) {
		const LegoFloat* selectedBorder = p_style == 2   ? g_menuPanelTuning.m_largeBorder
										  : p_style == 3 ? g_menuPanelTuning.m_smallBorder
														 : g_menuPanelTuning.m_border;
		border[0] = selectedBorder[0];
		border[1] = selectedBorder[1];
		origin[0] = p_size[0] * -0.5f + p_position[0];
		origin[1] = p_size[1] * -0.5f + p_position[1];
		sprites = m_sprites;
		shadows = m_shadows;
		recordIndex = p_style == 1 ? 188 : p_style == 2 ? 197 : p_style == 3 ? 206 : 179;
	}
	else {
		border[0] = g_menuPanelTuning.m_secondaryBorder[0];
		border[1] = g_menuPanelTuning.m_secondaryBorder[1];
		origin[0] = p_position[0];
		origin[1] = p_position[1];
		sprites = m_secondarySprites;
		shadows = m_secondaryShadows;
		recordIndex = p_style == 1 ? 224 : 215;
	}
	if (p_size[0] * 0.5f < border[0]) {
		border[0] = p_size[0] * 0.5f;
	}
	if (p_size[1] * 0.5f < border[1]) {
		border[1] = p_size[1] * 0.5f;
	}
	LegoFloat middle[2] = {p_size[0] - (border[0] + border[0]), p_size[1] - (border[1] + border[1])};
	LegoFloat shadowOffset[2] = {
		g_menuPanelTuning.m_shadowOffset[0] * p_size[0],
		g_menuPanelTuning.m_shadowOffset[1] * p_size[1]
	};
	ControllerEventRecord* records = ((ControllerEventRecordTable*) g_gameController.m_unk0x90)->m_records;
	LegoFloat position[2] = {0, 0};
	LegoFloat size[2] = {0, 0};
	for (LegoS32 i = 0; i < 9; ++i) {
		switch (i) {
		case 0:
			position[0] = origin[0];
			position[1] = origin[1];
			size[0] = border[0];
			size[1] = border[1];
			break;
		case 1:
			position[0] = (p_size[0] - border[0]) + position[0];
			break;
		case 2:
			position[1] = (p_size[1] - border[1]) + position[1];
			break;
		case 3:
			position[0] -= p_size[0] - border[0];
			break;
		case 4:
			position[0] = origin[0] + border[0];
			position[1] = origin[1];
			size[0] = middle[0];
			size[1] = border[1];
			break;
		case 5:
			position[0] = (p_size[0] - border[0]) + origin[0];
			position[1] = origin[1] + border[1];
			size[0] = border[0];
			size[1] = middle[1];
			break;
		case 6:
			position[0] = origin[0] + border[0];
			position[1] = (p_size[1] - border[1]) + origin[1];
			size[0] = middle[0];
			size[1] = border[1];
			break;
		case 7:
			position[0] = origin[0];
			position[1] = origin[1] + border[1];
			size[0] = border[0];
			size[1] = middle[1];
			break;
		case 8:
			position[0] = origin[0] + border[0];
			position[1] = origin[1] + border[1];
			size[0] = middle[0];
			size[1] = middle[1];
		}
		sprites[i]->SetPosition(position);
		sprites[i]->SetSize(size);
		memcpy(&sprites[i]->m_texture, &records[recordIndex + i], sizeof(ControllerEventRecord));
		sprites[i]->SetVisible(1);
		sprites[i]->SetAlpha(m_translucent == 0 ? 255 : 200);
		LegoFloat shadowPosition[2] = {position[0] + shadowOffset[0], position[1] + shadowOffset[1]};
		shadows[i]->SetPosition(shadowPosition);
		shadows[i]->SetSize(size);
		memcpy(&shadows[i]->m_texture, &records[recordIndex + i], sizeof(ControllerEventRecord));
		shadows[i]->SetVisible(1);
	}
}

// STUB: LEGORACERS2 0x00418fd0
LegoS32 MenuPanel::QuerySelection(LegoS32* p_style, LegoU32* p_hasSecondary)
{
	MenuManager* manager = (MenuManager*) m_context;
	LegoS32 selection = (LegoS32) manager->m_unk0x1a0;
	if (selection > 0 && selection < 0x23) {
		*p_style = ~manager->m_unk0x218 & 1;
	}
	else if ((selection >= 0x23 && selection <= 0x28) || selection == 0x29) {
		*p_style = 2;
	}
	else if (selection == 0x2b || selection == 0x2d || selection == 0x2e || selection == 0x2f || selection == 0x30) {
		*p_style = 0;
	}
	else {
		*p_style = selection == 0x2a ? 0 : -1;
	}
	*p_hasSecondary = manager->m_unk0x1dc;
	return selection;
}

// STUB: LEGORACERS2 0x00419090
LegoBool32 MenuPanel::QueryTargetGeometry(
	LegoS32 p_style,
	LegoFloat* p_position,
	LegoFloat* p_size,
	LegoFloat* p_secondaryPosition,
	LegoFloat* p_secondarySize,
	LegoU32* p_translucent
)
{
	if (p_style == -1) {
		p_position[0] = p_position[1] = p_size[0] = p_size[1] = 0;
		p_secondaryPosition[0] = p_secondaryPosition[1] = p_secondarySize[0] = p_secondarySize[1] = 0;
		*p_translucent = 0;
		return TRUE;
	}
	LegoFloat padding[2];
	GetPanelPadding(p_style, padding);
	LegoBool32 secondary = FALSE;
	LegoFloat topLeft[2] = {1, 1}, bottomRight[2] = {0, 0};
	LegoFloat secondaryTopLeft[2] = {1, 1}, secondaryBottomRight[2] = {0, 0};
	switch (m_contextType) {
	case 0: {
		MenuManager* manager = (MenuManager*) m_context;
		if (!manager->GetPanelRectangle(topLeft, bottomRight)) {
			return FALSE;
		}
		secondary = manager->m_unk0x1dc != 0;
		if (secondary) {
			manager->GetSecondaryPanelRectangle(secondaryTopLeft, secondaryBottomRight);
		}
		*p_translucent = manager->m_unk0x1ec;
		break;
	}
	case 1:
	case 2: {
		MenuWidgetBase* widget = (MenuWidgetBase*) m_context;
		topLeft[0] = widget->m_bounds[0];
		topLeft[1] = widget->m_bounds[1];
		bottomRight[0] = widget->m_bounds[2];
		bottomRight[1] = widget->m_bounds[3];
		secondary = widget->m_unk0x20 != 0;
		if (secondary) {
			// The extended bounds follow the common 0x38-byte widget prefix.
			const LegoFloat* extra = (const LegoFloat*) (widget + 1);
			secondaryTopLeft[0] = extra[0];
			secondaryTopLeft[1] = extra[1];
			secondaryBottomRight[0] = extra[2];
			secondaryBottomRight[1] = extra[3];
		}
		*p_translucent = p_style == 3;
		break;
	}
	case 3:
		((MenuTextPair*) m_context)->GetRectangle(topLeft, bottomRight);
		*p_translucent = 0;
	}
	p_position[0] = topLeft[0] - padding[0];
	p_position[1] = topLeft[1] - padding[1];
	p_position[0] = (bottomRight[0] + padding[0] + p_position[0]) * 0.5f;
	p_position[1] = (bottomRight[1] + padding[1] + p_position[1]) * 0.5f;
	p_size[0] = bottomRight[0] + padding[0];
	p_size[1] = bottomRight[1] + padding[1];
	p_size[0] -= topLeft[0] - padding[0];
	p_size[1] -= topLeft[1] - padding[1];
	if (!secondary) {
		p_secondaryPosition[0] = p_secondaryPosition[1] = p_secondarySize[0] = p_secondarySize[1] = 0;
		return TRUE;
	}
	p_secondaryPosition[0] = secondaryTopLeft[0];
	p_secondaryPosition[1] = secondaryTopLeft[1];
	p_secondarySize[0] = secondaryBottomRight[0];
	p_secondarySize[1] = secondaryBottomRight[1];
	p_secondarySize[0] -= p_secondaryPosition[0];
	p_secondarySize[1] -= p_secondaryPosition[1];
	p_secondaryPosition[0] = g_menuPanelTuning.m_secondaryOffset[0] + p_secondaryPosition[0];
	p_secondaryPosition[1] = g_menuPanelTuning.m_secondaryOffset[1] + p_secondaryPosition[1];
	p_secondarySize[0] -= g_menuPanelTuning.m_secondaryOffset[0];
	p_secondaryPosition[1] -= g_menuPanelTuning.m_secondaryInset[1];
	p_secondarySize[0] = g_menuPanelTuning.m_secondaryInset[0] + p_secondarySize[0];
	p_secondarySize[1] = g_menuPanelTuning.m_secondaryInset[1] + p_secondarySize[1];
	p_secondarySize[1] += g_menuPanelTuning.m_secondaryInset[1];
	return TRUE;
}
