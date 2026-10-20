#include "scene/scripts/foyerscenescheduledaction.h"

#include "frontend/foyer/controllerfoyertaskmanager.h"
#include "game/objects/gameobjectrenderprep.h"
#include "scene/scripts/foyersceneactionhelpers.h"

#include <math.h>

typedef LegoU32(__fastcall* FoyerSceneActionCallback)(FoyerSceneScheduledAction*, void*, LegoFloat);
typedef LegoFloat(__fastcall* FoyerSceneProgressCallback)(FoyerSceneScheduledAction*, void*, LegoFloat);

LegoU32 __fastcall ApplySceneTargetOffset(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall InterpolateSceneTargetOffset(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall ApplyTimedSceneTargetOffset(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall UpdateSceneTargetDistance(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall UpdateSceneTargetDistanceFromSource(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall InterpolateSceneTargetPosition(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall ApplyTimedSceneTargetPosition(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall ComposeSceneTargetRotation(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoU32 __fastcall GetFinalSceneActionState(FoyerSceneScheduledAction*, void*, LegoFloat);

LegoFloat __stdcall InputZeroAxis(LegoU32);
LegoFloat __fastcall ProgressCurrentValue(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressEaseIn(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressEaseOut(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressEaseInOut(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressEaseOutIn(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressElapsed(FoyerSceneScheduledAction*, void*, LegoFloat);
LegoFloat __fastcall ProgressAngle(FoyerSceneScheduledAction*, void*, LegoFloat);

// The main 16-entry dispatch view overlaps the eight-entry progress view at
// 0x532530. One backing array preserves the shared pointer storage.
// Retail callback pointer table: 0x00532508; all 18 slots are verified.
extern const FoyerSceneActionCallback g_foyerSceneActionCallbacks[18] = {
	(FoyerSceneActionCallback) &GetFinalSceneActionState,
	&ApplySceneTargetOffset,
	&InterpolateSceneTargetOffset,
	&ApplyTimedSceneTargetOffset,
	&UpdateSceneTargetDistance,
	&UpdateSceneTargetDistanceFromSource,
	&InterpolateSceneTargetPosition,
	(FoyerSceneActionCallback) &GetFinalSceneActionState,
	&ApplyTimedSceneTargetPosition,
	&ComposeSceneTargetRotation,
	(FoyerSceneActionCallback) &InputZeroAxis,
	(FoyerSceneActionCallback) &ProgressCurrentValue,
	(FoyerSceneActionCallback) &ProgressEaseIn,
	(FoyerSceneActionCallback) &ProgressEaseOut,
	(FoyerSceneActionCallback) &ProgressEaseInOut,
	(FoyerSceneActionCallback) &ProgressEaseOutIn,
	(FoyerSceneActionCallback) &ProgressElapsed,
	(FoyerSceneActionCallback) &ProgressAngle,
};

// STUB: LEGORACERS2 0x0045cab0
LegoU32 FoyerSceneScheduledAction::Dispatch(LegoFloat p_elapsed)
{
	return g_foyerSceneActionCallbacks[m_callbackIndex](this, NULL, p_elapsed * m_callbackScale);
}

// STUB: LEGORACERS2 0x0045cb70
void __fastcall UpdateFoyerSceneScheduledActionProgress(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_value)
{
	LegoFloat output = 0.0f;
	if (p_action->m_interpolationIndex < 8) {
		output = ((
			FoyerSceneProgressCallback*
		) &g_foyerSceneActionCallbacks[10])[p_action->m_interpolationIndex](p_action, NULL, p_value);
	}
	if (output < 20.0f) {
		output = 20.0f;
	}
	else if (output > 100.0f) {
		output = 100.0f;
	}
	if (p_action->m_firstUpdate == 0) {
		output = (output - p_action->m_startValue) * 0.01f + p_action->m_startValue;
	}
	p_action->m_startValue = output;
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = output;
}

// Retail 0x0045cbe0 is byte-identical to InputZeroAxis at 0x004aeff0.
// Both ignore their inputs and return float zero with RET 4.

// STUB: LEGORACERS2 0x0045cbf0
LegoFloat __fastcall ProgressCurrentValue(FoyerSceneScheduledAction* p_action, void*, LegoFloat)
{
	return p_action->m_output;
}

// STUB: LEGORACERS2 0x0045cc00
LegoFloat __fastcall ProgressEaseIn(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_argument)
{
	LegoFloat phase;
	if (p_action->m_elapsed <= p_action->m_endThreshold) {
		phase = p_action->m_inverseEndDuration * p_action->m_elapsed;
	}
	else {
		phase = 1.0f;
	}
	return (20.0f - 14.0f * phase) * atanf(p_action->m_referenceDistance / p_argument) * (180.0f / 3.1415927410125732f);
}

// STUB: LEGORACERS2 0x0045cc50
LegoFloat __fastcall ProgressEaseOut(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_argument)
{
	LegoFloat phase;
	if (p_action->m_elapsed <= p_action->m_startTime) {
		phase = p_action->m_inverseStartDuration * p_action->m_elapsed;
	}
	else {
		phase = 1.0f;
	}
	return (20.0f - 14.0f * phase) * atanf(p_action->m_referenceDistance / p_argument) * (180.0f / 3.1415927410125732f);
}

// STUB: LEGORACERS2 0x0045cca0
LegoFloat __fastcall ProgressEaseInOut(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_argument)
{
	LegoFloat phase;
	if (p_action->m_endThreshold <= p_action->m_elapsed) {
		if (p_action->m_startTime <= p_action->m_elapsed) {
			phase = 0.0f;
		}
		else {
			phase = (p_action->m_startTime - p_action->m_elapsed) * p_action->m_inverseEndDuration;
		}
	}
	else {
		phase = 1.0f;
	}
	return (20.0f - 14.0f * phase) * atanf(p_action->m_referenceDistance / p_argument) * (180.0f / 3.1415927410125732f);
}

// STUB: LEGORACERS2 0x0045cd10
LegoFloat __fastcall ProgressEaseOutIn(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_argument)
{
	LegoFloat phase;
	if (p_action->m_startTime <= p_action->m_elapsed) {
		phase = 0.0f;
	}
	else {
		phase = (p_action->m_startTime - p_action->m_elapsed) * p_action->m_inverseStartDuration;
	}
	return (20.0f - 14.0f * phase) * atanf(p_action->m_referenceDistance / p_argument) * (180.0f / 3.1415927410125732f);
}

// STUB: LEGORACERS2 0x0045cd60
LegoFloat __fastcall ProgressAngle(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_argument)
{
	return atanf(p_action->m_referenceDistance / p_argument) * (180.0f / 3.1415927410125732f) * 6.0f;
}

// STUB: LEGORACERS2 0x0045cdb0
LegoFloat __fastcall ProgressElapsed(FoyerSceneScheduledAction* p_action, void*, LegoFloat)
{
	return p_action->m_valueAt0x28;
}

// STUB: LEGORACERS2 0x0045cdc0
LegoU32 __fastcall ApplySceneTargetOffset(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	Vector3 up = {0.0f, 1.0f, 0.0f};
	Vector3* targetPosition = (Vector3*) ((LegoU8*) p_action->m_sceneTarget + 0x58);
	Vector3* lookAtPosition = (Vector3*) ((LegoU8*) p_action->m_sceneTarget + 0x70);
	SetGameObjectLookAt(p_action->m_owner, NULL, targetPosition, lookAtPosition, &up);
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = p_action->m_output;
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045ce20
LegoU32 __fastcall InterpolateSceneTargetOffset(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	UpdateScheduledActionTarget(p_action, NULL, p_elapsed);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	Vector3* targetPosition = (Vector3*) ((LegoU8*) p_action->m_sceneTarget + 0x58);
	SetGameObjectLookAt(p_action->m_owner, NULL, targetPosition, (Vector3*) ((LegoU8*) p_action + 0xac), &up);
	Vector3 delta = *(Vector3*) ((LegoU8*) p_action + 0xac);
	delta.m_x -= targetPosition->m_x;
	delta.m_y -= targetPosition->m_y;
	delta.m_z -= targetPosition->m_z;
	UpdateFoyerSceneScheduledActionProgress(
		p_action,
		NULL,
		sqrtf(delta.m_x * delta.m_x + delta.m_y * delta.m_y + delta.m_z * delta.m_z)
	);
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045ced0
LegoU32 __fastcall ApplyTimedSceneTargetOffset(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	Vector3 target = *(Vector3*) ((LegoU8*) p_action->m_sceneTarget + 0x58);
	Vector3* direction =
		UpdateScheduledPathPoint(p_action->m_source, NULL, p_action->m_phaseScale * p_action->m_elapsed);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	SetGameObjectLookAt(p_action->m_owner, NULL, &target, direction, &up);
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = p_action->m_output;
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045cf70
LegoU32 __fastcall UpdateSceneTargetDistance(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	Vector3 targetPosition;
	Vector3 up = {0.0f, 1.0f, 0.0f};
	if (p_action->m_state == 8) {
		UpdateScheduledActionFinalCamera(p_action, NULL);
		return UpdateScheduledActionState(p_action, NULL, p_elapsed);
	}
	UpdateScheduledActionTarget(p_action, NULL, p_elapsed);
	UpdateScheduledActionRotation(p_action, NULL, &targetPosition, p_elapsed);
	void* taskManager = GetControllerTaskManager();
	LegoU8* taskState = *(LegoU8**) ((LegoU8*) taskManager + 0x58);
	LegoU32 taskStateId = *(LegoU32*) (taskState + 4);
	if (taskStateId == 1) {
		if (IsScheduledActionOccluded(p_action, NULL, &targetPosition) != 0) {
			p_action->m_state = 8;
			p_action->m_firstUpdate = 1;
			p_action->m_endTime += 2.0f;
			UpdateScheduledActionFinalCamera(p_action, NULL);
			return UpdateScheduledActionState(p_action, NULL, p_elapsed);
		}
	}
	SetGameObjectLookAt(p_action->m_owner, NULL, &targetPosition, (Vector3*) ((LegoU8*) p_action + 0xac), &up);
	Vector3 delta = targetPosition;
	Vector3* offset = (Vector3*) ((LegoU8*) p_action + 0xac);
	delta.m_x -= offset->m_x;
	delta.m_y -= offset->m_y;
	delta.m_z -= offset->m_z;
	UpdateFoyerSceneScheduledActionProgress(
		p_action,
		NULL,
		sqrtf(delta.m_x * delta.m_x + delta.m_y * delta.m_y + delta.m_z * delta.m_z)
	);
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045d0a0
LegoU32 __fastcall UpdateSceneTargetDistanceFromSource(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	Vector3 sourcePosition;
	Vector3 targetPosition;
	Vector3 up = {0.0f, 1.0f, 0.0f};
	UpdateScheduledActionTarget(p_action, NULL, p_elapsed);
	UpdateScheduledActionRotation(p_action, NULL, &sourcePosition, p_elapsed);
	SetGameObjectLookAt(p_action->m_owner, NULL, &sourcePosition, (Vector3*) ((LegoU8*) p_action + 0xac), &up);
	targetPosition = *(Vector3*) ((LegoU8*) p_action + 0xac);
	targetPosition.m_x -= sourcePosition.m_x;
	targetPosition.m_y -= sourcePosition.m_y;
	targetPosition.m_z -= sourcePosition.m_z;
	UpdateFoyerSceneScheduledActionProgress(
		p_action,
		NULL,
		sqrtf(
			targetPosition.m_x * targetPosition.m_x + targetPosition.m_y * targetPosition.m_y +
			targetPosition.m_z * targetPosition.m_z
		)
	);
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045d160
LegoU32 __fastcall InterpolateSceneTargetPosition(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	void* targetTransform = *(void**) ((LegoU8*) p_action + 0x30);
	void** targetVtable = *(void***) targetTransform;
	Vector3 origin =
		*(Vector3*) ((const Vector3*(__fastcall*) (void*, void*) ) targetVtable[0x14 / 4])(targetTransform, NULL);
	Vector3* pointOnPath =
		UpdateScheduledPathPoint(p_action->m_source, NULL, p_action->m_phaseScale * p_action->m_elapsed);
	Vector3 direction = *pointOnPath;
	direction.m_x -= origin.m_x;
	direction.m_y -= origin.m_y;
	direction.m_z -= origin.m_z;
	LegoFloat distance = direction.Normalize();
	direction.m_x = pointOnPath->m_x + direction.m_x * (p_action->m_progress * distance);
	direction.m_y = pointOnPath->m_y + direction.m_y * (p_action->m_progress * distance);
	direction.m_z = pointOnPath->m_z + direction.m_z * (p_action->m_progress * distance);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	SetGameObjectLookAt(p_action->m_owner, NULL, &origin, &direction, &up);
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = p_action->m_output;
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045d270
LegoU32 __fastcall GetFinalSceneActionState(FoyerSceneScheduledAction*, void*, LegoFloat)
{
	return 2;
}

// STUB: LEGORACERS2 0x0045d280
LegoU32 __fastcall ApplyTimedSceneTargetPosition(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	UpdateScheduledActionTarget(p_action, NULL, p_elapsed);
	void* target = p_action->m_sceneTarget;
	void** vtable = *(void***) target;
	Vector3 targetPosition;
	if (((LegoU32(__fastcall*)(void*, void*)) vtable[0x54 / 4])(target, NULL) != 0) {
		LegoFloat amount = GetSceneTargetProgress(target, NULL, (const Vector3*) ((LegoU8*) p_action + 0x94));
		LegoFloat blend = *(LegoFloat*) ((LegoU8*) p_action + 0xfc) * amount;
		*(LegoFloat*) ((LegoU8*) p_action + 0xf4) = blend;
		LegoFloat* current = (LegoFloat*) ((LegoU8*) p_action + 0xf8);
		*current = (*current - blend) * p_elapsed * 2.0f + blend;
		targetPosition = *UpdateScheduledPathPoint(p_action->m_target, NULL, *current);
	}
	else {
		targetPosition =
			*UpdateScheduledPathPoint(p_action->m_target, NULL, p_action->m_phaseScale * p_action->m_elapsed);
	}
	Vector3 up = {0.0f, 1.0f, 0.0f};
	SetGameObjectLookAt(p_action->m_owner, NULL, &targetPosition, (Vector3*) ((LegoU8*) p_action + 0xac), &up);
	Vector3 delta = *(Vector3*) ((LegoU8*) p_action + 0xac);
	delta.m_x -= targetPosition.m_x;
	delta.m_y -= targetPosition.m_y;
	delta.m_z -= targetPosition.m_z;
	UpdateFoyerSceneScheduledActionProgress(
		p_action,
		NULL,
		sqrtf(delta.m_x * delta.m_x + delta.m_y * delta.m_y + delta.m_z * delta.m_z)
	);
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}

// STUB: LEGORACERS2 0x0045d3b0
LegoU32 __fastcall ComposeSceneTargetRotation(FoyerSceneScheduledAction* p_action, void*, LegoFloat p_elapsed)
{
	Vector3* first = UpdateScheduledPathPoint(p_action->m_target, NULL, p_action->m_elapsed * p_action->m_phaseScale);
	Vector3* second = UpdateScheduledPathPoint(p_action->m_source, NULL, p_action->m_elapsed * p_action->m_phaseScale);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	SetGameObjectLookAt(p_action->m_owner, NULL, first, second, &up);
	*(LegoFloat*) ((LegoU8*) p_action->m_owner + 0x1a4) = p_action->m_output;
	return UpdateScheduledActionState(p_action, NULL, p_elapsed);
}
