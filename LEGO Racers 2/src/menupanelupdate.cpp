#include "gamecontroller.h"
#include "menumanager.h"
#include "menupanel.h"

#include <math.h>

// STUB: LEGORACERS2 0x00418280
void MenuPanel::Update(LegoFloat p_elapsed)
{
	if (m_contextType == -1) {
		return;
	}
	if (m_contextType == 0) {
		LegoS32 selection = QuerySelection(&m_targetStyle, &m_targetHasSecondary);
		if (selection != m_selection && ((MenuManager*) g_gameController.m_menuManager)->m_unk0x1a4 != 0) {
			m_selection = selection;
			m_transition = 1;
			m_remaining = g_menuPanelTuning.m_duration;
			SetTransitionVelocity(1);
		}
	}
	else if ((m_contextType == 2 || m_contextType == 3) && *(LegoU32*) m_context != 0) {
		LegoFloat targetPosition[2], targetSize[2];
		QueryTargetGeometry(
			m_targetStyle,
			targetPosition,
			targetSize,
			m_targetSecondaryPosition,
			m_targetSecondarySize,
			&m_targetTranslucent
		);
		const LegoFloat* position = m_transition == 0 ? m_position : m_targetPosition;
		const LegoFloat* size = m_transition == 0 ? m_size : m_targetSize;
		if (!(fabs(position[0] - targetPosition[0]) <= 0.0001f && fabs(position[1] - targetPosition[1]) <= 0.0001f &&
			  fabs(size[0] - targetSize[0]) <= 0.0001f && fabs(size[1] - targetSize[1]) <= 0.0001f)) {
			m_targetPosition[0] = targetPosition[0];
			m_targetPosition[1] = targetPosition[1];
			m_targetSize[0] = targetSize[0];
			m_targetSize[1] = targetSize[1];
			m_transition = 3;
			m_remaining = g_menuPanelTuning.m_openStart;
			SetTransitionVelocity(3);
		}
	}
	m_remaining -= p_elapsed;
	LegoU32 oldTransition = m_transition;
	LegoFloat elapsed;
	if (m_transition == 1) {
		elapsed = p_elapsed;
		if (m_remaining < g_menuPanelTuning.m_closeEnd) {
			elapsed = (m_remaining - g_menuPanelTuning.m_closeEnd) + p_elapsed;
		}
		if (m_hasSecondary != 0) {
			m_secondarySize[0] = m_velocity[0] * elapsed + m_secondarySize[0];
			m_secondarySize[1] = m_velocity[1] * elapsed + m_secondarySize[1];
			if (m_secondarySize[0] < 0) {
				m_secondarySize[0] = 0;
			}
			if (m_secondarySize[1] < 0) {
				m_secondarySize[1] = 0;
			}
			UpdateSpriteGeometry(m_style, m_secondaryPosition, m_secondarySize, 1);
		}
		p_elapsed -= elapsed;
		if (m_remaining <= g_menuPanelTuning.m_closeEnd) {
			m_transition = 2;
			SetTransitionVelocity(2);
		}
	}
	if (m_transition == 2) {
		elapsed = p_elapsed;
		if (m_remaining < g_menuPanelTuning.m_openStart) {
			elapsed = (m_remaining - g_menuPanelTuning.m_openStart) + p_elapsed;
		}
		if (m_style != -1) {
			m_size[0] = m_velocity[0] * elapsed + m_size[0];
			m_size[1] = m_velocity[1] * elapsed + m_size[1];
			if (m_size[0] < 0) {
				m_size[0] = 0;
			}
			if (m_size[1] < 0) {
				m_size[1] = 0;
			}
			UpdateSpriteGeometry(m_style, m_position, m_size, 0);
		}
		p_elapsed -= elapsed;
		if (m_remaining <= g_menuPanelTuning.m_openStart) {
			if (m_contextType == 0) {
				if (QueryTargetGeometry(
						m_targetStyle,
						m_targetPosition,
						m_targetSize,
						m_targetSecondaryPosition,
						m_targetSecondarySize,
						&m_targetTranslucent
					)) {
					m_transition = 3;
					SetTransitionVelocity(3);
					ReleaseSprites();
					AllocateSprites(m_targetStyle != -1, m_targetHasSecondary);
				}
			}
			else {
				Deactivate();
			}
		}
	}
	if (m_transition == 3) {
		elapsed = p_elapsed;
		if (m_remaining < g_menuPanelTuning.m_openEnd) {
			elapsed = (m_remaining - g_menuPanelTuning.m_openEnd) + p_elapsed;
		}
		if (m_style != -1) {
			m_size[0] = m_velocity[0] * elapsed + m_size[0];
			m_size[1] = m_velocity[1] * elapsed + m_size[1];
			UpdateSpriteGeometry(m_style, m_position, m_size, 0);
		}
		p_elapsed -= elapsed;
		if (m_remaining <= g_menuPanelTuning.m_openEnd) {
			m_transition = 4;
			SetTransitionVelocity(4);
		}
	}
	if (m_transition == 4) {
		if (m_remaining < 0) {
			p_elapsed += m_remaining;
		}
		if (m_hasSecondary != 0) {
			m_secondarySize[0] = m_velocity[0] * p_elapsed + m_secondarySize[0];
			m_secondarySize[1] = m_velocity[1] * p_elapsed + m_secondarySize[1];
			UpdateSpriteGeometry(m_style, m_secondaryPosition, m_secondarySize, 1);
		}
		if (m_remaining <= 0) {
			m_transition = 0;
		}
	}
	if (oldTransition != 0) {
		UpdateClipping();
	}
}
