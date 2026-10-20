#include "decomp.h"
#include "types.h"

struct ControllerTaskPoint {
	LegoFloat x;
	LegoFloat y;
	LegoFloat z;
};

typedef const ControllerTaskPoint*(__fastcall* GetControllerTaskPoint)(void*, void*);
typedef LegoBool32(__fastcall* TestControllerTaskPoint)(void*, void*, const ControllerTaskPoint*);
typedef LegoBool32(__fastcall* HasControllerTaskOffset)(void*, void*, LegoU32);

struct ControllerTaskCandidateView {
	void** m_vtable;
	undefined m_gap0x04[0x2c];
	ControllerTaskPoint m_positionOffset; // +0x30
};

struct ControllerTaskTargetView {
	void** m_vtable;
	undefined m_gap0x04[0x54];
	ControllerTaskPoint m_position; // +0x58
};

static LegoFloat MeasureControllerTaskTargetDistance(const ControllerTaskPoint&, const ControllerTaskPoint&);
static LegoU32 ClassifyControllerTaskTargetDistance(LegoFloat);

// STUB: LEGORACERS2 0x0045bdd0
LegoU8* __fastcall FindControllerTaskCandidateTarget(LegoU8* p_state, void*, LegoU8* p_task)
{
	LegoU32 targetCount = *(LegoU32*) (p_state + 0x508);
	ControllerTaskCandidateView* candidate = *(ControllerTaskCandidateView**) (p_task + 0x34);
	GetControllerTaskPoint getPoint = (GetControllerTaskPoint) candidate->m_vtable[0x14 / 4];
	HasControllerTaskOffset hasOffset = (HasControllerTaskOffset) candidate->m_vtable[0];
	const ControllerTaskPoint* candidatePosition = getPoint(candidate, NULL);
	LegoFloat bestDistance = 12000.0f;
	LegoFloat bestOffsetDistance = bestDistance;
	LegoU8* bestTarget = NULL;
	LegoU32 i;
	for (i = 0; i < targetCount; ++i) {
		ControllerTaskTargetView* target = *(ControllerTaskTargetView**) (p_state + 0x408 + i * 4);
		TestControllerTaskPoint testPoint = (TestControllerTaskPoint) target->m_vtable[0x50 / 4];
		ControllerTaskPoint adjusted = *candidatePosition;
		LegoFloat distance;
		LegoFloat offsetDistance;
		if (!testPoint(target, NULL, candidatePosition)) {
			continue;
		}
		if (hasOffset(candidate, NULL, 0)) {
			adjusted.x += candidate->m_positionOffset.x * 1.5f;
			adjusted.y += candidate->m_positionOffset.y * 1.5f;
			adjusted.z += candidate->m_positionOffset.z * 1.5f;
			if (!testPoint(target, NULL, &adjusted)) {
				continue;
			}
		}
		distance = MeasureControllerTaskTargetDistance(target->m_position, *candidatePosition);
		if (distance < bestDistance) {
			offsetDistance = MeasureControllerTaskTargetDistance(target->m_position, adjusted);
			bestDistance = distance;
			bestOffsetDistance = offsetDistance;
			bestTarget = (LegoU8*) target;
		}
	}
	*(LegoU32*) (p_state + 0xc18) = ClassifyControllerTaskTargetDistance(bestDistance);
	*(LegoU32*) (p_state + 0xc1c) = ClassifyControllerTaskTargetDistance(bestOffsetDistance);
	return bestTarget;
}

// STUB: LEGORACERS2 0x0045c1d0
static LegoU32 ClassifyControllerTaskTargetDistance(LegoFloat p_distanceSquared)
{
	LegoFloat limit = 12000.0f;
	if (p_distanceSquared < limit * 0.6f) {
		return 0;
	}
	return p_distanceSquared >= limit * 0.85f ? 2 : 1;
}

// STUB: LEGORACERS2 0x0045d460
static LegoFloat __stdcall GetControllerTaskTargetMetric(LegoU32 p_target)
{
	if (p_target == 0) {
		return 0.0f;
	}
	return *(LegoFloat*) (*(LegoU32*) (p_target + 4) + 0x50);
}

static LegoFloat MeasureControllerTaskTargetDistance(
	const ControllerTaskPoint& p_first,
	const ControllerTaskPoint& p_second
)
{
	LegoFloat x = p_first.x - p_second.x;
	LegoFloat y = p_first.y - p_second.y;
	LegoFloat z = p_first.z - p_second.z;
	return x * x + y * y + z * z;
}
