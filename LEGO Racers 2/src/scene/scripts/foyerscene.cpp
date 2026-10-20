#include "scene/scripts/foyerscene.h"

#include "audio/service/audioservice.h"
#include "frontend/foyer/controllerfoyertaskmanager.h"
#include "frontend/foyer/foyerselectionhelpers.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gametemplateinstance.h"
#include "game/session/gamecontrollersession.h"
#include "memory/memoryheap.h"
#include "scene/animation/foyeranimation.h"
#include "scene/effects/controllersceneobject.h"
#include "scene/effects/secondaryfxmanager.h"
#include "scene/scripts/foyerscenecontrolleractions.h"
#include "scene/scripts/foyersceneevents.h"

#include <new>
#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00572e18
FoyerSceneResourceRegistry g_foyerSceneResourceRegistry;

// GLOBAL: LEGORACERS2 0x00572ddc
LegoU8 g_foyerSceneResourceInitFlags;

struct FoyerSceneChildTemplateStorage : GameObjectTemplate {
	CharBuffer24 m_firstDefaultName;
	CharBuffer24 m_secondDefaultName;
	LegoU32 m_unk0x64[8];

	FoyerSceneChildTemplateStorage() : GameObjectTemplate(), m_firstDefaultName(), m_secondDefaultName()
	{
		LegoU32 i;
		for (i = 0; i < 8; ++i) {
			m_unk0x64[i] = 0;
		}
	}
};
DECOMP_SIZE_ASSERT(FoyerSceneChildTemplateStorage, 0x84);

// GLOBAL: LEGORACERS2 0x00572d58
FoyerSceneChildTemplateStorage g_foyerSceneChildTemplate;

namespace
{
struct FoyerSceneTemplateStorage : GameObjectTemplate {
	FoyerSceneTemplateStorage()
	{
		m_flags = 0x0a;
		m_first = CharBuffer24("NoName");
		m_second = CharBuffer24("");
	}
};

LegoU8 FoldFoyerResourcePathByte(LegoU8 p_value)
{
	if (p_value >= 'a' && p_value <= 'z') {
		p_value = (LegoU8) (p_value & 0xdf);
	}
	return p_value;
}

LegoBool32 FoyerResourcePathsEqual(const LegoChar* p_left, const LegoChar* p_right)
{
	LegoU32 i = 0;
	for (;;) {
		LegoU8 left = FoldFoyerResourcePathByte((LegoU8) p_left[i]);
		LegoU8 right = FoldFoyerResourcePathByte((LegoU8) p_right[i]);
		if (left != right) {
			return FALSE;
		}
		if (p_left[i] == '\0') {
			return TRUE;
		}
		i++;
		if (i >= 0x80) {
			return TRUE;
		}
	}
}
} // namespace

// GLOBAL: LEGORACERS2 0x00572de0
FoyerSceneTemplateStorage g_foyerSceneTemplate;

// The event records occupy a fixed 12-entry, 0x94-byte array. Their concrete
// interfaces are recovered separately; these declarations preserve the receiver
// ABI seen in the resource callbacks.
void __cdecl FoyerSceneResourceExitCallback();
void __fastcall ReleaseFoyerSceneEventState(void*, void*);                                // 47c1f0
void __fastcall InitializeFoyerSceneEventState(void*, void*);                             // 47c090
LegoU32 __fastcall UpdateFoyerSceneTransition(FoyerSceneResourceView*, void*, LegoFloat); // 47f4b0
LegoU32 __fastcall UpdateFoyerSceneFade(FoyerSceneResourceView*, void*, LegoFloat);       // 47f630
LegoU32 __fastcall UpdateFoyerSceneHold(FoyerSceneResourceView*, void*, LegoFloat);       // 47f6a0
void __fastcall UpdateFoyerSceneEntry(void*, void*, LegoFloat);                           // 47c3d0
void __fastcall ResetFoyerSceneEntry(GameObject*, void*);                                 // 47c280
void __fastcall ParseFoyerSceneResourceEvents(GameObject*, void*);                        // 47cd00
void __fastcall ResetFoyerSceneAnimationState(LegoU32);                                   // 47e770
void __fastcall SetFoyerSceneAnimationMode(FoyerSceneResource*, void*, LegoU32);          // 47ff90
void __fastcall ResetFoyerSceneSelection(GameObject*, void*);                             // 47fda0
void __fastcall UpdateFoyerSceneInputMode(FoyerSceneResource*, void*, LegoFloat*);        // 47fe60
void __fastcall DispatchDueFoyerSceneActions(FoyerSceneResource*, void*);                 // 47f730
void __fastcall SetFoyerSceneInputAnimationState(GameObject*, void*, LegoU32);
void __fastcall ClearFoyerSceneActionParameters(void*, void*);

extern LegoU32 g_gameControllerStorage[];
extern LegoU32 g_unk0x00572f94;

namespace
{
typedef LegoFloat*(__fastcall* FoyerSceneGetPosition)(void*, void*);
typedef void(__fastcall* FoyerSceneSetTransform)(void*, void*, const LegoFloat*, LegoU32);

void SetFoyerSceneControllerState(void* p_controller, LegoU32 p_enabled)
{
	LegoU8* vtable = (LegoU8*) *(LegoU32*) p_controller;
	if (p_enabled != 0) {
		((void(__fastcall*)(void*, void*)) * (LegoU32*) (vtable + 0x78))(p_controller, NULL);
	}
	((void(__fastcall*)(void*, void*, LegoU32)) * (LegoU32*) (vtable + 0x7c))(p_controller, NULL, p_enabled);
	((void(__fastcall*)(void*, void*, LegoU32)) * (LegoU32*) (vtable + 0x74))(p_controller, NULL, p_enabled);
}
} // namespace

extern const FoyerSceneResourceDispatchView g_foyerSceneResourceDispatch = {
	&FoyerSceneResourceDestroy,    &GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,           &GameObjectBaseNoOp,
	&FoyerSceneResourceInitialize, &GameObjectBaseTrue,
	&FoyerSceneResourceUpdate,     &FoyerSceneResourceRemove,
	&GameObjectBaseZero,           &GameObjectBaseZero,
	&GameObjectBaseNoOp,           &GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,      &GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,           &GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,           &GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,   &GameObjectBaseNoOpOneArgument,
};

// STUB: LEGORACERS2 0x0047efa0
FoyerSceneResource::FoyerSceneResource(const CharBuffer128* p_resourcePath)
	: GameObject(0x700, (GameObjectTemplate*) &g_foyerSceneTemplate, NULL), m_resourceName("noname")
{
	LegoU32 i;
	m_unk0x58 = 0;
	m_unk0x68 = 0;
	m_eventCount = 0;
	for (i = 0; i < 8; ++i) {
		m_inputStates[i].m_audioSourceRecord = NULL;
	}
	m_unk0x5b4 = 0;
	m_unk0x9b8 = 0;
	m_unk0x9c0 = 0;
	m_unk0x9c8 = 1;
	m_unk0x9cc = 1;
	m_unk0x9e8 = 0;
	m_unk0x9ec = 0;
	m_unk0x9f0 = 0;
	m_unk0xa2c = 0;
	m_unk0xa30 = 0;
	m_unk0xa34 = 0;
	m_unk0xa38 = 0;
	for (i = 0; i < 128; ++i) {
		m_resourcePath[i] = p_resourcePath->m_data[i];
	}
	m_unk0xad8 = 0;
	m_unk0xadc = 0;
	m_vtable = (const GameObjectDispatchView*) &g_foyerSceneResourceDispatch;
}

// STUB: LEGORACERS2 0x0047f080
GameObject* __fastcall FoyerSceneResourceDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_foyerSceneResourceDispatch;
	p_object->GameObject::~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0047f0b0
void __fastcall FoyerSceneResourceInitialize(GameObject* p_object, void*)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_object;
	LegoS32 i;

	if ((resource->m_flags & 0x00800000) == 0) {
		LegoU8* allocation = (LegoU8*) AllocateHeapMemory(0x6f4);
		if (allocation != NULL) {
			*(LegoU32*) allocation = 12;
			resource->m_eventStorage = allocation + 4;
			for (i = 0; i < 12; ++i) {
				InitializeFoyerSceneEvent((FoyerSceneEventPrefix*) (allocation + 4 + i * 0x94), NULL);
			}
		}
		else {
			resource->m_eventStorage = NULL;
		}

		if ((g_foyerSceneResourceInitFlags & 1) == 0) {
			g_foyerSceneResourceInitFlags |= 1;
			g_foyerSceneChildTemplate.m_flags = 0x0a;
			g_foyerSceneChildTemplate.m_first = CharBuffer24("NoName");
			g_foyerSceneChildTemplate.m_second = CharBuffer24("");
			g_foyerSceneChildTemplate.m_firstDefaultName = CharBuffer24("");
			g_foyerSceneChildTemplate.m_secondDefaultName = CharBuffer24("");
			g_foyerSceneChildTemplate.m_unk0x64[0] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[1] = 1;
			g_foyerSceneChildTemplate.m_unk0x64[2] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[3] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[4] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[5] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[6] = 0;
			g_foyerSceneChildTemplate.m_unk0x64[7] = 0;
			atexit(FoyerSceneResourceExitCallback);
		}

		for (i = 0; i < 2; ++i) {
			void* objectStorage = AllocateHeapMemory(0xc0);
			GameObject* child = NULL;
			if (objectStorage != NULL) {
				child = new (objectStorage) GameTemplateInstance(&g_foyerSceneChildTemplate);
			}
			resource->m_children[i] = child;
			child->m_vtable->m_initialize(child, NULL);
			child->m_flags |= 4;
		}

		GameControllerSession* session = GetGameControllerSession();
		if (session->m_unk0x04 != 0 || g_gameController.m_state == 6) {
			ParseFoyerSceneResourceEvents(p_object, NULL);
		}

		for (i = 0; i < resource->m_unk0x9b8; ++i) {
			FoyerSceneObjectView* child = (FoyerSceneObjectView*) resource->m_objects[i];
			if (child->m_type != 2) {
				child->m_vtable->m_update(child, NULL);
			}
		}
		for (i = 0; i < resource->m_unk0x9b8; ++i) {
			FoyerSceneObjectView* child = (FoyerSceneObjectView*) resource->m_objects[i];
			if (child->m_type == 2) {
				child->m_vtable->m_update(child, NULL);
			}
		}

		for (i = 0; i < 12; ++i) {
			InitializeFoyerSceneEventState((LegoU8*) resource->m_eventStorage + i * 0x94, NULL);
		}
	}

	resource->GameObject::Initialize();
}

// FUNCTION: LEGORACERS2 0x0047f320 FOLDED
void __cdecl FoyerSceneResourceExitCallback()
{
}

// STUB: LEGORACERS2 0x0047f330
void __fastcall FoyerSceneResourceRemove(GameObject* p_object, void*)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_object;
	LegoS32 i;
	resource->GameObject::Remove();
	for (i = 11; i >= 0; --i) {
		ReleaseFoyerSceneEventState((LegoU8*) resource->m_eventStorage + 0x65c - (11 - i) * 0x94, NULL);
	}

	for (i = resource->m_unk0x9b8 - 1; i >= 0; --i) {
		FoyerSceneObjectView* child = (FoyerSceneObjectView*) resource->m_objects[i];
		child->m_vtable->m_destroyState(child, NULL);
	}
	for (i = resource->m_unk0x9b8 - 1; i >= 0; --i) {
		FreeHeapMemory(resource->m_objects[i]);
	}

	AudioService* audio = GetAudioService(TRUE);
	for (i = resource->m_unk0x5b4 - 1; i >= 0; --i) {
		audio->m_vtable->m_releaseSource(audio, NULL, resource->m_inputStates[i].m_audioSourceRecord);
	}

	for (i = 1; i >= 0; --i) {
		GameObject* child = resource->m_children[i];
		child->m_vtable->m_remove(child, NULL);
		child->m_vtable->m_destroy(child, NULL, 1);
	}
	FreeHeapMemory((LegoU8*) resource->m_eventStorage - 4);
}

// STUB: LEGORACERS2 0x0047f420
LegoU32 __fastcall FoyerSceneResourceUpdate(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_object;
	switch (resource->m_unk0x58) {
	case 1:
		UpdateFoyerSceneTransition((FoyerSceneResourceView*) resource, NULL, p_elapsedSeconds);
		return 1;
	case 2:
		UpdateFoyerSceneFade((FoyerSceneResourceView*) resource, NULL, p_elapsedSeconds);
		return 1;
	case 3:
		UpdateFoyerSceneHold((FoyerSceneResourceView*) resource, NULL, p_elapsedSeconds);
		return 1;
	default:
		return 0;
	}
}

// STUB: LEGORACERS2 0x0047f4b0
LegoU32 __fastcall UpdateFoyerSceneTransition(FoyerSceneResourceView* p_view, void*, LegoFloat p_elapsedSeconds)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_view;
	LegoFloat duration = 0.0f;
	LegoFloat* elapsed = (LegoFloat*) &p_elapsedSeconds;
	LegoFloat current = *(LegoFloat*) &resource->m_unk0x9c4;
	if (duration <= current) {
		UpdateFoyerSceneInputMode(resource, NULL, elapsed);
		*(LegoFloat*) &resource->m_unk0x9c4 = current + *elapsed;
		UpdateFoyerSceneEvents(p_view, NULL, *elapsed);
		DispatchDueFoyerSceneActions(resource, NULL);
		return 1;
	}
	current += p_elapsedSeconds;
	*(LegoFloat*) &resource->m_unk0x9c4 = current;
	if (current < duration) {
		return 1;
	}
	if (resource->m_unk0x9f0 != 0) {
		LegoU8* controller = (LegoU8*) *(LegoU32*) ((LegoU8*) resource + 0x60);
		LegoU8* vtable = (LegoU8*) *(LegoU32*) controller;
		LegoFloat* position = ((FoyerSceneGetPosition) * (LegoU32*) (vtable + 0x54))(controller, NULL);
		LegoFloat* orientation = ((FoyerSceneGetPosition) * (LegoU32*) (vtable + 0x58))(controller, NULL);
		for (LegoU32 i = 0; i < 3; ++i) {
			((LegoFloat*) &resource->m_opaque0x9f4)[i] = position[i];
		}
		for (LegoU32 j = 0; j < 4; ++j) {
			((LegoFloat*) &resource->m_opaque0x9f4)[3 + j] = orientation[j];
		}
	}
	{
		LegoU8* controller = (LegoU8*) *(LegoU32*) ((LegoU8*) resource + 0x60);
		LegoU8* vtable = (LegoU8*) *(LegoU32*) controller;
		LegoU8* target = (LegoU8*) *(LegoU32*) ((LegoU8*) resource + 0x9d4);
		LegoFloat* targetPosition = (LegoFloat*) (target + 0x58);
		LegoFloat* targetOrientation = (LegoFloat*) (target + 0x64);
		((FoyerSceneSetTransform) * (LegoU32*) (vtable + 0x5c))(controller, NULL, targetPosition, 0);
		((FoyerSceneSetTransform) * (LegoU32*) (vtable + 0x60))(controller, NULL, targetOrientation, 0);
		SetFoyerSceneControllerState(controller, 1);
	}
	if (resource->m_unk0x9ec != 0) {
		LegoU8* controller = (LegoU8*) *(LegoU32*) ((LegoU8*) resource + 0x60);
		if (*(LegoU32*) ((LegoU8*) resource + 0x64) != 2) {
			SetFoyerControllerMenuLoop((GameObject*) controller, NULL);
		}
		LegoU8* nested = (LegoU8*) *(LegoU32*) (controller + 0x1e8);
		LegoU8* transform = nested + 0x58;
		LegoU8* vtable = *(LegoU8**) transform;
		LegoU8* target = *(LegoU8**) ((LegoU8*) resource + 0x9e4);
		((void(__fastcall*)(void*, void*, const LegoFloat*)) *
		 (LegoU32*) (vtable + 0x20))(transform, NULL, (const LegoFloat*) (target + 0x58));
		((void(__fastcall*)(void*, void*, const LegoFloat*)) *
		 (LegoU32*) (vtable + 0x24))(transform, NULL, (const LegoFloat*) (target + 0x64));
		*(LegoU32*) (nested + 8) |= 2;
	}
	ResetSecondaryFxController(
		(GameObject*) *(void**) ((LegoU8*) g_gameController.m_secondaryFxManager + 0x58),
		NULL,
		1,
		0
	);
	resource->m_unk0x58 = 1;
	return 1;
}

// STUB: LEGORACERS2 0x0047f6e0
void __fastcall CompletePendingFoyerSceneActions(FoyerSceneResource* p_resource, void*)
{
	while ((LegoS32) p_resource->m_unk0x9bc < (LegoS32) p_resource->m_unk0x9b8) {
		FoyerSceneObjectView* action = (FoyerSceneObjectView*) p_resource->m_objects[p_resource->m_unk0x9bc];
		if (action->m_type == 5) {
			((void(__fastcall*)(FoyerSceneObjectView*, void*)) action->m_vtable->m_destroy)(action, NULL);
		}
		++p_resource->m_unk0x9bc;
	}
}

// STUB: LEGORACERS2 0x0047f820
FoyerSceneResourceView* __cdecl FindOrCreateFoyerSceneResource(const CharBuffer128* p_path)
{
	if (p_path->m_data[0] == '\0') {
		return NULL;
	}

	for (LegoS32 i = 0; i < g_foyerSceneResourceRegistry.m_count; ++i) {
		FoyerSceneResourceView* resource = g_foyerSceneResourceRegistry.m_entries[i];
		if (FoyerResourcePathsEqual(p_path->m_data, resource->m_resourcePath.m_data)) {
			return resource;
		}
	}

	void* storage = AllocateHeapMemory(sizeof(FoyerSceneResource));
	FoyerSceneResource* resource = new (storage) FoyerSceneResource(p_path);
	resource->m_flags |= 0x00400000;
	resource->m_vtable->m_initialize(resource, NULL);
	if ((resource->m_flags & 4) == 0) {
		resource->m_flags |= 4;
	}
	g_foyerSceneResourceRegistry.m_entries[g_foyerSceneResourceRegistry.m_count] = (FoyerSceneResourceView*) resource;
	g_foyerSceneResourceRegistry.m_count++;
	return (FoyerSceneResourceView*) resource;
}

// STUB: LEGORACERS2 0x0047f910
void __cdecl ReleaseFoyerSceneResource(const CharBuffer128* p_path)
{
	if (p_path->m_data[0] == '\0') {
		return;
	}

	FoyerSceneResourceView* resource =
		*(FoyerSceneResourceView**) ((LegoU8*) &g_foyerSceneResourceRegistry +
									 g_foyerSceneResourceRegistry.m_count * sizeof(void*));
	if (!FoyerResourcePathsEqual(p_path->m_data, resource->m_resourcePath.m_data) && GetMemoryHeap()->m_unk0x0c == 0) {
		return;
	}
	GameObject* object = (GameObject*) resource;
	object->m_vtable->m_remove(object, NULL);
	object->m_vtable->m_destroy(object, NULL, 1);
	g_foyerSceneResourceRegistry.m_count--;
}

// STUB: LEGORACERS2 0x0047f990
FoyerSceneResourceView* __cdecl FindFoyerSceneResource(const LegoChar* p_name)
{
	for (LegoS32 i = 0; i < g_foyerSceneResourceRegistry.m_count; ++i) {
		FoyerSceneResourceView* resource = g_foyerSceneResourceRegistry.m_entries[i];
		const LegoChar* candidate = resource->m_resourceName.m_data;
		LegoU32 offset = 0;
		for (;;) {
			if (candidate[offset] != p_name[offset]) {
				break;
			}
			if (candidate[offset] == '\0') {
				return resource;
			}
			if (candidate[offset + 1] != p_name[offset + 1]) {
				break;
			}
			if (candidate[offset + 1] == '\0') {
				return resource;
			}
			offset += 2;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x0047fb70
void __fastcall CompleteFoyerSceneTransition(FoyerSceneResource* p_resource, void*)
{
	CompletePendingFoyerSceneActions(p_resource, NULL);
	LegoU8* controller = (LegoU8*) *(LegoU32*) ((LegoU8*) p_resource + 0x60);
	LegoU32 mode = *(LegoU32*) (controller + 0x58);
	LegoU32 taskIndex = *(LegoU32*) ((LegoU8*) p_resource + 0x5c);
	LegoU32 taskStatus = *(LegoU32*) ((LegoU8*) GetGameControllerSession() + mode * 0x344 + 0x5c);
	void* taskManager = GetControllerTaskManager();
	SetControllerTaskActionStatus(taskManager, NULL, taskStatus, taskIndex);
	ClearFoyerNpcText(g_gameController.m_npcTextManager, NULL);
	ResetFoyerSceneSelection((GameObject*) p_resource, NULL);
	for (LegoU32 i = 0; i < 2; ++i) {
		ClearFoyerSceneActionParameters(p_resource->m_children[i], NULL);
	}
	if (*(LegoU32*) (controller + 0xa8) != 0) {
		*(LegoU32*) (controller + 0xac) = 1;
	}
	SetFoyerSceneAnimationMode(p_resource, NULL, 0x3f800000);
	if (p_resource->m_unk0x9e8 != 0) {
		SetFoyerControllerActive((GameObject*) controller, NULL, 0);
		((void(__fastcall*)(void*, void*, LegoU32)) * (LegoU32*) (*(LegoU8**) controller + 0x74))(controller, NULL, 0);
		SetFoyerSceneControllerState(controller, 0);
		RestoreFoyerControllerActionState((GameObject*) controller, NULL);
		if (*(LegoU32*) ((LegoU8*) p_resource + 0xa3c) == 1 || *(LegoU32*) ((LegoU8*) p_resource + 0xa3c) == 3 ||
			p_resource->m_unk0x9cc != 0) {
			ResetSecondaryFxController(
				(GameObject*) *(void**) ((LegoU8*) g_gameController.m_secondaryFxManager + 0x58),
				NULL,
				2,
				0
			);
		}
		if (p_resource->m_unk0x9ec != 0) {
			LegoU8* nested = (LegoU8*) *(LegoU32*) (controller + 0x1e8);
			*(LegoU32*) (nested + 8) &= ~2u;
			if (*(LegoU32*) ((LegoU8*) p_resource + 0x64) == 2) {
				StartFoyerControllerRunLoop((GameObject*) controller);
			}
			else {
				SetFoyerControllerRunMenu((GameObject*) controller);
			}
			if (p_resource->m_unk0x9f0 != 0) {
				LegoU8* vtable = (LegoU8*) *(LegoU32*) controller;
				((FoyerSceneSetTransform) *
				 (LegoU32*) (vtable + 0x5c))(controller, NULL, (const LegoFloat*) ((LegoU8*) p_resource + 0x9f4), 1);
				((FoyerSceneSetTransform) *
				 (LegoU32*) (vtable + 0x60))(controller, NULL, (const LegoFloat*) ((LegoU8*) p_resource + 0xa00), 1);
			}
		}
	}
	SetFoyerMenuSceneActive((GameObject*) g_gameController.m_unk0x88, NULL, (GameObject*) controller, 1);
	SetFoyerSceneInputAnimationState(GetControllerSceneObject(), NULL, 0);
	p_resource->m_unk0x58 = 0;
	{
		GameObject* sceneObject = GetControllerSceneObject();
		if ((sceneObject->m_flags & 8) != 0) {
			sceneObject->m_flags &= ~8u;
			sceneObject->m_flags |= 0x02000000;
		}
	}
	g_unk0x00572f94 = 0;
	p_resource->m_flags |= 4;
}
