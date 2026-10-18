#include "globalpause.h"

#include "audioservice.h"
#include "gameframe.h"
#include "inputcontrollermanager.h"
#include "inputdeviceservice.h"
#include "systemclock.h"

// STUB: LEGORACERS2 0x00434150
LegoFloat ReadSignedClockSeconds()
{
	LegoS32 seconds;
	LegoS32 microseconds;
	SystemClock* clock = GetSystemClock();
	clock->m_vtable->m_queryTime(clock, NULL, &seconds, &microseconds);
	// Retail wraps the integer sum to 32 bits, then uses signed FILD DWORD.
	LegoU32 elapsed = (LegoU32) seconds * 1000000 + (LegoU32) microseconds;
	return (LegoS32) elapsed * 0.000001f;
}

// STUB: LEGORACERS2 0x004bb740
void GlobalPauseController::SetObserver(PauseObserver* p_observer)
{
	m_observer = p_observer;
}

// STUB: LEGORACERS2 0x004bb750
void GlobalPauseController::ClearObserver()
{
	m_observer = NULL;
}

// STUB: LEGORACERS2 0x004bb7a0
void GlobalPauseController::EnterPause()
{
	m_lastTime = ReadSignedClockSeconds();
	if (m_observer != NULL) {
		m_observer->m_vtable->m_enter(m_observer, NULL, m_observerMode);
	}
	g_inputControllerManager->SetPaused(1);
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_setMusicEnabled(audio, NULL, 1);
	g_inputService->m_vtable->poll(g_inputService, NULL, 0);
	m_paused = 1;
}

// STUB: LEGORACERS2 0x004bb7f0
void GlobalPauseController::LeavePause()
{
	m_lastTime = ReadSignedClockSeconds();
	if (m_observer != NULL) {
		m_observer->m_vtable->m_leave(m_observer, NULL);
	}
	g_inputControllerManager->SetPaused(0);
	if (m_resumeInput != 0) {
		AudioService* audio = GetAudioService(TRUE);
		audio->m_vtable->m_setMusicEnabled(audio, NULL, 0);
		g_inputService->m_vtable->poll(g_inputService, NULL, 1);
	}
	m_paused = 0;
}

// STUB: LEGORACERS2 0x004bb850
void GlobalPauseController::Update()
{
	if (m_enterRequested == 1) {
		m_enterRequested = 0;
		EnterPause();
	}
	if (m_leaveRequested == 1) {
		m_leaveRequested = 0;
		LeavePause();
	}
	if (m_paused != 0) {
		LegoFloat time = ReadSignedClockSeconds();
		LegoFloat delta = time - m_lastTime;
		m_lastTime = time;
		if (m_observer != NULL) {
			m_observer->m_vtable->m_update(m_observer, NULL, delta);
		}
		SimulationScheduler* scheduler = GetInitializedSimulationScheduler();
		AudioService* audio = GetAudioService(TRUE);
		audio->m_vtable->m_update(audio, NULL, scheduler->m_frame);
	}
}

// STUB: LEGORACERS2 0x004bb8f0
void GlobalPauseController::SetDeadline(LegoFloat p_seconds)
{
	m_deadline = ReadSignedClockSeconds() + p_seconds;
}

// STUB: LEGORACERS2 0x004bb910
LegoBool32 GlobalPauseController::IsBeforeDeadline()
{
	if (!(m_deadline >= 0.0f)) {
		return FALSE;
	}
	LegoFloat time = ReadSignedClockSeconds();
	if (!(time >= m_lastTime) || m_deadline < time) {
		m_deadline = -1.0f;
		return FALSE;
	}
	return TRUE;
}
