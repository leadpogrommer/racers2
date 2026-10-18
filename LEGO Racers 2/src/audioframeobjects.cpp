#include "audioframes.h"

#include <math.h>

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

// FUNCTION: LEGORACERS2 0x004af2b0
LegoFloat __fastcall GetAudioVectorLength(const Vector3* p_vector, void*)
{
	return sqrt(p_vector->m_z * p_vector->m_z + p_vector->m_y * p_vector->m_y + p_vector->m_x * p_vector->m_x);
}

// STUB: LEGORACERS2 0x004b0df0
void __fastcall FindNearestAudioListener(AudioService* p_service, void*, const Vector3* p_position, Vector3* p_result)
{
	LegoFloat nearestDistance = 900001024.0f; // 0x4e5693b4
	Vector3 nearest;
	// Retail leaves these coordinates unwritten if no eligible listener wins.
	// This path remains unresolved; no fallback position is fabricated here.
	for (LegoU32 i = 0; i < 8; ++i) {
		AudioDeviceRecord& listener = p_service->m_devices[i];
		if (listener.m_active != 0 && listener.m_unk0x04 == 0 && listener.m_listenerId != 0) {
			Vector3 position;
			GetAudioGameObjectPosition((GameObject*) listener.m_listenerId, &position);
			LegoFloat dx = p_position->m_x - position.m_x;
			LegoFloat dy = p_position->m_y - position.m_y;
			LegoFloat dz = p_position->m_z - position.m_z;
			LegoFloat distance = dx * dx + dy * dy + dz * dz;
			if (distance < nearestDistance) {
				nearest = position;
				nearestDistance = distance;
			}
		}
	}
	*p_result = nearest;
}

// FUNCTION: LEGORACERS2 0x004f11a0
GameObject* __cdecl FindAudioGameObject(CharBuffer24 p_name)
{
	return FindGameObjectByName(p_name.m_data, 1, NULL);
}

// FUNCTION: LEGORACERS2 0x004f11c0
LegoBool32 __cdecl HasAudioGameObjectPosition(GameObject* p_object)
{
	if (p_object == NULL) {
		return FALSE;
	}
	return p_object->m_lifecycle0x54 != NULL;
}

// STUB: LEGORACERS2 0x004f11e0
void __cdecl GetAudioGameObjectPosition(GameObject* p_object, Vector3* p_result)
{
	struct Dispatch {
		undefined m_unk0x00[0x14];
		const Vector3*(__fastcall* m_getPosition)(GameLifecycleComponent*, void*);
	};
	if (HasAudioGameObjectPosition(p_object)) {
		GameLifecycleComponent* body = p_object->m_lifecycle0x54;
		const Vector3* position = ((const Dispatch*) body->m_vtable)->m_getPosition(body, NULL);
		*p_result = *position;
	}
	else {
		p_result->m_x = 0;
		p_result->m_y = 0;
		p_result->m_z = 0;
	}
}
