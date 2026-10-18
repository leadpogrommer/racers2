#include "audiochannelreload.h"
#include "audioframes.h"
#include "audioprovider.h"
#include "audioservice.h"
#include "diagnostics.h"
#include "vector3.h"

#include <math.h>

// GLOBAL: LEGORACERS2 0x006e47e8
AudioService* g_threeDimensionalUpdateService;

// GLOBAL: LEGORACERS2 0x006e47f4
AudioService* g_stereoUpdateService;

// GLOBAL: LEGORACERS2 0x006e47f8
LegoFloat g_stereoPanOrigin;

// GLOBAL: LEGORACERS2 0x006e4800
AudioService* g_monoUpdateService;

struct AudioSpatialTransformDispatch {
	typedef Vector3*(__fastcall* GetVector)(void*, void*);
	typedef LegoFloat*(__fastcall* GetBasis)(void*, void*);

	void* m_slots0x00[5];
	GetVector m_getPosition; // +0x14
	GetBasis m_getBasis;     // +0x18
};

struct AudioSpatialTransformView {
	const AudioSpatialTransformDispatch* m_vtable;
	LegoU8 m_unknown0x04[0xbc];
	Vector3 m_worldPosition; // +0xc0
};

struct AudioSpatialObjectView {
	LegoU8 m_unknown0x00[0x54];
	AudioSpatialTransformView* m_transform; // +0x54
};

DECOMP_STATIC_ASSERT(offsetof(AudioSpatialObjectView, m_transform) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(AudioSpatialTransformView, m_worldPosition) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(AudioSpatialTransformDispatch, m_getPosition) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(AudioSpatialTransformDispatch, m_getBasis) == 0x18);

// Helper declarations are below their recovered addresses in the retail file,
// but forward declarations keep the callback bodies readable here.
static void CopyAudioSpatialWorldPosition(const void* p_object, Vector3* p_position);
static void CopyAudioSpatialBasis(const void* p_object, Vector3* p_xAxis, Vector3* p_yAxis, Vector3* p_zAxis);
static void __fastcall FindNearestAudioListener(
	AudioService* p_service,
	void* p_edx,
	const Vector3* p_position,
	AudioDeviceRecord** p_listener
);
static void __fastcall EnsureThreeDimensionalBuffer(
	AudioBackendStateView* p_provider,
	void* p_edx,
	AudioSourceRecord* p_source
);
static LegoS32 ConvertAudioVolume(LegoFloat p_value, LegoFloat p_scale);

// STUB: LEGORACERS2 0x004b0ed0
static void __fastcall FindNearestAudioListener(
	AudioService* p_service,
	void*,
	const Vector3* p_position,
	AudioDeviceRecord** p_listener
)
{
	LegoFloat nearestDistance = 1000000000.0f;
	for (LegoS32 i = 0; i < 8; ++i) {
		AudioDeviceRecord* listener = &p_service->m_devices[i];
		if (listener->m_active == 0 || listener->m_unk0x04 != 0 || listener->m_listenerId == 0) {
			continue;
		}

		Vector3 listenerPosition;
		GetAudioGameObjectPosition((GameObject*) listener->m_listenerId, &listenerPosition);
		LegoFloat dx = p_position->m_x - listenerPosition.m_x;
		LegoFloat dy = p_position->m_y - listenerPosition.m_y;
		LegoFloat dz = p_position->m_z - listenerPosition.m_z;
		LegoFloat distanceSquared = dx * dx + dy * dy + dz * dz;
		if (distanceSquared < nearestDistance) {
			*p_listener = listener;
			nearestDistance = distanceSquared;
		}
	}
}

// STUB: LEGORACERS2 0x004efee0
void __fastcall UpdateThreeDimensionalAudioSource(AudioBackendStateView* p_provider, void*, AudioSourceRecord* p_source)
{
	IDirectSoundBuffer* playbackBuffer = p_source->m_playbackBuffer;
	if (playbackBuffer == NULL) {
		return;
	}

	if (g_threeDimensionalUpdateService == NULL) {
		g_threeDimensionalUpdateService = GetAudioService(TRUE);
	}
	if (p_source->m_unk0xc4 == 0) {
		EnsureThreeDimensionalBuffer(p_provider, NULL, p_source);
	}
	IDirectSound3DBuffer* spatialBuffer = p_source->m_spatialBuffer;
	if ((p_source->m_unk0xb8 & 0x80) == 0) {
		if (spatialBuffer == NULL) {
			return;
		}
		AudioDeviceRecord* listener = &g_threeDimensionalUpdateService->m_devices[0];
		if (listener->m_active != 0) {
			Vector3 listenerPosition;
			GetAudioGameObjectPosition((GameObject*) listener->m_listenerId, &listenerPosition);
			spatialBuffer
				->SetPosition(listenerPosition.m_x, listenerPosition.m_y, listenerPosition.m_z, DS3D_IMMEDIATE);
			Vector3 listenerVelocity;
			CopyAudioSpatialWorldPosition((void*) listener->m_listenerId, &listenerVelocity);
			spatialBuffer
				->SetVelocity(listenerVelocity.m_x, listenerVelocity.m_y, listenerVelocity.m_z, DS3D_IMMEDIATE);
		}
	}
	else {
		if (spatialBuffer == NULL) {
			return;
		}

		Vector3 sourcePosition;
		if (p_source->m_unk0x48 == 0 || !HasAudioGameObjectPosition((GameObject*) p_source->m_unk0x44)) {
			sourcePosition.m_x = *(LegoFloat*) &p_source->m_unk0x10;
			sourcePosition.m_y = *(LegoFloat*) &p_source->m_unk0x14;
			sourcePosition.m_z = *(LegoFloat*) &p_source->m_unk0x18;
		}
		else {
			GetAudioGameObjectPosition((GameObject*) p_source->m_unk0x44, &sourcePosition);
		}

		AudioDeviceRecord* listener = NULL;
		FindNearestAudioListener(g_threeDimensionalUpdateService, NULL, &sourcePosition, &listener);
		if (listener != NULL) {
			Vector3 listenerPosition;
			GetAudioGameObjectPosition((GameObject*) listener->m_listenerId, &listenerPosition);
			if (listener->m_unk0x08 == 0) {
				spatialBuffer->SetPosition(sourcePosition.m_x, sourcePosition.m_y, sourcePosition.m_z, DS3D_IMMEDIATE);
				Vector3 relative;
				relative.m_x = sourcePosition.m_x - listenerPosition.m_x;
				relative.m_y = sourcePosition.m_y - listenerPosition.m_y;
				relative.m_z = sourcePosition.m_z - listenerPosition.m_z;
				LegoFloat fade = (GetAudioVectorLength(&relative, NULL) - p_source->m_defaultRange) * 0.02f;
				if (!(fade <= 1.0f)) {
					fade = 1.0f;
				}
				else if (!(fade >= 0.0f)) {
					fade = 0.0f;
				}
				p_source->m_unk0xb4 = (1.0f - fade) * p_source->m_unk0xb4;
			}
			else {
				Vector3 primaryPosition;
				GetAudioGameObjectPosition(
					(GameObject*) g_threeDimensionalUpdateService->m_devices[0].m_listenerId,
					&primaryPosition
				);
				Vector3 relative;
				relative.m_x = sourcePosition.m_x - listenerPosition.m_x;
				relative.m_y = sourcePosition.m_y - listenerPosition.m_y;
				relative.m_z = sourcePosition.m_z - listenerPosition.m_z;
				Vector3 transformed;
				transformed.m_x = primaryPosition.m_x + relative.m_x;
				transformed.m_y = primaryPosition.m_y + relative.m_y;
				transformed.m_z = primaryPosition.m_z + relative.m_z;
				spatialBuffer->SetPosition(transformed.m_x, transformed.m_y, transformed.m_z, DS3D_IMMEDIATE);
				LegoFloat span = p_source->m_defaultFalloff + p_source->m_defaultRange;
				LegoFloat fade = (span - GetAudioVectorLength(&relative, NULL)) / span;
				if (!(fade <= 1.0f)) {
					fade = 1.0f;
				}
				else if (!(fade >= 0.0f)) {
					fade = 0.0f;
				}
				p_source->m_unk0xb4 = fade * p_source->m_unk0xb4;
			}
			if (p_source->m_unk0x48 == 0) {
				Vector3 velocity;
				velocity.m_x = *(LegoFloat*) &p_source->m_unk0x04;
				velocity.m_y = *(LegoFloat*) &p_source->m_unk0x08;
				velocity.m_z = *(LegoFloat*) &p_source->m_unk0x0c;
				spatialBuffer->SetVelocity(velocity.m_x, velocity.m_y, velocity.m_z, DS3D_IMMEDIATE);
			}
			else if (HasAudioGameObjectPosition((GameObject*) p_source->m_unk0x44)) {
				if ((p_source->m_unk0xb8 & 0x40) != 0) {
					Vector3 velocity;
					CopyAudioSpatialWorldPosition((void*) p_source->m_unk0x44, &velocity);
					spatialBuffer->SetVelocity(velocity.m_x, velocity.m_y, velocity.m_z, DS3D_IMMEDIATE);
				}
				else if (g_threeDimensionalUpdateService->m_devices[0].m_active != 0) {
					Vector3 velocity;
					CopyAudioSpatialWorldPosition(
						(void*) g_threeDimensionalUpdateService->m_devices[0].m_listenerId,
						&velocity
					);
					spatialBuffer->SetVelocity(velocity.m_x, velocity.m_y, velocity.m_z, DS3D_IMMEDIATE);
				}
			}
		}
	}

	LegoFloat volume = g_threeDimensionalUpdateService->m_volumeB * p_source->m_unk0xb4 * p_source->m_unk0x84;
	playbackBuffer->SetVolume(ConvertAudioVolume(volume, 10.0f));
}

// STUB: LEGORACERS2 0x004f02f0
static void __fastcall EnsureThreeDimensionalBuffer(AudioBackendStateView*, void*, AudioSourceRecord* p_source)
{
	if (p_source->m_unk0x44 != 0) {
		IDirectSound3DBuffer* buffer = NULL;
		if (p_source->m_playbackBuffer->QueryInterface(IID_IDirectSound3DBuffer, (void**) &buffer) == DS_OK) {
			p_source->m_spatialBuffer = buffer;
			if (p_source->m_defaultRange < p_source->m_defaultFalloff || p_source->m_defaultRange < 0.0f ||
				p_source->m_defaultFalloff < 0.0f) {
				PrintDiagnostic(
					"Sound Distance weirdness: min %f \tmax %f OBJECT %s",
					(double) p_source->m_defaultFalloff,
					(double) p_source->m_defaultRange,
					p_source->m_unkName0x4c
				);
			}
			buffer->SetMinDistance(p_source->m_defaultFalloff, DS3D_IMMEDIATE);
			buffer->SetMaxDistance(p_source->m_defaultRange, DS3D_IMMEDIATE);
		}
	}
}

// STUB: LEGORACERS2 0x004f0730
void __fastcall UpdateStereoAudioSource(AudioBackendStateView*, void*, AudioSourceRecord* p_source)
{
	IDirectSoundBuffer* playbackBuffer = p_source->m_playbackBuffer;
	if (playbackBuffer == NULL) {
		return;
	}
	if (g_stereoUpdateService == NULL) {
		g_stereoUpdateService = GetAudioService(TRUE);
	}
	if (p_source->m_unk0x40 == 1) {
		playbackBuffer->SetVolume(-8000);
		return;
	}

	LegoFloat volume;
	if ((p_source->m_unk0xb8 & 0x80) == 0) {
		volume = g_stereoUpdateService->m_volumeB * p_source->m_unk0x84;
	}
	else {
		Vector3 sourcePosition;
		if (p_source->m_unk0x48 == 0 || !HasAudioGameObjectPosition((GameObject*) p_source->m_unk0x44)) {
			sourcePosition.m_x = *(LegoFloat*) &p_source->m_unk0x10;
			sourcePosition.m_y = *(LegoFloat*) &p_source->m_unk0x14;
			sourcePosition.m_z = *(LegoFloat*) &p_source->m_unk0x18;
		}
		else {
			GetAudioGameObjectPosition((GameObject*) p_source->m_unk0x44, &sourcePosition);
		}

		AudioDeviceRecord* listener = NULL;
		FindNearestAudioListener(g_stereoUpdateService, NULL, &sourcePosition, &listener);
		if (listener == NULL) {
			return;
		}

		Vector3 listenerPosition;
		GetAudioGameObjectPosition((GameObject*) listener->m_listenerId, &listenerPosition);
		Vector3 listenerX;
		Vector3 listenerY;
		Vector3 listenerZ;
		CopyAudioSpatialBasis((void*) listener->m_listenerId, &listenerX, &listenerY, &listenerZ);

		Vector3 relative;
		relative.m_x = sourcePosition.m_x - listenerPosition.m_x;
		relative.m_y = sourcePosition.m_y - listenerPosition.m_y;
		relative.m_z = sourcePosition.m_z - listenerPosition.m_z;
		LegoFloat panX = relative.m_x * listenerX.m_x + relative.m_y * listenerX.m_y + relative.m_z * listenerX.m_z +
						 g_stereoPanOrigin;
		LegoFloat panY = relative.m_x * listenerY.m_x + relative.m_y * listenerY.m_y + relative.m_z * listenerY.m_z +
						 g_stereoPanOrigin;
		LegoFloat panZ = relative.m_x * listenerZ.m_x + relative.m_y * listenerZ.m_y + relative.m_z * listenerZ.m_z +
						 g_stereoPanOrigin;
		Vector3 pan;
		pan.m_x = panX;
		pan.m_y = panY;
		pan.m_z = panZ;
		LegoFloat distance = GetAudioVectorLength(&pan, NULL);
		if (panX != 0.0f && distance != 0.0f) {
			LegoFloat inverseDistance = 1.0f / distance;
			panX *= inverseDistance;
			panY *= inverseDistance;
			panZ *= inverseDistance;
		}
		playbackBuffer->SetPan((LONG) panX);

		LegoFloat distanceFade =
			(p_source->m_defaultFalloff + p_source->m_defaultRange - GetAudioVectorLength(&relative, NULL)) /
			(p_source->m_defaultFalloff + p_source->m_defaultRange);
		if (!(distanceFade <= 1.0f)) {
			distanceFade = 1.0f;
		}
		else if (!(distanceFade >= 0.0f)) {
			distanceFade = 0.0f;
		}
		volume = g_stereoUpdateService->m_volumeB * p_source->m_unk0xb4 * p_source->m_unk0x84 * distanceFade;
	}
	playbackBuffer->SetVolume(ConvertAudioVolume(volume, 4.0f));
}

// STUB: LEGORACERS2 0x004f0fd0
void __fastcall UpdateMonoAudioSource(AudioBackendStateView*, void*, AudioSourceRecord* p_source)
{
	IDirectSoundBuffer* playbackBuffer = p_source->m_playbackBuffer;
	if (playbackBuffer == NULL) {
		return;
	}
	if (g_monoUpdateService == NULL) {
		g_monoUpdateService = GetAudioService(TRUE);
	}
	if (p_source->m_unk0x40 == 1) {
		playbackBuffer->SetVolume(-8000);
		return;
	}

	LegoFloat volume;
	if ((p_source->m_unk0xb8 & 0x80) == 0) {
		volume = g_monoUpdateService->m_volumeB * p_source->m_unk0x84;
	}
	else {
		Vector3 sourcePosition;
		if (p_source->m_unk0x48 == 0 || !HasAudioGameObjectPosition((GameObject*) p_source->m_unk0x44)) {
			sourcePosition.m_x = *(LegoFloat*) &p_source->m_unk0x10;
			sourcePosition.m_y = *(LegoFloat*) &p_source->m_unk0x14;
			sourcePosition.m_z = *(LegoFloat*) &p_source->m_unk0x18;
		}
		else {
			GetAudioGameObjectPosition((GameObject*) p_source->m_unk0x44, &sourcePosition);
		}

		AudioDeviceRecord* listener = NULL;
		FindNearestAudioListener(g_monoUpdateService, NULL, &sourcePosition, &listener);
		Vector3 listenerPosition;
		listenerPosition.m_x = 0.0f;
		listenerPosition.m_y = 0.0f;
		listenerPosition.m_z = 0.0f;
		if (listener != NULL) {
			GetAudioGameObjectPosition((GameObject*) listener->m_listenerId, &listenerPosition);
		}
		Vector3 relative;
		relative.m_x = sourcePosition.m_x - listenerPosition.m_x;
		relative.m_y = sourcePosition.m_y - listenerPosition.m_y;
		relative.m_z = sourcePosition.m_z - listenerPosition.m_z;
		LegoFloat fade = (200.0f - GetAudioVectorLength(&relative, NULL)) * 0.005f;
		if (!(fade <= 1.0f)) {
			fade = 1.0f;
		}
		else if (!(fade >= 0.0f)) {
			fade = 0.0f;
		}
		volume = g_monoUpdateService->m_volumeB * p_source->m_unk0xb4 * p_source->m_unk0x84 * fade;
	}
	playbackBuffer->SetVolume(ConvertAudioVolume(volume, 4.0f));
}

// STUB: LEGORACERS2 0x004f1240
static void __cdecl CopyAudioSpatialWorldPosition(const void* p_object, Vector3* p_position)
{
	if (p_object != NULL) {
		AudioSpatialTransformView* transform = ((AudioSpatialObjectView*) p_object)->m_transform;
		*p_position = transform->m_worldPosition;
	}
}

// STUB: LEGORACERS2 0x004f1270
static void __cdecl CopyAudioSpatialBasis(const void* p_object, Vector3* p_xAxis, Vector3* p_yAxis, Vector3* p_zAxis)
{
	if (p_object != NULL) {
		AudioSpatialTransformView* transform = ((AudioSpatialObjectView*) p_object)->m_transform;
		LegoFloat* matrix = transform->m_vtable->m_getBasis(transform, NULL);
		p_xAxis->m_x = matrix[0];
		p_xAxis->m_y = matrix[3];
		p_xAxis->m_z = matrix[6];
		p_yAxis->m_x = matrix[1];
		p_yAxis->m_y = matrix[4];
		p_yAxis->m_z = matrix[7];
		p_zAxis->m_x = matrix[2];
		p_zAxis->m_y = matrix[5];
		p_zAxis->m_z = matrix[8];
	}
}

static LegoS32 ConvertAudioVolume(LegoFloat p_value, LegoFloat p_scale)
{
	LegoFloat gain = MapGameAudioGain(p_value);
	return (LegoS32) ((gain * p_scale - p_scale) * 1000.0f);
}
