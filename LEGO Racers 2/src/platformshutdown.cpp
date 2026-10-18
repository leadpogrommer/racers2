#include "audioservice.h"
#include "gamemanager.h"
#include "inputcontrollermanager.h"
#include "inputdeviceservice.h"
#include "memoryheap.h"
#include "platformservices.h"
#include "renderer.h"

// STUB: LEGORACERS2 0x004f6360
void ShutdownPlatformServices()
{
	GetMemoryHeap()->SetUnknownFlag();
	ShutdownGameManager();
	DestroyAudioService();
	g_inputControllerManager->Shutdown();
	g_inputService->m_vtable->shutdown(g_inputService, NULL);
	g_displayRenderer->ShutdownResources();
}
