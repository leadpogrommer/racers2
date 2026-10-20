#include "audioservice.h"
#include "charactervisualbehavior.h"
#include "foyeranimation.h"
#include "gameanimatedobject.h"
#include "matrix3.h"
#include "physicallifecycle.h"
#include "simulationtiming.h"

#include <math.h>

extern LegoFloat g_currentSimulationStep;

template <class T>
static T& BodyField(void* p_object, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_object + p_offset);
}

static PhysicalLifecycleState* BodyTransform(GameObject* p_body)
{
	return (PhysicalLifecycleState*) ((LegoU8*) p_body + 0x58);
}

static void SetBodyOrientation(GameObject* p_body, const Quaternion* p_orientation)
{
	PhysicalLifecycleState* transform = BodyTransform(p_body);
	typedef void(__fastcall * SetOrientation)(PhysicalLifecycleState*, void*, const Quaternion*);
	((const SetOrientation*) transform->m_base.m_vtable)[9](transform, NULL, p_orientation);
}

// FUNCTION: LEGORACERS2 0x0043abf0
LegoBool32 __fastcall GetCharacterRaceLookTarget(void* p_manager, void*, Vector3* p_result)
{
	if (BodyField<LegoU32>(p_manager, 0xdc) == 0) {
		return FALSE;
	}
	*p_result = BodyField<Vector3>(p_manager, 0xd0);
	return TRUE;
}

// STUB: LEGORACERS2 0x004525b0
void __fastcall ApplyCharacterBodyMotion(GameObject* p_body, void*, LegoFloat p_scale, LegoU32 p_preserveTimer)
{
	Vector3 target = BodyField<Vector3>(p_body, 0x1f8);
	PhysicalLifecycleState* transform = BodyTransform(p_body);
	const Quaternion* orientation = transform->GetTransformDispatch()->m_orientation(transform, NULL);
	Matrix3 rotation;
	rotation.FUN_004a4d30(&orientation->m_x);
	target.FUN_004a4e00(&rotation);
	LegoU32& flags = BodyField<LegoU32>(p_body, 0x2a8);
	flags &= ~8;
	if (target.Dot(&target) < 0.001f) {
		flags |= 8;
	}
	LegoFloat speed = BodyField<LegoFloat>(p_body, 0x2f4);
	target.m_x = target.m_x * p_scale * speed;
	target.m_y = target.m_y * p_scale * speed;
	target.m_z = target.m_z * p_scale * speed;
	Vector3 delta = target;
	delta.Subtract(&BodyField<Vector3>(p_body, 0x118));
	LegoFloat projected = delta.Dot(&BodyField<Vector3>(p_body, 0x2ac));
	Vector3 correction = delta;
	if (projected != 0.0f && (LegoFloat) sqrt(correction.Dot(&correction)) != 0.0f) {
		correction.Normalize();
		correction.Scale(projected);
		delta.Subtract(&correction);
	}
	LegoFloat inverseStep = 1.0f / g_currentSimulationStep;
	LegoFloat forceScale = BodyField<LegoFloat>(p_body, 0x178) * 0.4f;
	Vector3 force;
	force.m_x = forceScale * inverseStep * delta.m_x;
	force.m_y = delta.m_y * inverseStep * forceScale;
	force.m_z = inverseStep * delta.m_z * forceScale;
	LegoFloat& timer = BodyField<LegoFloat>(p_body, 0x2f8);
	if (p_preserveTimer == 0) {
		timer = 0.0f;
	}
	if (timer > 0.0f || (flags & 4) == 0) {
		force.m_x = force.m_y = force.m_z = 0.0f;
	}
	BodyField<Vector3>(p_body, 0x288) = force;
	BodyField<LegoU32>(p_body, 0x18c) |= 0x20000;
	flags |= 0x10;
	Vector3* facing = &BodyField<Vector3>(p_body, 0x2b8);
	Vector3* up = &BodyField<Vector3>(p_body, 0x2c4);
	Vector3 side;
	side.Cross(up, facing);
	LegoFloat length = (LegoFloat) sqrt(side.Dot(&side));
	if (length != 0.0f) {
		side.Scale(-1.0f / length);
		Vector3 adjusted;
		adjusted.Cross(up, &side);
		length = (LegoFloat) sqrt(adjusted.Dot(&adjusted));
		if (length != 0.0f) {
			adjusted.Scale(1.0f / length);
			*facing = adjusted;
		}
	}
	if (BodyField<LegoU32>(p_body, 0x2e8) != 0) {
		Vector3 zero;
		zero.m_x = zero.m_y = zero.m_z = 0.0f;
		Quaternion result;
		result.LookAt(facing, &zero, up, 0);
		SetBodyOrientation(p_body, &result);
	}
	timer -= g_currentSimulationStep;
	if (timer < 0.0f) {
		timer = 0.0f;
	}
}

// STUB: LEGORACERS2 0x00452a80
void __fastcall TurnCharacterBody(GameObject* p_body, void*, LegoFloat p_degrees)
{
	LegoFloat angle = 3.1415927410125732f * 0.0055555556900799274f * p_degrees * g_currentSimulationStep * 0.5f;
	LegoFloat sine;
	LegoFloat cosine;
	__asm {
		fld angle
		fcos
		fstp cosine
		fld angle
		fsin
		fstp sine
	}
	Vector3 axis = BodyField<Vector3>(p_body, 0x2c4);
	Quaternion orientation;
	orientation.m_x = axis.m_x * sine;
	orientation.m_y = axis.m_y * sine;
	orientation.m_z = axis.m_z * sine;
	orientation.m_w = cosine;
	Matrix3 rotation;
	rotation.FUN_004a4d30(&orientation.m_x);
	BodyField<Vector3>(p_body, 0x2b8).FUN_004a4e00(&rotation);
}

// STUB: LEGORACERS2 0x00453210
void __fastcall UpdateCharacterFootstepAudio(GameObject* p_body, void*, LegoU32 p_layer)
{
	if (BodyField<LegoU32>(p_body, 0x304) == 0) {
		return;
	}
	FoyerAnimationControllerView* animation = ((GameAnimatedObject*) p_body)->m_animation;
	FoyerAnimationLayerView* layer = animation->m_layers[p_layer];
	LegoFloat rate = 30.0f;
	if (layer->m_track != NULL) {
		rate = BodyField<LegoFloat>((void*) layer->m_track, 0x138);
	}
	LegoFloat frame = (30.0f / rate) * *(LegoFloat*) &layer->m_unk0x14;
	LegoU32& completed = BodyField<LegoU32>(p_body, 0x334);
	if (completed != 0 && frame < BodyField<LegoFloat>(p_body, 0x330)) {
		completed = 0;
	}
	LegoU32& index = BodyField<LegoU32>(p_body, 0x32c);
	LegoFloat* event = &BodyField<LegoFloat>(p_body, (index + 0x45) * 0x0c);
	if (event[0] <= frame && completed == 0) {
		PhysicalLifecycleState* transform = BodyTransform(p_body);
		transform->GetTransformDispatch()->m_position(transform, NULL);
		++index;
		if ((LegoS32) index >= BodyField<LegoS32>(p_body, 0x338)) {
			index = 0;
			completed = 1;
			BodyField<LegoFloat>(p_body, 0x330) = frame;
		}
		LegoS32 sourceIndex = ((LegoS32*) event)[2];
		if (BodyField<LegoU32>(p_body, 0x2fc) != 0 && sourceIndex >= 0) {
			AudioSourceRecord* source = BodyField<AudioSourceRecord*>(p_body, 0x30c + sourceIndex * 4);
			source->m_vtable->m_resetPlaybackState(source, NULL);
		}
	}
}

// STUB: LEGORACERS2 0x004c2b60
void __fastcall GetCharacterBodyHeadTransform(GameObject* p_body, void*, Vector3* p_position, Quaternion* p_orientation)
{
	PhysicalLifecycleState* transform = BodyTransform(p_body);
	Quaternion bodyOrientation = *transform->GetTransformDispatch()->m_orientation(transform, NULL);
	void* bones = BodyField<void*>(p_body, 0x194);
	LegoS32 index = BodyField<LegoS32>(p_body, 0x198);
	void* bone = (LegoU8*) bones + index * 0x48;
	Quaternion headOrientation = bodyOrientation;
	headOrientation.MultiplyRight(&BodyField<Quaternion>(bone, 0x2c));
	Vector3 position = BodyField<Vector3>(bone, 0x3c);
	Matrix3 rotation;
	rotation.FUN_004a4d30(&bodyOrientation.m_x);
	position.FUN_004a4e00(&rotation);
	position.Add(transform->GetTransformDispatch()->m_position(transform, NULL));
	rotation.FUN_004a4d30(&headOrientation.m_x);
	for (LegoU32 row = 0; row < 3; ++row) {
		LegoFloat first = rotation.m_values[row][0];
		rotation.m_values[row][0] = rotation.m_values[row][1];
		rotation.m_values[row][1] = first;
		rotation.m_values[row][2] *= -1.0f;
	}
	Quaternion temporary;
	temporary.FromMatrix(&rotation);
	*p_position = position;
	p_orientation->FromMatrix(&rotation);
}
