#include "controllerloadingprogress.h"

// GLOBAL: LEGORACERS2 0x0056be58
ControllerLoadingTuning g_controllerLoadingTuning;

// STUB: LEGORACERS2 0x004337c0
void InitializeControllerLoadingPhasePeriod()
{
	g_controllerLoadingTuning.m_phasePeriod = 3.14159265f * 2.0f;
}

// STUB: LEGORACERS2 0x004337e0
void InitializeControllerLoadingProgressOrigin()
{
	g_controllerLoadingTuning.m_progressOrigin[0] = 0.029296875f;
	g_controllerLoadingTuning.m_progressOrigin[1] = 0.0f + 0.848958313f;
}

// FUNCTION: LEGORACERS2 0x00433810
void InitializeControllerLoadingProgressSize()
{
	g_controllerLoadingTuning.m_progressSize[0] = 0.044921875f;
	g_controllerLoadingTuning.m_progressSize[1] = 0.125f;
}

// STUB: LEGORACERS2 0x00433840
void InitializeControllerLoadingEqualizerOrigin()
{
	g_controllerLoadingTuning.m_equalizerOrigin[0] = 0.60546875f;
	g_controllerLoadingTuning.m_equalizerOrigin[1] = 0.0f + 0.677083313f;
}

// FUNCTION: LEGORACERS2 0x00433870
void InitializeControllerLoadingEqualizerSize()
{
	g_controllerLoadingTuning.m_equalizerSize[0] = 0.0625f;
	g_controllerLoadingTuning.m_equalizerSize[1] = 0.0651041642f;
}

// FUNCTION: LEGORACERS2 0x004338a0
void InitializeControllerLoadingWordSize()
{
	g_controllerLoadingTuning.m_wordSize[0] = 0.25f;
	g_controllerLoadingTuning.m_wordSize[1] = 0.0416666679f;
}

namespace
{
struct ControllerLoadingTuningInitializer {
	ControllerLoadingTuningInitializer()
	{
		InitializeControllerLoadingPhasePeriod();
		InitializeControllerLoadingProgressOrigin();
		InitializeControllerLoadingProgressSize();
		InitializeControllerLoadingEqualizerOrigin();
		InitializeControllerLoadingEqualizerSize();
		InitializeControllerLoadingWordSize();
	}
};
ControllerLoadingTuningInitializer g_controllerLoadingTuningInitializer;
} // namespace
