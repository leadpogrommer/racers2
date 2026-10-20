#ifndef LR2_FOYERSCENEACTIONHELPERS_H
#define LR2_FOYERSCENEACTIONHELPERS_H

#include "foyerscenescheduledaction.h"
#include "types.h"
#include "vector3.h"

struct GameObject;

void __fastcall MarkReverbStateDirty(GameObject*, void*);
void __fastcall UpdateScheduledOwnerVelocity(GameObject*, void*);
LegoFloat __fastcall GetSceneTargetProgress(void*, void*, const Vector3*);
LegoU32 __fastcall UpdateScheduledActionState(FoyerSceneScheduledAction*, void*, LegoFloat);
void __fastcall InitializeFoyerSceneScheduledActionRecord(FoyerSceneScheduledAction*, void*);
void __fastcall UpdateScheduledActionTarget(FoyerSceneScheduledAction*, void*, LegoFloat);
void __fastcall UpdateScheduledActionRotation(FoyerSceneScheduledAction*, void*, Vector3*, LegoFloat);
void __fastcall UpdateScheduledActionFinalCamera(FoyerSceneScheduledAction*, void*);
LegoBool32 __fastcall IsScheduledActionOccluded(FoyerSceneScheduledAction*, void*, const Vector3*);
Vector3* __fastcall UpdateScheduledPathPoint(void*, void*, LegoFloat);

#endif // LR2_FOYERSCENEACTIONHELPERS_H
