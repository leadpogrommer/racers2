#include "core/application/commandline.h"
#include "frontend/foyer/foyermanager.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/runtime/gameframe.h"
#include "game/session/gamecontrollersession.h"
#include "input/controller/inputcontrollermanager.h"
#include "physics/timing/simulationscheduler.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "scene/effects/secondaryfxmanager.h"

#include <stddef.h>
#include <stdlib.h>

// These controller startup routines are recovered at their callers and remain
// separate helpers until their full bodies and dependencies are closed.
void __fastcall PrepareControllerSelection(GameController*, void*);
void __fastcall ResetControllerSelection(GameController*, void*);
void __fastcall PrepareFoyerControllerScene(GameController*, void*);
void __fastcall PrepareControllerSceneChildren(GameController*, void*);
void __fastcall PrepareSelectedGameModeScene(GameController*, void*);
void __fastcall PrepareSecondarySessionPlayers(GameObject*, void*);
void ApplicationNoOp();

struct GameControllerTimedEvents {
	LegoS32 m_count;
	GlobalOverlayRecord* m_records[180];
	GlobalOverlayRecord* m_endRecord;
	undefined m_unk0x2d8[4];
	LegoFloat m_upperLimit;
	undefined m_unk0x2e0[4];
	LegoFloat m_lowerLimit;
	LegoFloat m_origin;
	LegoU32 m_active;
	void Clear();
};
DECOMP_STATIC_ASSERT(offsetof(GameControllerTimedEvents, m_upperLimit) == 0x2dc);
DECOMP_STATIC_ASSERT(offsetof(GameControllerTimedEvents, m_lowerLimit) == 0x2e4);
DECOMP_STATIC_ASSERT(offsetof(GameControllerTimedEvents, m_origin) == 0x2e8);
DECOMP_STATIC_ASSERT(offsetof(GameControllerTimedEvents, m_active) == 0x2ec);

struct SecondaryManagerModeState {
	undefined m_prefix[0x178];
	LegoU32 m_flags;
};

struct SecondaryFxManagerView : GameObject {
	GameObject* m_controllerState;
};
DECOMP_STATIC_ASSERT(offsetof(SecondaryFxManagerView, m_controllerState) == 0x58);

struct ControllerStateObject : GameObject {
	LegoU32 m_state;
};
DECOMP_STATIC_ASSERT(offsetof(ControllerStateObject, m_state) == 0x58);

struct ControllerTrackDispatch {
	undefined m_prefix[0x54];
	void(__fastcall* m_selectTrack)(GameObject*, void*, LegoS32);
};

static void SetTimedOverlayActive(GlobalOverlayRecord* p_record, LegoBool32 p_active)
{
	if (p_active != FALSE) {
		p_record->m_flags |= 2;
	}
	else {
		p_record->m_flags &= ~2;
	}
}

static void ResetControllerSimulationScheduler()
{
	extern LegoU8 g_simulationSchedulerInitialized;
	if ((g_simulationSchedulerInitialized & 1) == 0) {
		g_simulationSchedulerInitialized |= 1;
		g_simulationScheduler.Reset(0);
		atexit(ApplicationNoOp);
	}
	g_simulationScheduler.Reset(0);
}

// STUB: LEGORACERS2 0x004082a0
LegoU32 GameController::Update(LegoU32 p_elapsed)
{
	(void) p_elapsed;
	if (m_startupReady == 0) {
		return 0;
	}

	switch (m_state) {
	case 0: {
		if (m_stateStarted != 0) {
			return 1;
		}
		SecondaryFxState* controllerState = ((SecondaryFxManager*) m_secondaryFxManager)->m_state;
		controllerState->SetMode(0, 0.0f);
		((SecondaryManager*) m_secondaryManager)->BeginStartup();
		break;
	}

	case 1: {
		SecondaryManager* secondary = (SecondaryManager*) m_secondaryManager;
		if (secondary->m_unk0x64 != 0) {
			if (secondary->PrepareSplash(1) == FALSE) {
				return 1;
			}
			secondary->ReleaseMenuResources(1);
		}
		PrepareSecondarySessionPlayers(secondary, NULL);

		if (FUN_004b4a60((LegoChar*) "/foyer") != FALSE) {
			m_state = 3;
		}
		else {
			if (FUN_004b4a60((LegoChar*) "/track") != FALSE) {
				GetGameControllerSession()->m_track = FUN_004b4b30((LegoChar*) "/track");
			}
			switch (GetGameControllerSession()->m_mode) {
			case 3:
				m_state = 5;
				break;
			case 4:
				m_state = 6;
				break;
			case 7:
				m_state = 3;
				break;
			default:
				m_state = 4;
				break;
			}
		}

		m_stateStarted = 0;
		PrepareControllerSelection(this, NULL);
		SecondaryManagerModeState* modeState = (SecondaryManagerModeState*) secondary->m_unk0x17c;
		LegoU32 culling = (modeState->m_flags >> 11) & 1;
		const RendererMainDispatch* rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
		rendererDispatch->m_setClockwiseCulling(g_displayRenderer, NULL, culling);
		g_inputControllerManager->m_unk0x68 = culling;
		return 1;
	}

	case 2:
		if (((SecondaryManager*) m_secondaryManager)->PrepareSplash(1) == FALSE) {
			return 1;
		}
		ResetControllerSelection(this, NULL);
		if (m_requestedState >= 0 && m_requestedState < 5) {
			GetGameControllerSession()->m_selectedMode = m_requestedState;
			m_requestedState = -1;
			m_state = 1;
			m_stateStarted = 0;
			return 1;
		}
		m_state = 0;
		m_stateStarted = 0;
		((const RendererMainDispatch*) g_displayRenderer->m_vtable)->m_setClockwiseCulling(g_displayRenderer, NULL, 0);
		g_inputControllerManager->m_unk0x68 = 0;
		return 1;

	case 3:
		if (m_stateStarted != 0) {
			return 1;
		}
		PrepareFoyerControllerScene(this, NULL);
		ResetControllerSimulationScheduler();
		PrepareControllerSceneChildren(&g_gameController, NULL);
		SetFoyerManagerMenuMode(m_foyerManager, NULL);
		break;

	case 4: {
		GameControllerTimedEvents* events = (GameControllerTimedEvents*) m_unk0xd8;
		if (events != NULL && events->m_active != 0) {
			LegoFloat offset[2] = {0.0f, events->m_origin};
			for (LegoS32 index = 0; index < events->m_count; ++index) {
				GlobalOverlayRecord* record = events->m_records[index];
				record->Translate(offset);
				LegoFloat positionY = record->m_positionY;
				LegoFloat end = record->GetLineHeight() + record->m_positionY;
				SetTimedOverlayActive(record, end > events->m_upperLimit && positionY < events->m_lowerLimit);
			}

			GlobalOverlayRecord* last = ((GlobalOverlayRecord**) events)[events->m_count];
			LegoFloat lastEnd = (last == NULL) ? -1.0f : last->GetLineHeight() + last->m_positionY;
			if (lastEnd < events->m_upperLimit) {
				events->Clear();
			}
		}

		if (m_stateStarted != 0) {
			return 1;
		}
		PrepareSelectedGameModeScene(this, NULL);
		if (*(LegoU32*) ((LegoU8*) m_networkApp + 0x5c) == 3) {
			((ControllerStateObject*) m_raceManager)->m_state = 0;
		}
		break;
	}

	case 5:
		if (m_stateStarted != 0) {
			return 1;
		}
		PrepareSelectedGameModeScene(this, NULL);
		ResetControllerSimulationScheduler();
		if (*(LegoU32*) ((LegoU8*) m_networkApp + 0x5c) == 3) {
			((ControllerStateObject*) m_battleManager)->m_state = 0;
		}
		break;

	case 6:
		if (m_stateStarted != 0) {
			return 1;
		}
		PrepareSelectedGameModeScene(this, NULL);
		ResetControllerSimulationScheduler();
		((ControllerTrackDispatch*) ((GameObject*) m_unk0xbc)->m_vtable)
			->m_selectTrack((GameObject*) m_unk0xbc, NULL, GetGameControllerSession()->m_track - 1);
		break;

	default:
		return 1;
	}

	m_stateStarted = 1;
	return 1;
}

// STUB: LEGORACERS2 0x004086a0
void GameControllerTimedEvents::Clear()
{
	if (m_active != 0) {
		for (LegoS32 index = 0; index < m_count; ++index) {
			m_records[index]->Release();
		}
		m_active = 0;
		m_count = 0;
	}
}
