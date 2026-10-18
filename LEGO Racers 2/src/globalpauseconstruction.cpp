#include "gameobjectdispatch.h"
#include "globalpause.h"

#include <new.h>
#include <stdlib.h>

// Retail table 0x005360d0 has the same twenty targets as the base-object table.
// The following 0x40490fdb float is separate data.
extern const GameObjectDispatchView g_globalPauseDispatch = GAME_OBJECT_BASE_DISPATCH_INITIALIZER;

// GLOBAL: LEGORACERS2 0x00679f88
LegoU32 g_globalPauseStorage[sizeof(GlobalPauseController) / sizeof(LegoU32)];

GlobalPauseController& g_globalPauseController = *(GlobalPauseController*) g_globalPauseStorage;

static void DestroyGlobalPauseController();

// STUB: LEGORACERS2 0x004bb670
void InitializeGlobalPauseController()
{
	CharBuffer24 second("");
	CharBuffer24 first("Global Pause Mode");
	GameObjectTemplate parameters;
	parameters.m_flags = 0x0a;
	parameters.m_first = first;
	parameters.m_second = second;
	new (g_globalPauseStorage) GlobalPauseController(&parameters);
}

// STUB: LEGORACERS2 0x004bb6d0
void RegisterGlobalPauseControllerExit()
{
	atexit(DestroyGlobalPauseController);
}

// STUB: LEGORACERS2 0x004bb6e0
static void DestroyGlobalPauseController()
{
	g_globalPauseController.GameObject::~GameObject();
}

// STUB: LEGORACERS2 0x004bb6f0
GlobalPauseController::GlobalPauseController(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_paused = 0;
	m_enterRequested = 0;
	m_leaveRequested = 0;
	m_observerMode = 0;
	m_resumeInput = 0;
	m_lastTime = 0;
	m_observer = NULL;
	m_deadline = -1;
	m_vtable = &g_globalPauseDispatch;
}

struct GlobalPauseInitialization {
	GlobalPauseInitialization()
	{
		InitializeGlobalPauseController();
		RegisterGlobalPauseControllerExit();
	}
};
static GlobalPauseInitialization g_globalPauseInitialization;
