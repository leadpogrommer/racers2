#include "audio/service/audioservice.h"
#include "game/objects/gamemanager.h"
#include "input/controller/inputcontrollermanager.h"
#include "input/service/inputdeviceservice.h"
#include "memory/memoryheap.h"
#include "platform/system/platformservices.h"
#include "render/core/renderer.h"

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
