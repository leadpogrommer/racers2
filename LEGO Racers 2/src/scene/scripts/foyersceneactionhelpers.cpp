#include "scene/scripts/foyersceneactionhelpers.h"

#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectrenderprep.h"
#include "math/matrix3.h"
#include "math/spline.h"
#include "physics/collision/collisionqueries.h"
#include "physics/collision/collisionservice.h"
#include "scene/scripts/foyersceneactionsbase.h"
#include "scene/scripts/foyerscenescheduledaction.h"

#include <math.h>

extern LegoFloat g_currentSimulationStep;

// GLOBAL: LEGORACERS2 0x0056fa88
void* g_foyerSceneDefaultTarget;

typedef void(__fastcall* ScheduledActionSetup)(FoyerSceneScheduledAction*, void*);

void __fastcall NoScheduledActionSetup(FoyerSceneScheduledAction*, void*)
{
}

void __fastcall InitializeScheduledActionPosition(FoyerSceneScheduledAction* p_action, void*)
{
	p_action->m_sceneTarget = g_foyerSceneDefaultTarget;
	p_action->m_origin = *(void**) ((LegoU8*) p_action + 0x30);
	if (p_action->m_state == 10) {
		p_action->m_state = 8;
	}
	else if (p_action->m_state == 9) {
		*(Vector3*) ((LegoU8*) p_action + 0xc0) = *(Vector3*) ((LegoU8*) p_action + 0x38);
		*(Vector3*) ((LegoU8*) p_action + 0xcc) = *(Vector3*) ((LegoU8*) p_action + 0x44);
	}
	*(LegoFloat*) ((LegoU8*) p_action + 0xd8) = 0.25f;
}

void __fastcall InitializeScheduledActionRotation(FoyerSceneScheduledAction* p_action, void*)
{
	p_action->m_sceneTarget = g_foyerSceneDefaultTarget;
	*(Vector3*) ((LegoU8*) p_action + 0xc0) = *(Vector3*) ((LegoU8*) p_action + 0x38);
	*(Vector3*) ((LegoU8*) p_action + 0xcc) = *(Vector3*) ((LegoU8*) p_action + 0x44);
	*(LegoFloat*) ((LegoU8*) p_action + 0xd8) = 0.25f;
}

void __fastcall InitializeScheduledActionPathProgress(FoyerSceneScheduledAction* p_action, void*)
{
	LegoU8* target = (LegoU8*) p_action->m_sceneTarget;
	void** targetVtable = *(void***) target;
	if (((LegoBool32(__fastcall*)(void*, void*)) targetVtable[0x54 / 4])(target, NULL) != 0) {
		void* transform = p_action->m_origin;
		void** transformVtable = *(void***) transform;
		const Vector3* position =
			((const Vector3*(__fastcall*) (void*, void*) ) transformVtable[0x14 / 4])(transform, NULL);
		LegoFloat progress = GetSceneTargetProgress(target, NULL, position);
		void* pathContext = (void*) *(LegoU32*) ((LegoU8*) p_action + 0x64);
		LegoFloat length = *(LegoFloat*) (*(LegoU8**) pathContext + 0x68);
		*(LegoFloat*) ((LegoU8*) p_action + 0xfc) = length;
		LegoFloat scaledProgress = length * progress;
		*(LegoFloat*) ((LegoU8*) p_action + 0xf8) = scaledProgress;
		*(LegoFloat*) ((LegoU8*) p_action + 0xf4) = scaledProgress;
	}
}

LegoFloat GetScheduledActionContextValue(void* p_context)
{
	if (p_context == NULL) {
		return 0.0f;
	}
	return *(LegoFloat*) (*(LegoU8**) ((LegoU8*) p_context + 4) + 0x50);
}

void __fastcall InitializeScheduledActionPathControl(FoyerSceneScheduledAction* p_action, void*)
{
	void* context = (void*) *(LegoU32*) ((LegoU8*) p_action + 0x50);
	if (context == NULL) {
		return;
	}
	void** vtable = *(void***) context;
	const Vector3* position = ((const Vector3*(__fastcall*) (void*, void*) ) vtable[0x14 / 4])(context, NULL);
	*(Vector3*) ((LegoU8*) p_action + 0xa0) = *position;
	*(LegoFloat*) ((LegoU8*) p_action + 0xb8) = *(LegoFloat*) ((LegoU8*) p_action + 0x54);
	LegoU32 varying = 0;
	for (LegoU32 i = 0; i < 3; ++i) {
		LegoFloat value = *(LegoFloat*) ((LegoU8*) p_action + 0x58 + i * 4);
		if (fabsf(value) > 0.0001f) {
			varying = 1;
		}
	}
	*(LegoU32*) ((LegoU8*) p_action + 0xbc) = varying;
}

extern const ScheduledActionSetup g_scheduledActionSetupCallbacks[10] = {
	&NoScheduledActionSetup,
	&NoScheduledActionSetup,
	&NoScheduledActionSetup,
	&NoScheduledActionSetup,
	&InitializeScheduledActionPosition,
	&InitializeScheduledActionRotation,
	&NoScheduledActionSetup,
	&NoScheduledActionSetup,
	&InitializeScheduledActionPathProgress,
	&NoScheduledActionSetup,
};

// STUB: LEGORACERS2 0x0045b660
LegoFloat __fastcall GetSceneTargetProgress(void* p_sceneTarget, void*, const Vector3* p_position)
{
	LegoU8* target = (LegoU8*) p_sceneTarget;
	Vector3 delta;
	delta.m_x = p_position->m_x - *(LegoFloat*) (target + 0x84);
	delta.m_y = p_position->m_y - *(LegoFloat*) (target + 0x88);
	delta.m_z = p_position->m_z - *(LegoFloat*) (target + 0x8c);
	LegoFloat dot = delta.m_x * *(LegoFloat*) (target + 0x90);
	dot += delta.m_y * *(LegoFloat*) (target + 0x94);
	dot += delta.m_z * *(LegoFloat*) (target + 0x98);
	dot *= *(LegoFloat*) (target + 0x9c);
	if (dot < 0.0f) {
		return 0.0f;
	}
	if (dot > 1.0f) {
		return 1.0f;
	}
	return dot;
}

// STUB: LEGORACERS2 0x0045ca30
void __fastcall InitializeFoyerSceneScheduledActionRecord(FoyerSceneScheduledAction* p_action, void*)
{
	g_scheduledActionSetupCallbacks[p_action->m_callbackIndex](p_action, NULL);
	p_action->m_referenceDistance = GetScheduledActionContextValue(p_action->m_origin);
	p_action->m_progress = GetScheduledActionContextValue((void*) *(LegoU32*) ((LegoU8*) p_action + 0x30));
	p_action->m_elapsed = 0.0f;
	p_action->m_firstUpdate = 1;
	p_action->m_endThreshold = p_action->m_startTime * 0.5f;
	p_action->m_inverseEndDuration = 1.0f / p_action->m_endThreshold;
	p_action->m_inverseStartDuration = 1.0f / p_action->m_startTime;
	p_action->m_phaseScale = 1.0f / p_action->m_endTime;
	InitializeScheduledActionPathControl(p_action, NULL);
}

// STUB: LEGORACERS2 0x0045cad0
LegoU32 __fastcall UpdateScheduledActionState(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	LegoU8* owner = (LegoU8*) p_action->m_owner;
	if (p_action->m_firstUpdate != 0) {
		p_action->m_firstUpdate = 0;
		void* ownerTransform = owner + 0x58;
		void** ownerVtable = *(void***) ownerTransform;
		((void(__fastcall*)(void*, void*)) ownerVtable[0x40 / 4])(ownerTransform, NULL);
		*(LegoU32*) (owner + 0x348) = 2;
		MarkReverbStateDirty(g_gameController.m_reverbManager, NULL);
	}
	UpdateScheduledOwnerVelocity((GameObject*) p_action->m_owner, NULL);

	LegoU32 active = p_action->m_callbackIndex == 3;
	if (active == 0) {
		void** targetVtable = *(void***) p_action->m_sceneTarget;
		active = ((
			LegoU32(__fastcall*)(void*, void*, const Vector3*)
		) targetVtable[0x50 / 4])(p_action->m_sceneTarget, NULL, (const Vector3*) ((LegoU8*) p_action + 0x94));
	}
	if (active != 0 || p_action->m_elapsed <= 1.5f) {
		p_action->m_elapsed += p_elapsed;
		if (p_action->m_elapsed < p_action->m_startTime) {
			return 0;
		}
		if (p_action->m_elapsed < p_action->m_endTime) {
			return 1;
		}
	}
	return 2;
}

// STUB: LEGORACERS2 0x0045d480
void __fastcall UpdateScheduledActionTarget(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	void* transform = (void*) p_action->m_origin;
	void** vtable = *(void***) transform;
	Vector3* worldPosition = (Vector3*) ((LegoU8*) p_action + 0x94);
	Vector3* target = (Vector3*) ((LegoU8*) p_action + 0xa0);
	LegoFloat smoothing = *(LegoFloat*) ((LegoU8*) p_action + 0xb8);
	const Vector3* origin = ((const Vector3*(__fastcall*) (void*, void*) ) vtable[0x14 / 4])(transform, NULL);
	*worldPosition = *origin;
	target->m_x += (origin->m_x - target->m_x) * smoothing * p_elapsed;
	target->m_y += (origin->m_y - target->m_y) * smoothing * p_elapsed;
	target->m_z += (origin->m_z - target->m_z) * smoothing * p_elapsed;
	Vector3* result = (Vector3*) ((LegoU8*) p_action + 0xac);
	if (*(LegoU32*) ((LegoU8*) p_action + 0xbc) != 0) {
		Vector3 rotatedOrigin = *(Vector3*) ((LegoU8*) p_action + 0x58);
		const Matrix3* rotation = ((const Matrix3*(__fastcall*) (void*, void*) ) vtable[0x18 / 4])(transform, NULL);
		rotatedOrigin.FUN_004a4e00(rotation);
		result->m_x = target->m_x + rotatedOrigin.m_x;
		result->m_y = target->m_y + rotatedOrigin.m_y;
		result->m_z = target->m_z + rotatedOrigin.m_z;
	}
	else {
		*result = *target;
	}
}

// STUB: LEGORACERS2 0x0045d690
void __fastcall UpdateScheduledActionRotation(
	FoyerSceneScheduledAction* p_action,
	void*,
	Vector3* p_position,
	LegoFloat p_elapsed
)
{
	void* rotationSource = *(void**) ((LegoU8*) p_action + 0x30);
	void** vtable = *(void***) rotationSource;
	*p_position = *(Vector3*) ((const Vector3*(__fastcall*) (void*, void*) ) vtable[0x14 / 4])(rotationSource, NULL);
	const Matrix3* matrix = ((const Matrix3*(__fastcall*) (void*, void*) ) vtable[0x18 / 4])(rotationSource, NULL);
	Vector3* currentDirection = (Vector3*) ((LegoU8*) p_action + 0xdc);
	Vector3* targetDirection = (Vector3*) ((LegoU8*) p_action + 0xe8);
	Vector3 matrixDirection = {matrix->m_values[0][2], matrix->m_values[1][2], matrix->m_values[2][2]};
	if (matrix->m_values[1][2] < 0.8f) {
		*targetDirection = matrixDirection;
	}
	if (p_action->m_firstUpdate != 0) {
		*currentDirection = matrixDirection;
		*targetDirection = matrixDirection;
	}
	currentDirection->m_x += (targetDirection->m_x - currentDirection->m_x) * 2.0f * p_elapsed;
	currentDirection->m_y += (targetDirection->m_y - currentDirection->m_y) * 2.0f * p_elapsed;
	currentDirection->m_z += (targetDirection->m_z - currentDirection->m_z) * 2.0f * p_elapsed;
	currentDirection->Normalize();

	LegoFloat* currentAngles = (LegoFloat*) ((LegoU8*) p_action + 0xc0);
	LegoFloat* targetAngles = (LegoFloat*) ((LegoU8*) p_action + 0xcc);
	LegoFloat angleScale = *(LegoFloat*) ((LegoU8*) p_action + 0xd8) * p_elapsed;
	currentAngles[0] += (targetAngles[0] - currentAngles[0]) * angleScale;
	currentAngles[1] += (targetAngles[1] - currentAngles[1]) * angleScale;
	currentAngles[2] += (targetAngles[2] - currentAngles[2]) * angleScale;
	LegoFloat xAngle = currentAngles[0];
	LegoFloat yAngle = currentAngles[1];
	LegoFloat zAngle = currentAngles[2];
	if (g_gameController.m_state == 6) {
		p_position->m_x +=
			xAngle * matrix->m_values[0][0] + yAngle * matrix->m_values[0][1] + zAngle * currentDirection->m_x;
		p_position->m_y +=
			xAngle * matrix->m_values[1][0] + yAngle * matrix->m_values[1][1] + zAngle * currentDirection->m_y;
		p_position->m_z +=
			xAngle * matrix->m_values[2][0] + yAngle * matrix->m_values[2][1] + zAngle * currentDirection->m_z;
	}
	else {
		p_position->m_x += xAngle * *(LegoFloat*) ((LegoU8*) p_action + 0xe4) + zAngle * currentDirection->m_x;
		p_position->m_y += yAngle;
		p_position->m_z += xAngle * -currentDirection->m_x + zAngle * *(LegoFloat*) ((LegoU8*) p_action + 0xe4);
	}
}

// STUB: LEGORACERS2 0x0045d920
void __fastcall UpdateScheduledActionFinalCamera(FoyerSceneScheduledAction* p_action, void*)
{
	LegoU8* originRecord = (LegoU8*) p_action->m_origin;
	LegoU8* cameraControl = *(LegoU8**) (originRecord + 8);
	LegoFloat lookDistance = 2.0f;
	if (*(LegoU32*) (cameraControl + 0x18) != 0) {
		lookDistance = *(LegoFloat*) (*(LegoU8**) (cameraControl + 0x10) + 0x38);
	}
	void* originTransform = p_action->m_origin;
	void** originVtable = *(void***) originTransform;
	const Vector3* originPosition =
		((const Vector3*(__fastcall*) (void*, void*) ) originVtable[0x14 / 4])(originTransform, NULL);
	void* rotationSource = *(void**) ((LegoU8*) p_action + 0x30);
	void** rotationVtable = *(void***) rotationSource;
	const Matrix3* matrix =
		((const Matrix3*(__fastcall*) (void*, void*) ) rotationVtable[0x18 / 4])(rotationSource, NULL);
	Vector3 position;
	Vector3 target;
	Vector3 up = {0.0f, 1.0f, 0.0f};
	LegoFloat distance = lookDistance * 1.25f;
	position.m_x = originPosition->m_x + matrix->m_values[0][2] * 3.0f + matrix->m_values[0][1] * distance;
	position.m_y = originPosition->m_y + matrix->m_values[1][2] * 3.0f + matrix->m_values[1][1] * distance;
	position.m_z = originPosition->m_z + matrix->m_values[2][2] * 3.0f + matrix->m_values[2][1] * distance;
	target.m_x = position.m_x + matrix->m_values[0][2] * 10.0f;
	target.m_y = position.m_y + matrix->m_values[1][2] * 10.0f;
	target.m_z = position.m_z + matrix->m_values[2][2] * 10.0f;
	SetGameObjectLookAt(p_action->m_owner, NULL, &position, &target, &up);
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = p_action->m_output;
}

// STUB: LEGORACERS2 0x0045da90
LegoBool32 __fastcall IsScheduledActionOccluded(
	FoyerSceneScheduledAction* p_action,
	void*,
	const Vector3* p_targetPosition
)
{
	void* originTransform = p_action->m_origin;
	void** vtable = *(void***) originTransform;
	const Vector3* originPosition =
		((const Vector3*(__fastcall*) (void*, void*) ) vtable[0x14 / 4])(originTransform, NULL);
	CollisionQueryResult queryResult;
	LegoU32 hits = g_collisionProxyService->m_vtable
					   ->m_querySegment(g_collisionProxyService, NULL, p_targetPosition, originPosition, &queryResult);
	return hits != 0 && queryResult.m_first != p_action->m_origin;
}

struct ScheduledPathView {
	SplinePath* m_path;
	Vector3 m_position;
	LegoU32 m_hasDynamicControl;
	LegoU8 m_padding0x14[0x0c];
	void* m_controlObject;
};

void __fastcall UpdateScheduledPathControl(struct ScheduledPathView* p_path, void*);

// STUB: LEGORACERS2 0x0045db10
Vector3* __fastcall UpdateScheduledPathPoint(void* p_context, void*, LegoFloat p_fraction)
{
	ScheduledPathView* p_path = (ScheduledPathView*) p_context;
	if (p_path->m_hasDynamicControl != 0) {
		UpdateScheduledPathControl(p_path, NULL);
	}
	LegoS32 segmentIndex = (LegoS32) p_path;
	p_path->m_path->FUN_004d1d90(&p_path->m_position, p_path->m_path->m_totalLength * p_fraction, &segmentIndex);
	return &p_path->m_position;
}

// STUB: LEGORACERS2 0x0045db50
void __fastcall UpdateScheduledPathControl(ScheduledPathView* p_path, void*)
{
	SplinePath* path = p_path->m_path;
	LegoS32 segmentCount = path->m_segmentCount;
	void* control = p_path->m_controlObject;
	void** vtable = *(void***) control;
	const Vector3* position = ((const Vector3*(__fastcall*) (void*, void*) ) vtable[0x14 / 4])(control, NULL);
	const Matrix3* matrix = ((const Matrix3*(__fastcall*) (void*, void*) ) vtable[0x18 / 4])(control, NULL);
	Vector3* endpoint = &path->m_segments[segmentCount - 1].m_vector0x00;
	endpoint->m_x = matrix->m_values[0][2] * -7.0f + position->m_x;
	endpoint->m_y = 4.0f - matrix->m_values[1][2] * 7.0f + position->m_y;
	endpoint->m_z = position->m_z + matrix->m_values[2][2] * -7.0f;
	path->FUN_004d1aa0(segmentCount - 2, 1.0f, 0.0f, 1);
	path->FUN_004d1a70();
}

// STUB: LEGORACERS2 0x004ba9f0
void __fastcall UpdateScheduledOwnerVelocity(GameObject* p_owner, void*)
{
	LegoU32* timer = (LegoU32*) ((LegoU8*) p_owner + 0x348);
	if (*timer != 0) {
		--*timer;
		return;
	}
	LegoFloat inverseStep = 1.0f / g_currentSimulationStep;
	LegoFloat* position = (LegoFloat*) ((LegoU8*) p_owner + 0x78);
	LegoFloat* target = (LegoFloat*) ((LegoU8*) p_owner + 0xb4);
	LegoFloat* velocity = (LegoFloat*) ((LegoU8*) p_owner + 0x118);
	velocity[0] = (position[0] - target[0]) * inverseStep;
	velocity[1] = (position[1] - target[1]) * inverseStep;
	velocity[2] = (position[2] - target[2]) * inverseStep;
}
