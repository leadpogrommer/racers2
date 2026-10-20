#include "frontend/preview/cardisplay.h"

#include "core/strings/charbuffer128.h"
#include "frontend/preview/menucarpreviewtuning.h"
#include "game/character/characterpartpreset.h"
#include "game/objects/gamemanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectgeometry.h"
#include "game/objects/gameobjectmodelbinding.h"
#include "game/objects/gametemplateinstance.h"
#include "math/matrix3.h"
#include "math/quaternion.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "render/models/modelbounds.h"
#include "render/models/modelresourcecache.h"
#include "render/models/modelresourceloading.h"

#include <math.h>
#include <new>
#include <string.h>

// GLOBAL: LEGORACERS2 0x0056d998
LegoChar g_carDisplayModelPathStorage[14][128];
DECOMP_SIZE_ASSERT(g_carDisplayModelPathStorage, 0x700);

struct CarDisplayTemplateTransform {
	LegoFloat m_position[3];
	LegoFloat m_rotation[4];
	LegoFloat m_scale[3];
	CarDisplayTemplateTransform* Initialize(
		const LegoU32* p_position,
		const LegoU32* p_rotation,
		LegoU32 p_scaleX,
		LegoU32 p_scaleY,
		LegoU32 p_scaleZ
	);
};
DECOMP_SIZE_ASSERT(CarDisplayTemplateTransform, 0x28);

struct CarDisplayTemplateBase {
	LegoU32 m_words[13];
	CarDisplayTemplateBase* Initialize(LegoU32* p_flags, const LegoU32* p_first, const LegoU32* p_second);
};
DECOMP_SIZE_ASSERT(CarDisplayTemplateBase, 0x34);

struct PreviewVectorAndWord {
	LegoU32 m_words[4];
	PreviewVectorAndWord* Copy(const LegoU32* p_vector, LegoU32 p_word);
};
DECOMP_SIZE_ASSERT(PreviewVectorAndWord, 0x10);

struct PreviewTemplateFlags {
	LegoU32 m_value;
	PreviewTemplateFlags* Pack(
		LegoU32 p_arg2,
		LegoU32 p_arg3,
		LegoU32 p_arg4,
		LegoU32 p_arg5,
		LegoU32 p_arg6,
		LegoU32 p_arg7,
		LegoU32 p_arg8,
		LegoU32 p_arg9,
		LegoU32 p_arg10,
		LegoU32 p_arg11
	);
};
DECOMP_SIZE_ASSERT(PreviewTemplateFlags, 4);

struct CarDisplayPartTemplate : GameObjectTemplate {
	CarDisplayTemplateTransform m_transform;
	LegoU8 m_modelPath[0x80];
	CarDisplayPartTemplate* Assemble(
		const CarDisplayTemplateBase* p_base,
		const CarDisplayTemplateTransform* p_transform,
		LegoU32 p_word0,
		LegoU32 p_word1,
		LegoU32 p_word2,
		LegoU32 p_word3,
		LegoU32 p_word4,
		LegoU32 p_word5,
		LegoU32 p_word6,
		LegoU32 p_word7,
		LegoU32 p_word8,
		LegoU32 p_word9,
		LegoU32 p_word10,
		LegoU32 p_word11,
		LegoU32 p_word12,
		LegoU32 p_word13,
		LegoU32 p_word14,
		LegoU32 p_word15,
		LegoU32 p_word16,
		LegoU32 p_word17,
		LegoU32 p_word18,
		LegoU32 p_word19,
		LegoU32 p_word20,
		LegoU32 p_word21,
		LegoU32 p_word22,
		LegoU32 p_word23,
		LegoU32 p_word24,
		LegoU32 p_word25,
		LegoU32 p_word26,
		LegoU32 p_word27,
		LegoU32 p_word28,
		LegoU32 p_word29,
		LegoU32 p_word30,
		LegoU32 p_word31
	);
};
DECOMP_SIZE_ASSERT(CarDisplayPartTemplate, 0xdc);
DECOMP_STATIC_ASSERT(offsetof(CarDisplayPartTemplate, m_transform) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(CarDisplayPartTemplate, m_modelPath) == 0x5c);

struct CarDisplayTransformTarget {
	const GameTemplateInstanceTargetDispatch* m_dispatch;
	GameObject* m_owner;
	void* m_collisionProxy;
	LegoU32 m_stateFlags;
	LegoFloat m_rotation[4];
	LegoFloat m_position[3];
	LegoFloat m_rotationMatrix[9];
	LegoFloat m_scale[3];
};
DECOMP_SIZE_ASSERT(CarDisplayTransformTarget, 0x5c);

struct CarDisplayTransformFunctionTable {
	void(__cdecl* m_slots[20])();
};
DECOMP_SIZE_ASSERT(CarDisplayTransformFunctionTable, 0x50);

struct CarDisplayModelObject : GameObject {
	CarDisplayTransformTarget m_transform;
};
DECOMP_SIZE_ASSERT(CarDisplayModelObject, 0xb4);

struct CarDisplayNameObject : GameObject {
	CarDisplayTransformTarget m_transform;
	LegoU32 m_state; // 0xb4
	Vector3 m_unk0xb8;
	LegoU32 m_unk0xc4;
	LegoU32 m_unk0xc8;
	LegoU32 m_group;
	LegoU32 m_item;
	ModelResource* m_displayModel;  // 0xd4
	ModelResource* m_originalModel; // 0xd8
	LegoU32 m_unk0xdc;
	Vector3 m_basePosition;     // 0xe0
	LegoFloat m_targetLow;      // 0xec
	LegoFloat m_height;         // 0xf0
	LegoU32 m_paused;           // 0xf4
	LegoFloat m_velocity;       // 0xf8
	LegoS32 m_phase;            // 0xfc
	LegoFloat m_phaseRemaining; // 0x100
};
DECOMP_SIZE_ASSERT(CarDisplayNameObject, 0x104);
DECOMP_STATIC_ASSERT(offsetof(CarDisplayNameObject, m_basePosition) == 0xe0);
DECOMP_STATIC_ASSERT(offsetof(CarDisplayNameObject, m_phaseRemaining) == 0x100);

struct CarDisplayIndexedRecordOwner : GameObject {
	LegoU32 m_records[0x2c0];
};
DECOMP_SIZE_ASSERT(CarDisplayIndexedRecordOwner, 0xb58);

struct CarDisplayIndexedRecordOwnerBaseDispatch {
	void(__cdecl* m_slots[22])();
};
DECOMP_SIZE_ASSERT(CarDisplayIndexedRecordOwnerBaseDispatch, 0x58);

struct CarDisplayIndexedRecordOwnerDispatch {
	void(__cdecl* m_slots[23])();
};
DECOMP_SIZE_ASSERT(CarDisplayIndexedRecordOwnerDispatch, 0x5c);

// GLOBAL: LEGORACERS2 0x006843dc
CarDisplayIndexedRecordOwner* g_carDisplayIndexedRecordOwner;

static const Vector3* __fastcall CarDisplayLifecycleGetPosition(void*, void*);
static const LegoFloat* __fastcall CarDisplayLifecycleGetAuxiliaryVector(void*, void*);
static const LegoFloat* __fastcall CarDisplayLifecycleGetRotation(void*, void*);
static void __fastcall CarDisplayLifecycleSetPosition(void*, void*, const Vector3*);
static void __fastcall CarDisplayLifecycleSetRotation(void*, void*, const LegoFloat*);
void __fastcall CarDisplayLifecycleCopyTargetData(void*, void*, LegoU32*, LegoU32, LegoU32);
static const LegoU32* __fastcall GetCarDisplayIndexedRecord(CarDisplayIndexedRecordOwner*, void*, LegoU32);
GameObject* __fastcall ConstructCarDisplayIndexedRecordOwner(void*, void*, LegoU32, GameObjectTemplate*);
static GameObject* __fastcall CarDisplayIndexedRecordOwnerDestroy(GameObject*, void*, LegoU32);
static void __fastcall NormalizeCarDisplayIndexedRecords(GameObject*, void*);
static LegoU32 __fastcall GetCarDisplayIndexedRecordOwnerSize(GameObject*, void*);
static GameObject* __fastcall CarDisplayModelTargetDestroy(void*, void*, LegoU32);
static GameObject* __fastcall CarDisplayNameTargetDestroy(void*, void*, LegoU32);
static void __fastcall AssignCarDisplayModelResource(GameObject*, void*, LegoU32, LegoU32, void*);
static void* __fastcall FindCarDisplayModelResource(const LegoU8*, void*, LegoU32, LegoU32);
static GameObject* __fastcall CarDisplayNameDestroy(GameObject*, void*, LegoU32);
static void __fastcall CarDisplayNameInitialize(GameObject*, void*);
static LegoU32 __fastcall CarDisplayNameUpdate(GameObject*, void*, LegoU32);
static void __fastcall CarDisplayNameRemove(GameObject*, void*);
static void __fastcall CarDisplayNameDraw(GameObject*, void*);
static GameObject* __fastcall CarDisplayModelDestroy(GameObject*, void*, LegoU32);
static void __fastcall CarDisplayModelInitialize(GameObject*, void*);
void __fastcall CarDisplayModelRemove(GameObject*, void*);
static CarDisplayModelObject* __fastcall ConstructCarDisplayModelObject(
	CarDisplayModelObject*,
	void*,
	LegoU32,
	GameObjectTemplate*
);
static void __fastcall DestructCarDisplayModelObject(GameObject*, void*);
void* __fastcall DestroyCarDisplayBaseTransform(void*, void*, LegoU32);
namespace
{
void MarkCarDisplayObjectForRemoval(GameObject* p_object);
}

extern const GameObjectDispatchView g_carDisplayModelObjectDispatch;
extern const GameObjectDispatchView g_carDisplayNameObjectDispatch;
extern const CarDisplayTransformFunctionTable g_carDisplayTransformDispatch;
extern const CarDisplayTransformFunctionTable g_carDisplayNameTransformDispatch;
extern const CarDisplayTransformFunctionTable g_carDisplayBaseTransformDispatch;
void __fastcall ApplyLookupResourceState(void*, void*, void*);

extern "C" int __cdecl _purecall();

#define CAR_DISPLAY_CALLBACK(p_function) ((void(__cdecl*)())(p_function))

extern const CarDisplayIndexedRecordOwnerDispatch g_carDisplayIndexedRecordOwnerDispatch;
// Retail ABI function-pointer table: 0x00536d08 (verified in scene-dispatch.csv).
const CarDisplayIndexedRecordOwnerDispatch g_carDisplayIndexedRecordOwnerDispatch = {
	{CAR_DISPLAY_CALLBACK(&CarDisplayIndexedRecordOwnerDestroy),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseApplyChildTransform),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseInitialize),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseTrue),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseRemove),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseDrawChild),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseUpdateRegion),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GetCarDisplayIndexedRecord),
	 CAR_DISPLAY_CALLBACK(&NormalizeCarDisplayIndexedRecords),
	 CAR_DISPLAY_CALLBACK(&GetCarDisplayIndexedRecordOwnerSize)}
};

extern const CarDisplayIndexedRecordOwnerBaseDispatch g_carDisplayIndexedRecordOwnerBaseDispatch;
// Retail ABI function-pointer table: 0x00536d64 (verified in scene-dispatch.csv).
const CarDisplayIndexedRecordOwnerBaseDispatch g_carDisplayIndexedRecordOwnerBaseDispatch = {
	{CAR_DISPLAY_CALLBACK(&CarDisplayIndexedRecordOwnerDestroy),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseApplyChildTransform),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseInitialize),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseTrue),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseRemove),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseDrawChild),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseUpdateRegion),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&_purecall),
	 CAR_DISPLAY_CALLBACK(&_purecall)}
};

// Retail ABI function-pointer table: 0x0052f318 (verified in scene-dispatch.csv).
const CarDisplayTransformFunctionTable g_carDisplayNameTransformDispatch = {
	{CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseTrue),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetAuxiliaryVector),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetRotation),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetRotation),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroTwoArguments),
	 CAR_DISPLAY_CALLBACK(&CarDisplayNameTargetDestroy),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp)}
};

// Retail ABI function-pointer table: 0x0053529c (verified in scene-dispatch.csv).
const CarDisplayTransformFunctionTable g_carDisplayTransformDispatch = {
	{CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseTrue),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetAuxiliaryVector),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetRotation),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetRotation),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroTwoArguments),
	 CAR_DISPLAY_CALLBACK(&CarDisplayModelTargetDestroy),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp)}
};

// Retail ABI function-pointer table: 0x00531c3c (verified in scene-dispatch.csv).
const CarDisplayTransformFunctionTable g_carDisplayBaseTransformDispatch = {
	{CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseTrue),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetAuxiliaryVector),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleGetRotation),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetPosition),
	 CAR_DISPLAY_CALLBACK(&CarDisplayLifecycleSetRotation),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZero),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseZeroTwoArguments),
	 CAR_DISPLAY_CALLBACK(&DestroyCarDisplayBaseTransform),
	 CAR_DISPLAY_CALLBACK(&GameObjectBaseNoOp)}
};

// Retail ABI function-pointer table: 0x0052f368 (verified in scene-dispatch.csv).
const GameObjectDispatchView g_carDisplayNameObjectDispatch = {
	&CarDisplayNameDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&CarDisplayNameInitialize,
	&GameObjectBaseTrue,
	(GameObjectDispatchView::ValueArgument) &CarDisplayNameUpdate,
	&CarDisplayNameRemove,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&CarDisplayNameDraw,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument
};

// Retail ABI function-pointer table: 0x005352ec (verified in scene-dispatch.csv).
const GameObjectDispatchView g_carDisplayModelObjectDispatch = {
	&CarDisplayModelDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&CarDisplayModelInitialize,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&CarDisplayModelRemove,
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

#undef CAR_DISPLAY_CALLBACK

// FUNCTION: LEGORACERS2 0x004037b0
static void __fastcall AssignCarDisplayModelResource(
	GameObject* p_object,
	void*,
	LegoU32 p_recordGroup,
	LegoU32 p_recordItem,
	void* p_resource
)
{
	if (p_resource != NULL) {
		GameObjectModelBinding* binding = (GameObjectModelBinding*) p_object->m_unk0x48;
		*(void**) ((LegoU8*) binding + 8) = p_resource;
		*(void**) ((LegoU8*) p_object + 0xd4) = p_resource;
		*(LegoU32*) ((LegoU8*) p_object + 0xcc) = p_recordGroup;
		*(LegoU32*) ((LegoU8*) p_object + 0xd0) = p_recordItem;
	}
}

// STUB: LEGORACERS2 0x00403890
static CarDisplayNameObject* __fastcall ConstructCarDisplayNameObject(
	CarDisplayNameObject* p_object,
	void*,
	GameObjectTemplate* p_template
)
{
	ConstructCarDisplayModelObject((CarDisplayModelObject*) p_object, NULL, 0x900, p_template);
	p_object->m_vtable = &g_carDisplayNameObjectDispatch;
	p_object->m_transform.m_dispatch = (const GameTemplateInstanceTargetDispatch*) &g_carDisplayNameTransformDispatch;
	p_object->m_originalModel = NULL;
	return p_object;
}

// FUNCTION: LEGORACERS2 0x00403940
static const Vector3* __fastcall CarDisplayLifecycleGetPosition(void* p_target, void*)
{
	return (const Vector3*) ((LegoU8*) p_target + 0x20);
}

// FUNCTION: LEGORACERS2 0x00403950
static const LegoFloat* __fastcall CarDisplayLifecycleGetAuxiliaryVector(void* p_target, void*)
{
	return (const LegoFloat*) ((LegoU8*) p_target + 0x2c);
}

// FUNCTION: LEGORACERS2 0x00403960
static const LegoFloat* __fastcall CarDisplayLifecycleGetRotation(void* p_target, void*)
{
	return (const LegoFloat*) ((LegoU8*) p_target + 0x10);
}

// STUB: LEGORACERS2 0x00403980
static GameObject* __fastcall CarDisplayNameDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->m_vtable = &g_carDisplayNameObjectDispatch;
	((CarDisplayNameObject*) p_object)->m_transform.m_dispatch =
		(const GameTemplateInstanceTargetDispatch*) &g_carDisplayNameTransformDispatch;
	DestructCarDisplayModelObject(p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004039c0
static void __fastcall CarDisplayNameInitialize(GameObject* p_object, void*)
{
	CarDisplayModelInitialize(p_object, NULL);
	*(LegoU32*) ((LegoU8*) p_object + 0xb4) = 1;
	*(LegoU32*) ((LegoU8*) p_object + 0xc8) = 0;
	*(LegoU32*) ((LegoU8*) p_object + 0xdc) = 0;
	memset((LegoU8*) p_object + 0xb8, 0, 0x0c);
	void* resource = *(void**) ((LegoU8*) p_object->m_unk0x48 + 8);
	*(void**) ((LegoU8*) p_object + 0xd4) = resource;
	*(void**) ((LegoU8*) p_object + 0xd8) = resource;
	// Retail 0x4039c0 obtains the initial resource through 0x4037e0, stores it
	// at +0xd4/+0xd8 and then enters the ordinary removal-state transition.
	MarkCarDisplayObjectForRemoval(p_object);
}

// STUB: LEGORACERS2 0x00403a30
static void __fastcall CarDisplayNameRemove(GameObject* p_object, void*)
{
	CarDisplayNameObject* name = (CarDisplayNameObject*) p_object;
	if (name->m_originalModel != NULL) {
		((GameObjectModelBinding*) name->m_unk0x48)->m_model = name->m_originalModel;
	}
	CarDisplayModelRemove(p_object, NULL);
}

// STUB: LEGORACERS2 0x00403a50
static LegoU32 __fastcall CarDisplayNameUpdate(GameObject* p_object, void*, LegoU32 p_delta)
{
	CarDisplayNameObject* name = (CarDisplayNameObject*) p_object;
	if ((name->m_state & 0x80) == 0) {
		if ((name->m_flags & 8) != 0) {
			name->m_flags = (name->m_flags & ~8u) | 0x02000000;
		}
	}
	else if ((name->m_flags & 8) == 0) {
		name->m_flags |= 8;
	}
	if ((name->m_state & 0x100) != 0) {
		if (name->m_paused == 0 || fabs(name->m_height - name->m_targetLow) >= 2.0f) {
			LegoFloat step;
			memcpy(&step, &p_delta, sizeof(step));
			if (name->m_phaseRemaining < step) {
				step = name->m_phaseRemaining;
			}
			name->m_phaseRemaining -= step;
			LegoS32 phase = name->m_phase;
			if (phase != 0) {
				name->m_height += step * name->m_velocity;
				LegoFloat target = phase == 1 ? name->m_basePosition.m_y : name->m_targetLow;
				if ((target - name->m_height) * name->m_velocity < 0.0f) {
					name->m_height = target;
				}
			}
			if (name->m_phaseRemaining < 0.01f) {
				name->m_phase = phase - 1;
				if (name->m_phase < -1) {
					name->m_phase = 1;
				}
				name->m_phaseRemaining = 0.3f;
				if (name->m_phase != 0 && name->m_paused == 0) {
					LegoFloat target = name->m_phase == 1 ? name->m_basePosition.m_y : name->m_targetLow;
					name->m_velocity = (1.0f / 0.3f) * (target - name->m_height);
				}
			}
		}
		else {
			name->m_phase = 1;
			name->m_phaseRemaining = 0.3f;
		}
	}
	GameObjectModelBinding* binding = (GameObjectModelBinding*) name->m_unk0x48;
	if (binding->m_model != name->m_displayModel) {
		binding->m_model = name->m_displayModel;
	}
	return 1;
}

// STUB: LEGORACERS2 0x00403bf0
static void __fastcall CarDisplayNameDraw(GameObject* p_object, void*)
{
	GameObjectModelBinding* binding = (GameObjectModelBinding*) p_object->m_unk0x48;
	if (binding == NULL || !p_object->IsVisible()) {
		return;
	}
	CarDisplayNameObject* name = (CarDisplayNameObject*) p_object;
	if ((name->m_state & 0x100) != 0) {
		ModelResource* model = (ModelResource*) binding->m_model;
		for (LegoS32 groupIndex = 0; groupIndex < model->m_groupCount; ++groupIndex) {
			ModelTextureGroup* group = &model->m_groups[groupIndex];
			for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
				group->m_primitives[primitive].m_flags |= 6;
			}
		}
		binding->m_unk0xC0 = 0;
		((GameObjectChildGeometry*) binding)->SetPosition(&name->m_basePosition.m_x);
		ApplyLookupResourceState(model, NULL, binding);
		Vector3 position = name->m_basePosition;
		position.m_y = name->m_height;
		for (LegoS32 nextGroup = 0; nextGroup < model->m_groupCount; ++nextGroup) {
			ModelTextureGroup* group = &model->m_groups[nextGroup];
			for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
				group->m_primitives[primitive].m_flags &= ~6u;
			}
		}
		binding->m_unk0xC0 = 0x3f000000;
		((GameObjectChildGeometry*) binding)->SetPosition(&position.m_x);
	}
	else {
		binding->m_unk0xC0 = 0;
	}
	binding->m_vtable->m_update(binding, NULL);
}

namespace
{
void UpdateCarDisplayRemovalState(GameObject* p_object);

// STUB: LEGORACERS2 0x00403d10
void PrepareCarDisplayObject(GameObject* p_object)
{
	GameObjectModelBinding* binding = *(GameObjectModelBinding**) ((LegoU8*) p_object + 0x48);
	if (binding != NULL) {
		binding->m_unk0xDC = 0;
		binding->m_flags |= 0x30;
	}
	if ((p_object->m_flags & 8) != 0) {
		p_object->m_flags = (p_object->m_flags & ~8) | 0x02000000;
	}
	p_object->m_flags &= ~4;
}

// STUB: LEGORACERS2 0x00403d50
void MarkCarDisplayObjectForRemoval(GameObject* p_object)
{
	LegoU32* state = (LegoU32*) ((LegoU8*) p_object + 0xb4);
	*state &= ~0x10;
	LegoU32* flags = &p_object->m_flags;
	if ((*flags & 8) == 0) {
		*flags |= 8;
	}
	if ((*flags & 4) == 0) {
		*flags |= 4;
	}
	UpdateCarDisplayRemovalState(p_object);
}

// STUB: LEGORACERS2 0x00403db0
void UpdateCarDisplayRemovalState(GameObject* p_object)
{
	LegoU32* modelFlags = (LegoU32*) ((LegoU8*) p_object + 0xb4);
	LegoU32 previous = *modelFlags;
	*modelFlags &= ~0x20;
	if ((previous & 0x10) == 0) {
		p_object->m_flags |= 8;
	}
	else if ((p_object->m_flags & 8) != 0) {
		p_object->m_flags = (p_object->m_flags & ~8u) | 0x02000000;
	}
}
} // namespace

// STUB: LEGORACERS2 0x00403fa0
static GameObject* __fastcall CarDisplayNameTargetDestroy(void* p_target, void*, LegoU32 p_flags)
{
	GameObject* owner = (GameObject*) ((LegoU8*) p_target - 0x58);
	return CarDisplayNameDestroy(owner, NULL, p_flags);
}

// STUB: LEGORACERS2 0x00403fd0
GameObject* CreateCarDisplayNameObject()
{
	CarDisplayPartTemplate templateData;
	memset(&templateData, 0, sizeof(templateData));
	templateData.m_flags = 1;
	templateData.m_first = CharBuffer24("NoName");
	templateData.m_second = CharBuffer24("");
	CharBuffer128 path("game data\\Arctic\\Objects\\Models\\cwheel.md2");
	memcpy(templateData.m_modelPath, path.m_data, sizeof(templateData.m_modelPath));
	static const LegoFloat position[3] = {0.0f, 0.0f, 0.0f};
	static const LegoFloat rotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};
	memcpy(templateData.m_transform.m_position, position, sizeof(position));
	memcpy(templateData.m_transform.m_rotation, rotation, sizeof(rotation));
	templateData.m_transform.m_scale[0] = 0.5f;
	templateData.m_transform.m_scale[1] = 0.5f;
	templateData.m_transform.m_scale[2] = 0.0f;
	void* storage = AllocateHeapMemory(0x104);
	if (storage == NULL) {
		return NULL;
	}
	CarDisplayNameObject* object = ConstructCarDisplayNameObject((CarDisplayNameObject*) storage, NULL, &templateData);
	object->m_vtable->m_initialize(object, NULL);
	return object;
}

static GameObject* CreateCarDisplayPartObject(GameObjectTemplate* p_template)
{
	void* storage = AllocateHeapMemory(sizeof(CarDisplayModelObject));
	if (storage == NULL) {
		return NULL;
	}
	CarDisplayModelObject* object =
		ConstructCarDisplayModelObject((CarDisplayModelObject*) storage, NULL, 0x900, p_template);
	object->m_vtable->m_initialize(object, NULL);
	return object;
}

// STUB: LEGORACERS2 0x00404120
PreviewVectorAndWord* PreviewVectorAndWord::Copy(const LegoU32* p_vector, LegoU32 p_word)
{
	m_words[0] = p_vector[0];
	m_words[1] = p_vector[1];
	m_words[2] = p_vector[2];
	m_words[3] = p_word;
	return this;
}

// STUB: LEGORACERS2 0x00404150
PreviewTemplateFlags* PreviewTemplateFlags::Pack(
	LegoU32 p_arg2,
	LegoU32 p_arg3,
	LegoU32 p_arg4,
	LegoU32 p_arg5,
	LegoU32 p_arg6,
	LegoU32 p_arg7,
	LegoU32 p_arg8,
	LegoU32 p_arg9,
	LegoU32 p_arg10,
	LegoU32 p_arg11
)
{
	m_value = p_arg11 & 1;
	m_value = (m_value << 2) | (p_arg10 & 1);
	m_value = (m_value << 1) | (p_arg9 & 1);
	m_value = (m_value << 9) | (p_arg8 & 0xff);
	m_value = (m_value << 8) | (p_arg6 & 0xff);
	m_value = (m_value << 1) | (p_arg7 & 1);
	m_value = (m_value << 1) | (p_arg3 & 1);
	m_value = (m_value << 1) | (p_arg4 & 1);
	m_value = (m_value << 1) | (p_arg2 & 1);
	m_value = (m_value << 1) | (p_arg5 & 1);
	return this;
}

// STUB: LEGORACERS2 0x004041e0
CarDisplayTemplateBase* CarDisplayTemplateBase::Initialize(
	LegoU32* p_flags,
	const LegoU32* p_first,
	const LegoU32* p_second
)
{
	m_words[0] = *p_flags;
	for (LegoS32 i = 0; i < 6; ++i) {
		m_words[i + 1] = p_first[i];
		m_words[i + 7] = p_second[i];
	}
	return this;
}

// STUB: LEGORACERS2 0x00404210
CarDisplayTemplateTransform* CarDisplayTemplateTransform::Initialize(
	const LegoU32* p_position,
	const LegoU32* p_rotation,
	LegoU32 p_scaleX,
	LegoU32 p_scaleY,
	LegoU32 p_scaleZ
)
{
	LegoU32* output = (LegoU32*) this;
	for (LegoS32 positionIndex = 0; positionIndex < 3; ++positionIndex) {
		output[positionIndex] = p_position[positionIndex];
	}
	for (LegoS32 rotationIndex = 0; rotationIndex < 4; ++rotationIndex) {
		output[rotationIndex + 3] = p_rotation[rotationIndex];
	}
	output[7] = p_scaleX;
	output[8] = p_scaleY;
	output[9] = p_scaleZ;
	return this;
}

// STUB: LEGORACERS2 0x00404260
CarDisplayPartTemplate* CarDisplayPartTemplate::Assemble(
	const CarDisplayTemplateBase* p_base,
	const CarDisplayTemplateTransform* p_transform,
	LegoU32 p_word0,
	LegoU32 p_word1,
	LegoU32 p_word2,
	LegoU32 p_word3,
	LegoU32 p_word4,
	LegoU32 p_word5,
	LegoU32 p_word6,
	LegoU32 p_word7,
	LegoU32 p_word8,
	LegoU32 p_word9,
	LegoU32 p_word10,
	LegoU32 p_word11,
	LegoU32 p_word12,
	LegoU32 p_word13,
	LegoU32 p_word14,
	LegoU32 p_word15,
	LegoU32 p_word16,
	LegoU32 p_word17,
	LegoU32 p_word18,
	LegoU32 p_word19,
	LegoU32 p_word20,
	LegoU32 p_word21,
	LegoU32 p_word22,
	LegoU32 p_word23,
	LegoU32 p_word24,
	LegoU32 p_word25,
	LegoU32 p_word26,
	LegoU32 p_word27,
	LegoU32 p_word28,
	LegoU32 p_word29,
	LegoU32 p_word30,
	LegoU32 p_word31
)
{
	LegoU32* output = (LegoU32*) this;
	const LegoU32* base = (const LegoU32*) p_base;
	const LegoU32* transform = (const LegoU32*) p_transform;
	for (LegoS32 baseIndex = 0; baseIndex < 13; ++baseIndex) {
		output[baseIndex] = base[baseIndex];
	}
	for (LegoS32 transformIndex = 0; transformIndex < 10; ++transformIndex) {
		output[transformIndex + 13] = transform[transformIndex];
	}
	const LegoU32 modelPath[32] = {p_word0,  p_word1,  p_word2,  p_word3,  p_word4,  p_word5,  p_word6,  p_word7,
								   p_word8,  p_word9,  p_word10, p_word11, p_word12, p_word13, p_word14, p_word15,
								   p_word16, p_word17, p_word18, p_word19, p_word20, p_word21, p_word22, p_word23,
								   p_word24, p_word25, p_word26, p_word27, p_word28, p_word29, p_word30, p_word31};
	for (LegoS32 pathIndex = 0; pathIndex < 32; ++pathIndex) {
		output[pathIndex + 23] = modelPath[pathIndex];
	}
	return this;
}

// FUNCTION: LEGORACERS2 0x00404fd0
static void* __fastcall FindCarDisplayModelResource(
	const LegoU8* p_resourceTable,
	void*,
	LegoU32 p_group,
	LegoU32 p_item
)
{
	return *(void**) (p_resourceTable + p_group * 0x13c + p_item * 0x28 + 0x288);
}

// FUNCTION: LEGORACERS2 0x004391c0
static GameObject* __fastcall CarDisplayModelDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	DestructCarDisplayModelObject(p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00440720
void InitializeCarDisplayModelPaths()
{
	CharBuffer128* paths = (CharBuffer128*) g_carDisplayModelPathStorage;
	paths[0].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
	paths[1].SetText("Game Data\\objects\\bricks\\adventures\\wheel jeep.MD2");
	paths[2].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\chill_wheel.MD2");
	paths[3].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\freeze_wheel.MD2");
	paths[4].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\lightor_wheel.MD2");
	paths[5].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\warrior_wheel.MD2");
	paths[6].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\duster_wheel.MD2");
	paths[7].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\ghost_wheel.MD2");
	paths[8].SetText("Game Data\\objects\\bricks\\car_craze\\wheels\\snake_wheel.MD2");
	paths[9].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
	paths[10].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
	paths[11].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
	paths[12].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
	paths[13].SetText("Game Data\\Arctic\\Cars\\Models\\buggy_wheel.MD2");
}

namespace
{
struct CarDisplayModelPathStartup {
	CarDisplayModelPathStartup() { InitializeCarDisplayModelPaths(); }
};

static CarDisplayModelPathStartup g_carDisplayModelPathStartup;
} // namespace

namespace
{
struct CarDisplayPartDispatch {
	void(__fastcall* m_destroy)(GameObject*, void*, LegoU32);
	undefined m_unk0x04[0x0c];
	void(__fastcall* m_initialize)(GameObject*, void*);
	undefined m_unk0x14[0x08];
	void(__fastcall* m_remove)(GameObject*, void*);
};

void DeleteCarDisplayObject(GameObject* p_object)
{
	CarDisplayPartDispatch* dispatch = (CarDisplayPartDispatch*) p_object->m_vtable;
	dispatch->m_remove(p_object, NULL);
	if (p_object != NULL) {
		dispatch->m_destroy(p_object, NULL, 1);
	}
}

} // namespace

// STUB: LEGORACERS2 0x00451600
void* __fastcall DestroyCarDisplayBaseTransform(void* p_target, void*, LegoU32 p_flags)
{
	((CarDisplayTransformTarget*) p_target)->m_dispatch =
		(const GameTemplateInstanceTargetDispatch*) &g_carDisplayBaseTransformDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_target);
	}
	return p_target;
}

// STUB: LEGORACERS2 0x0046a2e0
void InitializeCarDisplay(CarDisplay* p_display)
{
	static const LegoFloat zeroPosition[3] = {0.0f, 0.0f, 0.0f};
	static const LegoFloat identityRotation[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	p_display->m_nameObjects =
		(void**) GetMemoryHeap()->Allocate(0x90, "lego brick array in Car Display in car builder");
	for (LegoS32 i = 0; i < 36; ++i) {
		// 0x403fd0 builds and initializes one menu name object from the retail
		// name template. Its allocator and object constructor are kept in the
		// adjacent helper source so this loop preserves the retail 36-entry walk.
		p_display->m_nameObjects[i] = CreateCarDisplayNameObject();
	}

	CarDisplayTemplateTransform transform;
	transform.Initialize((const LegoU32*) zeroPosition, (const LegoU32*) identityRotation, 0, 0, 0);
	CarDisplayTemplateBase base;
	LegoU32 flags = 0x0e;
	CharBuffer24 first("NoName");
	CharBuffer24 second("");
	base.Initialize(&flags, (const LegoU32*) &first, (const LegoU32*) &second);
	for (LegoS32 group = 0; group < 9; ++group) {
		for (LegoS32 item = 0; item < 4; ++item) {
			// Retail clones the same 0xdc template, changes only its path, then
			// allocates the 0xb4 model object and invokes its +0x10 initializer.
			CarDisplayPartTemplate partTemplate;
			memset(&partTemplate, 0, sizeof(partTemplate));
			LegoU32* templateWords = (LegoU32*) &partTemplate;
			const LegoU32* baseWords = (const LegoU32*) &base;
			const LegoU32* transformWords = (const LegoU32*) &transform;
			for (LegoS32 baseIndex = 0; baseIndex < 13; ++baseIndex) {
				templateWords[baseIndex] = baseWords[baseIndex];
			}
			for (LegoS32 transformIndex = 0; transformIndex < 10; ++transformIndex) {
				templateWords[transformIndex + 13] = transformWords[transformIndex];
			}
			CharBuffer128* paths = (CharBuffer128*) g_carDisplayModelPathStorage;
			// The first nine source rows are the retail group model paths. The
			// model-object constructor consumes the template path at +0x5c.
			memcpy(partTemplate.m_modelPath, paths[group].m_data, sizeof(partTemplate.m_modelPath));
			GameObject* object = CreateCarDisplayPartObject(&partTemplate);
			p_display->m_parts[group * 4 + item] = object;
			object->m_flags |= 0x400000;
		}
	}
}

// Retail 0x46a500 applies one selected car record to its name and wheel objects.
// STUB: LEGORACERS2 0x0046a4a0
void ReleaseCarDisplay(CarDisplay* p_display)
{
	GameObject** part = (GameObject**) &p_display->m_parts[35];
	for (LegoS32 group = 0; group < 9; ++group) {
		for (LegoS32 item = 0; item < 4; ++item) {
			DeleteCarDisplayObject(*part);
			--part;
		}
	}

	void** name = (void**) ((LegoU8*) p_display->m_nameObjects + 0x8c);
	for (LegoS32 i = 0; i < 36; ++i) {
		DeleteCarDisplayObject((GameObject*) *name);
		--name;
	}

	FreeHeapMemory(p_display->m_nameObjects);
}

// STUB: LEGORACERS2 0x0046a500
void __fastcall UpdateCarDisplayChoice(
	CarDisplay* p_display,
	void*,
	const LegoU32* p_choiceRecord,
	const LegoFloat* p_vehicleData,
	const LegoFloat* p_rotation,
	LegoU32 p_mode
)
{
	LegoFloat offset[3];
	LegoFloat base[3];
	const LegoFloat* choiceFloats = (const LegoFloat*) p_choiceRecord;
	if (p_mode == 0) {
		const LegoU32 index = p_choiceRecord[0] * 5;
		const LegoFloat* source =
			(const LegoFloat*) ((const LegoU8*) p_display->m_resourceTable + p_choiceRecord[index + 8] * 0x28 +
								p_choiceRecord[index + 7] * 0x13c + 0x2a4);
		base[0] = source[0];
		base[1] = source[1];
		base[2] = source[2];
		offset[0] = base[0] + p_vehicleData[0];
		offset[1] = base[1] + p_vehicleData[1];
		offset[2] = base[2] + p_vehicleData[2];
	}
	else {
		base[0] = choiceFloats[3];
		base[1] = choiceFloats[4];
		base[2] = (choiceFloats[5] + choiceFloats[8]) * 0.5f;
		offset[0] = p_vehicleData[0] - (choiceFloats[3] + choiceFloats[6]) * 0.5f;
		offset[1] = p_vehicleData[1] - choiceFloats[4];
		offset[2] = p_vehicleData[2] - base[2];
	}

	const LegoS32 count = (LegoS32) p_choiceRecord[0];
	p_display->m_activeCount = count;
	const LegoFloat* record = (const LegoFloat*) (p_choiceRecord + 9);
	const LegoU32* recordWords = p_choiceRecord + 9;
	for (LegoS32 nameIndex = 0; nameIndex < count; ++nameIndex, record += 5, recordWords += 5) {
		void* resource = FindCarDisplayModelResource(
			(const LegoU8*) p_display->m_resourceTable,
			NULL,
			recordWords[3],
			recordWords[4]
		);
		GameObject* name = (GameObject*) p_display->m_nameObjects[nameIndex];
		AssignCarDisplayModelResource(name, NULL, recordWords[3], recordWords[4], resource);
		Matrix3 rotation;
		rotation.FUN_004a4d30(p_rotation);
		Vector3 position = {record[0], record[1], record[2]};
		position.FUN_004a4e00(&rotation);
		position.m_x += offset[0];
		position.m_y += offset[1];
		position.m_z += offset[2];
		CarDisplayTransformTarget* target = &((CarDisplayNameObject*) name)->m_transform;
		const GameTemplateInstanceTargetDispatch* dispatch = target->m_dispatch;
		dispatch->m_setPosition(target, NULL, &position);
		dispatch->m_setOrientation(target, NULL, p_rotation);
		PrepareCarDisplayObject(name);
	}

	LegoS32 tuningIndex = (LegoS32) p_choiceRecord[2];
	if (p_choiceRecord[1] < 3) {
		tuningIndex = 0;
	}
	for (LegoS32 partIndex = 0; partIndex < 36; ++partIndex) {
		((GameObject*) p_display->m_parts[partIndex])->m_flags |= 8;
	}
	const LegoS32 groupCount = (LegoS32) g_characterPartPresets[tuningIndex].m_kind;
	if (groupCount < 9) {
		LegoFloat wheelRotation[4];
		wheelRotation[0] = p_rotation[2];
		wheelRotation[1] = p_rotation[3];
		wheelRotation[2] = -p_rotation[0];
		wheelRotation[3] = -p_rotation[1];
		Matrix3 rotation;
		rotation.FUN_004a4d30(p_rotation);
		for (LegoS32 corner = 0; corner < 4; ++corner) {
			const CharacterPartPreset* preset = &g_characterPartPresets[tuningIndex];
			LegoFloat sideX;
			LegoFloat sideZ;
			if (groupCount >= 2 && groupCount <= 8) {
				sideX = corner == 0 || corner == 2 ? preset->m_first.m_x + preset->m_second.m_x
												   : preset->m_first.m_x - preset->m_second.m_x;
				sideZ = corner < 2 ? preset->m_first.m_z + preset->m_second.m_z
								   : preset->m_first.m_z - preset->m_second.m_z;
			}
			else if (corner == 0) {
				sideX = preset->m_first.m_x - preset->m_second.m_x;
				sideZ = preset->m_first.m_z + preset->m_second.m_z;
			}
			else if (corner == 1) {
				sideX = preset->m_first.m_x + preset->m_second.m_x;
				sideZ = preset->m_first.m_z + preset->m_second.m_z;
			}
			else if (corner == 3) {
				sideX = preset->m_first.m_x + preset->m_second.m_x;
				sideZ = preset->m_first.m_z - preset->m_second.m_z;
			}
			else {
				sideX = preset->m_first.m_x - preset->m_second.m_x;
				sideZ = preset->m_first.m_z - preset->m_second.m_z;
			}
			Vector3 position = {sideX, preset->m_first.m_y, sideZ};
			position.FUN_004a4e00(&rotation);
			position.m_x += offset[0];
			position.m_y += offset[1];
			position.m_z += offset[2];
			GameObject* part = (GameObject*) p_display->m_parts[groupCount * 4 + corner];
			CarDisplayTransformTarget* target = &((CarDisplayModelObject*) part)->m_transform;
			const GameTemplateInstanceTargetDispatch* dispatch = target->m_dispatch;
			dispatch->m_setPosition(target, NULL, &position);
			dispatch->m_setOrientation(target, NULL, (corner & 1) ? p_rotation : wheelRotation);
			part->m_flags = (part->m_flags & ~8) | 0x02000000;
		}
	}
}

// STUB: LEGORACERS2 0x0046a880
void ResetCarDisplay(CarDisplay* p_display)
{
	for (LegoS32 partIndex = 0; partIndex < 36; ++partIndex) {
		GameObject* part = (GameObject*) p_display->m_parts[partIndex];
		LegoU32* flags = (LegoU32*) ((LegoU8*) part + 8);
		*flags |= 8;
	}

	for (LegoS32 nameIndex = p_display->m_activeCount - 1; nameIndex >= 0; --nameIndex) {
		MarkCarDisplayObjectForRemoval((GameObject*) p_display->m_nameObjects[nameIndex]);
	}
	p_display->m_activeCount = 0;
	p_display->m_selectedChoice = 0;
}

// STUB: LEGORACERS2 0x0046a8e0
void __fastcall ApplyCarDisplayChoice(
	CarDisplay* p_display,
	void*,
	const LegoU32* p_choiceRecord,
	const LegoFloat* p_vehicleData,
	const LegoFloat* p_rotation,
	LegoU32 p_mode
)
{
	if (p_display->m_activeCount > 0) {
		ResetCarDisplay(p_display);
	}
	UpdateCarDisplayChoice(p_display, NULL, p_choiceRecord, p_vehicleData, p_rotation, p_mode);
	p_display->m_selectedChoice = (LegoS32) p_choiceRecord;
}

// STUB: LEGORACERS2 0x004a3cd0
static CarDisplayModelObject* __fastcall ConstructCarDisplayModelObject(
	CarDisplayModelObject* p_object,
	void*,
	LegoU32 p_type,
	GameObjectTemplate* p_template
)
{
	new (p_object) GameObject(p_type, p_template, &p_object->m_transform);
	p_object->m_transform.m_owner = p_object;
	p_object->m_transform.m_collisionProxy = NULL;
	p_object->m_transform.m_stateFlags &= ~3u;
	memcpy(p_object->m_transform.m_rotation, (LegoU8*) p_template + 0x40, 0x10);
	((Quaternion*) p_object->m_transform.m_rotation)->Normalize();
	Matrix3 rotation;
	rotation.FUN_004a4d30(p_object->m_transform.m_rotation);
	memcpy(p_object->m_transform.m_rotationMatrix, rotation.m_values, sizeof(p_object->m_transform.m_rotationMatrix));
	memcpy(p_object->m_transform.m_position, (LegoU8*) p_template + 0x34, 0x0c);
	memcpy(p_object->m_transform.m_scale, (LegoU8*) p_template + 0x50, 0x0c);
	p_object->m_vtable = &g_carDisplayModelObjectDispatch;
	p_object->m_transform.m_dispatch = (const GameTemplateInstanceTargetDispatch*) &g_carDisplayTransformDispatch;
	ConstructGameObjectModelBinding(
		p_object,
		NULL,
		(LegoU8*) p_template + 0x5c,
		p_object->m_transform.m_position,
		p_object->m_transform.m_rotation,
		0
	);
	return p_object;
}

// STUB: LEGORACERS2 0x004a3db0
static void __fastcall DestructCarDisplayModelObject(GameObject* p_object, void*)
{
	p_object->m_vtable = &g_carDisplayModelObjectDispatch;
	CarDisplayTransformTarget* target = &((CarDisplayModelObject*) p_object)->m_transform;
	target->m_dispatch = (const GameTemplateInstanceTargetDispatch*) &g_carDisplayTransformDispatch;
	DestroyGameObjectModelBindingForOwner(p_object, NULL);
	target->m_dispatch = (const GameTemplateInstanceTargetDispatch*) &g_carDisplayBaseTransformDispatch;
	p_object->~GameObject();
}

// STUB: LEGORACERS2 0x004a3de0
static void __fastcall CarDisplayModelInitialize(GameObject* p_object, void*)
{
	InitializeGameObjectModelBinding((GameObjectModelBinding*) p_object->m_unk0x48, NULL);
	p_object->Initialize();
}

// STUB: LEGORACERS2 0x004a63b0
static void __fastcall CarDisplayLifecycleSetPosition(void* p_target, void*, const Vector3* p_position)
{
	CarDisplayTransformTarget* target = (CarDisplayTransformTarget*) p_target;
	memcpy(target->m_position, p_position, sizeof(target->m_position));
	GameObjectDispatchView* ownerDispatch = (GameObjectDispatchView*) target->m_owner->m_vtable;
	ownerDispatch->m_action0x04(target->m_owner, NULL, (LegoU32) target->m_position, (LegoU32) target->m_rotation);
}

// STUB: LEGORACERS2 0x004a63f0
static void __fastcall CarDisplayLifecycleSetRotation(void* p_target, void*, const LegoFloat* p_rotation)
{
	CarDisplayTransformTarget* target = (CarDisplayTransformTarget*) p_target;
	memcpy(target->m_rotation, p_rotation, sizeof(target->m_rotation));
	Matrix3 matrix;
	matrix.FUN_004a4d30(target->m_rotation);
	memcpy(target->m_rotationMatrix, matrix.m_values, sizeof(target->m_rotationMatrix));
	GameObjectDispatchView* ownerDispatch = (GameObjectDispatchView*) target->m_owner->m_vtable;
	ownerDispatch->m_action0x04(target->m_owner, NULL, (LegoU32) target->m_position, (LegoU32) target->m_rotation);
}

// STUB: LEGORACERS2 0x004a6460
void __fastcall CarDisplayLifecycleCopyTargetData(void* p_target, void*, LegoU32* p_output, LegoU32, LegoU32)
{
	const LegoU32 index = *(const LegoU32*) ((const LegoU8*) p_target + 0x58);
	typedef const LegoU32*(__fastcall * GetRecord)(CarDisplayIndexedRecordOwner*, void*, LegoU32);
	GetRecord getRecord = *(GetRecord*) ((const LegoU8*) *(void**) g_carDisplayIndexedRecordOwner + 0x50);
	const LegoU32* record = getRecord(g_carDisplayIndexedRecordOwner, NULL, index);
	memcpy(p_output, record, 5 * sizeof(LegoU32));
}

// STUB: LEGORACERS2 0x004bcd80
static GameObject* __fastcall CarDisplayModelTargetDestroy(void* p_target, void*, LegoU32 p_flags)
{
	GameObject* owner = (GameObject*) ((LegoU8*) p_target - 0x58);
	return CarDisplayModelDestroy(owner, NULL, p_flags);
}

// STUB: LEGORACERS2 0x004bcef0
void __fastcall CarDisplayModelRemove(GameObject* p_object, void*)
{
	p_object->Remove();
	ReleaseGameObjectModelBindingResources((GameObjectModelBinding*) p_object->m_unk0x48, NULL);
}

// FUNCTION: LEGORACERS2 0x004e7de0
static const LegoU32* __fastcall GetCarDisplayIndexedRecord(
	CarDisplayIndexedRecordOwner* p_owner,
	void*,
	LegoU32 p_index
)
{
	return p_owner->m_records + p_index * 5;
}

// STUB: LEGORACERS2 0x004e7e20
GameObject* __fastcall ConstructCarDisplayIndexedRecordOwner(void*, void*, LegoU32, GameObjectTemplate* p_template)
{
	void* storage = AllocateHeapMemory(sizeof(CarDisplayIndexedRecordOwner));
	if (storage == NULL) {
		return NULL;
	}
	CarDisplayIndexedRecordOwner* owner = (CarDisplayIndexedRecordOwner*) storage;
	new (owner) GameObject(0x900, p_template, NULL);
	owner->m_vtable = (const GameObjectDispatchView*) &g_carDisplayIndexedRecordOwnerBaseDispatch;
	g_carDisplayIndexedRecordOwner = owner;
	memcpy(owner->m_records, (const LegoU8*) p_template + 0x34, 0x2c0 * sizeof(LegoU32));
	owner->m_vtable = (const GameObjectDispatchView*) &g_carDisplayIndexedRecordOwnerDispatch;
	return owner;
}

// FUNCTION: LEGORACERS2 0x004e7e80
static LegoU32 __fastcall GetCarDisplayIndexedRecordOwnerSize(GameObject*, void*)
{
	return sizeof(CarDisplayIndexedRecordOwner);
}

// STUB: LEGORACERS2 0x004e7e90
static GameObject* __fastcall CarDisplayIndexedRecordOwnerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_carDisplayIndexedRecordOwnerBaseDispatch;
	g_carDisplayIndexedRecordOwner = NULL;
	p_object->~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004e7ec0
static void __fastcall NormalizeCarDisplayIndexedRecords(GameObject* p_object, void*)
{
	CarDisplayIndexedRecordOwner* owner = (CarDisplayIndexedRecordOwner*) p_object;
	static const LegoFloat scale = 3.1250002384185791015625f;
	static const LegoFloat offset = 2.1250002384185791015625f;
	for (LegoS32 i = 0; i < 0x40; ++i) {
		LegoFloat* value = (LegoFloat*) &owner->m_records[i * 5];
		*value = (*value * scale - offset) * *value;
	}
}

// Shared construction path used by the brick builder's 36 part objects.
GameObject* CreateCarDisplayPartForModel(const LegoChar* p_modelPath, LegoU32 p_flags)
{
	CarDisplayPartTemplate data;
	memset(&data, 0, sizeof(data));
	data.m_flags = p_flags;
	data.m_first = CharBuffer24("NoName");
	data.m_second = CharBuffer24("");
	data.m_transform.m_rotation[3] = 1.0f;
	memcpy(data.m_modelPath, p_modelPath, sizeof(data.m_modelPath));
	return CreateCarDisplayPartObject(&data);
}
void DestroyCarDisplayModelObject(GameObject* p_object)
{
	DeleteCarDisplayObject(p_object);
}
