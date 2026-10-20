#ifndef LR2_CHARACTERVISUALBEHAVIOR_H
#define LR2_CHARACTERVISUALBEHAVIOR_H

#include "game/character/charactervisual.h"
#include "math/vector3.h"

void __fastcall SetCharacterVisualMode(CharacterVisual*, void*, LegoU32);
void __fastcall InitializeCharacterVisualFootstepAudio(CharacterVisual*, void*);
void __fastcall UpdateCharacterDrivingAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall UpdateCharacterIdleAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall UpdateCharacterSelectionAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall UpdateCharacterRunningAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall UpdateCharacterHiddenAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall UpdateCharacterPassiveAnimation(CharacterVisual*, void*, LegoFloat);
void __fastcall ApplyCharacterBodyMotion(GameObject*, void*, LegoFloat, LegoU32);
void __fastcall TurnCharacterBody(GameObject*, void*, LegoFloat);
void __fastcall UpdateCharacterFootstepAudio(GameObject*, void*, LegoU32);
void __fastcall GetCharacterBodyHeadTransform(GameObject*, void*, Vector3*, Quaternion*);
void __fastcall GetCharacterSelectionPosition(void*, void*, Vector3*);
LegoBool32 __fastcall GetCharacterRaceLookTarget(void*, void*, Vector3*);
void __fastcall AddCharacterFootstepTrail(
	void*,
	void*,
	const Vector3*,
	const Vector3*,
	GameLifecycleComponent*,
	LegoU32
);

#endif
