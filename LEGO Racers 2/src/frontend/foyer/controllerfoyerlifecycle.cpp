#include "frontend/foyer/controllerfoyerlifecycle.h"

#include "audio/service/audioservice.h"
#include "core/application/commandline.h"
#include "core/strings/charbuffer128.h"
#include "core/strings/charbuffer24.h"
#include "frontend/foyer/foyermanager.h"
#include "game/character/bodypartmanager.h"
#include "game/session/gamecontrollersession.h"
#include "scene/scripts/foyerscene.h"

#include <stdio.h>
#include <string.h>

namespace
{
struct ControllerSceneMaskService {
	LegoU8 m_prefix[0x58];
	LegoU32 m_masks[10][8];
	CharBuffer24 m_names[10];
};
DECOMP_STATIC_ASSERT(offsetof(ControllerSceneMaskService, m_names) == 0x198);

struct ControllerSceneStackFrame {
	CharBuffer24 m_name;
	LegoU32 m_unk0x18;
	LegoU32 m_objectCount;
	LegoU32 m_flag253;
};
DECOMP_SIZE_ASSERT(ControllerSceneStackFrame, 0x24);

struct ControllerSceneStack {
	LegoU8 m_prefix[0x32e0];
	ControllerSceneMaskService* m_maskService;
	LegoS32 m_objectCount;
	GameObject** m_objects;
	GameObject** m_sortedObjects;
	LegoS32 m_frameCount;
	ControllerSceneStackFrame m_frames[1];
};
DECOMP_STATIC_ASSERT(offsetof(ControllerSceneStack, m_frameCount) == 0x32f0);
DECOMP_STATIC_ASSERT(offsetof(ControllerSceneStack, m_frames) == 0x32f4);
} // namespace

LegoBool32 __fastcall CopyControllerSceneTypeMask(ControllerSceneMaskService*, void*, const CharBuffer24*, LegoU32*);
void __fastcall PopControllerSceneStack(ControllerSceneStack*, void*, const CharBuffer24*);
void __fastcall RemoveControllerSceneStackObjects(ControllerSceneStack*, void*);
void __fastcall ReleaseOtherFoyerSceneResources(FoyerManager*, void*, LegoS32);

// STUB: LEGORACERS2 0x00409880
void __fastcall ResetFoyerGameControllerState(GameController* p_controller, void*)
{
	FoyerManager* foyer = (FoyerManager*) p_controller->m_foyerManager;
	if (foyer->m_unk0xe4[0] == 0) {
		ReleaseOtherFoyerSceneResources(foyer, NULL, p_controller->m_unk0xe0);
	}
	SetControllerFaceTexturesLoaded(p_controller, NULL, 0);

	GameObject** sessionObject = (GameObject**) ((LegoU8*) GetGameControllerSession() + 0x28);
	(*sessionObject)->m_vtable->m_remove(*sessionObject, NULL);
	if (*sessionObject != NULL) {
		(*sessionObject)->m_vtable->m_destroy(*sessionObject, NULL, 1);
	}
	*sessionObject = NULL;
	GameObject* selection = (GameObject*) p_controller->m_unk0x88;
	selection->m_vtable->m_remove(selection, NULL);
	selection = (GameObject*) p_controller->m_unk0x88;
	if (selection != NULL) {
		selection->m_vtable->m_destroy(selection, NULL, 1);
	}
	p_controller->m_unk0x88 = NULL;
	RemoveControllerSceneStackObjects((ControllerSceneStack*) p_controller->m_unk0xdc, NULL);
	CharBuffer24 name(foyer->m_unk0xe4[0] == 0 ? "Foyer" : "Explore");
	PopControllerSceneStack((ControllerSceneStack*) p_controller->m_unk0xdc, NULL, &name);
}

// STUB: LEGORACERS2 0x0040a250
void __fastcall SetControllerFaceTexturesLoaded(GameController* p_controller, void*, LegoU32 p_loaded)
{
	LegoS32 group = *(LegoS32*) ((LegoU8*) GetGameControllerSession() + 0x374);
	if (group >= 0) {
		for (LegoU32 i = 0; i < 8; ++i) {
			SetBodypartFaceTextureLoaded((BodypartManager*) p_controller->m_bodypartManager, NULL, group, i, p_loaded);
		}
	}
}

// STUB: LEGORACERS2 0x0040a3b0
void __fastcall StopControllerCdTrack(GameController*, void*)
{
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_stopCdTrack(audio, NULL);
}

// STUB: LEGORACERS2 0x0043ad60
void __fastcall PreloadOtherFoyerSceneResources(FoyerManager* p_manager, void*, LegoS32 p_mode)
{
	CharBuffer128 path;
	LegoS32 worlds = p_mode == 0 ? 5 : 1;
	for (LegoS32 world = 0; world < worlds; ++world) {
		if (world != p_mode) {
			LegoS32 scenes = p_mode != 0 ? 1 : (world == 4 ? 5 : 4);
			for (LegoS32 scene = 0; scene < scenes; ++scene) {
				ComposeFoyerSceneResourcePath(p_manager, NULL, &path, p_mode, world, scene);
				*(FoyerSceneResourceView**) ((LegoU8*) p_manager + 0xec + world * 0x14 + scene * 4) =
					FindOrCreateFoyerSceneResource(&path);
			}
		}
	}
}

// STUB: LEGORACERS2 0x0043ae20
void __fastcall ReleaseOtherFoyerSceneResources(FoyerManager* p_manager, void*, LegoS32 p_mode)
{
	CharBuffer128 path;
	for (LegoS32 world = p_mode != 0 ? 0 : 4; world >= 0; --world) {
		if (world != p_mode) {
			for (LegoS32 scene = 4; scene >= 0; --scene) {
				LegoU32* loaded = (LegoU32*) ((LegoU8*) p_manager + 0xec + world * 0x14 + scene * 4);
				if (*loaded != 0) {
					ComposeFoyerSceneResourcePath(p_manager, NULL, &path, p_mode, world, scene);
					ReleaseFoyerSceneResource(&path);
					*loaded = 0;
				}
			}
		}
	}
}

// STUB: LEGORACERS2 0x0043aec0
void __fastcall ComposeFoyerSceneResourcePath(
	FoyerManager*,
	void*,
	CharBuffer128* p_path,
	LegoS32 p_mode,
	LegoS32 p_world,
	LegoS32 p_scene
)
{
	// GLOBAL: LEGORACERS2 0x00541c0c
	static const LegoChar* scenes[5] = {"jump short", "jump full", "go away", "no jump", "defeat bosses"};
	// GLOBAL: LEGORACERS2 0x00541c20
	static const LegoChar* worlds[5] = {"sandy", "arctic", "dino", "lom", "cc"};
	// GLOBAL: LEGORACERS2 0x00541c34
	static const LegoChar* modes[5] = {"sandy island", "arctic", "adventurers", "mars", "carcraze"};
	LegoChar levelName[64];
	LegoBool32 defaultWorld = FALSE;
	if (FUN_004b4a60("/levelname")) {
		FUN_004b4a80("/levelname", levelName, sizeof(levelName));
		defaultWorld = strcmp(levelName, "geoff#\\default world") == 0;
	}
	sprintf(
		p_path->m_data,
		defaultWorld ? "Game Data\\scripts\\%s\\Default Jump Point\\%s %s.txt"
					 : "Game Data\\scripts\\%s\\Jump Point\\%s %s.txt",
		modes[p_mode],
		worlds[p_world],
		scenes[p_scene]
	);
}

// STUB: LEGORACERS2 0x0049b2e0
void __fastcall PopControllerSceneStack(ControllerSceneStack* p_stack, void*, const CharBuffer24*)
{
	--p_stack->m_frameCount;
	LegoS32 count = p_stack->m_frames[p_stack->m_frameCount].m_objectCount;
	while (count > 0) {
		--p_stack->m_objectCount;
		GameObject* object = p_stack->m_objects[p_stack->m_objectCount];
		if (object != NULL) {
			object->m_vtable->m_destroy(object, NULL, 1);
		}
		p_stack->m_objects[p_stack->m_objectCount] = NULL;
		--count;
	}
}

// STUB: LEGORACERS2 0x0049b4d0
void __fastcall RemoveControllerSceneStackObjects(ControllerSceneStack* p_stack, void*)
{
	for (LegoS32 frameIndex = p_stack->m_frameCount - 1; frameIndex >= 0; --frameIndex) {
		ControllerSceneStackFrame* frame = &p_stack->m_frames[frameIndex];
		LegoU32 mask[8] = {0};
		CopyControllerSceneTypeMask(p_stack->m_maskService, NULL, &frame->m_name, mask);
		LegoU32 bit = 1u << (253 & 31);
		if (((mask[253 >> 5] & bit) != 0) != frame->m_flag253) {
			mask[253 >> 5] ^= bit;
		}
		for (LegoS32 i = p_stack->m_objectCount - 1; i >= 0; --i) {
			GameObject* object = p_stack->m_sortedObjects[i];
			LegoU32 type = (object->m_flags >> 5) & 255;
			if ((mask[type >> 5] & (1u << (type & 31))) != 0) {
				object->m_vtable->m_remove(object, NULL);
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x004e6610
LegoBool32 __fastcall CopyControllerSceneTypeMask(
	ControllerSceneMaskService* p_service,
	void*,
	const CharBuffer24* p_name,
	LegoU32* p_mask
)
{
	for (LegoS32 i = 0; i < 10; ++i) {
		if (strcmp(p_service->m_names[i].m_data, p_name->m_data) == 0) {
			memcpy(p_mask, p_service->m_masks[i], 8 * sizeof(LegoU32));
			return TRUE;
		}
	}
	return FALSE;
}
