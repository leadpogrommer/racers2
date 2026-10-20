#include "scene/scripts/foyersceneeventstate.h"

#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gametemplateinstance.h"
#include "math/matrix3.h"
#include "math/quaternion.h"
#include "math/spline.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "scene/scripts/foyerscene.h"

#include <new>
#include <string.h>

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

struct FoyerSceneEventStateTemplate : GameObjectTemplate {
	CharBuffer24 m_firstDefaultName;
	CharBuffer24 m_secondDefaultName;
	LegoU32 m_runtimeValues[4];
};

DECOMP_SIZE_ASSERT(FoyerSceneEventStateTemplate, 0x74);

struct FoyerSceneTargetAnimationState {
	LegoU8 m_prefix[0x84];
	LegoU32 m_mode;
	LegoU32 m_unk0x88;
	LegoFloat m_unk0x8c;
	void* m_transientSlots[4];
};

void __fastcall SetFoyerSceneEventStateOffset(FoyerSceneEventStateObject*, void*);

// STUB: LEGORACERS2 0x004c5790
void __fastcall ClearFoyerSceneTargetTransientBindings(FoyerSceneTargetAnimationState* p_state, void*)
{
	LegoS32 i;
	for (i = 0; i < 4; ++i) {
		void* slot = p_state->m_transientSlots[i];
		if (slot != NULL && *(LegoU32*) slot != 0) {
			*(LegoU32*) ((LegoU8*) slot + 0x14) = 0;
		}
	}
	if (p_state->m_mode == 2) {
		p_state->m_unk0x88 = 0;
	}
}

const FoyerSceneEventStateDispatchView g_foyerSceneEventStateDispatch = {
	{&GameObjectBaseDestroy,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeFoyerSceneEventStateObject,
	 &GameObjectBaseTrue,
	 (GameObjectDispatchView::ValueArgument) &UpdateFoyerSceneEventStateObject,
	 &GameObjectBaseRemove,
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
	 &GameObjectBaseNoOpOneArgument},
	&GetFoyerSceneEventStateTypeAndFlags,
	&CreateFoyerSceneEventStateObject,
	&GameObjectBaseZero,
	&PrepareFoyerSceneEventStateRecord
};

LegoU32* __fastcall GetFoyerSceneEventStateTypeAndFlags(FoyerSceneEventStateObject* p_object, void*)
{
	return &p_object->m_type;
}

// STUB: LEGORACERS2 0x004d62b0
FoyerSceneEventStateObject::FoyerSceneEventStateObject(GameObjectTemplate* p_template)
	: GameObject(0x700, p_template, NULL), m_firstName(), m_secondName(), m_unk0x88(0), m_unk0x8c(0), m_unk0x94(0),
	  m_unk0x98(0), m_stateFlags(2), m_unk0xa0(0), m_unk0xa4(0), m_unk0xa8(0), m_unk0xbc(0)
{
	const FoyerSceneEventStateTemplate* source = (const FoyerSceneEventStateTemplate*) p_template;
	LegoU32 i;
	for (i = 0; i < 4; ++i) {
		m_templateValues[i] = source->m_runtimeValues[i];
	}
	m_vtable = (const GameObjectDispatchView*) &g_foyerSceneEventStateDispatch;
}

FoyerSceneEventStateObject::~FoyerSceneEventStateObject()
{
	m_vtable = &g_gameObjectBaseDispatch;
}

// STUB: LEGORACERS2 0x004d6350
void __fastcall InitializeFoyerSceneEventStateObject(GameObject* p_object, void*)
{
	FoyerSceneEventStateObject* state = (FoyerSceneEventStateObject*) p_object;
	const FoyerSceneEventStateTemplate* source = (const FoyerSceneEventStateTemplate*) state->m_template;
	CharBuffer24 emptyName("");

	state->GameObject::Initialize();
	state->m_firstName = source->m_firstDefaultName;
	state->m_secondName = source->m_secondDefaultName;
	state->m_unk0x8c = 0;
	state->m_unk0x88 = 0;
	if (strncmp(state->m_firstName.m_data, emptyName.m_data, sizeof(state->m_firstName.m_data)) == 0 ||
		strncmp(state->m_secondName.m_data, emptyName.m_data, sizeof(state->m_secondName.m_data)) == 0) {
		state->m_stateFlags |= 0x20;
		return;
	}

	SplinePath* path = (SplinePath*) FindGameObjectByName(state->m_firstName.m_data, 1, NULL);
	GameObject* target = FindGameObjectByName(state->m_secondName.m_data, 1, NULL);
	state->m_unk0x88 = (LegoU32) path;
	state->m_unk0x8c = (LegoU32) target;
	GameLifecycleComponent* lifecycle = target->m_lifecycle0x54;
	LegoU32 active = lifecycle->m_vtable->m_isActive(lifecycle, NULL, 0);
	lifecycle->m_stateFlags = (lifecycle->m_stateFlags ^ (active != 0)) & 1 ^ lifecycle->m_stateFlags;
	state->m_unk0x90 = *(LegoU32*) &path->m_totalLength;
	state->m_unk0xbc = 1;
	*(LegoFloat*) &state->m_unk0x98 = path->m_totalLength * *(LegoFloat*) &state->m_templateValues[2] * 0.01f;
	if (*(LegoU32*) ((LegoU8*) target + 0x1e4) != 0) {
		SetFoyerSceneEventStateOffset(state, NULL);
	}
	state->m_unk0xa0 = 0;
	state->m_unk0xa4 = 0;
	state->m_unk0xa8 = 0;
}

// STUB: LEGORACERS2 0x004d65d0
LegoU32 __fastcall UpdateFoyerSceneEventStateObject(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	FoyerSceneEventStateObject* state = (FoyerSceneEventStateObject*) p_object;
	GameObject* target = (GameObject*) state->m_unk0x8c;
	SplinePath* path = (SplinePath*) state->m_unk0x88;
	if ((state->m_stateFlags & 0x20) != 0) {
		return 0;
	}
	if (state->m_unk0xbc != 0 && *(LegoU32*) ((LegoU8*) target + 0x1e4) != 0) {
		SetFoyerSceneEventStateOffset(state, NULL);
	}

	Vector3 position;
	Vector3 lookAtPosition;
	Vector3 tangent;
	Vector3 origin;
	Vector3 targetVelocity;
	Quaternion orientation;
	Matrix3 rotation;
	LegoS32 segmentIndex = 0;
	LegoFloat targetSpeedX = *(LegoFloat*) ((LegoU8*) target + 0x1f8);
	LegoFloat targetSpeedY = *(LegoFloat*) ((LegoU8*) target + 0x1fc);
	LegoFloat targetSpeed = *(LegoFloat*) ((LegoU8*) target + 0x200);
	LegoFloat distance = targetSpeed * p_elapsedSeconds + *(LegoFloat*) &state->m_unk0x98;
	LegoFloat t;
	state->m_unk0x94 = *(LegoU32*) &targetSpeed;
	*(LegoFloat*) &state->m_unk0xa0 += targetSpeedX * p_elapsedSeconds;
	*(LegoFloat*) &state->m_unk0xa4 += targetSpeedY * p_elapsedSeconds;
	if (distance < *(LegoFloat*) &state->m_unk0x90) {
		if (distance < 0.0f) {
			if ((state->m_stateFlags & 2) != 0) {
				distance += *(LegoFloat*) &state->m_unk0x90;
			}
			else {
				distance = 0.0f;
			}
		}
	}
	else if ((state->m_stateFlags & 2) != 0) {
		distance -= *(LegoFloat*) &state->m_unk0x90;
	}
	else {
		distance = *(LegoFloat*) &state->m_unk0x90;
	}
	t = path->FUN_004d1d90(&position, distance, &segmentIndex);
	*(LegoFloat*) &state->m_unk0x98 = distance;
	orientation.m_x = 0.0f;
	orientation.m_y = 0.0f;
	orientation.m_z = 0.0f;
	orientation.m_w = 1.0f;
	tangent.m_x = 0.0f;
	tangent.m_y = 0.0f;
	tangent.m_z = 0.0f;
	origin.m_x = 0.0f;
	origin.m_y = 0.0f;
	origin.m_z = 0.0f;
	if ((state->m_stateFlags & 4) != 0) {
		path->FUN_004d1390(&tangent, segmentIndex, t);
	}
	path->FUN_004d11c0(&lookAtPosition, segmentIndex, t);
	orientation.LookAt(&lookAtPosition, &origin, &tangent, 0);
	rotation.FUN_004a4d30(&orientation.m_x);
	Vector3 offset;
	offset.m_x = *(LegoFloat*) &state->m_unk0xa0 + *(LegoFloat*) &state->m_templateValues[0];
	offset.m_y = *(LegoFloat*) &state->m_unk0xa4 + *(LegoFloat*) &state->m_templateValues[1];
	offset.m_z = *(LegoFloat*) &state->m_unk0xa8;
	offset.FUN_004a4e00(&rotation);
	position.m_x += offset.m_x;
	position.m_y += offset.m_y;
	position.m_z += offset.m_z;

	GameTemplateInstanceTargetView* targetView = (GameTemplateInstanceTargetView*) target;
	const GameTemplateInstanceTargetDispatch* dispatch = targetView->m_dispatch;
	void* targetInterface = (LegoU8*) target + 0x58;
	if (dispatch->m_isActive(targetInterface, NULL) != 0) {
		const Vector3* current = dispatch->m_getPosition(targetInterface, NULL);
		LegoFloat inverseElapsed = 1.0f / p_elapsedSeconds;
		targetVelocity.m_x = (position.m_x - current->m_x) * inverseElapsed;
		targetVelocity.m_y = (position.m_y - current->m_y) * inverseElapsed;
		targetVelocity.m_z = (position.m_z - current->m_z) * inverseElapsed;
		LegoFloat* velocities = (LegoFloat*) ((LegoU8*) target + 0x118);
		velocities[0] = targetVelocity.m_x;
		velocities[1] = targetVelocity.m_y;
		velocities[2] = targetVelocity.m_z;
		memset((LegoU8*) target + 0x124, 0, 0x0c);
		LegoFloat scale = *(LegoFloat*) ((LegoU8*) target + 0x178);
		*(LegoFloat*) ((LegoU8*) target + 0x10c) = velocities[0] * scale;
		*(LegoFloat*) ((LegoU8*) target + 0x110) = velocities[1] * scale;
		*(LegoFloat*) ((LegoU8*) target + 0x114) = velocities[2] * scale;
		memset((LegoU8*) target + 0x100, 0, 0x0c);
	}
	dispatch->m_setPosition(targetInterface, NULL, &position);
	dispatch->m_setOrientation(targetInterface, NULL, &orientation.m_x);
	return 1;
}

// STUB: LEGORACERS2 0x004d6a00
FoyerSceneEventStateObject* __stdcall CreateFoyerSceneEventStateObject(void*, GameObjectTemplate* p_template)
{
	void* storage = AllocateHeapMemory(sizeof(FoyerSceneEventStateObject));
	if (storage == NULL) {
		return NULL;
	}
	return new (storage) FoyerSceneEventStateObject(p_template);
}

// STUB: LEGORACERS2 0x004d6a30
LegoU32 __stdcall PrepareFoyerSceneEventStateRecord(
	void* p_destination,
	LegoU32* p_recordSize,
	LegoU32 p_type,
	LegoU32 p_availableBytes
)
{
	LegoU32* values;
	if ((p_type != 2 && p_type != 3) || p_availableBytes < 0x74) {
		return 0;
	}
	*p_recordSize = 0x74;
	values = (LegoU32*) ((LegoU8*) p_destination + 0x64);
	if (p_type == 2) {
		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
	}
	values[3] = 0x3f800000;
	return 1;
}

// STUB: LEGORACERS2 0x004d6aa0
void __fastcall SetFoyerSceneEventStateOffset(FoyerSceneEventStateObject* p_state, void*)
{
	GameObject* target = (GameObject*) p_state->m_unk0x8c;
	FoyerSceneTargetAnimationState* targetState =
		(FoyerSceneTargetAnimationState*) *(LegoU32*) ((LegoU8*) target + 0x1e4);
	ClearFoyerSceneTargetTransientBindings(targetState, NULL);
	targetState->m_unk0x8c = *(LegoFloat*) &p_state->m_templateValues[3];
	p_state->m_unk0xbc = 0;
}
