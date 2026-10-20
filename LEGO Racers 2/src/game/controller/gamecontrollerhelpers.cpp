#include "audio/service/audioservice.h"
#include "core/strings/charbuffer24.h"
#include "frontend/foyer/controllerfoyerlifecycle.h"
#include "frontend/foyer/controllerfoyertaskmanager.h"
#include "frontend/foyer/foyermanager.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectrenderprep.h"
#include "game/session/gamecontrollerplayers.h"
#include "game/session/gamecontrollersession.h"
#include "game/session/gamecontrollersessionqueries.h"
#include "game/state/gamerandom.h"
#include "input/controller/inputcontrollermanager.h"
#include "input/service/inputdeviceservice.h"
#include "memory/heapaddresswalk.h"
#include "memory/memoryheap.h"
#include "network/networkdeferred.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "scene/effects/controllersceneobject.h"

#include <new.h>
#include <stdio.h>

struct ControllerModeMenuDispatch {
	void(__fastcall* m_destroy)(void*, void*, LegoU32);
	undefined m_unk0x04[0x0c];
	void(__fastcall* m_initialize)(void*, void*);
	undefined m_unk0x14[0x08];
	void(__fastcall* m_update)(void*, void*);
};
DECOMP_STATIC_ASSERT(offsetof(ControllerModeMenuDispatch, m_update) == 0x1c);

void __fastcall InitializeControllerSelectionProgress(void*, void*, LegoU32);                     // 4338f0
void __fastcall UpdateControllerSelectionProgress(void*, void*, LegoU32);                         // 4339e0
void __fastcall SetControllerSelectionProgressValue(void*, void*, LegoFloat);                     // 502a10
void __fastcall LoadControllerSceneResources(GameController*, void*, LegoU32);                    // 40a170
void __fastcall BuildControllerSelectionWidgets(GameController*, void*);                          // 4092d0
void __fastcall ConfigureControllerSelectionLayout(GameController*, void*, LegoFloat, LegoFloat); // 408bf0
void* __cdecl GetControllerTaskManager();                                                         // 45ba20
void* __fastcall BuildControllerModeTemplate(GameController*, void*, LegoS32);                    // 408910
void* __fastcall ConstructControllerModeMenu(void*, void*, void*);                                // 49ae10
void __fastcall ResetActiveTextBoxManager(GameObject*, void*);                                    // 439030
void __fastcall ClearTextBoxManagerResources(GameObject*, void*);                                 // 438540
void __cdecl ResetControllerInputState();                                                         // 47e3b0
void __cdecl ResetControllerTaskOverlayState();                                                   // 45ba70
void __fastcall RefreshControllerSelection(GameController*, void*);                               // 4091e0
void __fastcall ResetControllerSelectionWidgets(GameController*, void*);                          // 409550
void __fastcall ReleaseControllerSceneResources(GameController*, void*, LegoU32);                 // 40a1d0
void __fastcall AddControllerSceneLabel(void*, void*, const CharBuffer24*, LegoU32);              // 49b190
void __fastcall SetControllerSceneProgressRange(void*, void*, LegoFloat, LegoFloat);              // 49b340
void* __fastcall ConstructControllerHudManager(void*, void*, GameObjectTemplate*);                // 432e40
LegoU32 __cdecl SelectControllerSceneRecord(LegoU32);                                             // 4487b0
void __fastcall SetControllerBodyInputEnabled(void*, void*, LegoU32);
void __fastcall FinalizeControllerSelectionProgress(void*, void*);              // 4339a0
void __fastcall UpdateControllerScene(GameController*, void*);                  // 409440
GameObject* __fastcall ConstructRaceManager(void*, void*, GameObjectTemplate*); // 40b9c0
GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);
void __fastcall FUN_00411110(void*, void*);
void __fastcall FUN_0040eb40(GameObject*, void*);
void RequestDeferredWindowDestruction();

struct ControllerFoyerModeView : GameObject {
	undefined m_prefix[0xe8 - 0x58];
	LegoU32 m_isExploreMode;
};

struct ControllerManagerSceneStateView : GameObject {
	undefined m_prefix[0x64 - 0x58];
	LegoU32 m_currentOverlay;
};

struct ControllerSceneObjectStateView {
	undefined m_prefix[0x58];
	LegoU32 m_active;
	LegoU32 m_sceneToken;
};

struct ControllerSceneInputOwnerView {
	undefined m_prefix[0x80];
	void* m_inputOwner;
};

struct ControllerHudTemplateView {
	GameObjectTemplate m_base;
	LegoU32 m_bonusMode;
};
DECOMP_STATIC_ASSERT(offsetof(ControllerHudTemplateView, m_bonusMode) == 0x34);

struct RaceManagerSceneView : GameObject {
	undefined m_prefix0x58[0x64 - 0x58];
	void* m_checkpointSupervisor;
	void* m_aiSupervisor;
};
DECOMP_STATIC_ASSERT(offsetof(RaceManagerSceneView, m_checkpointSupervisor) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(RaceManagerSceneView, m_aiSupervisor) == 0x68);

union ControllerTemplateStorage {
	LegoU32 m_alignment;
	LegoU8 m_bytes[sizeof(GameObjectTemplate)];
};

union ControllerHudTemplateStorage {
	LegoU32 m_alignment;
	LegoU8 m_bytes[sizeof(ControllerHudTemplateView)];
};

static GameObjectTemplate* InitializeControllerTemplate(
	ControllerTemplateStorage* p_storage,
	LegoU32 p_flags,
	const LegoChar* p_first,
	const LegoChar* p_second
)
{
	GameObjectTemplate* result = (GameObjectTemplate*) p_storage->m_bytes;
	new (&result->m_first) CharBuffer24(p_first);
	new (&result->m_second) CharBuffer24(p_second);
	result->m_flags = p_flags;
	return result;
}

static ControllerHudTemplateView* InitializeControllerHudTemplate(
	ControllerHudTemplateStorage* p_storage,
	LegoU32 p_bonusMode
)
{
	ControllerHudTemplateView* result = (ControllerHudTemplateView*) p_storage->m_bytes;
	new (&result->m_base.m_first) CharBuffer24("HUD_Manager");
	new (&result->m_base.m_second) CharBuffer24("");
	result->m_base.m_flags = 2;
	result->m_bonusMode = p_bonusMode;
	return result;
}

struct ControllerNetworkStateView : GameObject {
	LegoU32 m_unk0x58;
	LegoU32 m_state;
};

// STUB: LEGORACERS2 0x00408710
void __fastcall PrepareControllerSelection(GameController* p_controller, void*)
{
	const RendererMainDispatch* rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	LegoU32 selectionQuality = g_gameController.m_unk0x348;
	rendererDispatch->m_setTextureQuality(g_displayRenderer, NULL, selectionQuality);
	rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 0);

	void* selectionProgress = g_gameController.m_unk0xd4;
	InitializeControllerSelectionProgress(selectionProgress, NULL, 0);
	if (rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 2) == 0) {
		RequestDeferredWindowDestruction();
	}
	UpdateControllerSelectionProgress(selectionProgress, NULL, 1);
	rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 1);
	LoadControllerSceneResources(p_controller, NULL, 0);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.05f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	BuildControllerSelectionWidgets(p_controller, NULL);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.06f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	ConfigureControllerSelectionLayout(p_controller, NULL, 0.06f, 0.2f);
	GetControllerTaskManager();
	GetControllerSceneObject();
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.21f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);

	void* storage = AllocateHeapMemory(0x345c);
	void* modeMenu = NULL;
	if (storage != NULL) {
		void* modeTemplate =
			BuildControllerModeTemplate(p_controller, NULL, GetGameControllerSession()->m_selectedMode);
		modeMenu = ConstructControllerModeMenu(storage, NULL, modeTemplate);
	}
	p_controller->m_unk0xdc = (LegoU32) modeMenu;
	ControllerModeMenuDispatch* dispatch = (ControllerModeMenuDispatch*) *(void**) modeMenu;
	dispatch->m_initialize(modeMenu, NULL);

	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.22f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	p_controller->m_unk0xe0 = GetGameControllerSession()->m_selectedMode;
}

// STUB: LEGORACERS2 0x00408850
void __fastcall ResetControllerSelection(GameController* p_controller, void*)
{
	ResetActiveTextBoxManager(p_controller->m_textBoxManager, NULL);

	void* modeMenu = (void*) p_controller->m_unk0xdc;
	if (modeMenu != NULL) {
		ControllerModeMenuDispatch* dispatch = (ControllerModeMenuDispatch*) *(void**) modeMenu;
		dispatch->m_update(modeMenu, NULL);
		modeMenu = (void*) p_controller->m_unk0xdc;
		if (modeMenu != NULL) {
			dispatch = (ControllerModeMenuDispatch*) *(void**) modeMenu;
			dispatch->m_destroy(modeMenu, NULL, 1);
		}
		p_controller->m_unk0xdc = 0;
	}

	ResetControllerInputState();
	ResetControllerTaskOverlayState();
	RefreshControllerSelection(p_controller, NULL);
	ResetControllerSelectionWidgets(p_controller, NULL);
	ReleaseControllerSceneResources(p_controller, NULL, 0);
	p_controller->m_unk0xe0 = -1;
	g_inputService->m_vtable->poll(g_inputService, NULL, 0);

	LegoU32 controllerInput[2] = {0, 0};
	g_inputControllerManager->GetControllerInputForOwner((LegoU32) p_controller, controllerInput, 0, 1);

	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_update(audio, NULL, 0xffffffff);
	audio = GetAudioService(TRUE);
	audio->m_vtable->m_unk0x50(audio, NULL, 0);
}

struct GameControllerSceneChildrenView {
	GameObject* m_children[3];
};

DECOMP_SIZE_ASSERT(GameControllerSceneChildrenView, 0x0c);

// STUB: LEGORACERS2 0x004095b0
void __fastcall PrepareControllerSceneChildren(GameController* p_controller, void*)
{
	GameControllerSceneChildrenView* view = (GameControllerSceneChildrenView*) &p_controller->m_unk0xe4;
	for (LegoS32 index = 0; index < 3; ++index) {
		GameObject* child = view->m_children[index];
		if (child != NULL) {
			UpdateGameObjectRenderGeometry(child, NULL);
		}
	}
}

// STUB: LEGORACERS2 0x004095e0
void __fastcall PrepareFoyerControllerScene(GameController* p_controller, void*)
{
	ControllerFoyerModeView* foyerManager = (ControllerFoyerModeView*) p_controller->m_foyerManager;
	const LegoChar* title = (foyerManager->m_isExploreMode != 0) ? "Explore" : "Foyer";
	CharBuffer24 titleBuffer(title);
	AddControllerSceneLabel((void*) p_controller->m_unk0xdc, NULL, &titleBuffer, 1);

	void* selectionProgress = g_gameController.m_unk0xd4;
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.25f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	SetControllerSceneProgressRange((void*) p_controller->m_unk0xdc, NULL, 0.25f, 0.8f);

	ControllerHudTemplateStorage hudTemplateStorage;
	ControllerHudTemplateView* hudTemplate = InitializeControllerHudTemplate(&hudTemplateStorage, 2);
	void* hudStorage = AllocateHeapMemory(0x3f4);
	GameObject* hudManager = (hudStorage == NULL)
								 ? NULL
								 : (GameObject*) ConstructControllerHudManager(hudStorage, NULL, &hudTemplate->m_base);
	p_controller->m_unk0x88 = hudManager;
	hudManager->m_vtable->m_initialize(hudManager, NULL);
	hudManager->m_flags |= 0x400000;

	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.9f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	GameControllerSession* session = GetGameControllerSession();
	StartupPlayerSettings* playerSettings = (StartupPlayerSettings*) ((LegoU8*) session + 0x4c);
	playerSettings[0].m_unk0x10 = 0;
	session->m_unk0x28[0] = SelectControllerSceneRecord(0);
	((ControllerManagerSceneStateView*) p_controller->m_foyerManager)->m_currentOverlay = session->m_unk0x28[0];

	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.95f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	SetControllerFaceTexturesLoaded(p_controller, NULL, 1);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.97f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	if (foyerManager->m_isExploreMode == 0) {
		PreloadOtherFoyerSceneResources((FoyerManager*) p_controller->m_foyerManager, NULL, p_controller->m_unk0xe0);
	}

	ControllerSceneObjectStateView* sceneObject = (ControllerSceneObjectStateView*) GetControllerSceneObject();
	sceneObject->m_active = 1;
	sceneObject->m_sceneToken = session->m_unk0x28[0];
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.99f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);

	LegoU32 sceneInput = IsControllerBodyInputEnabled(p_controller->m_unk0x8c, NULL);
	void* inputOwner = ((ControllerSceneInputOwnerView*) p_controller->m_unk0xa4)->m_inputOwner;
	SetControllerBodyInputEnabled(inputOwner, NULL, sceneInput);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 1.0f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	FinalizeControllerSelectionProgress(selectionProgress, NULL);

	const RendererMainDispatch* rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	if (rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 2) == 0) {
		RequestDeferredWindowDestruction();
	}
	UpdateControllerScene(p_controller, NULL);
	g_gameController.m_unk0x348 = g_displayRenderer->m_vtable->m_textureQuality(g_displayRenderer, NULL);

	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_unk0x38(audio, NULL);
	MemoryHeap* heap = GetMemoryHeap();
	WalkHeapAddressRange(heap, NULL);
}

// STUB: LEGORACERS2 0x00409950
void __fastcall PrepareSelectedGameModeScene(GameController* p_controller, void*)
{
	GameRandomState* randomState = GetGameRandomState();
	randomState->m_checkpoint0 = randomState->m_floatStreams[0].m_frame;
	randomState->m_checkpoint1 = randomState->m_integerStreams[0].m_state;
	randomState->m_unk0x48 = 0;

	GameControllerSession* session = GetGameControllerSession();
	if (p_controller->m_state == 4) {
		ControllerTemplateStorage raceTemplateStorage;
		GameObjectTemplate* raceTemplate = InitializeControllerTemplate(&raceTemplateStorage, 2, "Race Manager", "");
		void* raceStorage = AllocateHeapMemory(0x7c);
		GameObject* raceManager = (raceStorage == NULL) ? NULL : ConstructRaceManager(raceStorage, NULL, raceTemplate);
		p_controller->m_raceManager = raceManager;
		raceManager->m_flags |= 0x400000;
		raceManager->m_vtable->m_initialize(raceManager, NULL);
	}

	void* selectionProgress = g_gameController.m_unk0xd4;
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.23f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	CharBuffer24 staticLabel("Static");
	AddControllerSceneLabel((void*) p_controller->m_unk0xdc, NULL, &staticLabel, 1);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.24f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);

	LegoChar modeText[44];
	if (p_controller->m_state == 4) {
		sprintf(modeText, "Track %d", session->m_track);
	}
	else if (p_controller->m_state == 5) {
		sprintf(modeText, "Battle");
	}
	else if (p_controller->m_state == 6) {
		sprintf(modeText, "Bonus");
	}
	CharBuffer24 modeLabel(modeText);
	AddControllerSceneLabel((void*) p_controller->m_unk0xdc, NULL, &modeLabel, 0);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.25f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	SetControllerSceneProgressRange((void*) p_controller->m_unk0xdc, NULL, 0.25f, 0.85f);

	ControllerHudTemplateStorage hudTemplateStorage;
	ControllerHudTemplateView* hudTemplateView =
		InitializeControllerHudTemplate(&hudTemplateStorage, (p_controller->m_state == 6));
	void* hudStorage = AllocateHeapMemory(0x3f4);
	GameObject* hudManager =
		(hudStorage == NULL) ? NULL
							 : (GameObject*) ConstructControllerHudManager(hudStorage, NULL, &hudTemplateView->m_base);
	p_controller->m_unk0x88 = hudManager;
	hudManager->m_vtable->m_initialize(hudManager, NULL);
	hudManager->m_flags |= 0x400000;
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.9f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);

	if (p_controller->m_state != 5) {
		if (p_controller->m_state == 4) {
			LegoChar supervisorText[24];
			sprintf(supervisorText, "AI Supervisor Track %d", session->m_track);
			void* supervisor = FindGameObjectByName(supervisorText, 1, 0);
			if (p_controller->m_raceManager != NULL) {
				((RaceManagerSceneView*) p_controller->m_raceManager)->m_aiSupervisor = supervisor;
			}
		}
		else {
			CharBuffer24 supervisorText("AI Supervisor Bonus");
			void* supervisor = FindGameObjectByName(supervisorText.m_data, 1, 0);
			if (p_controller->m_raceManager != NULL) {
				((RaceManagerSceneView*) p_controller->m_raceManager)->m_aiSupervisor = supervisor;
			}
		}
	}

	if (p_controller->m_state == 4) {
		CharBuffer24 checkpointText;
		sprintf(checkpointText.m_data, "CP Supervisor Track %d", session->m_track);
		void* checkpointSupervisor = FindGameObjectByName(checkpointText.m_data, 1, 0);
		((RaceManagerSceneView*) p_controller->m_raceManager)->m_checkpointSupervisor = checkpointSupervisor;
		*(LegoU32*) ((LegoU8*) checkpointSupervisor + 0x60) = session->m_unk0x14;
		*(LegoU32*) ((LegoU8*) checkpointSupervisor + 0x64) = (session->m_mode == 5);
		if (session->m_mode == 6) {
			FUN_00411110(p_controller->m_unk0x8c, NULL);
		}
		else if (session->m_selectedMode == 0) {
			session->m_unk0x20 = 0.35f;
		}
		else if (session->m_selectedMode == 2) {
			session->m_unk0x20 = 0.5f;
		}
		else {
			session->m_unk0x20 = 0.7f;
		}
	}

	LegoS32 activePlayerIndex = 0;
	StartupPlayerSettings* playerSettings = (StartupPlayerSettings*) ((LegoU8*) session + 0x4c);
	for (LegoS32 player = 0; player < (LegoS32) session->m_unk0x18; ++player) {
		if (session->m_unk0x28[player] == 0) {
			if (playerSettings[player].m_unk0x0c == 1) {
				playerSettings[player].m_unk0x10 = activePlayerIndex++;
			}
			else {
				playerSettings[player].m_unk0x10 = -1;
			}
			session->m_unk0x28[player] = SelectControllerSceneRecord(0);
		}
	}

	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.95f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	SetControllerFaceTexturesLoaded(p_controller, NULL, 1);
	SetControllerSelectionProgressValue(selectionProgress, NULL, 0.98f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);

	ControllerSceneObjectStateView* sceneObject = (ControllerSceneObjectStateView*) GetControllerSceneObject();
	if (session->m_unk0x04 != 0) {
		sceneObject->m_active = 1;
		sceneObject->m_sceneToken = session->m_unk0x28[0];
	}
	else {
		sceneObject->m_active = 0;
	}
	LegoU32 sceneInput = IsControllerBodyInputEnabled(p_controller->m_unk0x8c, NULL);
	void* inputOwner = ((ControllerSceneInputOwnerView*) p_controller->m_unk0xa4)->m_inputOwner;
	SetControllerBodyInputEnabled(inputOwner, NULL, sceneInput);
	if (((ControllerNetworkStateView*) p_controller->m_networkApp)->m_state != 3) {
		ConfigureNetworkGame((NetworkAppLayer*) p_controller->m_networkApp, NULL);
	}

	SetControllerSelectionProgressValue(selectionProgress, NULL, 1.0f);
	UpdateControllerSelectionProgress(selectionProgress, NULL, 0);
	FinalizeControllerSelectionProgress(selectionProgress, NULL);
	const RendererMainDispatch* rendererDispatch = (const RendererMainDispatch*) g_displayRenderer->m_vtable;
	if (rendererDispatch->m_setDeviceResourceMode(g_displayRenderer, NULL, 2) == 0) {
		RequestDeferredWindowDestruction();
	}
	UpdateControllerScene(p_controller, NULL);
	g_gameController.m_unk0x348 = g_displayRenderer->m_vtable->m_textureQuality(g_displayRenderer, NULL);
	if (p_controller->m_state == 4) {
		FUN_0040eb40(p_controller->m_raceManager, NULL);
	}
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_unk0x38(audio, NULL);
	MemoryHeap* heap = GetMemoryHeap();
	WalkHeapAddressRange(heap, NULL);
}

struct TextBoxManagerResetView {
	undefined m_prefix[0x58];
	GameObject* m_nestedManager;
};

struct NestedTextBoxManagerResetView {
	undefined m_prefix[0x58];
	LegoU32 m_active;
};

// STUB: LEGORACERS2 0x00439030
void __fastcall ResetActiveTextBoxManager(GameObject* p_textBoxManager, void*)
{
	GameObject* nestedManager = ((TextBoxManagerResetView*) p_textBoxManager)->m_nestedManager;
	if (((NestedTextBoxManagerResetView*) nestedManager)->m_active != 0) {
		ClearTextBoxManagerResources(nestedManager, NULL);
	}
}

// STUB: LEGORACERS2 0x00475b80
void __fastcall SetControllerBodyInputEnabled(void* p_group, void*, LegoU32 p_enabled)
{
	LegoU8* group = (LegoU8*) p_group;
	*(LegoU32*) (group + 0x10) = p_enabled;
	LegoU32 active = p_enabled != 0;
	for (LegoU32 i = 0; i < 4; ++i) {
		LegoU8* body = ((LegoU8**) group)[i];
		*(LegoU32*) (body + 0x468) = active;
	}
}
