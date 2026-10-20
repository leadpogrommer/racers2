#include "scene/scripts/foyersceneaudiotarget.h"

#include "frontend/foyer/controllerfoyertaskmanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "math/matrix3.h"
#include "math/quaternion.h"
#include "math/spline.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "scene/scripts/foyersceneactionsbase.h"

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FoyerSceneTargetTemplate : GameObjectTemplate {
	Vector3 m_position;
	Quaternion m_orientation;
	LegoU32 m_registrationMode;
};
DECOMP_SIZE_ASSERT(FoyerSceneTargetTemplate, 0x54);
struct FoyerSceneAudioTarget : GameObject {
	FoyerSceneAudioTarget(FoyerSceneTargetTemplate*);
	Vector3 m_position;                    // 0x58
	Vector3 m_direction;                   // 0x64
	Vector3 m_lookPoint;                   // 0x70
	void* m_splineContext;                 // 0x7c
	SplinePath* m_track;                   // 0x80
	Vector3 m_trackStart;                  // 0x84
	Vector3 m_trackDelta;                  // 0x90
	LegoFloat m_inverseTrackLengthSquared; // 0x9c
	LegoU32 m_registrationMode;
};
DECOMP_SIZE_ASSERT(FoyerSceneAudioTarget, 0xa4);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneAudioTarget, m_direction) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneAudioTarget, m_splineContext) == 0x7c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneAudioTarget, m_registrationMode) == 0xa0);
struct FoyerSceneTargetDispatch {
	GameObjectDispatchView m_object;
	LegoU32(__fastcall* m_acceptPosition)(GameObject*, void*, const Vector3*);
};
struct FoyerSceneTargetBaseDispatch {
	FoyerSceneTargetDispatch m_base;
	GameObjectDispatchView::Value m_hasTrack;
};
static void __fastcall InitializeFoyerSceneAudioTarget(GameObject*, void*);
static void __fastcall RemoveFoyerSceneAudioTarget(GameObject*, void*);
static LegoU32 __fastcall TestFoyerSceneTargetPosition(GameObject*, void*, const Vector3*);
static LegoU32 __fastcall HasFoyerSceneTargetTrack(GameObject*, void*);
static LegoU32 __fastcall AcceptFoyerSceneTargetPosition(GameObject*, void*, const Vector3*);
static void __fastcall BindFoyerSceneTargetSpline(FoyerSceneAudioTarget*, void*, SplinePath*);
static void __fastcall BindFoyerSceneTargetTrack(FoyerSceneAudioTarget*, void*, SplinePath*);
static void __fastcall RegisterControllerFoyerSceneTarget(void*, void*, GameObject*);

#define FOYER_TARGET_OBJECT_INITIALIZER                                                                                \
	{&GameObjectBaseDestroy,                                                                                           \
	 &GameObjectBaseApplyChildTransform,                                                                               \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &InitializeFoyerSceneAudioTarget,                                                                                 \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseZeroOneArgument,                                                                                   \
	 &RemoveFoyerSceneAudioTarget,                                                                                     \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseDrawChild,                                                                                         \
	 &GameObjectBaseNoOpOneArgument,                                                                                   \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseNoOpTwoArguments,                                                                                  \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseZeroTwoArguments,                                                                                  \
	 &GameObjectBaseUpdateRegion,                                                                                      \
	 &GameObjectBaseNoOpOneArgument}
// Retail pointer tables: base 0x00532310 (22 slots), default 0x005323b8 (21).
extern const FoyerSceneTargetBaseDispatch g_foyerSceneTargetBaseDispatch = {
	{FOYER_TARGET_OBJECT_INITIALIZER, &TestFoyerSceneTargetPosition},
	&HasFoyerSceneTargetTrack
};
extern const FoyerSceneTargetDispatch g_foyerSceneAudioTargetDispatch = {
	FOYER_TARGET_OBJECT_INITIALIZER,
	&AcceptFoyerSceneTargetPosition
};
#undef FOYER_TARGET_OBJECT_INITIALIZER

// GLOBAL: LEGORACERS2 0x0056f888
static LegoU32 g_foyerSceneAudioTargetStorage[sizeof(FoyerSceneAudioTarget) / sizeof(LegoU32)];
// GLOBAL: LEGORACERS2 0x005435c4
GameObject* g_foyerSceneAudioTarget = (GameObject*) g_foyerSceneAudioTargetStorage;
GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

// STUB: LEGORACERS2 0x0045b2b0
FoyerSceneAudioTarget::FoyerSceneAudioTarget(FoyerSceneTargetTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_splineContext = NULL;
	m_track = NULL;
	m_registrationMode = p_template->m_registrationMode;
	m_vtable = &g_foyerSceneTargetBaseDispatch.m_base.m_object;
	Matrix3 matrix;
	matrix.FUN_004a4d30(&p_template->m_orientation.m_x);
	m_direction.m_x = matrix.m_values[0][2];
	m_direction.m_y = matrix.m_values[1][2];
	m_direction.m_z = matrix.m_values[2][2];
	m_position = p_template->m_position;
	m_lookPoint = p_template->m_position;
	m_lookPoint.Add(&m_direction);
	m_direction.m_y = 0.0f;
	m_direction.Normalize();
	m_flags |= 4;
	m_flags |= 8;
	m_flags |= 2;
}

// FUNCTION: LEGORACERS2 0x0045b3c0
static LegoU32 __fastcall HasFoyerSceneTargetTrack(GameObject* p_object, void*)
{
	return ((FoyerSceneAudioTarget*) p_object)->m_track != NULL;
}

// STUB: LEGORACERS2 0x0045b3d0
static void __fastcall InitializeFoyerSceneAudioTarget(GameObject* p_object, void*)
{
	FoyerSceneAudioTarget* target = (FoyerSceneAudioTarget*) p_object;
	if ((target->m_flags & 0x800000) == 0) {
		if (target->m_registrationMode == 0) {
			RegisterControllerFoyerSceneTarget(GetControllerTaskManager(), NULL, target);
		}
		const LegoChar* numberText = strrchr(target->m_first.m_data, ' ');
		LegoS32 number = numberText == NULL ? 0 : atoi(numberText + 1);
		LegoChar name[64];
		sprintf(name, "cine_spline%i", number);
		CharBuffer24 splineName(name);
		SplinePath* spline = (SplinePath*) FindGameObjectByName(splineName.m_data, 0, NULL);
		if (spline != NULL) {
			BindFoyerSceneTargetSpline(target, NULL, spline);
		}
		sprintf(name, "cine_track%i", number);
		CharBuffer24 trackName(name);
		SplinePath* track = (SplinePath*) FindGameObjectByName(trackName.m_data, 0, NULL);
		if (track != NULL) {
			BindFoyerSceneTargetTrack(target, NULL, track);
		}
	}
	target->GameObject::Initialize();
}

// FUNCTION: LEGORACERS2 0x0045b4a0
static void __fastcall RemoveFoyerSceneAudioTarget(GameObject* p_object, void*)
{
	FoyerSceneAudioTarget* target = (FoyerSceneAudioTarget*) p_object;
	target->GameObject::Remove();
	if (target->m_splineContext != NULL) {
		FreeHeapMemory(target->m_splineContext);
	}
}

// STUB: LEGORACERS2 0x0045b4c0
static LegoU32 __fastcall TestFoyerSceneTargetPosition(GameObject* p_object, void*, const Vector3* p_position)
{
	FoyerSceneAudioTarget* target = (FoyerSceneAudioTarget*) p_object;
	Vector3 delta = *p_position;
	delta.Subtract(&target->m_position);
	LegoFloat distanceSquared = delta.m_x * delta.m_x + delta.m_y * delta.m_y + delta.m_z * delta.m_z;
	if (distanceSquared > 12000.0f || distanceSquared < 40.0f) {
		return 0;
	}
	delta.m_y = 0.0f;
	delta.Normalize();
	return delta.Dot(&target->m_direction) >= 0.0f;
}

// STUB: LEGORACERS2 0x0045b580
static void __fastcall BindFoyerSceneTargetSpline(FoyerSceneAudioTarget* p_target, void*, SplinePath* p_path)
{
	p_target->m_splineContext = AllocateHeapMemory(0x30);
	if (p_target->m_splineContext != NULL) {
		*(LegoU32*) ((LegoU8*) p_target->m_splineContext + 0x10) = 0;
	}
	BindFoyerActionPath(p_target->m_splineContext, NULL, p_path);
}

// FUNCTION: LEGORACERS2 0x0045b5c0
void* __fastcall GetFoyerSceneTargetSpline(GameObject* p_object, void*)
{
	return ((FoyerSceneAudioTarget*) p_object)->m_splineContext;
}

// STUB: LEGORACERS2 0x0045b5d0
static void __fastcall BindFoyerSceneTargetTrack(FoyerSceneAudioTarget* p_target, void*, SplinePath* p_track)
{
	p_target->m_track = p_track;
	p_target->m_trackStart = p_track->m_segments[0].m_vector0x00;
	p_target->m_trackDelta = p_track->m_segments[1].m_vector0x00;
	p_target->m_trackDelta.Subtract(&p_target->m_trackStart);
	const Vector3* delta = &p_target->m_trackDelta;
	p_target->m_inverseTrackLengthSquared =
		1.0f / (delta->m_x * delta->m_x + delta->m_y * delta->m_y + delta->m_z * delta->m_z);
}

// STUB: LEGORACERS2 0x0045b780
static FoyerSceneAudioTarget* __fastcall ConstructDefaultFoyerSceneAudioTarget(FoyerSceneAudioTarget* p_target)
{
	FoyerSceneTargetTemplate objectTemplate;
	objectTemplate.m_flags = 0;
	objectTemplate.m_first = CharBuffer24("NoName");
	objectTemplate.m_second = CharBuffer24("");
	objectTemplate.m_position.SetValues(0.0f, 0.0f, 0.0f);
	objectTemplate.m_orientation.m_x = 0.0f;
	objectTemplate.m_orientation.m_y = 0.0f;
	objectTemplate.m_orientation.m_z = 0.0f;
	objectTemplate.m_orientation.m_w = 1.0f;
	objectTemplate.m_registrationMode = 1;
	new (p_target) FoyerSceneAudioTarget(&objectTemplate);
	p_target->m_vtable = &g_foyerSceneAudioTargetDispatch.m_object;
	return p_target;
}
static void ConstructStaticFoyerSceneAudioTarget();
static void RegisterStaticFoyerSceneAudioTargetExit();
static void DestroyStaticFoyerSceneAudioTarget();

// FUNCTION: LEGORACERS2 0x0045b850
static void InitializeStaticFoyerSceneAudioTarget()
{
	ConstructStaticFoyerSceneAudioTarget();
	RegisterStaticFoyerSceneAudioTargetExit();
}

// FUNCTION: LEGORACERS2 0x0045b860
static void ConstructStaticFoyerSceneAudioTarget()
{
	ConstructDefaultFoyerSceneAudioTarget((FoyerSceneAudioTarget*) g_foyerSceneAudioTargetStorage);
}

// FUNCTION: LEGORACERS2 0x0045b870
static void RegisterStaticFoyerSceneAudioTargetExit()
{
	atexit(DestroyStaticFoyerSceneAudioTarget);
}

// FUNCTION: LEGORACERS2 0x0045b880
static void DestroyStaticFoyerSceneAudioTarget()
{
	((FoyerSceneAudioTarget*) g_foyerSceneAudioTargetStorage)->GameObject::~GameObject();
}

// STUB: LEGORACERS2 0x0045bb60
static void __fastcall RegisterControllerFoyerSceneTarget(void* p_manager, void*, GameObject* p_target)
{
	struct TargetRecords {
		LegoU8 m_prefix[0x408];
		GameObject* m_targets[64];
		LegoS32 m_count;
	};
	TargetRecords* records = *(TargetRecords**) ((LegoU8*) p_manager + 0x58);
	if (records->m_count < 64) {
		records->m_targets[records->m_count++] = p_target;
	}
}

// FUNCTION: LEGORACERS2 0x0049a640 FOLDED
static LegoU32 __fastcall AcceptFoyerSceneTargetPosition(GameObject*, void*, const Vector3*)
{
	return 1;
}

struct FoyerSceneAudioTargetStartup {
	FoyerSceneAudioTargetStartup() { InitializeStaticFoyerSceneAudioTarget(); }
};
static FoyerSceneAudioTargetStartup g_foyerSceneAudioTargetStartup;
