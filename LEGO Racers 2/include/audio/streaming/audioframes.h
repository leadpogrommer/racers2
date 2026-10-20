#ifndef LR2_AUDIOFRAMES_H
#define LR2_AUDIOFRAMES_H

#include "audio/service/audioservice.h"
#include "core/strings/charbuffer24.h"
#include "game/objects/gamemanager.h"
#include "math/vector3.h"

LegoFloat __fastcall GetAudioVectorLength(const Vector3*, void*);
void __fastcall ProcessAudioSourceFrames(AudioService*, void*);
void __fastcall FindNearestAudioListener(AudioService*, void*, const Vector3*, Vector3*);
GameObject* __cdecl FindAudioGameObject(CharBuffer24 p_name);
LegoBool32 __cdecl HasAudioGameObjectPosition(GameObject*);
void __cdecl GetAudioGameObjectPosition(GameObject*, Vector3*);
#endif
