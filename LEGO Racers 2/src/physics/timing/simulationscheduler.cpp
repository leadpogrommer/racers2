#include "physics/timing/simulationscheduler.h"

#include "core/application/application.h"
#include "input/controller/inputcontrollermanager.h"
#include "physics/timing/simulationtiming.h"
#include "platform/timing/systemclock.h"

// GLOBAL: LEGORACERS2 0x006ff520
SimulationScheduler g_simulationScheduler;

// This gate is shared with unrecovered game-state code; its purpose is not
// established sufficiently to name it as a general simulation pause flag.
// GLOBAL: LEGORACERS2 0x00679fe0
LegoU32 g_unk0x00679fe0;

// GLOBAL: LEGORACERS2 0x005499bc
LegoU32 g_forceInitialSimulationStep = 1;

// GLOBAL: LEGORACERS2 0x00578c10
static LegoU32 g_developmentSimulationToggle;

// STUB: LEGORACERS2 0x004a3810
void SimulationScheduler::Reset(LegoU32 p_recordingMode)
{
	m_frame = 0;
	m_frameMicroseconds = 0;
	m_frameSeconds = 0;
	m_lastMicroseconds = 0;
	m_lastSeconds = 0;
	SystemClock* clock = GetSystemClock();
	clock->m_vtable->m_reset(clock, NULL);
	g_inputControllerManager->ResetRecording(p_recordingMode);
}

// STUB: LEGORACERS2 0x004a3840
LegoBool32 SimulationScheduler::NeedsSimulationStep(LegoFloat* p_interpolation)
{
	if (g_unk0x00679fe0 != 0) {
		*p_interpolation = 0.0f;
		return FALSE;
	}
	LegoU32 seconds;
	LegoU32 microseconds;
	LegoU32 frame;
	g_inputControllerManager->GetElapsedTime(&seconds, &microseconds, &frame);
	Application* application = GetApplication();
	if (application->m_vtable->m_developmentMode(application, NULL) != 0) {
		g_developmentSimulationToggle ^= 1;
		*p_interpolation = 0.0f;
		return g_developmentSimulationToggle;
	}
	if (g_forceInitialSimulationStep != 0) {
		g_forceInitialSimulationStep = 0;
		*p_interpolation = 0.0f;
		return TRUE;
	}

	m_frameSeconds = m_frame / g_simulationFrameRate;
	m_frameMicroseconds = ((m_frame % g_simulationFrameRate) * 1000000) / g_simulationFrameRate;
	LegoU32 frameSeconds = m_frameSeconds;
	LegoU32 frameMicroseconds = m_frameMicroseconds;
	LegoBool32 behind = frameSeconds < seconds || (frameSeconds == seconds && frameMicroseconds < microseconds);
	LegoU32 elapsed = seconds * 1000000 - m_lastSeconds * 1000000 - m_lastMicroseconds + microseconds;
	if (elapsed <= 1000000 && behind != FALSE) {
		return TRUE;
	}

	// Retail multiplies in 32 bits and then zero-extends for FILD QWORD.
	LegoU32 offset =
		(frameSeconds * 1000000 - seconds * 1000000 - microseconds + frameMicroseconds) * g_simulationFrameRate;
	LegoFloat interpolation = (LegoFloat) offset * 0.000001f;
	*p_interpolation = interpolation;
	if (!(interpolation >= 0.0f) || interpolation > 1.001f) {
		*p_interpolation = 0.0f;
	}
	m_lastSeconds = seconds;
	m_lastMicroseconds = microseconds;
	return FALSE;
}
