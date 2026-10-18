#ifndef LR2_SIMULATIONSCHEDULER_H
#define LR2_SIMULATIONSCHEDULER_H

#include "decomp.h"
#include "types.h"

// The five evidenced fields used by the fixed-step frame scheduler.
class SimulationScheduler {
public:
	void Reset(LegoU32 p_recordingMode);
	LegoBool32 NeedsSimulationStep(LegoFloat* p_interpolation);

	LegoU32 m_frameSeconds;
	LegoU32 m_frameMicroseconds;
	LegoU32 m_lastSeconds;
	LegoU32 m_lastMicroseconds;
	LegoU32 m_frame;
};

DECOMP_SIZE_ASSERT(SimulationScheduler, 0x14);

extern SimulationScheduler g_simulationScheduler;
extern LegoU32 g_unk0x00679fe0;
extern LegoU32 g_forceInitialSimulationStep;

#endif // LR2_SIMULATIONSCHEDULER_H
