#include "pausedetector.h"

#include "gamecontroller.h"
#include "gamecontrollerinitialization.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "globalpause.h"
#include "secondarymanager.h"
#include "simulationscheduler.h"

#include <new.h>

// GLOBAL: LEGORACERS2 0x00572f94
LegoU32 g_unk0x00572f94;

void* __fastcall SelectPauseTargetData(GameObject* p_object, void* p_unusedEdx, LegoU32 p_mode);
LegoBool32 __fastcall PauseConditionSlotIsSet(SecondaryManagerMediaState* p_state, void* p_unusedEdx);
void __fastcall RequestPauseForObject(GlobalPauseController* p_controller, void* p_unusedEdx, GameObject* p_object);

namespace
{
struct PauseTargetFlagsView {
	undefined m_unk0x00[0x04];
	LegoU32 m_flags; // 0x04
};

struct PauseManagerStateView {
	undefined m_unk0x00[0x58];
	LegoS32 m_state; // 0x58
	undefined m_unk0x5c[4];
	LegoS32 m_secondaryState; // 0x60
};

struct PauseManagerRequestView {
	undefined m_unk0x00[0x58];
	void* m_request; // 0x58
};

} // namespace

// Retail constructor first installs the base-compatible table at 0x0052f63c;
// its startup caller then replaces it with the update table at 0x0052f5ec.
extern const GameObjectDispatchView g_pauseDetectorInitialDispatch = GAME_OBJECT_BASE_DISPATCH_INITIALIZER;
extern const GameObjectDispatchView g_pauseDetectorDispatch = {
	&GameObjectBaseDestroy,      &GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&GameObjectBaseInitialize,   &GameObjectBaseTrue,
	&PauseDetectorUpdate,        &GameObjectBaseRemove,
	&GameObjectBaseZero,         &GameObjectBaseZero,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument,
};

// STUB: LEGORACERS2 0x00407f00
PauseDetector::PauseDetector(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_vtable = &g_pauseDetectorInitialDispatch;
}

// This startup bridge applies the final dispatch after the retail constructor
// returns, matching the store at 0x004073c2.
GameObject* __fastcall ConstructPauseDetector(void* p_storage, void*, GameObjectTemplate* p_template)
{
	PauseDetector* detector = new (p_storage) PauseDetector(p_template);
	detector->m_vtable = &g_pauseDetectorDispatch;
	return detector;
}

// STUB: LEGORACERS2 0x0040b4e0
LegoU32 __fastcall PauseDetectorUpdate(GameObject*, void*, LegoU32)
{
	if (g_gameControllerSession->m_unk0x00 == 2 || g_unk0x00679fe0 != 0 ||
		g_globalPauseController.IsBeforeDeadline() != FALSE) {
		return 0;
	}

	LegoBool32 shouldRequestPause = FALSE;
	switch (g_gameController.m_state) {
	case 3:
		shouldRequestPause =
			(((PauseManagerStateView*) g_gameController.m_foyerManager)->m_state == 1 &&
			 ((PauseManagerStateView*) g_gameController.m_secondaryManager)->m_state != 2);
		break;
	case 4: {
		PauseManagerStateView* manager = (PauseManagerStateView*) g_gameController.m_raceManager;
		shouldRequestPause = manager->m_secondaryState == 0 && manager->m_state >= 1 && manager->m_state <= 2 &&
							 g_gameControllerSession->m_mode != 8;
		break;
	}
	case 5:
		shouldRequestPause = ((PauseManagerStateView*) g_gameController.m_battleManager)->m_state == 3;
		break;
	case 6:
		shouldRequestPause = *(LegoU32*) ((LegoU8*) g_gameController.m_unk0xbc + 0xf8) == 2;
		break;
	}

	if (g_unk0x00572f94 != 0) {
		shouldRequestPause = FALSE;
	}
	PauseManagerRequestView* requestState = (PauseManagerRequestView*) g_gameController.m_secondaryFxManager;
	if (*(LegoU32*) requestState->m_request != 0) {
		shouldRequestPause = FALSE;
	}

	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	if (secondary->m_state != 9 || shouldRequestPause == FALSE) {
		return 0;
	}

	GameObject* selectedObject = NULL;
	LegoBool32 foundInputObject = FALSE;
	GameControllerSession* session = g_gameControllerSession;
	LegoS32 index = 0;
	if ((LegoS32) session->m_unk0x18 > 0) {
		for (;;) {
			session = g_gameControllerSession;
			if (index >= (LegoS32) session->m_unk0x18) {
				break;
			}
			GameObject* object = (GameObject*) session->m_unk0x28[index];
			if (object != NULL && ((LegoU32*) object)[0x6c / 4] == 1) {
				void* selectedData = SelectPauseTargetData(object, NULL, 0);
				if ((((PauseTargetFlagsView*) selectedData)->m_flags & 0x200) != 0) {
					selectedObject = object;
					foundInputObject = TRUE;
					break;
				}
			}
			++index;
		}
	}

	if (PauseConditionSlotIsSet(secondary->m_unk0x78, NULL) != FALSE) {
		selectedObject = secondary;
	}
	else if (foundInputObject == FALSE) {
		return 0;
	}

	RequestPauseForObject(&g_globalPauseController, NULL, selectedObject);
	return TRUE;
}

// STUB: LEGORACERS2 0x0042b300
LegoBool32 __fastcall PauseConditionSlotIsSet(SecondaryManagerMediaState* p_state, void*)
{
	if (p_state->IsSlotEnabled(2) == FALSE) {
		return FALSE;
	}
	p_state->Refresh(4);
	return (((const LegoU8*) p_state)[0x28] & 0x40) >> 6;
}

// STUB: LEGORACERS2 0x0044a520
void* __fastcall SelectPauseTargetData(GameObject* p_object, void*, LegoU32 p_mode)
{
	LegoU32 state = *(LegoU32*) ((LegoU8*) p_object + 0x68);
	if (p_mode == 0 && state != 1 && state != 3) {
		return (LegoU8*) p_object + 0xc8;
	}
	return (LegoU8*) p_object + 0xb4;
}

// FUNCTION: LEGORACERS2 0x004bb760
void __fastcall RequestPauseForObject(GlobalPauseController* p_controller, void*, GameObject* p_object)
{
	p_controller->m_enterRequested = 1;
	p_controller->m_observerMode = (LegoU32) p_object;
}
