#include "platform/system/platformservices.h"

#include "core/application/application.h"
#include "input/controller/inputcontrollermanager.h"
#include "input/service/inputdeviceservice.h"
#include "physics/timing/simulationtiming.h"
#include "render/core/renderer.h"

// STUB: LEGORACERS2 0x004f6310
LegoBool32 InitializePlatformServices()
{
	if (g_displayRenderer->InitializeResources() == FALSE) {
		return FALSE;
	}
	if (g_inputService->m_vtable->initialize(g_inputService, NULL) == FALSE) {
		return FALSE;
	}
	Application* application = GetApplication();
	LegoU32 pollMultiplier = application->m_vtable->m_value0x74(application, NULL);
	g_inputControllerManager->InitializeTiming(g_simulationFrameRate, pollMultiplier);
	return TRUE;
}
