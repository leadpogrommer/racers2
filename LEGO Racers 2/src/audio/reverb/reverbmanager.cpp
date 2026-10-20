#include "audio/reverb/reverbmanager.h"

#include "audio/service/audioservice.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/session/gamecontrollersession.h"
#include "scene/camera/gamecameraregistry.h"
#include "scene/terrain/terrain.h"

#include <new.h>

// GLOBAL: LEGORACERS2 0x0054225c
LegoU32 g_reverbDefaultState = 1;

static LegoU32 __fastcall UpdateReverbManager(GameObject* p_manager, void*, LegoU32 p_delta)
{
	return ((ReverbManager*) p_manager)->Update(p_delta);
}

namespace
{
const Vector3* GetCameraPosition(GameObject* p_camera)
{
	void* component = (LegoU8*) p_camera + 0x58;
	const void* const* table = *(const void* const**) component;
	typedef const Vector3*(__fastcall * GetPosition)(void*, void*);
	return ((GetPosition) table[0x14 / 4])(component, NULL);
}
} // namespace

// Retail ABI function-pointer table: 0x00531248, twenty slots.
const GameObjectDispatchView g_reverbManagerDispatch = {&GameObjectBaseDestroy,      &GameObjectBaseApplyChildTransform,
														&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
														&GameObjectBaseInitialize,   &GameObjectBaseTrue,
														&UpdateReverbManager,        &GameObjectBaseRemove,
														&GameObjectBaseZero,         &GameObjectBaseZero,
														&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
														&GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
														&GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
														&GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
														&GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument};

// STUB: LEGORACERS2 0x0043d720
GameObject* __fastcall ConstructReverbManager(void* p_storage, void*, GameObjectTemplate* p_template)
{
	ReverbManager* manager = (ReverbManager*) p_storage;
	new (manager) GameObject(0xd00, p_template, NULL);
	manager->m_vtable = &g_reverbManagerDispatch;
	manager->Reset();
	return manager;
}

// FUNCTION: LEGORACERS2 0x0043d750
void ReverbManager::Reset()
{
	m_volumeCount = 0;
	m_defaultFirstParameter = 0.0f;
	m_defaultSecondParameter = 0.0f;
	m_defaultState = g_reverbDefaultState;
	for (LegoS32 index = 0; index < 2; ++index) {
		m_selectedVolumes[index] = NULL;
		m_cameras[index] = NULL;
	}
	m_dirty = 1;
	m_currentState = g_reverbDefaultState;
	m_countdown = 8;
}

// STUB: LEGORACERS2 0x0043d7b0
void ReverbManager::Configure(LegoU32 p_state, LegoFloat p_first, LegoFloat p_second)
{
	m_defaultState = p_state;
	m_defaultFirstParameter = p_first;
	m_defaultSecondParameter = p_second;
	GetGameManager();
	LegoS32 count = CountActiveGameCameras();
	for (LegoS32 index = 0; index < count; ++index) {
		m_cameras[index] = FindActiveGameCamera(GetGameManager(), NULL, index);
	}
	m_selectedVolumes[0] = NULL;
	m_selectedVolumes[1] = NULL;
	m_countdown = 8;
	m_dirty = 1;
}

// FUNCTION: LEGORACERS2 0x0043d830
LegoBool32 ReverbManager::CameraHasNoVolume(GameObject* p_camera) const
{
	for (LegoS32 index = 0; index < 2; ++index) {
		if (m_cameras[index] == NULL) {
			return FALSE;
		}
		if (m_cameras[index] == p_camera) {
			return m_selectedVolumes[index] == NULL;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x0043d870
LegoBool32 ReverbManager::HasCameraMoved(LegoS32 p_index, LegoFloat p_x, LegoFloat p_y, LegoFloat p_z) const
{
	LegoFloat dx = p_x - m_lastPositions[p_index].m_x;
	LegoFloat dy = p_y - m_lastPositions[p_index].m_y;
	LegoFloat dz = p_z - m_lastPositions[p_index].m_z;
	return dz * dz + dy * dy + dx * dx > 4.0f;
}

// STUB: LEGORACERS2 0x0043d8d0
LegoU32 ReverbManager::Update(LegoU32)
{
	if ((LegoS32) g_gameController.m_state < 3) {
		return 0;
	}
	if (m_countdown > 0) {
		--m_countdown;
		if (m_countdown != 0) {
			return 1;
		}
		m_dirty = 1;
	}
	GetGameManager();
	LegoS32 count = CountActiveGameCameras();
	for (LegoS32 index = 0; index < count; ++index) {
		GameObject* camera = FindActiveGameCamera(GetGameManager(), NULL, index);
		Vector3 position = *GetCameraPosition(camera);
		if (m_dirty != 0 || HasCameraMoved(index, position.m_x, position.m_y, position.m_z)) {
			RecomputeCameraVolume(index, &position);
		}
		else {
			Vector3 direction = position;
			direction.Subtract(&m_lastPositions[index]);
			LegoFloat distance = 0.0f;
			LegoBool32 intersects = FALSE;
			LegoS32 volumeIndex = -1;
			if (m_selectedVolumes[index] != NULL) {
				intersects = m_selectedVolumes[index]
								 ->IntersectSegment(&m_lastPositions[index], &direction, &position, &distance);
			}
			else {
				for (volumeIndex = 0; volumeIndex < m_volumeCount; ++volumeIndex) {
					if (m_volumes[volumeIndex]
							->IntersectSegment(&m_lastPositions[index], &direction, &position, &distance)) {
						intersects = TRUE;
						break;
					}
				}
			}
			if (intersects != FALSE) {
				// Retail treats an unordered distance like a non-positive one.
				if (!(distance > 0.0f)) {
					ResetCameraVolume(index, 0);
				}
				else if (volumeIndex >= 0) {
					SelectCameraVolume(index, m_volumes[volumeIndex]);
				}
			}
		}
		m_lastPositions[index] = position;
	}
	m_dirty = 0;
	return 1;
}

// STUB: LEGORACERS2 0x0043dad0
void ReverbManager::SelectCameraVolume(LegoS32 p_index, ReverbVolumeView* p_volume)
{
	if (m_selectedVolumes[p_index] != p_volume) {
		m_selectedVolumes[p_index] = p_volume;
		if (p_index == 0 && GetGameControllerSession()->m_unk0x00 != 1) {
			m_currentState = p_volume->m_unk0x60;
			AudioService* audio = GetAudioService(TRUE);
			p_volume = m_selectedVolumes[0];
			audio->m_vtable
				->m_setParameters(audio, NULL, p_volume->m_firstAudioParameter, p_volume->m_secondAudioParameter);
		}
	}
}

// STUB: LEGORACERS2 0x0043db30
void ReverbManager::ResetCameraVolume(LegoS32 p_index, LegoU32 p_force)
{
	if (p_force != 0 || m_selectedVolumes[p_index] != NULL) {
		m_selectedVolumes[p_index] = NULL;
		if (p_force != 0 || (p_index == 0 && GetGameControllerSession()->m_unk0x00 != 1)) {
			m_currentState = m_defaultState;
			AudioService* audio = GetAudioService(TRUE);
			audio->m_vtable->m_setParameters(audio, NULL, m_defaultFirstParameter, m_defaultSecondParameter);
		}
	}
}

// STUB: LEGORACERS2 0x0043dba0
void ReverbManager::DisableOutput()
{
	m_currentState = 1;
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_setParameters(audio, NULL, 0.0f, 0.0f);
}

// FUNCTION: LEGORACERS2 0x0043dbc0
void ReverbManager::RestoreOutput()
{
	ReverbVolumeView* volume = m_selectedVolumes[0];
	if (volume != NULL) {
		m_selectedVolumes[0] = NULL;
		SelectCameraVolume(0, volume);
	}
	else {
		ResetCameraVolume(0, 1);
	}
}

// FUNCTION: LEGORACERS2 0x0043dbf0
void ReverbManager::RecomputeCameraVolume(LegoS32 p_index, const Vector3* p_position)
{
	ReverbVolumeView* volume = FindVolume(p_position);
	if (volume == NULL) {
		ResetCameraVolume(p_index, 0);
	}
	else {
		SelectCameraVolume(p_index, volume);
	}
}

// FUNCTION: LEGORACERS2 0x0043dc30
void __fastcall MarkReverbStateDirty(GameObject* p_manager, void*)
{
	((ReverbManager*) p_manager)->m_dirty = 1;
}

// STUB: LEGORACERS2 0x0043dc40
ReverbVolumeView* ReverbManager::FindVolume(const Vector3* p_position) const
{
	LegoU32 underground = 0;
	Terrain* terrain = (Terrain*) GetGameControllerSession()->m_unk0x48;
	if (terrain != NULL) {
		Vector3 surface, normal;
		terrain->ProjectToSurface(p_position, &surface, &normal);
		underground = !(p_position->m_y >= surface.m_y);
	}
	for (LegoS32 index = 0; index < m_volumeCount; ++index) {
		if (m_volumes[index]->ContainsPoint(p_position, underground)) {
			return m_volumes[index];
		}
	}
	return NULL;
}
