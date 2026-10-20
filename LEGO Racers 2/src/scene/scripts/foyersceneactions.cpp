#include "scene/scripts/foyersceneactions.h"

#include "game/controller/gamecontroller.h"
#include "math/clipplanes.h"
#include "math/quaternion.h"
#include "math/vector3.h"
#include "physics/collision/collisionservice.h"
#include "physics/timing/simulationtiming.h"
#include "scene/effects/secondaryfxmanager.h"
#include "scene/scripts/foyerscene.h"
#include "scene/scripts/foyersceneactionsbase.h"

#include <math.h>
#include <string.h>

extern void __fastcall GameObjectBaseNoOp(GameObject*, void*);
extern void __fastcall GameObjectBaseNoOpOneArgument(GameObject*, void*, LegoU32);
extern void __fastcall ResetFoyerSceneSelection(GameObject*, void*);
extern void __fastcall InitializeFoyerSceneSecondaryFxTransition(void*, void*, LegoU32, LegoU32, LegoU32);
extern void __fastcall DrawFoyerSceneCircles(void*, void*, const Vector3*);
extern void __fastcall ApplyFoyerSceneActionParameters(
	FoyerSceneResource*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoFloat
);
extern GameObject* __cdecl GetControllerSceneObject();
extern void __fastcall SetFoyerSceneInputAnimationState(GameObject*, void*, LegoU32);
extern void __fastcall SetFoyerSceneAnimationMode(FoyerSceneResource*, void*, LegoU32);
extern void __fastcall SetNpcTextManagerFade(GameObject*, void*, LegoFloat);
extern void __fastcall EnterSecondaryManagerMode(GameObject*, void*, LegoU32);
void __fastcall SetSparkyMenuMode(FoyerSceneResource*, void*);
void __fastcall UpdateFoyerSceneTargetOscillation(void*, void*);

void __fastcall FoyerSceneActionScaleNoOp(void*, void*, LegoFloat)
{
}

void __fastcall UpdateAction8(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	ResetFoyerSceneSelection((GameObject*) words[3], NULL);
}

void __fastcall UpdateAction9(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	GameObject* manager = g_gameController.m_secondaryFxManager;
	ResetSecondaryFxController((GameObject*) *(void**) ((LegoU8*) manager + 0x58), NULL, words[4], 0);
}

void __fastcall UpdateAction10(void*, void*)
{
	GameObject* manager = (GameObject*) *(void**) ((LegoU8*) &g_gameController + 0xa4);
	InitializeFoyerSceneSecondaryFxTransition(*(void**) ((LegoU8*) manager + 0x68), NULL, 0x3f800000, 0x41200000, 0);
}

void __fastcall UpdateAction11(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	GameObject* manager = (GameObject*) *(void**) ((LegoU8*) &g_gameController + 0xa4);
	DrawFoyerSceneCircles(*(void**) ((LegoU8*) manager + 0x58), NULL, (Vector3*) (words[4] + 0x58));
}

void __fastcall UpdateAction12(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	LegoU32* target = (LegoU32*) words[4];
	target[0x64 / 4] = words[5];
	UpdateFoyerSceneTargetOscillation(target, NULL);
}

void __fastcall UpdateAction13(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	void* source = (void*) words[4];
	void* target = (void*) words[5];
	void** table = *(void***) ((LegoU8*) source + 0x58);
	((void(__fastcall*)(void*, void*, void*)) table[0x20 / 4])((LegoU8*) source + 0x58, NULL, (LegoU8*) target + 0x58);
	table = *(void***) ((LegoU8*) source + 0x58);
	((void(__fastcall*)(void*, void*, void*)) table[0x24 / 4])((LegoU8*) source + 0x58, NULL, (LegoU8*) target + 0x64);
	*(LegoU32*) ((LegoU8*) source + 8) |= 2;
	DetachCollisionProxy(*(CollisionProxy**) ((LegoU8*) source + 0x60));
}

void __fastcall UpdateAction14(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	ApplyFoyerSceneActionParameters(
		(FoyerSceneResource*) words[3],
		NULL,
		words[4],
		words[5],
		words[6],
		words[7],
		*(LegoFloat*) &words[8]
	);
}

void __fastcall UpdateAction15(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	SetFoyerSceneTargetAnimation((void*) words[4], NULL, words[5]);
}

void __fastcall UpdateAction17(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	GameObject* manager = g_gameController.m_npcTextManager;
	*(LegoU32*) ((LegoU8*) manager + 0x6c) = words[4];
}

void __fastcall UpdateAction18(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	LegoU32* flags = (LegoU32*) ((LegoU8*) words[4] + 8);
	LegoU32 value = words[5] != 0;
	if (((*flags >> 3) & 1) != value) {
		*flags = (*flags & ~8) | (value << 3);
		if (value == 0) {
			*flags |= 0x02000000;
		}
	}
	if (((*flags >> 2) & 1) != value) {
		*flags = (*flags & ~4) | (value << 2);
	}
}

void __fastcall UpdateAction19(void* p_action, void*)
{
	LegoU32* words = (LegoU32*) p_action;
	SetSparkyMenuMode((FoyerSceneResource*) words[3], NULL);
}

void InitializeActionHeader(
	void* p_action,
	const LegoU32* p_words,
	LegoU32 p_type,
	const FoyerSceneActionDispatch* p_dispatch
)
{
	LegoU32* words = (LegoU32*) p_action;
	words[0] = (LegoU32) p_dispatch;
	words[1] = p_type;
	words[2] = p_words[1];
	words[3] = p_words[0];
}

void __fastcall ScaleFoyerSceneAction14(void*, void*, LegoFloat);

#define ACTION_DISPATCH(name, update)                                                                                  \
	extern const FoyerSceneActionDispatch name = {                                                                     \
		update,                                                                                                        \
		(void(__fastcall*)(void*, void*)) GameObjectBaseNoOp,                                                          \
		(void(__fastcall*)(void*, void*)) GameObjectBaseNoOp,                                                          \
		FoyerSceneActionScaleNoOp                                                                                      \
	}

ACTION_DISPATCH(g_foyerSceneActionDispatch8, UpdateAction8);
ACTION_DISPATCH(g_foyerSceneActionDispatch9, UpdateAction9);
ACTION_DISPATCH(g_foyerSceneActionDispatch10, UpdateAction10);
ACTION_DISPATCH(g_foyerSceneActionDispatch11, UpdateAction11);
ACTION_DISPATCH(g_foyerSceneActionDispatch12, UpdateAction12);
ACTION_DISPATCH(g_foyerSceneActionDispatch13, UpdateAction13);
extern const FoyerSceneActionDispatch g_foyerSceneActionDispatch14 = {
	&UpdateAction14,
	(void(__fastcall*)(void*, void*)) GameObjectBaseNoOp,
	(void(__fastcall*)(void*, void*)) GameObjectBaseNoOp,
	&ScaleFoyerSceneAction14
};
ACTION_DISPATCH(g_foyerSceneActionDispatch15, UpdateAction15);
ACTION_DISPATCH(g_foyerSceneActionDispatch17, UpdateAction17);
ACTION_DISPATCH(g_foyerSceneActionDispatch18, UpdateAction18);
ACTION_DISPATCH(g_foyerSceneActionDispatch19, UpdateAction19);

#undef ACTION_DISPATCH

// STUB: LEGORACERS2 0x004800f0
void __fastcall SetSparkyMenuMode(FoyerSceneResource* p_resource, void*)
{
	EnterSecondaryManagerMode(g_gameController.m_secondaryManager, NULL, 0x2d);
	p_resource->m_unk0x58 = 4;
	SetFoyerSceneInputAnimationState(GetControllerSceneObject(), NULL, 0);
	*(LegoU32*) ((LegoU8*) p_resource + 0xa10) = 0;
	SetFoyerSceneAnimationMode(p_resource, NULL, 0x3f800000);
	SetNpcTextManagerFade(g_gameController.m_npcTextManager, NULL, 0.0f);
}

// STUB: LEGORACERS2 0x00480c10
void __fastcall InitializeFoyerSceneAction8(void* p_action, void*, const LegoU32* p_words)
{
	InitializeActionHeader(p_action, p_words, 8, &g_foyerSceneActionDispatch8);
}

// STUB: LEGORACERS2 0x00480c50
void __fastcall InitializeFoyerSceneAction9(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 9, &g_foyerSceneActionDispatch9);
	words[4] = p_words[2];
}

// STUB: LEGORACERS2 0x00480ca0
void __fastcall InitializeFoyerSceneAction10(void* p_action, void*, const LegoU32* p_words)
{
	InitializeActionHeader(p_action, p_words, 10, &g_foyerSceneActionDispatch10);
}

// STUB: LEGORACERS2 0x00480cf0
void __fastcall InitializeFoyerSceneAction12(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 12, &g_foyerSceneActionDispatch12);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x00480d40
void __fastcall InitializeFoyerSceneAction14(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 14, &g_foyerSceneActionDispatch14);
	memcpy(words + 4, p_words + 2, 5 * sizeof(LegoU32));
}

// FUNCTION: LEGORACERS2 0x00480db0
void __fastcall ScaleFoyerSceneAction14(void* p_action, void*, LegoFloat p_scale)
{
	*(LegoFloat*) ((LegoU8*) p_action + 0x20) /= p_scale;
}

// STUB: LEGORACERS2 0x00480dc0
void __fastcall InitializeFoyerSceneAction13(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 13, &g_foyerSceneActionDispatch13);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x00480e50
void __fastcall InitializeFoyerSceneAction17(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 17, &g_foyerSceneActionDispatch17);
	words[4] = p_words[2];
}

// STUB: LEGORACERS2 0x00480e90
void __fastcall InitializeFoyerSceneAction18(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 18, &g_foyerSceneActionDispatch18);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x00480f40
void __fastcall InitializeFoyerSceneAction11(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 11, &g_foyerSceneActionDispatch11);
	words[4] = p_words[2];
}

// STUB: LEGORACERS2 0x00480f90
void __fastcall InitializeFoyerSceneAction19(void* p_action, void*, const LegoU32* p_words)
{
	InitializeActionHeader(p_action, p_words, 19, &g_foyerSceneActionDispatch19);
}

// STUB: LEGORACERS2 0x00480fd0
void __fastcall InitializeFoyerSceneAction15(void* p_action, void*, const LegoU32* p_words)
{
	LegoU32* words = (LegoU32*) p_action;
	InitializeActionHeader(p_action, p_words, 15, &g_foyerSceneActionDispatch15);
	words[4] = p_words[2];
	words[5] = p_words[3];
}

// STUB: LEGORACERS2 0x004d7460
void __fastcall UpdateFoyerSceneTargetOscillation(void* p_target, void*)
{
	LegoU8* bytes = (LegoU8*) p_target;
	LegoFloat* direction = (LegoFloat*) (bytes + 0x58);
	LegoFloat phase =
		3.1415927410125732f * *(LegoFloat*) (bytes + 0x64) * 2.0f * 0.01666666753590107f * g_simulationStepSeconds;
	*(LegoFloat*) (bytes + 0x68) = phase;
	((Vector3*) direction)->Normalize();
	LegoFloat angle = phase * 0.5f;
	LegoFloat sine = sinf(angle);
	LegoFloat cosine = cosf(angle);
	LegoFloat* output = (LegoFloat*) (bytes + 0x6c);
	output[0] = direction[0] * sine;
	output[1] = direction[1] * sine;
	output[2] = direction[2] * sine;
	output[3] = cosine;
	((Quaternion*) output)->Normalize();
}
