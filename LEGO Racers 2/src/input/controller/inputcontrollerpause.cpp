#include "input/controller/inputcontrollermanager.h"

// STUB: LEGORACERS2 0x004b8f40
void InputControllerManager::SetPaused(LegoU32 p_paused)
{
	if (m_paused == p_paused) {
		return;
	}
	if (p_paused == 1) {
		GetTimingState((ControllerTimingState*) &m_unk0x6c);
		m_paused = 1;
		return;
	}

	ControllerTimingState state = {0, 0, 0, 0, 0, 0};
	GetTimingState(&state);
	LegoU32 microseconds = state.m_absoluteMicroseconds - m_unk0x70;
	while ((LegoS32) microseconds < 0) {
		--state.m_absoluteSeconds;
		microseconds += 1000000;
	}
	m_originMicroseconds += microseconds;
	while (m_originMicroseconds > 999999) {
		m_originMicroseconds -= 1000000;
		++m_originSeconds;
	}
	m_originSeconds += state.m_absoluteSeconds - m_unk0x6c;
	m_originFrame += state.m_absoluteFrame - m_unk0x7c;
	m_paused = p_paused;
}
