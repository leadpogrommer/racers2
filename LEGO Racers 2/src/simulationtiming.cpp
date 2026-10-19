#include "simulationtiming.h"

#include "application.h"

// GLOBAL: LEGORACERS2 0x00579140
LegoU32 g_simulationFrameRate;

// GLOBAL: LEGORACERS2 0x005790d0
LegoFloat g_simulationStepSeconds;

// GLOBAL: LEGORACERS2 0x00579114
LegoFloat g_simulationStepMilliseconds;

// STUB: LEGORACERS2 0x004a5e90
void InitializeSimulationFrameRate()
{
	Application* application = GetApplication();
	g_simulationFrameRate = application->m_vtable->m_frameRate(application, NULL);
}

// FUNCTION: LEGORACERS2 0x004a5ec0
void InitializeSimulationStepSeconds()
{
	g_simulationStepSeconds = 1.0f / (LegoS32) g_simulationFrameRate;
}

// FUNCTION: LEGORACERS2 0x004a5ef0
void InitializeSimulationStepMilliseconds()
{
	g_simulationStepMilliseconds = 1000.0f / (LegoS32) g_simulationFrameRate;
}

// GLOBAL: LEGORACERS2 0x00579128
LegoFloat g_simulationFrameRateScale;

// FUNCTION: LEGORACERS2 0x004a5f20
void InitializeSimulationFrameRateScale()
{
	g_simulationFrameRateScale = 30.0f / (LegoS32) g_simulationFrameRate;
}

namespace
{
struct SimulationTimingStartup {
	SimulationTimingStartup()
	{
		InitializeSimulationFrameRate();
		InitializeSimulationStepSeconds();
		InitializeSimulationStepMilliseconds();
		InitializeSimulationFrameRateScale();
	}
} g_simulationTimingStartup;
} // namespace
