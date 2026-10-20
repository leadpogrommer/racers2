#ifndef LR2_GAMEFRAME_H
#define LR2_GAMEFRAME_H

#include "physics/timing/simulationscheduler.h"

SimulationScheduler* GetInitializedSimulationScheduler();
void AdvanceGameFrame();

#endif // LR2_GAMEFRAME_H
