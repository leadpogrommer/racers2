#include "foyermanager.h"

#include "application.h"
#include "audioservice.h"
#include "collisionservice.h"
#include "foyerscene.h"
#include "foyerscenenames.h"
#include "gamecontroller.h"
#include "gamecontrollerplayers.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "inputdeviceservice.h"
#include "memoryheap.h"
#include "screenprinter.h"
#include "secondaryfxmanager.h"
#include "secondarymanager.h"
#include "simulationscheduler.h"
#include "simulationtiming.h"

#include <stdlib.h>

// These are separate retail helpers used by the foyer state machine. Their
// declarations preserve the observed receiver and stack-argument contracts;
// implementations are recovered in their owning source units.
void SelectFoyerInputTarget(GameController*, LegoS32*, LegoS32*);
void __fastcall ResetFoyerInputMode(GameObject*, void*, LegoU32 p_mode);              // 43ac30
LegoS32 __fastcall GetFoyerSceneIndex(void*, void*, LegoU32 p_sessionValue);          // 411a30
void __fastcall LoadFoyerScene(FoyerSceneResourceView*, void*, GameObject*, LegoU32); // 47fa00
void __fastcall PrepareFoyerSceneSelection(void*, void*, GameObject*);                // 433050
void __fastcall UpdateFoyerSceneSelection(GameObject*, void*);                        // 43b070
void __fastcall ApplyFoyerSceneSelection(GameObject*, void*, LegoU32 p_selection);    // 43aaf0
void __fastcall ApplyFoyerSceneOverlay(GameObject*, void*, LegoS32 p_overlay);        // 43aac0
void __fastcall ExitFoyerScene(GameObject*, void*);                                   // 43aa60
void __fastcall SetFoyerSceneTransition(GameObject*, void*, LegoU32 p_mode);          // 43aba0
void __fastcall CompleteFoyerSessionTransition(void*, void*);                         // 411e50
void __fastcall ResetFoyerSceneState(void*, void*, LegoU32 p_mode);                   // 411db0
void __fastcall PrepareSecondarySessionPlayers(GameObject*, void*);                   // 428650
LegoU32 __fastcall IsFoyerSceneTransitionComplete(FoyerSceneResourceView*, void*);    // 4801c0
LegoS32 __fastcall GetFoyerSelectedObjectIndex(void*, void*, LegoU32 p_mode);         // 411cb0
LegoS32 __fastcall GetFoyerObjectSelection(void*, void*, const LegoFloat* p_state);   // 43c330
void __stdcall SetFoyerSceneParameter(LegoU32 p_value);                               // 43b1f0
void __fastcall StartFoyerSceneSelectionTransition(FoyerManager*, LegoU32 p_state);
void __fastcall StartControllerMusic(GameController*, void*, LegoS32, LegoS32); // 40a290
void __fastcall ClearFoyerNpcText(GameObject*, void*);                          // 460070
LegoU32 __fastcall GetFoyerInputMask(SecondaryManagerMediaState*, void*);       // 42b330
void ApplicationNoOp();
GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

struct FoyerSceneManagerDispatchView {
	undefined m_prefix0x00[0x54];
	void*(__fastcall* m_getCurrentState)(void*, void*);
	undefined m_unk0x58[4];
	void(__fastcall* m_setSceneObjectState)(void*, void*, LegoU32, LegoU32);
	void(__fastcall* m_setSceneInputState)(void*, void*, LegoU32, LegoU32);
};

extern LegoU8 g_simulationSchedulerInitialized;

extern const FoyerManagerDispatch g_foyerManagerDispatch = {
	&FoyerManagerDestroy,        &GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&FoyerManagerInitialize,     &GameObjectBaseTrue,
	&FoyerManagerUpdate,         &GameObjectBaseRemove,
	&GameObjectBaseZero,         &GameObjectBaseZero,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument,
	&FoyerManagerGetObjectSize,
};

// STUB: LEGORACERS2 0x00411d40
void SelectFoyerInputTarget(GameController* p_settings, LegoS32* p_sceneNameIndex, LegoS32* p_objectStateIndex)
{
	GameControllerSession* session = GetGameControllerSession();
	if (session->m_selectedMode != 0) {
		*p_sceneNameIndex = -1;
		*p_objectStateIndex = -1;
		return;
	}

	LegoS32 configuredIndex = *(LegoS32*) ((LegoU8*) p_settings + 0x1de4);
	*p_sceneNameIndex = (configuredIndex > 0 && configuredIndex < 7) ? configuredIndex : -1;
	*p_objectStateIndex = *(LegoS32*) ((LegoU8*) p_settings + 0x1dec);
}

// STUB: LEGORACERS2 0x0043a2f0
FoyerManager::FoyerManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	LegoU32 i;

	m_state = 9;
	m_previousState = 9;
	m_previousSelection = -1;
	m_sceneState = NULL;
	for (i = 0; i < 13; ++i) {
		m_objectStates[i] = NULL;
		m_sceneObjects[i] = NULL;
	}
	for (i = 0; i < 4; ++i) {
		m_unk0xd0[i] = 0;
	}
	m_previousSessionState = -1;
	m_unk0xe4[0] = 0;
	m_unk0xe4[1] = 0;
	m_unk0xe4[2] = 0;
	for (i = 0; i < 24; ++i) {
		m_unk0xf0[i] = 0;
	}
	m_menuResource = NULL;
	m_menuResourceType = -1;
	m_vtable = (const GameObjectDispatchView*) &g_foyerManagerDispatch;
}

namespace
{
void PrepareFoyerManagerForDestruction(GameObject* p_object)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_foyerManagerDispatch;
	p_object->GameObject::~GameObject();
}
} // namespace

// STUB: LEGORACERS2 0x0043a390
GameObject* __fastcall FoyerManagerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	PrepareFoyerManagerForDestruction(p_object);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0043a3c0
void __fastcall FoyerManagerInitialize(GameObject* p_object, void*)
{
	p_object->GameObject::Initialize();
}

// STUB: LEGORACERS2 0x0043a480
LegoU32 __fastcall FoyerManagerUpdate(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	FoyerManager* manager = (FoyerManager*) p_object;
	GameControllerSession* session = GetGameControllerSession();
	GameObject* activeSceneObject;
	LegoS32 sceneIndex;
	LegoS32 selectedObject;
	LegoS32 i;
	LegoU32 inputMask;
	FoyerSceneResourceView* resource;

	switch (manager->m_state) {
	case 0: {
		manager->m_previousSelection = -1;
		if (session->m_unk0x04 == 0 || manager->m_unk0xe4[0] != 0) {
			sceneIndex = 8;
		}
		else {
			sceneIndex = *(LegoS32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1df0);
		}
		activeSceneObject = (GameObject*) manager->m_sceneObjects[sceneIndex];
		FoyerSceneManagerDispatchView* stateDispatch = (FoyerSceneManagerDispatchView*) *(void**) manager->m_sceneState;
		stateDispatch
			->m_setSceneObjectState(manager->m_sceneState, NULL, (LegoU32) ((LegoU8*) activeSceneObject + 0x58), 1);
		stateDispatch
			->m_setSceneInputState(manager->m_sceneState, NULL, (LegoU32) ((LegoU8*) activeSceneObject + 0x64), 1);
		PrepareFoyerSceneSelection(g_gameController.m_unk0x88, NULL, manager->m_sceneState);
		ResetFoyerInputMode(p_object, NULL, 1);
		manager->m_menuResource = NULL;
		sceneIndex = GetFoyerSceneIndex(g_gameController.m_unk0x8c, NULL, session->m_selectedMode);
		manager->m_menuResourceType = sceneIndex;
		if (sceneIndex != -1) {
			const LegoChar* name;
			if (sceneIndex == 12) {
				name = g_foyerIntroScene;
			}
			else if (sceneIndex == 13) {
				name = g_worldCompletedScene;
			}
			else {
				name = (const LegoChar*) g_foyerSceneNames + sceneIndex * sizeof(CharBuffer24);
			}
			resource = FindFoyerSceneResource(name);
			manager->m_menuResource = resource;
			if (resource == NULL) {
				manager->m_menuResourceType = -1;
			}
			else {
				LoadFoyerScene(resource, NULL, manager->m_sceneState, 1);
				manager->m_state = 2;
				ResetFoyerInputMode(p_object, NULL, 0);
			}
		}
		if (manager->m_menuResource == NULL) {
			manager->m_state = 1;
		}
		AudioService* audio = GetAudioService(TRUE);
		audio->m_vtable->m_setMusicEnabled(audio, NULL, 0);
		StartControllerMusic(&g_gameController, NULL, 2, 1);
		g_inputService->m_vtable->poll(g_inputService, NULL, 1);
		UpdateFoyerSceneSelection(p_object, NULL);
		if (session->m_selectedMode == 0) {
			SetFoyerSceneParameter(*(LegoU32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1de4));
		}
		if ((g_simulationSchedulerInitialized & 1) == 0) {
			g_simulationSchedulerInitialized |= 1;
			atexit(ApplicationNoOp);
		}
		g_simulationScheduler.Reset(0);
		return 1;
	}
	case 1: {
		selectedObject = -1;
		for (i = 0; i < 13; ++i) {
			if (manager->m_objectStates[i] != NULL) {
				FoyerSceneManagerDispatchView* stateDispatch =
					(FoyerSceneManagerDispatchView*) *(void**) manager->m_sceneState;
				void* currentState = stateDispatch->m_getCurrentState(manager->m_sceneState, NULL);
				if (GetFoyerObjectSelection(manager->m_objectStates[i], NULL, (const LegoFloat*) currentState) != 0) {
					selectedObject = *(LegoS32*) ((LegoU8*) manager->m_objectStates[i] + 0x68);
					break;
				}
			}
		}
		if (manager->m_previousSelection != selectedObject) {
			switch (selectedObject) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4: {
				LegoU32 selectionWords[8];
				LegoU32* selectionSource = (LegoU32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e04);
				for (i = 0; i < 8; ++i) {
					selectionWords[i] = selectionSource[i];
				}
				if (((GameControllerSelection*) selectionWords)
							->IsTrackUnlocked(session->m_selectedMode, selectedObject + 1) != 0 ||
					session->m_unk0x04 == 0) {
					ApplyFoyerSceneSelection(p_object, NULL, selectedObject + 1);
					manager->m_previousSelection = selectedObject;
					return 1;
				}
				break;
			}
			case 5:
			case 6:
			case 7:
				break;
			case 8:
				sceneIndex = GetFoyerSelectedObjectIndex(g_gameController.m_unk0x8c, NULL, 0);
				resource = (FoyerSceneResourceView*) manager->m_unk0xd0[sceneIndex];
				if (resource != NULL) {
					LoadFoyerScene(resource, NULL, manager->m_sceneState, 1);
					manager->m_menuResourceType = sceneIndex;
					manager->m_menuResource = resource;
					manager->m_state = 2;
					ResetFoyerInputMode(p_object, NULL, 0);
					manager->m_previousSelection = selectedObject;
					return 1;
				}
				break;
			default:
				((ScreenPrinter*) g_gameController.m_screenPrinter)->ClearText(0);
			}
		}
		manager->m_previousSelection = selectedObject;
		return 1;
	}
	case 2:
		if (IsFoyerSceneTransitionComplete((FoyerSceneResourceView*) manager->m_menuResource, NULL) != 0) {
			resource = (FoyerSceneResourceView*) manager->m_menuResource;
			manager->m_menuResource = NULL;
			switch (manager->m_menuResourceType) {
			case 3:
			case 4:
			case 5:
			case 6:
				ApplyFoyerSceneSelection(p_object, NULL, manager->m_menuResourceType - 2);
				break;
			case 7:
			case 8:
				sceneIndex = *(LegoS32*) ((LegoU8*) resource + 0xa3c);
				if (sceneIndex == 2) {
					ApplyFoyerSceneOverlay(p_object, NULL, manager->m_previousSelection - 8);
				}
				else if (sceneIndex == 3) {
					CompleteFoyerSessionTransition(g_gameController.m_unk0x8c, NULL);
					manager->m_state = 1;
				}
				else {
					ResetFoyerInputMode(p_object, NULL, 1);
					manager->m_state = 1;
				}
				break;
			case 9:
			case 10:
			case 11:
				CompleteFoyerSessionTransition(g_gameController.m_unk0x8c, NULL);
				manager->m_state = 1;
				break;
			default:
				manager->m_state = 1;
				ResetFoyerInputMode(p_object, NULL, 1);
				if (session->m_selectedMode == 0) {
					SetFoyerSceneParameter(manager->m_menuResourceType + 1);
				}
				break;
			}
			if (manager->m_menuResource == NULL) {
				manager->m_menuResourceType = -1;
				return 1;
			}
			break;
		}
	case 3:
		if (**(LegoU32**) ((LegoU8*) g_gameController.m_secondaryFxManager + 0x58) == 0) {
			manager->m_state = manager->m_previousState;
			manager->m_previousState = 9;
			return 1;
		}
		break;
	case 4:
		ExitFoyerScene(p_object, NULL);
		session->m_mode = 6;
		PrepareSecondarySessionPlayers(g_gameController.m_secondaryManager, NULL);
		g_gameController.m_state = 2;
		g_gameController.m_stateStarted = session->m_selectedMode;
		g_gameController.m_requestedState = 0;
		return 1;
	case 5:
		ExitFoyerScene(p_object, NULL);
		ResetFoyerSceneState(g_gameController.m_unk0x8c, NULL, (LegoU32) manager->m_previousSessionState);
		manager->m_previousSessionState = -1;
		return 1;
	case 6:
		ExitFoyerScene(p_object, NULL);
		session->m_mode = 4;
		PrepareSecondarySessionPlayers(g_gameController.m_secondaryManager, NULL);
		g_gameController.m_state = 2;
		g_gameController.m_stateStarted = session->m_selectedMode;
		g_gameController.m_requestedState = 0;
		return 1;
	case 7:
		inputMask = GetFoyerInputMask(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x78, NULL);
		manager->m_menuTimeout -= p_elapsedSeconds;
		if (!(manager->m_menuTimeout >= 0.0f) || (inputMask & 0x202) != 0) {
			ClearFoyerNpcText(g_gameController.m_npcTextManager, NULL);
			SetFoyerSceneTransition(p_object, NULL, 0);
			manager->m_state = 1;
			return 1;
		}
		break;
	case 8:
		ExitFoyerScene(p_object, NULL);
		if (manager->m_unk0xe4[0] == 0 && manager->m_unk0xe4[1] == 0) {
			g_gameController.m_state = 2;
			g_gameController.m_requestedState = 0;
			return 1;
		}
		g_gameController.m_stateStarted = session->m_selectedMode;
		g_gameController.m_state = 2;
		g_gameController.m_requestedState = 0;
		break;
	}
	return 1;
}

// STUB: LEGORACERS2 0x0043aab0
void __fastcall SetFoyerManagerMenuMode(GameObject* p_manager, void*)
{
	StartFoyerSceneSelectionTransition((FoyerManager*) p_manager, 8);
}

// STUB: LEGORACERS2 0x0043aac0
void __fastcall ApplyFoyerSceneOverlay(GameObject* p_manager, void*, LegoS32 p_overlay)
{
	FoyerManager* manager = (FoyerManager*) p_manager;
	manager->m_state = 5;
	manager->m_previousSessionState = p_overlay;
	*(LegoS32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1df0) = g_gameControllerSession->m_selectedMode + 8;
}

// STUB: LEGORACERS2 0x0043aaf0
void __fastcall ApplyFoyerSceneSelection(GameObject* p_manager, void*, LegoU32 p_selection)
{
	FoyerManager* manager = (FoyerManager*) p_manager;
	g_gameControllerSession->m_track = p_selection;
	if (g_gameControllerSession->m_selectedMode == 0) {
		manager->m_state = 4;
	}
	else {
		StartFoyerSceneSelectionTransition(manager, 4);
	}
	*(LegoS32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1df0) = (LegoS32) p_selection - 1;
}

// STUB: LEGORACERS2 0x0043ab70
void __fastcall StartFoyerSceneSelectionTransition(FoyerManager* p_manager, LegoU32 p_state)
{
	p_manager->m_state = 3;
	p_manager->m_previousState = p_state;
	SecondaryFxManager* manager = (SecondaryFxManager*) g_gameController.m_secondaryFxManager;
	manager->m_state->SetMode(3, 0.7f);
}

// STUB: LEGORACERS2 0x0043aba0
void __fastcall SetFoyerSceneTransition(GameObject* p_object, void*, LegoU32 p_mode)
{
	FoyerManager* manager = (FoyerManager*) p_object;
	GameObject* sceneState = manager->m_sceneState;
	GameObjectDispatchView* dispatch = (GameObjectDispatchView*) sceneState->m_vtable;
	LegoS32 count = g_gameControllerSession->m_unk0x18;
	for (LegoS32 i = 0; i < count; ++i) {
		if (p_mode != 0) {
			((void(__fastcall*)(GameObject*, void*)) * (void**) ((LegoU8*) dispatch + 0x78))(sceneState, NULL);
		}
		((void(__fastcall*)(GameObject*, void*, LegoU32)) *
		 (void**) ((LegoU8*) dispatch + 0x7c))(sceneState, NULL, p_mode);
		((void(__fastcall*)(GameObject*, void*, LegoU32)) *
		 (void**) ((LegoU8*) dispatch + 0x74))(sceneState, NULL, p_mode);
	}
	ResetFoyerInputMode(p_object, NULL, p_mode == 0);
}

// STUB: LEGORACERS2 0x0043ac30
void __fastcall ResetFoyerInputMode(GameObject* p_object, void*, LegoU32 p_mode)
{
	FoyerManager* manager = (FoyerManager*) p_object;
	if (g_gameControllerSession->m_unk0x04 == 0 || manager->m_unk0xe4[0] != 0) {
		return;
	}
	if (p_mode == 0) {
		manager->m_unk0xd0[3] = 0;
		return;
	}
	LegoS32 sceneNameIndex = -1;
	LegoS32 objectStateIndex = -1;
	manager->m_unk0xd0[3] = 1;
	SelectFoyerInputTarget((GameController*) g_gameController.m_unk0x8c, &sceneNameIndex, &objectStateIndex);
	if (sceneNameIndex != -1) {
		GameObject* namedObject = FindGameObjectByName(g_foyerSceneNames[sceneNameIndex].m_data, 1, NULL);
		if (namedObject == NULL) {
			manager->m_unk0xd0[3] = 0;
			return;
		}
		manager->m_unk0xd0[0] = *(LegoU32*) ((LegoU8*) namedObject + 0x58);
		manager->m_unk0xd0[1] = *(LegoU32*) ((LegoU8*) namedObject + 0x5c);
		manager->m_unk0xd0[2] = *(LegoU32*) ((LegoU8*) namedObject + 0x60);
	}
	else if (objectStateIndex != -1) {
		GameObject* objectState = (GameObject*) manager->m_objectStates[objectStateIndex];
		if (objectState == NULL) {
			manager->m_unk0xd0[3] = 0;
			return;
		}
		manager->m_unk0xd0[0] = *(LegoU32*) ((LegoU8*) objectState + 0x58);
		manager->m_unk0xd0[1] = *(LegoU32*) ((LegoU8*) objectState + 0x5c);
		manager->m_unk0xd0[2] = *(LegoU32*) ((LegoU8*) objectState + 0x60);
	}
	else {
		manager->m_unk0xd0[3] = 0;
	}
}

// STUB: LEGORACERS2 0x0043b000
void __fastcall RedirectFoyerSceneMode(GameObject* p_object, void*, LegoU32 p_mode)
{
	FoyerManager* manager = (FoyerManager*) p_object;
	LegoS32 selected = GetFoyerSelectedObjectIndex(g_gameController.m_unk0x8c, NULL, p_mode);
	FoyerSceneResourceView* scene =
		*(FoyerSceneResourceView**) ((LegoU8*) manager + 0xd0 + (selected + p_mode * 5) * 4);
	if (scene != NULL) {
		LegoU32 state = *(LegoU32*) ((LegoU8*) scene + 0x58);
		if (state < 1 || state > 4) {
			LoadFoyerScene(scene, NULL, manager->m_sceneState, 1);
			manager->m_menuResourceType = selected;
			manager->m_menuResource = scene;
			manager->m_state = 2;
			ResetFoyerInputMode(p_object, NULL, 0);
			manager->m_previousSelection = p_mode + 8;
		}
	}
}

// STUB: LEGORACERS2 0x0043b070
void __fastcall UpdateFoyerSceneSelection(GameObject* p_object, void*)
{
	FoyerManager* manager = (FoyerManager*) p_object;
	GameControllerSession* session = GetGameControllerSession();
	if (session->m_selectedMode == 0 || manager->m_unk0xe4[0] != 0) {
		return;
	}

	GameObject* bossBarrier = FindGameObjectByName("boss_barrier", 0, NULL);
	GameObject* bossBarrier2 = FindGameObjectByName("boss_barrier2", 0, NULL);
	LegoU32 unlocked = ((GameControllerSelection*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e04))
						   ->IsTrackUnlocked(session->m_selectedMode, 5);
	if (unlocked == 0) {
		if (bossBarrier2 != NULL) {
			bossBarrier2->m_flags |= 8;
			DetachCollisionProxy(bossBarrier2->m_lifecycle0x54->m_collisionProxy);
		}
		if (bossBarrier != NULL) {
			if ((bossBarrier->m_flags & 8) != 0) {
				bossBarrier->m_flags = (bossBarrier->m_flags & ~8u) | 0x02000000;
			}
			AttachCollisionProxy(bossBarrier->m_lifecycle0x54->m_collisionProxy);
		}
	}
	else {
		if (bossBarrier != NULL) {
			bossBarrier->m_flags |= 8;
			DetachCollisionProxy(bossBarrier->m_lifecycle0x54->m_collisionProxy);
		}
		if (bossBarrier2 != NULL) {
			if ((bossBarrier2->m_flags & 8) != 0) {
				bossBarrier2->m_flags = (bossBarrier2->m_flags & ~8u) | 0x02000000;
			}
			AttachCollisionProxy(bossBarrier2->m_lifecycle0x54->m_collisionProxy);
		}
	}
}

// FUNCTION: LEGORACERS2 0x004954a0
LegoU32 __fastcall FoyerManagerGetObjectSize(GameObject*, void*)
{
	return 0x15c;
}
