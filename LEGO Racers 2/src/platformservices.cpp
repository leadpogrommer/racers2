#include "platformservices.h"

#include "application.h"
#include "inputcontrollermanager.h"
#include "inputdeviceservice.h"
#include "renderer.h"
#include "simulationtiming.h"

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
