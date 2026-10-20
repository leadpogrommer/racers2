#include "game/objects/gametemplateinstance.h"

#include "game/objects/gameobjectdispatch.h"
#include "math/matrix3.h"
#include "math/quaternion.h"
#include "math/spline.h"
#include "memory/memoryheap.h"

#include <new>
#include <string.h>

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

struct GameTemplateInstanceTemplate : GameObjectTemplate {
	CharBuffer24 m_firstDefaultName;
	CharBuffer24 m_secondDefaultName;
	LegoU32 m_values[8];
};

DECOMP_SIZE_ASSERT(GameTemplateInstanceTemplate, 0x84);

struct GameTemplateInstanceTemplateTransform : GameObjectTemplate {
	Vector3 m_position;
};

DECOMP_STATIC_ASSERT(sizeof(GameTemplateInstanceExtendedDispatchView) == 0x60);

const GameTemplateInstanceExtendedDispatchView g_gameTemplateInstanceDispatch = {
	{&GameTemplateInstanceDestroy,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeGameTemplateInstance,
	 &GameObjectBaseTrue,
	 (GameObjectDispatchView::ValueArgument) &UpdateGameTemplateInstance,
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
	&GetGameTemplateInstanceTypeAndFlags,
	&CreateGameTemplateInstance,
	&GameObjectBaseZero,
	&PrepareGameTemplateInstanceRecord
};

LegoU32* __fastcall GetGameTemplateInstanceTypeAndFlags(GameTemplateInstance* p_object, void*)
{
	return &p_object->m_type;
}

GameObject* __fastcall GameTemplateInstanceDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	((GameTemplateInstance*) p_object)->~GameTemplateInstance();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004d6b00
GameTemplateInstance::GameTemplateInstance(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, NULL), m_firstDefaultName(), m_secondDefaultName(), m_unk0x88(0), m_unk0x8c(0),
	  m_unk0x98(0), m_templateFlags(0)
{
	const GameTemplateInstanceTemplate* source = (const GameTemplateInstanceTemplate*) p_template;
	LegoU32 i;

	m_firstDefaultName = source->m_firstDefaultName;
	m_secondDefaultName = source->m_secondDefaultName;
	m_unk0x94 = source->m_values[0];
	for (i = 0; i < 5; ++i) {
		m_templateValues[i] = source->m_values[i + 2];
	}

	if (source->m_values[1] != 0) {
		m_templateFlags |= 4;
	}
	if (*(LegoFloat*) &m_templateValues[0] != 0.0f) {
		m_templateFlags |= 2;
	}
	if (*(LegoFloat*) &m_templateValues[1] != 0.0f) {
		m_templateFlags |= 8;
	}
	if (source->m_values[7] != 0) {
		m_templateFlags |= 0x10;
	}
	m_vtable = (const GameObjectDispatchView*) &g_gameTemplateInstanceDispatch;
}

GameTemplateInstance::~GameTemplateInstance()
{
	m_vtable = &g_gameObjectBaseDispatch;
}

// STUB: LEGORACERS2 0x004d6c00
void __fastcall InitializeGameTemplateInstance(GameObject* p_object, void*)
{
	GameTemplateInstance* instance = (GameTemplateInstance*) p_object;
	GameTemplateInstanceTemplate* source;
	SplinePath* path = NULL;
	GameObject* target = NULL;

	instance->GameObject::Initialize();
	source = (GameTemplateInstanceTemplate*) instance->m_template;
	instance->m_firstDefaultName = source->m_firstDefaultName;
	instance->m_secondDefaultName = source->m_secondDefaultName;

	if (instance->m_firstDefaultName.m_data[0] == '\0') {
		instance->m_templateFlags |= 0x80;
	}
	else {
		path = (SplinePath*) FindGameObjectByName(instance->m_firstDefaultName.m_data, 1, NULL);
		instance->m_unk0x88 = (LegoU32) path;
	}

	if (instance->m_secondDefaultName.m_data[0] == '\0') {
		instance->m_templateFlags |= 0x80;
	}
	else {
		target = FindGameObjectByName(instance->m_secondDefaultName.m_data, 1, NULL);
		instance->m_unk0x8c = (LegoU32) target;
		if (target != NULL && target->m_lifecycle0x54 != NULL) {
			GameLifecycleComponent* lifecycle = target->m_lifecycle0x54;
			LegoU32 active = lifecycle->m_vtable->m_isActive(lifecycle, NULL, 0);
			lifecycle->m_stateFlags = (lifecycle->m_stateFlags ^ (active != 0)) & 1 ^ lifecycle->m_stateFlags;
		}
	}

	*(LegoFloat*) &instance->m_unk0x90 = path != NULL ? path->m_totalLength : 0.0f;
	if ((instance->m_templateFlags & 8) != 0) {
		if (*(LegoFloat*) &instance->m_templateValues[1] <= 0.0f) {
			LegoFloat lowerLimit = *(LegoFloat*) &instance->m_unk0x90 * -0.5f;
			if (*(LegoFloat*) &instance->m_templateValues[1] < lowerLimit) {
				*(LegoFloat*) &instance->m_templateValues[1] = lowerLimit;
			}
		}
		else {
			LegoFloat upperLimit = *(LegoFloat*) &instance->m_unk0x90 * 0.5f;
			if (upperLimit < *(LegoFloat*) &instance->m_templateValues[1]) {
				*(LegoFloat*) &instance->m_templateValues[1] = upperLimit;
			}
		}
	}
	*(LegoFloat*) &instance->m_unk0x98 =
		*(LegoFloat*) &instance->m_templateValues[4] * *(LegoFloat*) &instance->m_unk0x90 * 0.01f;

	if ((instance->m_templateFlags & 0x10) != 0 && path != NULL && target != NULL && target->m_template != NULL) {
		Vector3 targetPosition = ((GameTemplateInstanceTemplateTransform*) target->m_template)->m_position;
		Vector3 pathStart;
		path->FUN_004d0f60(&pathStart, 0, 0.0f);
		*(LegoFloat*) &instance->m_unk0xb4[0] = targetPosition.m_x - pathStart.m_x;
		*(LegoFloat*) &instance->m_unk0xb4[4] = targetPosition.m_y - pathStart.m_y;
		*(LegoFloat*) &instance->m_unk0xb4[8] = targetPosition.m_z - pathStart.m_z;
	}
}

// STUB: LEGORACERS2 0x004d6e30
LegoU32 __fastcall UpdateGameTemplateInstance(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	GameTemplateInstance* instance = (GameTemplateInstance*) p_object;
	if ((instance->m_templateFlags & 0x80) != 0) {
		return 0;
	}
	UpdateGameTemplateInstancePath(
		instance,
		NULL,
		(SplinePath*) instance->m_unk0x88,
		(GameObject*) instance->m_unk0x8c,
		p_elapsedSeconds
	);
	return 1;
}

// STUB: LEGORACERS2 0x004d6e60
void __fastcall UpdateGameTemplateInstancePath(
	GameTemplateInstance* p_instance,
	void*,
	SplinePath* p_path,
	GameObject* p_target,
	LegoFloat p_elapsedSeconds
)
{
	LegoFloat distance = p_elapsedSeconds * *(LegoFloat*) &p_instance->m_unk0x94 + *(LegoFloat*) &p_instance->m_unk0x98;
	if (distance < *(LegoFloat*) &p_instance->m_unk0x90) {
		if (distance < 0.0f) {
			distance += *(LegoFloat*) &p_instance->m_unk0x90;
		}
	}
	else {
		distance -= *(LegoFloat*) &p_instance->m_unk0x90;
	}

	LegoS32 segmentIndex = 0;
	Vector3 position;
	Vector3 lookAtPosition;
	Vector3 tangent;
	Vector3 origin;
	Quaternion orientation;
	Matrix3 rotation;
	LegoFloat t = p_path->FUN_004d1d90(&position, distance, &segmentIndex);
	*(LegoFloat*) &p_instance->m_unk0x98 = distance;
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

	if ((p_instance->m_templateFlags & 4) != 0) {
		if ((p_instance->m_templateFlags & 8) == 0) {
			p_path->FUN_004d11c0(&lookAtPosition, segmentIndex, t);
		}
		else {
			p_path->FUN_004d1d90(&lookAtPosition, distance, &segmentIndex);
		}
		p_path->FUN_004d1390(&tangent, segmentIndex, t);
		if (*(LegoFloat*) &p_instance->m_unk0x94 < 0.0f) {
			tangent.m_x *= -1.0f;
			tangent.m_y *= -1.0f;
			tangent.m_z *= -1.0f;
		}
		orientation.LookAt(&lookAtPosition, &origin, &tangent, 0);
	}

	rotation.FUN_004a4d30(&orientation.m_x);
	position.m_x += *(LegoFloat*) &p_instance->m_templateValues[2];
	position.m_y += *(LegoFloat*) &p_instance->m_templateValues[3];
	position.m_z = 0.0f;
	position.FUN_004a4e00(&rotation);
	if ((p_instance->m_templateFlags & 0x10) != 0) {
		position.m_x += *(LegoFloat*) &p_instance->m_unk0xb4[0];
		position.m_y += *(LegoFloat*) &p_instance->m_unk0xb4[4];
		position.m_z += *(LegoFloat*) &p_instance->m_unk0xb4[8];
	}

	GameTemplateInstanceTargetView* targetView = (GameTemplateInstanceTargetView*) p_target;
	const GameTemplateInstanceTargetDispatch* dispatch = targetView->m_dispatch;
	void* targetInterface = (LegoU8*) p_target + 0x58;
	if (dispatch->m_isActive(targetInterface, NULL) != 0) {
		const Vector3* current = dispatch->m_getPosition(targetInterface, NULL);
		LegoFloat inverseElapsed = 1.0f / p_elapsedSeconds;
		LegoFloat* targetVelocity = (LegoFloat*) ((LegoU8*) p_target + 0x118);
		targetVelocity[0] = (position.m_x - current->m_x) * inverseElapsed;
		targetVelocity[1] = (position.m_y - current->m_y) * inverseElapsed;
		targetVelocity[2] = (position.m_z - current->m_z) * inverseElapsed;
		memset((LegoU8*) p_target + 0x124, 0, 0x0c);
		LegoFloat scale = *(LegoFloat*) ((LegoU8*) p_target + 0x178);
		*(LegoFloat*) ((LegoU8*) p_target + 0x10c) = targetVelocity[0] * scale;
		*(LegoFloat*) ((LegoU8*) p_target + 0x110) = targetVelocity[1] * scale;
		*(LegoFloat*) ((LegoU8*) p_target + 0x114) = targetVelocity[2] * scale;
		memset((LegoU8*) p_target + 0x100, 0, 0x0c);
	}

	const Vector3* current = dispatch->m_getPosition(targetInterface, NULL);
	Vector3 velocity;
	LegoFloat inverseElapsed = 1.0f / p_elapsedSeconds;
	velocity.m_x = (position.m_x - current->m_x) * inverseElapsed;
	velocity.m_y = (position.m_y - current->m_y) * inverseElapsed;
	velocity.m_z = (position.m_z - current->m_z) * inverseElapsed;
	dispatch->m_setPosition(targetInterface, NULL, &position);
	if ((p_instance->m_templateFlags & 4) != 0) {
		dispatch->m_setOrientation(targetInterface, NULL, &orientation.m_x);
	}
}

// STUB: LEGORACERS2 0x004d7360
GameTemplateInstance* __stdcall CreateGameTemplateInstance(void*, GameObjectTemplate* p_template)
{
	void* storage = AllocateHeapMemory(sizeof(GameTemplateInstance));
	if (storage == NULL) {
		return NULL;
	}
	return new (storage) GameTemplateInstance(p_template);
}

// STUB: LEGORACERS2 0x004d7390
LegoU32 __stdcall PrepareGameTemplateInstanceRecord(
	void* p_destination,
	LegoU32* p_recordSize,
	LegoU32 p_type,
	LegoU32 p_availableBytes
)
{
	if (p_type != 2 || p_availableBytes < 0x84) {
		return 0;
	}
	*p_recordSize = 0x84;
	memset((LegoU8*) p_destination + 0x74, 0, 0x0c);
	return 1;
}
