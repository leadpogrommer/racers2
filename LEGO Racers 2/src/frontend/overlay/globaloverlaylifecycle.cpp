#include "frontend/overlay/globaloverlaymanager.h"

#include <new.h>
#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00655228
LegoU32 g_globalOverlayManagerStorage[sizeof(GlobalOverlayManager) / sizeof(LegoU32)];

GlobalOverlayManager& g_globalOverlayManager = *(GlobalOverlayManager*) g_globalOverlayManagerStorage;

DECOMP_STATIC_ASSERT(sizeof(g_globalOverlayManagerStorage) == sizeof(GlobalOverlayManager));

static void DestroyGlobalOverlayManager();

// FUNCTION: LEGORACERS2 0x004b2d30
void InitializeGlobalOverlayManagerAndRegisterExit()
{
	InitializeGlobalOverlayManager();
	RegisterGlobalOverlayManagerExit();
}

// STUB: LEGORACERS2 0x004b2d40
void InitializeGlobalOverlayManager()
{
	CharBuffer24 first("Global Overlay Manager");
	CharBuffer24 second("");
	GameObjectTemplate parameters;
	parameters.m_flags = 0x0b;
	parameters.m_first = first;
	parameters.m_second = second;
	new (g_globalOverlayManagerStorage) GlobalOverlayManager(&parameters);
}

// FUNCTION: LEGORACERS2 0x004b2da0
void RegisterGlobalOverlayManagerExit()
{
	atexit(DestroyGlobalOverlayManager);
}

// STUB: LEGORACERS2 0x004b2db0
static void DestroyGlobalOverlayManager()
{
	for (LegoU32 index = 0; index < 200; ++index) {
		g_globalOverlayManager.m_records[index].m_vtable = &g_globalOverlayRecordDispatch;
	}
	g_globalOverlayManager.GameObject::~GameObject();
}

struct GlobalOverlayInitialization {
	GlobalOverlayInitialization() { InitializeGlobalOverlayManagerAndRegisterExit(); }
};

static GlobalOverlayInitialization g_globalOverlayInitialization;
