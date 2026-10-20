#include "audio/reverb/reverbmanager.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontrollerinitialization.h"
#include "game/pause/globalpause.h"
#include "input/controller/inputcontrollermanager.h"
#include "memory/memoryheap.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"

void __fastcall EnterSecondaryManagerMode(GameObject*, void*, LegoU32);

struct GamePauseObserver : PauseObserver {
	LegoU32 m_mode;
	LegoU32 m_inputLatch[4];
	GlobalOverlaySprite* m_cover;
};
DECOMP_SIZE_ASSERT(GamePauseObserver, 0x1c);
struct GamePauseObserverDispatch {
	void(__fastcall* m_initialize)(GamePauseObserver*, void*);
	void(__fastcall* m_release)(GamePauseObserver*, void*);
	void(__fastcall* m_enter)(GamePauseObserver*, void*, LegoU32);
	void(__fastcall* m_leave)(GamePauseObserver*, void*);
	void(__fastcall* m_update)(GamePauseObserver*, void*, LegoFloat);
};
static void __fastcall InitializeGamePauseObserver(GamePauseObserver*, void*);
static void __fastcall ReleaseGamePauseObserver(GamePauseObserver*, void*);
static void __fastcall UpdateGamePauseObserver(GamePauseObserver*, void*, LegoFloat);
static void __fastcall EnterGamePauseObserver(GamePauseObserver*, void*, LegoU32);
static void __fastcall LeaveGamePauseObserver(GamePauseObserver*, void*);
static void __fastcall RegisterGamePauseObserver(GamePauseObserver*, void*);
static void __fastcall ClearGamePauseObserver(GamePauseObserver*, void*);
static void __fastcall SetGamePauseObserverMode(GamePauseObserver*, void*, LegoU32);
static void __stdcall AdvancePauseOverlays(LegoU32);
// Retail52f5d8 completefive-callbackobserverinterface.
extern const GamePauseObserverDispatch g_gamePauseObserverDispatch = {
	&InitializeGamePauseObserver,
	&ReleaseGamePauseObserver,
	&EnterGamePauseObserver,
	&LeaveGamePauseObserver,
	&UpdateGamePauseObserver
};

// Inlined allocation in GameController::Initialize407250.
void* ConstructGamePauseObserver(void* p_storage)
{
	GamePauseObserver* observer = (GamePauseObserver*) p_storage;
	observer->m_mode = 0;
	for (LegoU32 i = 0; i < 4; ++i) {
		observer->m_inputLatch[i] = 0;
	}
	observer->m_cover = NULL;
	observer->m_vtable = (const PauseObserverDispatch*) &g_gamePauseObserverDispatch;
	return observer;
}

// STUB: LEGORACERS2 0x0040b670
static void __fastcall InitializeGamePauseObserver(GamePauseObserver* p_observer, void*)
{
	RegisterGamePauseObserver(p_observer, NULL);
}
// STUB: LEGORACERS2 0x0040b680
static void __fastcall ReleaseGamePauseObserver(GamePauseObserver* p_observer, void*)
{
	if (p_observer->m_cover != NULL) {
		p_observer->m_cover->Release();
		p_observer->m_cover = NULL;
	}
	ClearGamePauseObserver(p_observer, NULL);
}
// STUB: LEGORACERS2 0x0040b6a0
static void __fastcall UpdateGamePauseObserver(GamePauseObserver*, void*, LegoFloat p_delta)
{
	union {
		LegoFloat m_float;
		LegoU32 m_word;
	} elapsed;
	elapsed.m_float = p_delta;
	AdvancePauseOverlays(elapsed.m_word);
	GameObject* network = g_gameController.m_networkApp;
	if (*(LegoU32*) ((LegoU8*) network + 0x5c) != 3) {
		network->m_vtable->m_update(network, NULL, elapsed.m_word);
		return;
	}
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (secondary->m_state == 3 || secondary->m_state == 4) {
		secondary->UpdateSpecialMode(elapsed.m_word);
	}
	else {
		secondary->UpdateMenuMode(0, elapsed.m_word);
	}
}
// STUB: LEGORACERS2 0x0040b6f0
static void __fastcall EnterGamePauseObserver(GamePauseObserver* p_observer, void*, LegoU32 p_mode)
{
	SetGamePauseObserverMode(p_observer, NULL, p_mode);
	for (LegoU32 i = 0; i < 4; ++i) {
		p_observer->m_inputLatch[i] = 0;
	}
	EnterSecondaryManagerMode(g_gameController.m_secondaryManager, NULL, g_gameController.m_state == 3 ? 36 : 35);
	if (p_observer->m_cover == NULL) {
		// Retail leaves tile-dimension words8/9 untouched in this creation record.
		LegoU32 parameters[21];
		parameters[0] = 0;
		parameters[1] = 0;
		parameters[2] = 0x3f800000;
		parameters[3] = 0x3f800000;
		parameters[4] = 0;
		parameters[5] = 0;
		parameters[6] = 0x3f800000;
		parameters[7] = 0x3f800000;
		parameters[10] = 0x80ffffff;
		parameters[11] = 0xffffffff;
		parameters[12] = 0;
		parameters[13] = 0x3f800000;
		parameters[14] = 0x3f800000;
		parameters[15] = 0;
		parameters[16] = 0;
		parameters[17] = 5;
		parameters[18] = 0;
		parameters[19] = 0;
		parameters[20] = 0;
		p_observer->m_cover = g_globalOverlayManager.AllocateSprite(parameters);
		p_observer->m_cover->SetList(0);
	}
	((ReverbManager*) g_gameController.m_reverbManager)->DisableOutput();
	g_inputControllerManager->m_unk0x68 = 0;
}
// STUB: LEGORACERS2 0x0040b850
static void __fastcall LeaveGamePauseObserver(GamePauseObserver* p_observer, void*)
{
	((ReverbManager*) g_gameController.m_reverbManager)->RestoreOutput();
	if (p_observer->m_cover != NULL) {
		p_observer->m_cover->Release();
		p_observer->m_cover = NULL;
	}
	g_globalPauseController.SetDeadline(0.3f);
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	LegoU32 culling = (*(LegoU32*) ((LegoU8*) secondary->m_unk0x17c + 0x178) >> 11) & 1;
	DisplayRenderer* renderer = g_displayRenderer;
	((const RendererMainDispatch*) renderer->m_vtable)->m_setClockwiseCulling(renderer, NULL, culling);
	g_inputControllerManager->m_unk0x68 = culling;
	p_observer->m_mode = 0;
}
// STUB: LEGORACERS2 0x004bb980
static void __fastcall RegisterGamePauseObserver(GamePauseObserver* p_observer, void*)
{
	g_globalPauseController.SetObserver(p_observer);
}
// STUB: LEGORACERS2 0x004bb990
static void __fastcall ClearGamePauseObserver(GamePauseObserver*, void*)
{
	g_globalPauseController.ClearObserver();
}
// STUB: LEGORACERS2 0x004bb9a0
static void __fastcall SetGamePauseObserverMode(GamePauseObserver* p_observer, void*, LegoU32 p_mode)
{
	p_observer->m_mode = p_mode;
}
// STUB: LEGORACERS2 0x004bb9b0
static void __stdcall AdvancePauseOverlays(LegoU32 p_elapsed)
{
	GlobalOverlayManagerUpdateRecords(&g_globalOverlayManager, NULL, p_elapsed);
}
