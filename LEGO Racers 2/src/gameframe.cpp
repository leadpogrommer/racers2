#include "gameframe.h"

#include "application.h"
#include "audioservice.h"
#include "gamemanager.h"
#include "globalpause.h"
#include "platformfpu.h"
#include "profiler.h"
#include "simulationtiming.h"

#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x005499c0
LegoU32 g_initializeGameObjects = 1;

// GLOBAL: LEGORACERS2 0x006ff500
LegoU8 g_simulationSchedulerInitialized;

void ApplicationNoOp();

// Source helper for the repeated retail lazy-initialization sequences.
SimulationScheduler* GetInitializedSimulationScheduler()
{
	if ((g_simulationSchedulerInitialized & 1) == 0) {
		g_simulationSchedulerInitialized |= 1;
		g_simulationScheduler.Reset(0);
		atexit(ApplicationNoOp);
	}
	return &g_simulationScheduler;
}

// STUB: LEGORACERS2 0x004a3aa0
void AdvanceGameFrame()
{
	CharBuffer24 totalName("Total Time");
	void* totalRange = BeginProfileRange(&totalName);
	Application* application = GetApplication();
	if (application->m_vtable->m_value0x1c(application, NULL) != 0) {
		return;
	}

	GameManager* manager = GetGameManager();
	LegoFloat stepSeconds = g_simulationStepSeconds;
	if (g_initializeGameObjects != 0 || GetApplication()->IsDevelopmentMode()) {
		manager->InitializeRegisteredObjects();
		manager = GetGameManager();
	}
	SimulationScheduler* scheduler = GetInitializedSimulationScheduler();
	InitializeRendererFloatingPoint();

	LegoFloat interpolation;
	while (scheduler->NeedsSimulationStep(&interpolation)) {
		CharBuffer24 moveName("Move All Time");
		void* moveRange = BeginProfileRange(&moveName);
		manager->SetControllerFrame(&scheduler->m_frame);
		if (g_gameObjectWorkListsValid != 1) {
			manager->RebuildObjectWorkLists();
		}
		manager->PrepareComponents();
		manager->MoveObjects(stepSeconds);
		EndProfileRange(moveRange);

		CharBuffer24 updateName("Update All Time");
		void* updateRange = BeginProfileRange(&updateName);
		union {
			LegoFloat m_float;
			LegoU32 m_bits;
		} step;
		step.m_float = stepSeconds;
		manager->UpdateObjects(step.m_bits);
		if (GetApplication()->IsDevelopmentMode()) {
			manager->InitializeRegisteredObjects();
		}
		++scheduler->m_frame;
		EndProfileRange(updateRange);
	}

	CharBuffer24 soundName("Update Sound Time");
	void* soundRange = BeginProfileRange(&soundName);
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_update(audio, NULL, scheduler->m_frame);
	EndProfileRange(soundRange);
	g_globalPauseController.Update();
	if (g_gameObjectWorkListsValid != 1) {
		manager->RebuildObjectWorkLists();
	}
	manager->RenderFrame(interpolation);
	application = GetApplication();
	application->m_vtable->m_action0x28(application, NULL);
	g_initializeGameObjects = 0;
	EndProfileRange(totalRange);
}
