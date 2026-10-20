#include "game/controller/gamecontroller.h"
#include "game/objects/gamemanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/session/gamecontrollersession.h"
#include "game/state/gamerandom.h"
#include "memory/memoryheap.h"
#include "scene/scripts/foyersceneactionhelpers.h"
#include "scene/scripts/foyersceneactionsbase.h"
#include "scene/scripts/foyerscenescheduledaction.h"

#include <new>
#include <string.h>

struct ControllerTaskSourceDispatch {
	void(__fastcall* m_destroy)(void*, void*, LegoU32);
	void(__fastcall* m_unknown0x04)(GameObject*, void*);
	void(__fastcall* m_unknown0x08)(GameObject*, void*);
	LegoU32(__fastcall* m_getCount)(GameObject*, void*);
	GameObject*(__fastcall* m_getSceneChild)(GameObject*, void*, LegoU32);
	LegoU32(__fastcall* m_prepare)(GameObject*, void*, LegoFloat, LegoU32, void*);
};

struct ControllerTaskManagerView {
	GameObject m_object;
	void* m_taskState;
};

struct ControllerTaskManagerTemplateStorage {
	GameObjectTemplate m_template;
};

// GLOBAL: LEGORACERS2 0x0056f950
static ControllerTaskManagerTemplateStorage g_controllerTaskManagerTemplateStorage;
static LegoU32 g_controllerTaskManagerTemplateInitialized;

// GLOBAL: LEGORACERS2 0x0056f988
static ControllerTaskManagerView* g_controllerTaskManager;

// GLOBAL: LEGORACERS2 0x0056fb34
static const ControllerTaskSourceDispatch* g_controllerTaskSourceVtable;

static void __fastcall DestroyControllerTaskSource(void*, void*, LegoU32);
static void __fastcall ControllerTaskSourceNoOp(void*, void*);
static LegoU32 __fastcall GetControllerTaskSourceCount(GameObject*, void*);
static GameObject* __fastcall GetControllerTaskSceneChild(GameObject*, void*, LegoU32);
static LegoU32 __fastcall PrepareControllerTaskSource(GameObject*, void*, LegoFloat, LegoU32, void*);
static void* __fastcall InitializeControllerTaskSource(void*, void*);
static void InitializeControllerTaskRecord(LegoU32*);
static void InitializeControllerTaskInputRecord(LegoU8*);
static void InitializeControllerTaskManagerSourceRecords(LegoU8*);
static void SortControllerTaskSources(LegoU8*);
static void InitializeIdleControllerTaskAction(LegoU8*, LegoU32);
static void SelectControllerTaskAction(LegoU8*, LegoU32, LegoU32);
static void RefreshControllerTaskRecord(LegoU8*, LegoU8*);
static LegoU32 IsControllerTaskSourceCompatible(LegoU8*, LegoU32, LegoU8*);
LegoU32 __fastcall GetControllerTaskTypeForTarget(GameObject*, void*);
LegoU8* __fastcall FindControllerTaskCandidateTarget(LegoU8*, void*, LegoU8*);
static GameObject* __fastcall DestroyControllerTaskManager(GameObject*, void*, LegoU32);
static void __fastcall InitializeControllerTaskManager(GameObject*, void*);
static void __fastcall RemoveControllerTaskManager(GameObject*, void*);
static LegoU32 __fastcall UpdateControllerTaskManager(GameObject*, void*, LegoU32);
static void __fastcall ReleaseControllerTaskManagerState(ControllerTaskManagerView*);

// UpdateControllerTaskManager passes the active record at state + 4 + n*0x100;
// that is the FoyerSceneScheduledAction object itself.
static LegoU32 __fastcall UpdateFoyerSceneTaskAction(void* p_record, void*, LegoFloat p_elapsed)
{
	return ((FoyerSceneScheduledAction*) p_record)->Dispatch(p_elapsed);
}

static void __fastcall RecomputeControllerTaskAction(LegoU8* p_payload, void*)
{
	InitializeFoyerSceneScheduledActionRecord((FoyerSceneScheduledAction*) (p_payload - 0x1c), NULL);
}

// Retail dispatch table at 0x00532488.
extern const ControllerTaskSourceDispatch g_controllerTaskSourceDispatch = {
	&DestroyControllerTaskSource,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GetControllerTaskSourceCount,
	&GetControllerTaskSceneChild,
	&PrepareControllerTaskSource
};

extern const void* g_controllerTaskSourceRemovedDispatch[4] = {
	(void*) &DestroyControllerTaskSource,
	(void*) &ControllerTaskSourceNoOp,
	(void*) &ControllerTaskSourceNoOp,
	(void*) &ControllerTaskSourceNoOp
};

// Retail dispatch table at 0x0053241c.
extern const GameObjectDispatchView g_controllerTaskManagerDispatch = {
	&DestroyControllerTaskManager,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeControllerTaskManager,
	&GameObjectBaseZero,
	&UpdateControllerTaskManager,
	&RemoveControllerTaskManager,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument
};

// The CRT invokes this initializer before controller access, matching the
// retail template-construction hook at 0x0045b8c0.
void __cdecl InitializeControllerTaskManagerTemplate()
{
	if ((g_controllerTaskManagerTemplateInitialized & 1) == 0) {
		GameObjectTemplate* objectTemplate = &g_controllerTaskManagerTemplateStorage.m_template;
		new (objectTemplate) GameObjectTemplate;
		objectTemplate->m_flags = 10;
		objectTemplate->m_first = CharBuffer24("NoName");
		objectTemplate->m_second = CharBuffer24("");
		g_controllerTaskManagerTemplateInitialized |= 1;
	}
}

struct ControllerTaskManagerTemplateStartup {
	ControllerTaskManagerTemplateStartup() { InitializeControllerTaskManagerTemplate(); }
};
static ControllerTaskManagerTemplateStartup g_controllerTaskManagerTemplateStartup;

// STUB: LEGORACERS2 0x0045b920
static ControllerTaskManagerView* ConstructControllerTaskManager(ControllerTaskManagerView* p_manager)
{
	LegoU8* state;
	LegoS32 i;
	InitializeControllerTaskManagerTemplate();
	new (&p_manager->m_object) GameObject(0x900, &g_controllerTaskManagerTemplateStorage.m_template, NULL);
	p_manager->m_object.m_vtable = &g_controllerTaskManagerDispatch;
	state = (LegoU8*) AllocateHeapMemory(0xc20);
	if (state != NULL) {
		for (i = 0; i < 4; ++i) {
			InitializeControllerTaskInputRecord(state + 4 + i * 0x100);
		}
		for (i = 0; i < 16; ++i) {
			InitializeControllerTaskRecord((LegoU32*) (state + 0x610 + i * 0x5c));
		}
	}
	p_manager->m_taskState = state;
	if (state != NULL) {
		*(LegoU32*) state = 0;
		*(LegoU32*) (state + 0x508) = 0;
	}
	return p_manager;
}

// FUNCTION: LEGORACERS2 0x0045b9b0
static GameObject* __fastcall DestroyControllerTaskManager(GameObject* p_object, void*, LegoU32 p_flags)
{
	ReleaseControllerTaskManagerState((ControllerTaskManagerView*) p_object);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0045b9d0
static void __fastcall ReleaseControllerTaskManagerState(ControllerTaskManagerView* p_manager)
{
	LegoS32 i;
	LegoU8* state = (LegoU8*) p_manager->m_taskState;
	p_manager->m_object.m_vtable = &g_controllerTaskManagerDispatch;
	if (state != NULL) {
		for (i = 0; i < 4; ++i) {
			((void(__fastcall*)(void*, void*)) & GameObjectBaseNoOp)(state + 4 + i * 0x100, NULL);
		}
		FreeHeapMemory(state);
		p_manager->m_taskState = NULL;
	}
	p_manager->m_object.~GameObject();
}

// STUB: LEGORACERS2 0x0045ba20
void* __cdecl GetControllerTaskManager()
{
	if (g_controllerTaskManager == NULL) {
		g_controllerTaskManager = (ControllerTaskManagerView*) AllocateHeapMemory(0x5c);
		if (g_controllerTaskManager == NULL) {
			return NULL;
		}
		ConstructControllerTaskManager(g_controllerTaskManager);
		InitializeControllerTaskManager((GameObject*) g_controllerTaskManager, NULL);
		g_controllerTaskManager->m_object.m_flags |= 0x400000;
	}
	return g_controllerTaskManager;
}

// STUB: LEGORACERS2 0x0045ba70
void __cdecl ResetControllerTaskOverlayState()
{
	if (g_controllerTaskManager != NULL) {
		GameObject* manager = &g_controllerTaskManager->m_object;
		manager->m_vtable->m_remove(manager, NULL);
		if (g_controllerTaskManager != NULL) {
			manager = &g_controllerTaskManager->m_object;
			manager->m_vtable->m_destroy(manager, NULL, 1);
		}
		g_controllerTaskManager = NULL;
	}
}

// STUB: LEGORACERS2 0x0045baa0
static void __fastcall InitializeControllerTaskManager(GameObject* p_object, void*)
{
	ControllerTaskManagerView* manager = (ControllerTaskManagerView*) p_object;
	LegoU8* state = (LegoU8*) manager->m_taskState;
	LegoU32* interfaceObject;
	LegoU32 count;
	LegoU32 i;
	if ((p_object->m_flags & 0x800000) == 0 && state != NULL) {
		interfaceObject = (LegoU32*) AllocateHeapMemory(8);
		if (interfaceObject != NULL) {
			InitializeControllerTaskSource(interfaceObject, NULL);
			interfaceObject[1] = 0;
		}
		*(LegoU32*) state = (LegoU32) interfaceObject;
		if (interfaceObject != NULL) {
			g_controllerTaskSourceVtable = &g_controllerTaskSourceDispatch;
		}
	}
	p_object->GameObject::Initialize();
	if (state == NULL || *(LegoU32*) state == 0) {
		return;
	}
	count = GetControllerTaskSourceCount(NULL, NULL);
	InitializeControllerTaskManagerSourceRecords(state);
	for (i = 0; i < 16; ++i) {
		*(LegoU32*) (state + 0xbd0 + i * 4) = (LegoU32) (state + 0x610 + i * 0x5c);
	}
}

// STUB: LEGORACERS2 0x0045bb20
static void __fastcall RemoveControllerTaskManager(GameObject* p_object, void*)
{
	ControllerTaskManagerView* manager = (ControllerTaskManagerView*) p_object;
	LegoU8* state = (LegoU8*) manager->m_taskState;
	p_object->GameObject::Remove();
	if (state == NULL || *(LegoU32*) state == 0) {
		return;
	}
	DestroyControllerTaskSource((void*) *(LegoU32*) state, NULL, 1);
	*(LegoU32*) state = 0;
}

// STUB: LEGORACERS2 0x0045bb90
static void InitializeControllerTaskManagerSourceRecords(LegoU8* p_state)
{
	LegoU32 count = GetControllerTaskSourceCount(NULL, NULL);
	LegoU32 i;
	*(LegoU32*) (p_state + 0x404) = count;
	for (i = 0; i < count; ++i) {
		LegoU32* actionWords = (LegoU32*) (p_state + 0x20 + i * 0x100);
		GameObject* source = GetControllerTaskSceneChild(NULL, NULL, i);
		*(LegoU32*) (p_state + 8 + i * 0x100) = (LegoU32) source;
		memcpy(actionWords, g_foyerSceneActionDefaults, 23 * sizeof(LegoU32));
		*(LegoU32*) (p_state + 4 + i * 0x100) = 0;
	}
}

// STUB: LEGORACERS2 0x0045bc10
void __fastcall SetControllerTaskActionStatus(void* p_manager, void*, LegoU32 p_index, LegoU32 p_status)
{
	LegoU8* state = (LegoU8*) ((ControllerTaskManagerView*) p_manager)->m_taskState;
	LegoU8* record;
	LegoU8* target;
	LegoU32* flags;
	if (state == NULL || p_index >= 4) {
		return;
	}
	record = state + 4 + p_index * 0x100;
	if (*(LegoU32*) record == p_status) {
		return;
	}
	*(LegoU32*) record = p_status;
	target = (LegoU8*) *(LegoU32*) (record + 4);
	if (p_status == 0) {
		flags = (LegoU32*) (target + 8);
		*flags &= ~4u;
		*(LegoFloat*) (target + 0x1a4) = *(LegoFloat*) (record + 0x0c);
		return;
	}
	*(LegoU32*) (record + 0x0c) = 0x42c80000;
	flags = (LegoU32*) (target + 8);
	*flags |= 4u;
	memcpy(record + 0x1c, g_foyerSceneActionDefaults, 23 * sizeof(LegoU32));
	*(LegoU32*) (record + 0x1c) = 0;
	*(LegoU32*) (record + 0x90) = 1;
}

// STUB: LEGORACERS2 0x0045bcc0
void __fastcall SetControllerTaskActionRecord(void* p_manager, void*, LegoU32 p_index, const LegoU32* p_words)
{
	LegoU8* taskState = (LegoU8*) ((ControllerTaskManagerView*) p_manager)->m_taskState;
	if (taskState == NULL || p_index >= 4 || p_words == NULL) {
		return;
	}
	LegoU8* inputRecord = taskState + p_index * 0x100;
	memcpy(inputRecord + 0x20, p_words, 23 * sizeof(LegoU32));
	InitializeFoyerSceneScheduledActionRecord((FoyerSceneScheduledAction*) (inputRecord + 4), NULL);
}

// STUB: LEGORACERS2 0x0045bcf0
static LegoU32 __fastcall UpdateControllerTaskManager(GameObject* p_object, void*, LegoU32 p_delta)
{
	LegoU8* state = (LegoU8*) ((ControllerTaskManagerView*) p_object)->m_taskState;
	LegoU32 count;
	LegoU32 index;
	LegoU32 changed = 0;
	if (state == NULL) {
		return 0;
	}
	count = *(LegoU32*) (state + 0x404);
	for (index = 0; index < count; ++index) {
		LegoU8* sourceRecord = state + 4 + index * 0x100;
		LegoU32 status = *(LegoU32*) sourceRecord;
		if (status == 1) {
			*(LegoU32*) (state + 0xc14) = 0;
			changed = 1;
			LegoU32 result = UpdateFoyerSceneTaskAction(sourceRecord, NULL, *(LegoFloat*) &p_delta);
			if (result == 1) {
				*(LegoU32*) (state + 0xc14) = 1;
			}
			else if (result != 2) {
				continue;
			}
			SelectControllerTaskAction(state, index, p_delta);
		}
		else if (status == 2) {
			changed = 1;
			UpdateFoyerSceneTaskAction(sourceRecord, NULL, *(LegoFloat*) &p_delta);
		}
	}
	return changed;
}

// STUB: LEGORACERS2 0x0045bda0
static LegoU32 IsControllerTaskSourceCompatible(LegoU8* p_state, LegoU32 p_index, LegoU8* p_task)
{
	LegoU32 current = *(LegoU32*) (p_state + p_index * 0x100 + 0x70);
	return current == 0 || current != *(LegoU32*) (p_task + 0x50);
}

// STUB: LEGORACERS2 0x0045bfb0
static void SortControllerTaskSources(LegoU8* p_state)
{
	LegoU32 count = *(LegoU32*) (p_state + 0xc10);
	LegoU32 i;
	for (i = 1; i < count; ++i) {
		LegoU32 record = *(LegoU32*) (p_state + 0xbd0 + i * 4);
		LegoS32 j = (LegoS32) i;
		while (j > 0) {
			LegoU32 previous = *(LegoU32*) (p_state + 0xbd0 + (j - 1) * 4);
			if (*(LegoS32*) (previous + 4) <= *(LegoS32*) (record + 4)) {
				break;
			}
			*(LegoU32*) (p_state + 0xbd0 + j * 4) = previous;
			--j;
		}
		*(LegoU32*) (p_state + 0xbd0 + j * 4) = record;
	}
}

// STUB: LEGORACERS2 0x0045c010
static void SelectControllerTaskAction(LegoU8* p_state, LegoU32 p_index, LegoU32 p_elapsed)
{
	LegoU8* source = (LegoU8*) *(LegoU32*) p_state;
	ControllerTaskSourceDispatch* dispatch;
	LegoU8* output = p_state + 0x610;
	LegoU32 count;
	LegoU32 i;
	LegoU32* vtable;
	if (source == NULL) {
		return;
	}
	dispatch = (ControllerTaskSourceDispatch*) *(LegoU32*) source;
	vtable = (LegoU32*) dispatch;
	count = ((LegoU32(__fastcall*)(GameObject*, void*, LegoU32, LegoU32, void*))
				 vtable[5])((GameObject*) source, NULL, p_elapsed, 0x10, output);
	*(LegoU32*) (p_state + 0xc10) = count;
	SortControllerTaskSources(p_state);
	for (i = 0; i < count; ++i) {
		LegoU8* task = (LegoU8*) *(LegoU32*) (p_state + 0xbd0 + i * 4);
		if (*(LegoU32*) (p_state + 0xc14) == 1 &&
			(LegoS32) * (LegoU32*) (task + 4) <= *(LegoS32*) (p_state + p_index * 0x100 + 0x24)) {
			return;
		}
		if (*(LegoU32*) (task + 0x50) == 0) {
			*(LegoU32*) (task + 0x50) = (LegoU32) FindControllerTaskCandidateTarget(p_state, NULL, task);
		}
		if (*(LegoU32*) (task + 0x50) != 0) {
			RefreshControllerTaskRecord(p_state, task);
		}
		if (IsControllerTaskSourceCompatible(p_state, p_index, task) != 0) {
			memcpy(p_state + p_index * 0x100 + 0x20, task, 23 * sizeof(LegoU32));
			RecomputeControllerTaskAction(p_state + p_index * 0x100 + 0x20, NULL);
			return;
		}
		if (*(LegoU32*) (task + 4) == 10 || *(LegoU32*) (task + 4) == 8) {
			break;
		}
	}
	if (*(LegoU32*) (p_state + 0xc14) == 0 || (count != 0 && *(LegoU32*) (*(LegoU32*) (p_state + 0xbd0) + 4) == 10)) {
		InitializeIdleControllerTaskAction(p_state, p_index);
	}
}

// STUB: LEGORACERS2 0x0045c100
static void InitializeIdleControllerTaskAction(LegoU8* p_state, LegoU32 p_index)
{
	LegoU8* sourceRecord = p_state + p_index * 0x100;
	LegoU8* action = sourceRecord + 0x20;
	if (*(LegoU32*) action != 4) {
		memcpy(action, g_foyerSceneActionDefaults, 23 * sizeof(LegoU32));
		*(LegoU32*) (action + 0x14) = *(LegoU32*) (*(LegoU32*) (p_state + 0xbd0) + 0x34);
		*(LegoU32*) action = 4;
		*(LegoU32*) (action + 8) = 1;
		*(LegoU32*) (action + 0x54) = 0x3fc00000;
		*(LegoU32*) (action + 0x58) = 0x7f7fffff;
		RecomputeControllerTaskAction(action, NULL);
	}
}

// STUB: LEGORACERS2 0x0045c160
static void RefreshControllerTaskRecord(LegoU8* p_state, LegoU8* action)
{
	if (*(LegoU32*) action == 0) {
		LegoU32 target = *(LegoU32*) (action + 0x50);
		LegoU32 selected = target != 0 ? *(LegoU32*) (target + 0x7c) : 0;
		if (selected != 0) {
			*(LegoU32*) (action + 0x48) = selected;
			*(LegoU32*) action = 8;
			*(LegoU32*) (action + 8) = 1;
			return;
		}
		*(LegoU32*) action = 2;
		if (*(LegoU32*) (action + 8) == 0) {
			LegoU32 offset = *(LegoU32*) (p_state + 0xc18) * 3 + *(LegoU32*) (p_state + 0xc1c);
			*(LegoU32*) (action + 8) = *(LegoU32*) (0x543610 + offset * 4);
		}
	}
}

// FUNCTION: LEGORACERS2 0x0045c210
LegoU32 __fastcall GetControllerTaskActionStatus(void* p_manager, void*, LegoU32 p_index)
{
	LegoU8* state = *(LegoU8**) ((LegoU8*) p_manager + 0x58);
	return *(LegoU32*) (state + p_index * 0x100 + 4);
}

// STUB: LEGORACERS2 0x0045c330
static void* __fastcall InitializeControllerTaskSource(void* p_source, void*)
{
	*(const ControllerTaskSourceDispatch**) p_source = &g_controllerTaskSourceDispatch;
	return p_source;
}

static void __fastcall ControllerTaskSourceNoOp(void*, void*)
{
}

static LegoU32 __fastcall ControllerTaskSourceZero(GameObject*, void*)
{
}

// STUB: LEGORACERS2 0x0045c340
static void __fastcall DestroyControllerTaskSource(void* p_source, void*, LegoU32 p_free)
{
	*(const void**) p_source = (const void*) g_controllerTaskSourceRemovedDispatch;
	if ((p_free & 1) != 0) {
		FreeHeapMemory(p_source);
	}
}

// STUB: LEGORACERS2 0x0045c360
static LegoU32 __fastcall GetControllerTaskSourceCount(GameObject*, void*)
{
	return GetGameControllerSession()->m_unk0x00 == 0 ? 1 : 2;
}

// STUB: LEGORACERS2 0x0045c370
static GameObject* __fastcall GetControllerTaskSceneChild(GameObject*, void*, LegoU32 p_index)
{
	LegoU32* children = (LegoU32*) ((LegoU8*) &g_gameController + 0xe4);
	return (GameObject*) children[p_index];
}

// STUB: LEGORACERS2 0x0045c380
static LegoU32 __fastcall PrepareControllerTaskSource(
	GameObject*,
	void*,
	LegoFloat p_parameter,
	LegoU32,
	void* p_records
)
{
	GameRandomState* randomState = GetGameRandomState();
	GameFloatRandom* random = &randomState->m_floatStreams[1];
	GameControllerSession* session = GetGameControllerSession();
	LegoU32 frame = randomState->m_integerStreams[0].m_multiplier * random->m_frame + random->m_increment;
	LegoU32 randomBits;
	LegoFloat randomValue;
	LegoU32 count = session->m_unk0x18;
	LegoU32 i;
	random->m_frame = frame;
	randomBits = (random->m_mantissaMask & frame) | random->m_state;
	randomValue = (*(LegoFloat*) &randomBits - 1.0f) * 3.0f;
	for (i = 0; i < count; ++i) {
		GameObject* target = (GameObject*) session->m_unk0x28[i];
		LegoU8* task = (LegoU8*) p_records + 4 + i * 0x5c;
		LegoU32 type = GetControllerTaskTypeForTarget(target, NULL);
		memcpy(task, g_foyerSceneActionDefaults, 23 * sizeof(LegoU32));
		if (*(LegoU32*) &p_parameter == *(LegoU32*) ((LegoU8*) target + 0x58) && type < 8) {
			type = 8;
		}
		*(LegoU32*) task = type;
		*(LegoU32*) (task + 0x30) = *(LegoU32*) ((LegoU8*) target + 0x1e8);
		if (*(LegoU32*) (task + 0x30) != 0) {
			*(LegoU32*) (task + 0x30) += 0x58;
		}
		*(LegoU32*) (task + 0x54) = 0x40f00000;
		*(LegoFloat*) (task + 0x50) = type == 9 ? 1.5f : randomValue + 1.5f;
	}
	return count;
}

static void InitializeControllerTaskRecord(LegoU32* p_record)
{
	InitializeFoyerSceneActionDefaultRecord(p_record, NULL);
}

// STUB: LEGORACERS2 0x0045c6b0
static void InitializeControllerTaskInputRecord(LegoU8* p_record)
{
	LegoU32* words = (LegoU32*) p_record;
	words[1] = 0;
	words[2] = 0;
	InitializeControllerTaskRecord(words);
	*(LegoU32*) (p_record + 0x8c) = 0x3f800000;
	*(LegoU32*) (p_record + 0xdc) = 0;
	*(LegoU32*) (p_record + 0xe0) = 0;
	*(LegoU32*) (p_record + 0xe4) = 0x3f800000;
	*(LegoU32*) (p_record + 0xe8) = 0;
	*(LegoU32*) (p_record + 0xec) = 0;
	*(LegoU32*) (p_record + 0xf0) = 0x3f800000;
}
