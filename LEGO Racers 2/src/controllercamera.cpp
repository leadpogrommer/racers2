#include "controllercamera.h"

#include "controllerfoyertaskmanager.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gameobjectrenderprep.h"
#include "physicallifecycle.h"
#include "reverbmanager.h"

#include <math.h>

LegoU32 __fastcall GetControllerTaskActionStatus(void*, void*, LegoU32);

template <class T>
static T& CameraField(void* p_object, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_object + p_offset);
}

static PhysicalLifecycleState* CameraTransform(GameObject* p_object)
{
	return (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
}

static const Vector3* CameraObjectPosition(GameObject* p_object)
{
	PhysicalLifecycleState* transform = CameraTransform(p_object);
	return transform->GetTransformDispatch()->m_position(transform, NULL);
}

static const Matrix3* CameraObjectRotation(GameObject* p_object)
{
	PhysicalLifecycleState* transform = CameraTransform(p_object);
	return transform->GetTransformDispatch()->m_rotation(transform, NULL);
}

// STUB: LEGORACERS2 0x00449750
LegoU32 __fastcall UpdateControllerFollowCamera(
	void* p_controller,
	void*,
	GameObject* p_camera,
	LegoU32* p_updated,
	LegoFloat p_distance,
	LegoFloat p_height
)
{
	void* manager = GetControllerTaskManager();
	if (GetControllerTaskActionStatus(manager, NULL, CameraField<LegoU32>(p_controller, 0x58)) == 1) {
		return 0;
	}
	GameObject* subject = CameraField<GameObject*>(p_controller, 0x1e8);
	void* environment = CameraField<void*>(g_gameController.m_secondaryManager, 0x17c);
	Vector3 position;
	Vector3 target;
	Vector3 up;
	if ((CameraField<LegoU32>(environment, 0x178) & 0x400) != 0) {
		target = *CameraObjectPosition(subject);
		LegoU32 overhead = ((ReverbManager*) g_gameController.m_reverbManager)->FindVolume(&target) == NULL &&
						   (CameraField<LegoU32>(p_controller, 0xd8) & 4) == 0;
		if (overhead == CameraField<LegoU32>(p_controller, 0x1bc)) {
			if (CameraField<LegoS32>(p_controller, 0x1c0) > 0) {
				--CameraField<LegoS32>(p_controller, 0x1c0);
			}
		}
		else if (++CameraField<LegoS32>(p_controller, 0x1c0) >= 5) {
			CameraField<LegoU32>(p_controller, 0x1c0) = 0;
			CameraField<LegoU32>(p_controller, 0x1bc) = CameraField<LegoU32>(p_controller, 0x1bc) == 0;
			CameraField<LegoU32>(p_controller, 0xac) = 1;
		}
		if (CameraField<LegoU32>(p_controller, 0x1bc) != 0) {
			up.m_x = -1.0f;
			up.m_y = up.m_z = 0.0f;
			if (g_gameControllerSession->m_mode != 4 && g_gameControllerSession->m_mode != 3) {
				LegoS32 mode = g_gameControllerSession->m_selectedMode;
				if (mode == 3) {
					up.m_x = 1.0f;
				}
				else if (mode == 1 || mode == 2 || mode == 4) {
					up.m_x = 0.0f;
					up.m_z = 1.0f;
				}
			}
			position = target;
			const Vector3* velocity = &CameraField<Vector3>(subject, 0x118);
			p_distance = (LegoFloat) sqrt(velocity->Dot(velocity)) * 2.237414598464966f * 0.4f + 24.0f;
			if (g_gameControllerSession->m_selectedMode == 2 && p_distance > 32.0f) {
				p_distance = 32.0f;
			}
			position.m_y += p_distance;
			SetGameObjectLookAt(p_camera, NULL, &position, &target, &up);
			const Matrix3* rotation = CameraObjectRotation(subject);
			LegoFloat x = rotation->m_values[2][0] * p_distance * 0.5f;
			LegoFloat z = rotation->m_values[0][0] * p_distance * 0.6666600108146667f;
			if (g_gameControllerSession->m_unk0x00 == 1) {
				x *= 0.5f;
				z *= 0.5f;
			}
			if ((CameraField<LegoU32>(p_controller, 0xd8) & 0x100) == 0) {
				position.m_x -= x;
				position.m_z += z;
			}
			else {
				position.m_x += x;
				position.m_z -= z;
			}
			if (CameraField<LegoU32>(p_controller, 0xac) == 0) {
				position.Scale(0.1f);
				CameraField<Vector3>(p_controller, 0x180).Scale(0.9f);
				position.Add(&CameraField<Vector3>(p_controller, 0x180));
			}
			else {
				CameraField<LegoU32>(p_controller, 0xac) = 0;
			}
			PhysicalLifecycleState* transform = CameraTransform(p_camera);
			transform->GetTransformDispatch()->m_setPosition(transform, NULL, &position);
			CameraField<Vector3>(p_controller, 0x180) = position;
			CameraField<Vector3>(p_controller, 0x180) = *CameraObjectPosition(p_camera);
			CameraField<Vector3>(p_controller, 0x168) = position;
			CameraField<Vector3>(p_controller, 0x168).Subtract(&target);
			*p_updated = 1;
			return 1;
		}
	}
	if (g_gameControllerSession->m_unk0x00 == 1) {
		p_height *= 0.7f;
	}
	if ((g_gameController.m_state == 4 && CameraField<LegoS32>(g_gameController.m_raceManager, 0x58) < 2) ||
		(g_gameController.m_state == 5 && CameraField<LegoS32>(g_gameController.m_battleManager, 0x58) < 3)) {
		p_distance *= 1.5f;
		p_height = 0.0f;
	}
	target = *CameraObjectPosition(subject);
	const Matrix3* rotation = CameraObjectRotation(subject);
	Vector3 forward;
	forward.m_x = rotation->m_values[0][2];
	forward.m_y = rotation->m_values[1][2];
	forward.m_z = rotation->m_values[2][2];
	Vector3 vertical;
	vertical.m_x = rotation->m_values[0][1];
	vertical.m_y = rotation->m_values[1][1];
	vertical.m_z = rotation->m_values[2][1];
	up.m_x = up.m_z = 0.0f;
	up.m_y = 1.0f;
	if ((CameraField<LegoU32>(p_controller, 0xd8) & 4) != 0) {
		LegoFloat blend = CameraField<LegoFloat>(p_controller, 0xfc);
		if (blend == 1.0f) {
			up = vertical;
		}
		else {
			up.m_x = vertical.m_x * blend + 0.0f;
			up.m_y = vertical.m_y * blend + (1.0f - blend);
			up.m_z = vertical.m_z * blend + 0.0f;
			up.Normalize();
		}
	}
	// Retail chooses its fallback only when all three component differences
	// exceed the tolerance; this is deliberately not a usual parallel test.
	if ((LegoFloat) fabs(up.m_x - forward.m_x) > 0.0001f && (LegoFloat) fabs(up.m_y - forward.m_y) > 0.0001f &&
		(LegoFloat) fabs(up.m_z - forward.m_z) > 0.0001f) {
		forward.m_x = forward.m_y = 0.0f;
		forward.m_z = -1.0f;
	}
	else {
		CameraField<Vector3>(p_controller, 0x174) = forward;
	}
	target.m_x = up.m_x * 3.0f + target.m_x;
	target.m_y = up.m_y * 3.0f + target.m_y;
	target.m_z = up.m_z * 3.0f + target.m_z;
	LegoFloat height = p_distance * p_height * 0.5f;
	position.m_x = vertical.m_x * height + -p_distance * forward.m_x + target.m_x;
	position.m_y = vertical.m_y * height + -p_distance * forward.m_y + target.m_y;
	position.m_z = vertical.m_z * height + -p_distance * forward.m_z + target.m_z;
	if (CameraField<LegoU32>(p_controller, 0xac) == 0) {
		const Vector3* velocity = &CameraField<Vector3>(subject, 0x118);
		LegoFloat speed = forward.Dot(velocity);
		LegoFloat factor = 0.04f;
		if (speed < 0.0f) {
			factor = speed * -0.01f;
			if (factor > 0.4f) {
				factor = 0.4f;
			}
			factor += 0.04f;
		}
		Vector3 previous = CameraField<Vector3>(p_controller, 0x180);
		position.Subtract(&previous);
		position.Scale(factor);
		position.Add(&previous);
		position.Subtract(&target);
		LegoFloat length = (LegoFloat) sqrt(position.Dot(&position));
		if (length > p_distance * 1.5f) {
			position.Scale(p_distance * 1.5f / length);
		}
		position.Add(&target);
	}
	else {
		CameraField<LegoU32>(p_controller, 0xac) = 0;
	}
	SetGameObjectLookAt(p_camera, NULL, &position, &target, &up);
	CameraField<Vector3>(p_controller, 0x180) = *CameraObjectPosition(p_camera);
	if ((CameraField<LegoU32>(p_controller, 0xd8) & 0x100) != 0) {
		position.m_x = up.m_x * 3.0f + target.m_x;
		position.m_y = up.m_y * 3.0f + target.m_y;
		position.m_z = up.m_z * 3.0f + target.m_z;
		target.m_x -= forward.m_x * 10.0f;
		target.m_y -= forward.m_y * 10.0f;
		target.m_z -= forward.m_z * 10.0f;
		SetGameObjectLookAt(p_camera, NULL, &position, &target, &up);
	}
	LegoU32 flags = CameraField<LegoU32>(environment, 0x178);
	if ((flags & 0x1000) != 0 && (flags & 0x400) == 0) {
		const Vector3* velocity = &CameraField<Vector3>(subject, 0x118);
		LegoFloat speed = (LegoFloat) sqrt(velocity->Dot(velocity)) * 2.237414598464966f * 0.007692307699471712f;
		if (speed > 1.0f) {
			speed = 1.0f;
		}
		LegoFloat fieldOfView = CameraField<LegoFloat>(p_controller, 0x12c) * 0.75f + (speed * 50.0f + 90.0f) * 0.25f;
		CameraField<LegoFloat>(p_controller, 0x12c) = fieldOfView;
		CameraField<LegoFloat>(p_camera, 0x1a4) = fieldOfView;
	}
	else {
		CameraField<LegoFloat>(p_camera, 0x1a4) = 90.0f;
	}
	CameraField<Vector3>(p_controller, 0x168) = position;
	CameraField<Vector3>(p_controller, 0x168).Subtract(&target);
	*p_updated = 1;
	return 1;
}

// STUB: LEGORACERS2 0x0044a4f0
const LegoU32* __fastcall GetCharacterControllerInput(void* p_controller, void*, LegoU32 p_forceLive)
{
	LegoU32 mode = CameraField<LegoU32>(p_controller, 0x68);
	return &CameraField<LegoU32>(p_controller, p_forceLive != 0 || mode == 1 || mode == 3 ? 0xb0 : 0xc4);
}
