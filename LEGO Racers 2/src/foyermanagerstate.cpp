#include "audioservice.h"
#include "controllerfoyerlifecycle.h"
#include "foyermanager.h"
#include "foyerscene.h"
#include "foyerscenecontrolleractions.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "gamepreferences.h"
#include "gametemplateinstance.h"
#include "globaloverlaymanager.h"
#include "inputdeviceservice.h"
#include "npctextmanager.h"
#include "secondarymanager.h"
#include "vector3.h"

#include <stdio.h>

void __fastcall SetFoyerSceneTransition(GameObject*, void*, LegoU32);
void __fastcall EnterSecondaryManagerMode(GameObject*, void*, LegoU32);
void __fastcall ResetFoyerSceneStateObjects(GameObject*, void*); // 00433090

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);
LegoU32 __fastcall IsFoyerModeUnlocked(void*, void*, LegoU32);
void __fastcall SetFoyerSceneTransition(GameObject*, void*, LegoU32);
static GameObject* __fastcall GetFoyerSceneEventObject(FoyerSceneResource*, void*, LegoU32);

// GLOBAL: LEGORACERS2 0x0056cb80
static Vector3 g_foyerUnplacedPosition;

struct FoyerNpcPlacement {
	LegoU32 m_position;
	LegoU32 m_visible;
};
// GLOBAL: LEGORACERS2 0x00530c9c
static const FoyerNpcPlacement g_foyerNpcPlacement[8][4] = {
	{{0, 0}, {0, 0}, {0, 0}, {0, 0}},
	{{0, 0}, {0, 0}, {0, 0}, {0, 0}},
	{{1, 0}, {1, 0}, {1, 0}, {1, 0}},
	{{2, 1}, {0, 0}, {0, 0}, {0, 0}},
	{{2, 1}, {2, 1}, {0, 0}, {0, 0}},
	{{2, 1}, {2, 1}, {2, 1}, {0, 0}},
	{{2, 1}, {2, 1}, {2, 1}, {2, 1}},
	{{2, 1}, {2, 1}, {2, 1}, {2, 1}},
};
static void __fastcall SetFoyerSceneObjectLayer(GameObject*, void*, LegoU32);
void __fastcall ConfigureFoyerSceneObjectForController(GameObject*, void*, GameObject*);

static LegoFloat FoyerProjectionScale(LegoFloat p_angle)
{
	static const LegoFloat half = 0.5f;
	LegoFloat result;
	__asm {
  fld p_angle
  fmul half
  fptan
  fstp st(0)
  fdivr half
  fstp result
	}
	return result;
}

static void ClearFoyerSceneObjectOverlays(GameObject*);
void __fastcall ApplyFoyerSceneStateMode(GameObject*, void*, LegoU32);
void __fastcall SetSecondaryManagerMoviePath(SecondaryManager*, void*, const LegoChar*);
void __fastcall StopControllerCdTrack(GameController*, void*);

namespace
{

void ResetFoyerSceneObject(GameObject* p_object)
{
	ClearFoyerSceneObjectOverlays(p_object);
	*(LegoU32*) ((LegoU8*) p_object + 0x58) = 7;
}

} // namespace

// STUB: LEGORACERS2 0x00411db0
void __fastcall ResetFoyerSceneState(void* p_settings, void*, LegoU32 p_mode)
{
	LegoU8* settings = (LegoU8*) p_settings;
	GameControllerSession* session = GetGameControllerSession();
	// GLOBAL: LEGORACERS2 0x0053eb08
	static const LegoChar* const movies[5] = {
		"Game Data\\Movies\\test2.mpg",
		"Game Data\\Movies\\ARCTIC.mpg",
		"Game Data\\Movies\\DINO.mpg",
		"Game Data\\Movies\\MARS.mpg",
		"Game Data\\Movies\\XALAX.mpg"
	};

	session->m_selectedMode = p_mode;
	*(LegoU32*) (settings + 0x1de0) = p_mode;
	g_gameController.m_state = 2;
	g_gameController.m_stateStarted = 0;

	if (p_mode == 0) {
		*(LegoS32*) (settings + 0x1de8) = -1;
		session->m_mode = 7;
		g_gameController.m_requestedState = 0;
		return;
	}

	g_gameController.m_state = 2;
	g_gameController.m_stateStarted = 0;
	SetSecondaryManagerMoviePath((SecondaryManager*) g_gameController.m_secondaryManager, NULL, movies[p_mode]);
	*(LegoS32*) (settings + 0x1de8) = IsFoyerModeUnlocked(settings + 0x1e04, NULL, p_mode) ? 2 : 1;
}

// STUB: LEGORACERS2 0x00411e50
void __fastcall CompleteFoyerSessionTransition(void* p_settings, void*)
{
	SetFoyerSceneTransition(g_gameController.m_foyerManager, NULL, 1);
	*(LegoS32*) ((LegoU8*) p_settings + 0x1de8) = 3;
	EnterSecondaryManagerMode(g_gameController.m_secondaryManager, NULL, 0x2e);
	SetNpcTextManagerText(g_gameController.m_npcTextManager, NULL, 0, 0x35, -1.0f, 0);
}

// STUB: LEGORACERS2 0x00429790
void __fastcall SetSecondaryManagerMoviePath(SecondaryManager* p_manager, void*, const LegoChar* p_movie)
{
	const LegoChar* prefix = "";
	GetRegistryMediaPath(&prefix, FALSE);
	sprintf(p_manager->m_startupPath.m_data, "%s%s", prefix, p_movie);
}

// STUB: LEGORACERS2 0x0042f490
void __fastcall ConfigureFoyerSceneObjectForController(GameObject* p_object, void*, GameObject* p_controller)
{
	LegoU8* object = (LegoU8*) p_object;
	LegoU8* controller = (LegoU8*) p_controller;
	if (*(LegoU32*) (*(LegoU8**) (object + 0x60) + 0x3d0) == 0 && g_gameController.m_state == 4) {
		LegoU8* scene = *(LegoU8**) ((LegoU8*) g_gameController.m_raceManager + 0x64);
		if (scene == NULL) {
			return;
		}
		void* target = *(void**) (controller + 0x1e8);
		LegoS32 count = *(LegoS32*) (scene + 0x138);
		LegoU8* match = NULL;
		for (LegoS32 i = 0; i < count; ++i) {
			LegoU8* record = scene + 0x140 + i * 0x44;
			if (*(void**) record == target) {
				match = record;
				break;
			}
		}
		*(void**) (object + 0x68) = match;
	}
	*(GameObject**) (object + 0x64) = p_controller;
	LegoU8* camera = *(LegoU8**) (controller + 0xa8);
	*(void**) (object + 0x6c) = camera;
	SetFoyerSceneObjectLayer(p_object, NULL, *(LegoU32*) (camera + 0x1c8));
	LegoU8* attachment = *(LegoU8**) (object + 0x1a0);
	if (attachment != NULL) {
		*(LegoU32*) (attachment + 0x184) = 0;
		*(void**) (attachment + 0x188) = camera;
	}
	*(LegoU32*) (object + 0x1f4) = 0;
	*(LegoFloat*) (object + 0x1f8) = 1.0f;
	*(LegoFloat*) (object + 0x200) = FoyerProjectionScale(*(LegoFloat*) (camera + 0x1d0));
	*(LegoU32*) (object + 0x58) = 0;
	*(LegoU32*) (object + 0x5c) = 0;
	*(LegoFloat*) (object + 0x204) = FoyerProjectionScale(*(LegoFloat*) (camera + 0x1d4));
	*(LegoFloat*) (object + 0x1bc) = *(LegoFloat*) (controller + 0x190);
	*(LegoU32*) (object + 0x1a4) = 0;
	*(LegoU32*) (object + 0x1a8) = 0;
	GameControllerSession* session = GetGameControllerSession();
	if (session->m_mode == 5) {
		LegoU8* record =
			(LegoU8*) GetGameControllerSettings() + 0x244 + (session->m_selectedMode * 5 + session->m_track) * 24 - 24;
		*(LegoU32*) (object + 0x1c0) = *(LegoU32*) (record + 0x14);
		*(LegoFloat*) (object + 0x1c4) = -1.0f;
	}
	*(LegoS32*) (object + 0x1d0) = -1;
	p_object->m_flags &= ~4u;
}

// STUB: LEGORACERS2 0x0042f600
void __fastcall ResetFoyerSceneObjectForExit(GameObject* p_object, void*)
{
	ResetFoyerSceneObject(p_object);
}

// FUNCTION: LEGORACERS2 0x004302c0
static void __fastcall SetFoyerSceneObjectLayer(GameObject* p_object, void*, LegoU32 p_layer)
{
	LegoU8* bytes = (LegoU8*) p_object;
	for (LegoS32 i = 0; i < 59; ++i) {
		GlobalOverlaySprite* sprite = *(GlobalOverlaySprite**) (bytes + 0xb4 + i * 4);
		if (sprite != NULL) {
			sprite->m_unk0x50 = p_layer;
		}
	}
	for (LegoS32 j = 0; j < 17; ++j) {
		GlobalOverlayRecord* text = *(GlobalOverlayRecord**) (bytes + 0x70 + j * 4);
		if (text != NULL) {
			text->m_type = p_layer;
		}
	}
}

// STUB: LEGORACERS2 0x00432d90
static void ClearFoyerSceneObjectOverlays(GameObject* p_object)
{
	LegoU8* bytes = (LegoU8*) p_object;
	for (LegoU32 i = 0; i < 17; ++i) {
		GlobalOverlayRecord* record = *(GlobalOverlayRecord**) (bytes + 0x70 + i * 4);
		if (record != NULL) {
			record->InitializeTextByIndex(-1, 0, 0.0f);
		}
	}
	for (LegoU32 j = 0; j < 59; ++j) {
		GlobalOverlaySprite* sprite = *(GlobalOverlaySprite**) (bytes + 0xb4 + j * 4);
		if (sprite != NULL) {
			sprite->SetVisible(0);
		}
	}
	GameObject* attachment = *(GameObject**) (bytes + 0x1a0);
	if (attachment != NULL) {
		attachment->m_flags |= 8;
	}
}

// STUB: LEGORACERS2 0x00433050
void __fastcall PrepareFoyerSceneSelection(void* p_selection, void*, GameObject* p_controller)
{
	LegoU8* selection = (LegoU8*) p_selection;
	LegoS32 count = *(LegoS32*) (selection + 0x60);
	for (LegoS32 i = 0; i < count; ++i) {
		GameObject* object = *(GameObject**) (selection + 0x58 + i * 4);
		if (*(LegoU32*) ((LegoU8*) object + 0x58) == 7) {
			ConfigureFoyerSceneObjectForController(object, NULL, p_controller);
			return;
		}
	}
}

// STUB: LEGORACERS2 0x00433090
void __fastcall ResetFoyerSceneStateObjects(GameObject* p_state, void*)
{
	LegoU8* bytes = (LegoU8*) p_state;
	LegoS32 count = *(LegoS32*) (bytes + 0x60);
	for (LegoS32 i = 0; i < count; ++i) {
		GameObject* object = *(GameObject**) (bytes + 0x58 + i * 4);
		if (*(LegoU32*) ((LegoU8*) object + 0x58) != 7) {
			ResetFoyerSceneObject(object);
		}
	}
}

// STUB: LEGORACERS2 0x0043aa60
void __fastcall ExitFoyerScene(GameObject* p_manager, void*)
{
	StopControllerCdTrack(&g_gameController, NULL);
	g_inputService->m_vtable->poll(g_inputService, NULL, 0);
	FoyerManager* manager = (FoyerManager*) p_manager;
	ApplyFoyerSceneStateMode(manager->m_sceneState, NULL, 1);
	ResetFoyerSceneStateObjects((GameObject*) g_gameController.m_unk0x88, NULL);
	ResetFoyerGameControllerState(&g_gameController, NULL);
	manager->m_state = 9;
}

// FUNCTION: LEGORACERS2 0x0043b1d0
static void InitializeFoyerUnplacedPosition()
{
	g_foyerUnplacedPosition.m_x = 0.0f;
	g_foyerUnplacedPosition.m_y = 0.0f;
	g_foyerUnplacedPosition.m_z = 0.0f;
}

// STUB: LEGORACERS2 0x0043b1f0
void __stdcall SetFoyerSceneParameter(LegoU32 p_value)
{
	// GLOBAL: LEGORACERS2 0x00541c58
	static const LegoChar* npcNames[4] = {"workman2", "postman", "fireman", "policeman"};
	// GLOBAL: LEGORACERS2 0x00541c48
	static const LegoChar* challengeNames[4] =
		{"workmanchallenge", "postmanchallenge", "firemanchallenge", "policemanchallenge"};
	// GLOBAL: LEGORACERS2 0x00530c7c
	static const LegoChar* positions[8] = {
		"intro3_workman2pos",
		"default_workman2pos",
		"intro3_postmanpos",
		"default_postmanpos",
		"intro3_firemanpos",
		"default_firemanpos",
		"intro3_policemanpos",
		"default_policemanpos"
	};
	static LegoBool32 initialized;
	if (!initialized) {
		InitializeFoyerUnplacedPosition();
		initialized = TRUE;
	}
	for (LegoS32 i = 0; i < 4; ++i) {
		CharBuffer24 npcName(npcNames[i]);
		GameObject* npc = FindGameObjectByName(npcName.m_data, 1, NULL);
		CharBuffer24 challengeName(challengeNames[i]);
		GameObject* challenge = FindGameObjectByName(challengeName.m_data, 1, NULL);
		const FoyerNpcPlacement* placement = &g_foyerNpcPlacement[p_value][i];
		void* target = (LegoU8*) npc + 0x58;
		const GameTemplateInstanceTargetDispatch* dispatch = *(const GameTemplateInstanceTargetDispatch**) target;
		if (placement->m_position == 0) {
			dispatch->m_setPosition(target, NULL, &g_foyerUnplacedPosition);
		}
		else {
			CharBuffer24 positionName(positions[i * 2 + placement->m_position - 1]);
			GameObject* position = FindGameObjectByName(positionName.m_data, 1, NULL);
			dispatch->m_setPosition(target, NULL, (const Vector3*) ((LegoU8*) position + 0x58));
			dispatch->m_setOrientation(target, NULL, (const LegoFloat*) ((LegoU8*) position + 0x64));
		}
		LegoU32 hidden = placement->m_visible == 0 ? 4 : 0;
		if ((challenge->m_flags & 4) != hidden) {
			challenge->m_flags = (challenge->m_flags & ~4u) | hidden;
		}
	}
	if (p_value == 1 && IsFoyerModeUnlocked((LegoU8*) g_gameController.m_unk0x8c + 0x1e04, NULL, 0)) {
		FoyerSceneResource* intro = (FoyerSceneResource*) FindFoyerSceneResource("intro");
		GameObject* sparky = GetFoyerSceneEventObject(intro, NULL, 1);
		CharBuffer24 positionName("intro2_sparkypos");
		GameObject* position = FindGameObjectByName(positionName.m_data, 1, NULL);
		void* target = (LegoU8*) sparky + 0x58;
		const GameTemplateInstanceTargetDispatch* dispatch = *(const GameTemplateInstanceTargetDispatch**) target;
		dispatch->m_setPosition(target, NULL, (const Vector3*) ((LegoU8*) position + 0x58));
		dispatch->m_setOrientation(target, NULL, (const LegoFloat*) ((LegoU8*) position + 0x64));
	}
}

// STUB: LEGORACERS2 0x004496a0
void __fastcall ApplyFoyerSceneStateMode(GameObject* p_state, void*, LegoU32 p_mode)
{
	LegoU8* table = (LegoU8*) *(void**) p_state;
	if (p_mode != 0) {
		((void(__fastcall*)(GameObject*, void*)) * (void**) (table + 0x78))(p_state, NULL);
	}
	((void(__fastcall*)(GameObject*, void*, LegoU32)) * (void**) (table + 0x7c))(p_state, NULL, p_mode);
	((void(__fastcall*)(GameObject*, void*, LegoU32)) * (void**) (table + 0x74))(p_state, NULL, p_mode);
}
// FUNCTION: LEGORACERS2 0x004801a0
static GameObject* __fastcall GetFoyerSceneEventObject(FoyerSceneResource* p_scene, void*, LegoU32 p_index)
{
	return *(GameObject**) ((LegoU8*) p_scene->m_eventStorage + p_index * 0x94);
}
