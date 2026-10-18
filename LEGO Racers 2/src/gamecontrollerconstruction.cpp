#include "gamecontroller.h"

#include <new.h>
#include <stdlib.h>

void __fastcall InitializeGameControllerWord(LegoU32*, void*);
static void DestroyGameControllerAtExit();

// GLOBAL: LEGORACERS2 0x005542c0
GameObjectTemplate g_gameControllerTemplate;

// GLOBAL: LEGORACERS2 0x005543f8
LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];
GameController& g_gameController = *(GameController*) g_gameControllerStorage;

// STUB: LEGORACERS2 0x00406ee0
void InitializeGameControllerTemplate()
{
	CharBuffer24 second("");
	CharBuffer24 first("Game Manager");
	GameObjectTemplate parameters;
	parameters.m_flags = 2;
	parameters.m_first = first;
	parameters.m_second = second;
	g_gameControllerTemplate = parameters;
}

// STUB: LEGORACERS2 0x00406f50
void ConstructGameController()
{
	new (g_gameControllerStorage) GameController(&g_gameControllerTemplate);
}

// STUB: LEGORACERS2 0x00406f60
void RegisterGameControllerExit()
{
	atexit(DestroyGameControllerAtExit);
}

// STUB: LEGORACERS2 0x00406f70
static void DestroyGameControllerAtExit()
{
	g_gameController.~GameController();
}

// STUB: LEGORACERS2 0x00406fa0
GameController::GameController(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_state = 0;
	m_stateStarted = 0;
	m_requestedState = -1;
	m_startupReady = 0;
	m_unk0x68 = 0;
	m_pauseDetector = NULL;
	m_unk0x70 = NULL;
	m_localizedStrings = NULL;
	m_secondaryManager = NULL;
	m_raceManager = NULL;
	m_battleManager = NULL;
	m_foyerManager = NULL;
	m_unk0x88 = NULL;
	m_unk0x8c = NULL;
	m_unk0x90 = NULL;
	m_textBoxManager = NULL;
	m_npcTextManager = NULL;
	m_bodypartManager = NULL;
	m_unk0xa0 = 0;
	m_unk0xa4 = 0;
	m_unk0xac = 0;
	m_menuManager = NULL;
	m_unk0xb4 = NULL;
	m_networkApp = NULL;
	m_unk0xbc = NULL;
	m_killSphereSupervisor = NULL;
	m_reverbManager = NULL;
	m_unk0xd0 = 0;
	m_unk0xd4 = NULL;
	m_unk0xd8 = NULL;
	m_unk0xdc = 0;
	m_unk0xe0 = -1;
	LegoU32 i, j;
	for (i = 0; i < 2; ++i) {
		m_names[i].Initialize();
	}
	for (i = 0; i < 2; ++i) {
		m_pairGroups[i].Initialize();
	}
	m_selection.Initialize();
	InitializeGameControllerWord(&m_unk0x370, NULL);
	for (i = 0; i < 3; ++i) {
		for (j = 0; j < 25; ++j) {
			m_scores[i][j].Initialize();
		}
	}
	for (i = 0; i < 40; ++i) {
		GameControllerItem* items = i < 39 ? m_itemBlocks[i].m_items : m_lastBlockItems;
		for (j = 0; j < 36; ++j) {
			items[j].Initialize();
		}
	}
	for (i = 0; i < 2; ++i) {
		for (j = 0; j < 4; ++j) {
			m_blockState[i][j] = 0;
		}
	}
	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 36; ++j) {
			m_playerBlocks[i].m_items[j].Initialize();
		}
		for (j = 0; j < 4; ++j) {
			m_playerBlocks[i].m_state[j] = 0;
		}
	}
	m_vtable = &g_gameControllerDispatch.m_base;
	m_unk0xe4 = 0;
	m_unk0xe8 = 0;
	m_unk0xec = 0;
}

// STUB: LEGORACERS2 0x00407240
GameController::~GameController()
{
	m_vtable = &g_gameControllerDispatch.m_base;
}

struct GameControllerInitialization {
	GameControllerInitialization()
	{
		InitializeGameControllerTemplate();
		ConstructGameController();
		RegisterGameControllerExit();
	}
};
static GameControllerInitialization g_gameControllerInitialization;
