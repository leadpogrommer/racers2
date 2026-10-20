#include "foyersceneactionsbase.h"

#include "cardisplay.h"
#include "charactervisual.h"
#include "controllerfoyertaskmanager.h"
#include "foyerscene.h"
#include "foyersceneaudiotarget.h"
#include "foyersceneevents.h"
#include "foyersceneparser.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "gamerandom.h"
#include "localizedstringtable.h"
#include "matrix3.h"
#include "memoryheap.h"
#include "npctextmanager.h"
#include "quaternion.h"
#include "spline.h"
#include "vector3.h"

#include <string.h>

extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch0;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch1;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch2;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch3;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch4;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch5;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch6;
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch7;
void __fastcall ResetFoyerSceneEventResetTarget(void*, void*);
void __fastcall BindFoyerSceneEventResetTarget(void*, void*, const void*, void*);
void __fastcall UpdateFoyerSceneEventResetTarget(void*, void*);

struct FoyerActionPath {
	SplinePath* m_path;
	LegoU8 m_unknown0x04[0x0c];
	LegoU32 m_initialized;
	LegoU8 m_unknown0x14[0x30 - 0x14];
};
DECOMP_SIZE_ASSERT(FoyerActionPath, 0x30);
struct FoyerActionFixedTarget {
	const CarDisplayTransformFunctionTable* m_dispatch;
	GameObject* m_owner;
	void* m_collisionProxy;
	LegoU32 m_flags;
	Quaternion m_orientation;
	Vector3 m_position;
	Matrix3 m_rotation;
	LegoFloat m_scale[3];
};
DECOMP_SIZE_ASSERT(FoyerActionFixedTarget, 0x5c);
static void* __fastcall CreateFoyerActionFixedTarget(void*, void*, GameObject*);
static FoyerActionPath* __fastcall CreateFoyerActionPath(void*, void*, SplinePath*);
void __fastcall BindFoyerActionPath(void*, void*, SplinePath*);
void __fastcall UpdateFoyerSceneTaskSourceAction(void*, void*);
void __fastcall PrepareFoyerSceneTaskSourceAction(void*, void*);
void __fastcall PrepareFoyerSceneSubtitleAction(void*, void*);
void __fastcall UpdateFoyerSceneSubtitleAction(void*, void*);
static LegoS32 __fastcall FindFoyerActionAtOrAfter(FoyerSceneResource*, void*, LegoFloat);
static void __fastcall ShiftFoyerActionTimes(FoyerSceneResource*, void*, LegoFloat, LegoFloat);
static void __fastcall ScaleFoyerActionTimes(FoyerSceneResource*, void*, LegoFloat, LegoFloat, LegoFloat);

// GLOBAL: LEGORACERS2 0x0056fa28
LegoU32 g_foyerSceneActionDefaults[23];

// FUNCTION: LEGORACERS2 0x0045c620
void __cdecl InitializeFoyerSceneActionDefaultsAtStartup()
{
	InitializeFoyerSceneActionDefaults();
}

// STUB: LEGORACERS2 0x0045c630
void __cdecl InitializeFoyerSceneActionDefaults()
{
	InitializeFoyerSceneActionDefaultRecord(g_foyerSceneActionDefaults, NULL);
}

// STUB: LEGORACERS2 0x0045c660
void __fastcall InitializeFoyerSceneActionDefaultRecord(LegoU32* p_words, void*)
{
	p_words[0] = 0;
	p_words[2] = 0;
	p_words[3] = 0;
	p_words[5] = 0;
	p_words[6] = 10;
	p_words[7] = 0;
	p_words[8] = 0;
	p_words[9] = 0;
	p_words[10] = 0;
	p_words[11] = 0;
	p_words[12] = 0;
	p_words[13] = 0;
	p_words[14] = 0x41800000;
	p_words[15] = 0;
	p_words[16] = 0;
	p_words[17] = 0;
	p_words[18] = 0;
	p_words[19] = 0;
	p_words[20] = 0;
}

// STUB: LEGORACERS2 0x0045dc40
void __fastcall BindFoyerActionPath(void* p_context, void*, SplinePath* p_path)
{
	*(SplinePath**) p_context = p_path;
	p_path->FUN_004d1a20();
}

// FUNCTION: LEGORACERS2 0x0047c870
void __fastcall SetFoyerSceneEventFaceState(void* p_event, void*, LegoU32 p_faceState, LegoFloat p_duration)
{
	FoyerSceneEventPrefix* event = (FoyerSceneEventPrefix*) p_event;
	GameFloatRandom* random;
	LegoU32 frame;
	LegoU32 randomBits;
	LegoFloat randomValue;

	event->m_faceState = p_faceState;
	event->m_faceDuration = p_duration;
	event->m_faceElapsed = 0.0f;
	event->m_unk0x08 = *(LegoU32*) ((LegoU8*) event->m_unk0x00 + 0x39c);
	if (p_faceState != 8 && p_faceState != 9) {
		((CharacterVisual*) event->m_unk0x08)->SetFaceVariant((LegoS32) p_faceState);
		return;
	}

	event->m_faceToggle = 0;
	random = &GetGameRandomState()->m_floatStreams[1];
	frame = random->m_multiplier * random->m_frame + random->m_increment;
	random->m_frame = frame;
	randomBits = (random->m_mantissaMask & frame) | random->m_state;
	randomValue = *(LegoFloat*) &randomBits;
	event->m_nextFaceTime = (randomValue - 1.0f) * 0.2f + 0.1f;
	((CharacterVisual*) event->m_unk0x08)->SetFaceVariant(0);
}

// STUB: LEGORACERS2 0x0047c900
void __fastcall ClearFoyerSceneEventAttachment(void* p_event, void*, const void* p_attachment)
{
	LegoU8* event = (LegoU8*) p_event;
	void* resetTarget = *(void**) (event + 0x7c);
	BindFoyerSceneEventResetTarget(resetTarget, NULL, p_attachment, *(void**) event);
	UpdateFoyerSceneEventResetTarget(resetTarget, NULL);
	*(LegoU32*) (event + 0x80) = 0;
}

// STUB: LEGORACERS2 0x0047c940
void __fastcall SetFoyerSceneEventPosition(void* p_event, void*, const LegoFloat* p_position)
{
	LegoU8* event = (LegoU8*) p_event;
	void* owner = *(void**) event;
	void* transform = (LegoU8*) owner + 0x58;
	void** dispatch = *(void***) transform;
	*(LegoU32*) (event + 0x80) = 0;
	ResetFoyerSceneEventResetTarget(p_event, NULL);
	((void(__fastcall*)(void*, void*, const LegoFloat*)) dispatch[0x20 / 4])(transform, NULL, p_position);
}

// STUB: LEGORACERS2 0x0047c970
void __fastcall SetFoyerSceneEventOrientation(void* p_event, void*, const LegoFloat* p_orientation)
{
	LegoU8* event = (LegoU8*) p_event;
	void* owner = *(void**) event;
	void* transform = (LegoU8*) owner + 0x58;
	void** dispatch = *(void***) transform;
	Matrix3 matrix;
	Vector3 direction;
	((void(__fastcall*)(void*, void*, const LegoFloat*)) dispatch[0x24 / 4])(transform, NULL, p_orientation);
	matrix.FUN_004a4d30(p_orientation);
	direction.SetValues(0.0f, 0.0f, 1.0f)->FUN_004a4e00(&matrix);
	(void) event;
	(void) direction;
}

// STUB: LEGORACERS2 0x0047c9c0
void __fastcall AttachFoyerSceneEventTarget(void* p_event, void*, void* p_target, const LegoFloat* p_offset)
{
	LegoU8* event = (LegoU8*) p_event;
	*(void**) (event + 0x84) = p_target;
	*(const LegoFloat**) (event + 0x88) = p_offset;
	*(LegoU32*) (event + 0x80) = 1;
	ResetFoyerSceneEventResetTarget(p_event, NULL);
}

// STUB: LEGORACERS2 0x0047fe40
void __fastcall InvokeFoyerSceneInputAction(void* p_resource, void*, LegoU32 p_index)
{
	LegoU8* resource = (LegoU8*) p_resource;
	void* target = *(void**) (resource + 0x118 + p_index * 0xa8);
	void** dispatch = *(void***) target;
	((void(__fastcall*)(void*, void*)) dispatch[0x34 / 4])(target, NULL);
}

static void InitializeFoyerSceneActionHeader(
	void* p_action,
	const LegoU32* p_words,
	LegoU32 p_type,
	const FoyerSceneActionDispatch* p_dispatch
)
{
	LegoU32* words = (LegoU32*) p_action;
	words[1] = p_type;
	words[2] = p_words[1];
	words[3] = p_words[0];
	words[0] = (LegoU32) p_dispatch;
}

struct FoyerSceneActionDefaultsStartup {
	FoyerSceneActionDefaultsStartup() { InitializeFoyerSceneActionDefaultsAtStartup(); }
};
static FoyerSceneActionDefaultsStartup g_foyerSceneActionDefaultsStartup;

// STUB: LEGORACERS2 0x00480210
static void __fastcall ShiftFoyerActionTimes(FoyerSceneResource* p_scene, void*, LegoFloat p_start, LegoFloat p_delta)
{
	LegoS32 index = FindFoyerActionAtOrAfter(p_scene, NULL, p_start);
	if (index != -1) {
		for (; index < (LegoS32) p_scene->m_unk0x9b8; ++index) {
			LegoFloat* time = (LegoFloat*) ((LegoU8*) p_scene->m_objects[index] + 8);
			*time = *time + p_delta;
		}
	}
	*(LegoFloat*) &p_scene->m_unk0x9c0 = p_delta + *(LegoFloat*) &p_scene->m_unk0x9c0;
}

// STUB: LEGORACERS2 0x00480270
static void __fastcall ScaleFoyerActionTimes(
	FoyerSceneResource* p_scene,
	void*,
	LegoFloat p_start,
	LegoFloat p_end,
	LegoFloat p_newEnd
)
{
	LegoS32 index = FindFoyerActionAtOrAfter(p_scene, NULL, p_start);
	if (index == -1) {
		return;
	}
	LegoFloat scale = (p_newEnd - p_start) / (p_end - p_start);
	do {
		void* action = p_scene->m_objects[index];
		LegoFloat* time = (LegoFloat*) ((LegoU8*) action + 8);
		if (*time >= p_end) {
			return;
		}
		*time = (*time - p_start) * scale + p_start;
		(*(const FoyerSceneActionDispatch**) action)->m_scale(action, NULL, scale);
		++index;
	} while (index < (LegoS32) p_scene->m_unk0x9b8);
}

// STUB: LEGORACERS2 0x00480300
static LegoS32 __fastcall FindFoyerActionAtOrAfter(FoyerSceneResource* p_scene, void*, LegoFloat p_start)
{
	for (LegoS32 index = 0; index < (LegoS32) p_scene->m_unk0x9b8; ++index) {
		if (*(LegoFloat*) ((LegoU8*) p_scene->m_objects[index] + 8) >= p_start) {
			return index;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x00480400
void __fastcall InitializeFoyerSceneAction0(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 0, &g_foyerSceneActionDispatch0);
	words[4] = p_words[2];
}

// STUB: LEGORACERS2 0x00480430
void __fastcall UpdateFoyerSceneInputAction(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	InvokeFoyerSceneInputAction((void*) words[3], NULL, words[4]);
}

// STUB: LEGORACERS2 0x00480480
void __fastcall InitializeFoyerSceneAction1(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 1, &g_foyerSceneActionDispatch1);
	memcpy(words + 4, g_foyerSceneActionDefaults, sizeof(g_foyerSceneActionDefaults));
	words[0] = (LegoU32) &g_foyerSceneActionDispatch1;
	words[4] = p_words[2];
	words[6] = p_words[3];
	words[7] = p_words[4];
	words[10] = p_words[8];
	words[11] = p_words[9];
	words[12] = p_words[10];
	words[13] = p_words[11];
	words[14] = p_words[12];
	words[15] = p_words[13];
	words[16] = p_words[14];
	words[19] = p_words[15];
	words[20] = p_words[16];
	words[21] = p_words[17];
	words[18] = p_words[18];
	words[26] = p_words[7];
	words[27] = p_words[5];
	words[28] = p_words[6];
	words[29] = 0;
	words[30] = 0;
}

// STUB: LEGORACERS2 0x00480540
void __fastcall UpdateFoyerSceneTaskSourceAction(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	void* manager = GetControllerTaskManager();
	FoyerSceneResource* scene = *(FoyerSceneResource**) (bytes + 0x0c);
	LegoU32 player = *(LegoU32*) (*(LegoU8**) ((LegoU8*) scene + 0x60) + 0x58);
	LegoU32 task = *(LegoU32*) ((LegoU8*) g_gameControllerSession + player * 0x344 + 0x5c);
	for (LegoU32 source = 0; source < 2; ++source) {
		LegoU32 index = *(LegoU32*) (bytes + 0x6c + source * 4);
		if (index < 12) {
			GameObject* object = *(GameObject**) ((LegoU8*) scene->m_eventStorage + index * 0x94);
			*(void**) (bytes + 0x24 + source * 0x20) = object == NULL ? NULL : (LegoU8*) object + 0x58;
		}
	}
	SetControllerTaskActionRecord(manager, NULL, task, (const LegoU32*) (bytes + 0x10));
}

// STUB: LEGORACERS2 0x004805d0
void __fastcall PrepareFoyerSceneTaskSourceAction(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	FoyerSceneResource* scene = *(FoyerSceneResource**) (bytes + 0x0c);
	LegoFloat* duration = (LegoFloat*) (bytes + 0x68);
	if (*duration == 0.0f || *duration != *duration) {
		*duration = FindNextFoyerActionTime(scene, NULL, *(LegoFloat*) (bytes + 8), 1) - *(LegoFloat*) (bytes + 8);
	}
	*(LegoFloat*) (bytes + 0x64) = *duration;
	LegoU32 type = *(LegoU32*) (bytes + 0x10);
	LegoU32 first = *(LegoU32*) (bytes + 0x6c);
	LegoU32 second = *(LegoU32*) (bytes + 0x70);
	switch (type) {
	case 1:
	case 2:
	case 3:
		*(LegoU32*) (bytes + 0x60) = first;
		break;
	case 4:
	case 5:
	case 6:
		if (first > 12) {
			*(void**) (bytes + 0x24) = ((GameObject*) first)->m_lifecycle0x54;
		}
		break;
	case 7:
	case 8:
	case 9:
		*(void**) (bytes + 0x58) = CreateFoyerActionPath(p_action, NULL, (SplinePath*) first);
		*(LegoU32*) (bytes + 0x74) = 1;
		*(GameObject**) (bytes + 0x60) = g_foyerSceneAudioTarget;
		break;
	}
	switch (type) {
	case 1:
	case 7:
		*(LegoU32*) (bytes + 0x10) = type == 1 ? 2 : 8;
		*(void**) (bytes + 0x44) = CreateFoyerActionFixedTarget(p_action, NULL, (GameObject*) second);
		*(LegoU32*) (bytes + 0x78) = 1;
		break;
	case 2:
	case 5:
	case 8:
		if (second > 12) {
			*(void**) (bytes + 0x44) = ((GameObject*) second)->m_lifecycle0x54;
		}
		break;
	case 3:
	case 6:
	case 9:
		*(void**) (bytes + 0x5c) = CreateFoyerActionPath(p_action, NULL, (SplinePath*) second);
		*(LegoU32*) (bytes + 0x78) = 1;
		break;
	}
}

// STUB: LEGORACERS2 0x004806f0
static void* __fastcall CreateFoyerActionFixedTarget(void*, void*, GameObject* p_owner)
{
	if (p_owner == NULL) {
		return NULL;
	}
	Vector3 position = *(Vector3*) ((LegoU8*) p_owner + 0x58);
	FoyerActionFixedTarget* target = (FoyerActionFixedTarget*) AllocateHeapMemory(0x5c);
	if (target == NULL) {
		return NULL;
	}
	target->m_owner = p_owner;
	target->m_collisionProxy = NULL;
	target->m_scale[0] = target->m_scale[1] = target->m_scale[2] = 0.0f;
	target->m_orientation.m_x = target->m_orientation.m_y = target->m_orientation.m_z = 0.0f;
	target->m_orientation.m_w = 1.0f;
	target->m_position = position;
	target->m_flags &= ~3u;
	target->m_dispatch = &g_carDisplayBaseTransformDispatch;
	target->m_orientation.Normalize();
	target->m_rotation.FUN_004a4d30((const LegoFloat*) &target->m_orientation);
	return target;
}

// STUB: LEGORACERS2 0x004807d0
static FoyerActionPath* __fastcall CreateFoyerActionPath(void*, void*, SplinePath* p_path)
{
	FoyerActionPath* context = (FoyerActionPath*) AllocateHeapMemory(0x30);
	if (context != NULL) {
		context->m_initialized = 0;
	}
	BindFoyerActionPath(context, NULL, p_path);
	return context;
}

// STUB: LEGORACERS2 0x00480800
void __fastcall ReleaseFoyerSceneAction1Resources(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	void* secondaryResource = *(void**) (bytes + 0x5c);
	void* audioSource = *(void**) (bytes + 0x44);
	void* primaryResource = *(void**) (bytes + 0x58);
	if (secondaryResource != NULL && *(LegoU32*) (bytes + 0x78) != 0) {
		FreeHeapMemory(secondaryResource);
	}
	if (audioSource != NULL && *(LegoU32*) (bytes + 0x78) != 0) {
		void** dispatch = *(void***) audioSource;
		((void(__fastcall*)(void*, void*, LegoU32)) dispatch[0x48 / 4])(audioSource, NULL, 1);
	}
	if (primaryResource != NULL && *(LegoU32*) (bytes + 0x74) != 0) {
		FreeHeapMemory(primaryResource);
	}
}

// FUNCTION: LEGORACERS2 0x00480860
void __fastcall ScaleFoyerSceneAction1(void* p_action, void*, LegoFloat p_scale)
{
	LegoU8* bytes = (LegoU8*) p_action;
	*(LegoFloat*) (bytes + 0x64) *= p_scale;
	*(LegoFloat*) (bytes + 0x68) *= p_scale;
}

// STUB: LEGORACERS2 0x00480880
void __fastcall InitializeFoyerSceneAction2(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 2, &g_foyerSceneActionDispatch2);
	memcpy(words + 5, p_words + 3, 6 * sizeof(LegoU32));
	words[11] = p_words[9];
}

// STUB: LEGORACERS2 0x004808d0
void __fastcall PrepareFoyerSceneSubtitleAction(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	FoyerSceneResource* scene = *(FoyerSceneResource**) (bytes + 0x0c);
	LegoFloat* duration = (LegoFloat*) (bytes + 0x2c);
	if (!(*duration >= 0.0f)) {
		*duration = FindNextFoyerActionTime(scene, NULL, *(LegoFloat*) (bytes + 8), 2) - *duration;
	}
	LegoFloat interval = GetNpcTextCharacterInterval(g_gameController.m_textBoxManager, NULL);
	const LegoU16* text;
	FindNpcTextStringByName(GetLocalizedStringTable(), NULL, (const LegoChar*) (bytes + 0x14), &text);
	LegoFloat characters = (LegoFloat) (LegoS32) CountNpcTextDisplayCharacters(text);
	if (!(*duration - 0.5f >= characters * interval)) {
		LegoFloat requiredDuration = characters * interval + 0.5f;
		ShiftFoyerActionTimes(scene, NULL, *duration + *(LegoFloat*) (bytes + 8), requiredDuration - *duration);
		ScaleFoyerActionTimes(
			scene,
			NULL,
			*(LegoFloat*) (bytes + 8),
			*(LegoFloat*) (bytes + 8) + *duration,
			requiredDuration + *(LegoFloat*) (bytes + 8)
		);
		*duration = requiredDuration;
	}
}

// STUB: LEGORACERS2 0x004809b0
void __fastcall UpdateFoyerSceneSubtitleAction(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	if (*(LegoS32*) (bytes + 0x10) == -1) {
		*(LegoU32*) (bytes + 0x10) = 0x3d;
	}
	SetNpcTextManagerTextByName(
		g_gameController.m_npcTextManager,
		NULL,
		*(LegoU32*) (bytes + 0x10),
		(const LegoChar*) (bytes + 0x14),
		*(LegoFloat*) (bytes + 0x2c),
		0
	);
}

// STUB: LEGORACERS2 0x004809e0
void __fastcall InitializeFoyerSceneAction3(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 3, &g_foyerSceneActionDispatch3);
	memcpy(words + 5, p_words + 3, 6 * sizeof(LegoU32));
	words[11] = p_words[9];
	words[12] = p_words[10];
}

// STUB: LEGORACERS2 0x00480a30
void __fastcall UpdateFoyerSceneAnimationAction(void* p_action, void*)
{
	LegoU8* bytes = (LegoU8*) p_action;
	LegoU32* words = (LegoU32*) p_action;
	SetFoyerSceneEventAnimation(
		(FoyerSceneEventPrefix*) words[4],
		NULL,
		(const LegoChar*) (bytes + 0x14),
		*(LegoFloat*) (bytes + 0x30),
		*(LegoFloat*) (bytes + 0x2c)
	);
}

// FUNCTION: LEGORACERS2 0x00480a50
void __fastcall ScaleFoyerSceneAction3(void* p_action, void*, LegoFloat p_scale)
{
	LegoFloat* values = (LegoFloat*) ((LegoU8*) p_action + 0x2c);
	values[0] *= p_scale;
	values[1] /= p_scale;
}

// STUB: LEGORACERS2 0x00480a70
void __fastcall InitializeFoyerSceneAction4(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 4, &g_foyerSceneActionDispatch4);
	memcpy(words + 4, p_words + 2, 3 * sizeof(LegoU32));
}

// STUB: LEGORACERS2 0x00480ab0
void __fastcall UpdateFoyerSceneFaceAction(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	SetFoyerSceneEventFaceState((void*) words[4], NULL, words[5], *(LegoFloat*) &words[6]);
}

// FUNCTION: LEGORACERS2 0x00480ad0
void __fastcall ScaleFoyerSceneAction4(void* p_action, void*, LegoFloat p_scale)
{
	*(LegoFloat*) ((LegoU8*) p_action + 0x18) *= p_scale;
}

// STUB: LEGORACERS2 0x00480ae0
void __fastcall InitializeFoyerSceneAction5(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 5, &g_foyerSceneActionDispatch5);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x00480b20
void __fastcall UpdateFoyerScenePositionAction(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	LegoU8* positionTarget = (LegoU8*) words[5];
	SetFoyerSceneEventPosition((void*) words[4], NULL, (const LegoFloat*) (positionTarget + 0x58));
	SetFoyerSceneEventOrientation((void*) words[4], NULL, (const LegoFloat*) (positionTarget + 0x64));
}

// STUB: LEGORACERS2 0x00480b50
void __fastcall InitializeFoyerSceneAction6(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 6, &g_foyerSceneActionDispatch6);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x00480b90
void __fastcall UpdateFoyerSceneAttachmentAction(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	ClearFoyerSceneEventAttachment((void*) words[4], NULL, (const void*) words[5]);
}

// STUB: LEGORACERS2 0x00480ba0
void __fastcall InitializeFoyerSceneAction7(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeFoyerSceneActionHeader(p_action, p_words, 7, &g_foyerSceneActionDispatch7);
	memcpy(words + 4, p_words + 2, 4 * sizeof(LegoU32));
}

// STUB: LEGORACERS2 0x00480bf0
void __fastcall UpdateFoyerSceneTargetOffsetAction(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	AttachFoyerSceneEventTarget(
		(void*) words[4],
		NULL,
		(void*) words[5],
		(const LegoFloat*) ((LegoU8*) p_action + 0x18)
	);
}

void __fastcall ScaleFoyerSceneActionNoOp(void*, void*, LegoFloat)
{
}

void __fastcall FoyerSceneActionNoOp(void*, void*)
{
}

// Retail callback order is update, state cleanup, removal, scale.
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch0 =
	{&UpdateFoyerSceneInputAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneActionNoOp};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch1 = {
	&UpdateFoyerSceneTaskSourceAction,
	&PrepareFoyerSceneTaskSourceAction,
	&ReleaseFoyerSceneAction1Resources,
	&ScaleFoyerSceneAction1
};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch2 = {
	&UpdateFoyerSceneSubtitleAction,
	&PrepareFoyerSceneSubtitleAction,
	&FoyerSceneActionNoOp,
	&ScaleFoyerSceneActionNoOp
};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch3 =
	{&UpdateFoyerSceneAnimationAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneAction3};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch4 =
	{&UpdateFoyerSceneFaceAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneAction4};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch5 =
	{&UpdateFoyerScenePositionAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneActionNoOp};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch6 =
	{&UpdateFoyerSceneAttachmentAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneActionNoOp};
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch7 =
	{&UpdateFoyerSceneTargetOffsetAction, &FoyerSceneActionNoOp, &FoyerSceneActionNoOp, &ScaleFoyerSceneActionNoOp};

// STUB: LEGORACERS2 0x004d64d0
void __fastcall BindFoyerSceneEventResetTarget(void* p_effect, void*, const void* p_attachment, void* p_owner)
{
	LegoU8* effect = (LegoU8*) p_effect;
	LegoU8* attachment = (LegoU8*) p_attachment;
	LegoU8* owner = (LegoU8*) p_owner;
	void* lifecycle = *(void**) (owner + 0x54);
	void** lifecycleDispatch = *(void***) lifecycle;
	LegoU32 active = ((LegoU32(__fastcall*)(void*, void*, LegoU32)) lifecycleDispatch[0])(lifecycle, NULL, 0);
	LegoU32* lifecycleFlags = (LegoU32*) ((LegoU8*) lifecycle + 0x0c);
	*(void**) (effect + 0x88) = (void*) p_attachment;
	*(void**) (effect + 0x8c) = p_owner;
	*lifecycleFlags = (*lifecycleFlags ^ (active != 0)) & 1 ^ *lifecycleFlags;
	*(LegoU32*) (effect + 0x90) = *(LegoU32*) (attachment + 0x68);
}

// STUB: LEGORACERS2 0x004d6530
void __fastcall UpdateFoyerSceneEventResetTarget(void* p_effect, void*)
{
	LegoU8* effect = (LegoU8*) p_effect;
	void* owner = *(void**) (effect + 0x8c);
	void* attachment = *(void**) (effect + 0x88);
	if (owner != NULL && attachment != NULL) {
		void* lifecycle = *(void**) ((LegoU8*) owner + 0x54);
		void** lifecycleDispatch = *(void***) lifecycle;
		LegoU32 active = ((LegoU32(__fastcall*)(void*, void*, LegoU32)) lifecycleDispatch[0])(lifecycle, NULL, 0);
		LegoU32* lifecycleFlags = (LegoU32*) ((LegoU8*) lifecycle + 0x0c);
		*(LegoU32*) (effect + 0x9c) &= ~0x20u;
		*(LegoFloat*) (effect + 0x98) = *(LegoFloat*) (effect + 0xb4) * *(LegoFloat*) (effect + 0x90) * 0.01f;
		*lifecycleFlags = (*lifecycleFlags ^ (active != 0)) & 1 ^ *lifecycleFlags;
	}
}
