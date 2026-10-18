#include "gamerender.h"

#include "gamemanager.h"
#include "quaternion.h"
#include "vector3.h"

#include <stddef.h>

namespace
{

struct GameObjectTransformDispatch {
	typedef void(__fastcall* Apply)(GameObject*, void*, const Vector3*, const Quaternion*);
	void* m_unk0x00;
	Apply m_applyTransform;
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectTransformDispatch, m_applyTransform) == 0x04);

LegoS32 FixedPointFloor(LegoU32 p_value)
{
	LegoU32 rounded = p_value + ((p_value >> 31) & 0x3ff);
	LegoU32 shifted = rounded >> 10;
	if ((rounded & 0x80000000) != 0) {
		shifted |= 0xffc00000;
	}
	return (LegoS32) shifted;
}

LegoU32 WrapMultiply(LegoS32 p_first, LegoS32 p_second)
{
	return (LegoU32) p_first * (LegoU32) p_second;
}

} // namespace

// FUNCTION: LEGORACERS2 0x004a5780
void Quaternion::Interpolate(const Quaternion* p_first, const Quaternion* p_second, LegoFloat p_factor)
{
	Quaternion target = *p_second;
	LegoFloat dot = target.m_z * p_first->m_z;
	dot += target.m_y * p_first->m_y;
	dot += target.m_x * p_first->m_x;
	dot += target.m_w * p_first->m_w;
	if (!(dot >= 0.0f)) {
		target.m_x *= -1.0f;
		target.m_y *= -1.0f;
		target.m_z *= -1.0f;
		target.m_w *= -1.0f;
	}

	Vector3* position = (Vector3*) this;
	position->FUN_004a4ce0((const Vector3*) p_first, (const Vector3*) &target, p_factor);
	m_w = p_factor * p_first->m_w + (1.0f - p_factor) * target.m_w;
	Normalize();
}
// STUB: LEGORACERS2 0x004aa7b0
void GameManager::UpdateComponents(LegoFloat p_frameDelta)
{
	LegoBool32 interpolate = (p_frameDelta != 0.0f && p_frameDelta == p_frameDelta) ? TRUE : FALSE;

	GameObject** current = g_gameObjectsByType;
	for (LegoU32 remaining = (LegoU32) g_gameObjectsByTypeCount; remaining != 0; --remaining, ++current) {
		GameObject* object = *current;
		LegoU32 flags = object->m_flags;
		if ((flags & 0x00000008) != 0 || ((flags >> 1) & 0x00000008) != 0) {
			continue;
		}

		void* component = object->m_lifecycle0x54;
		if (component == NULL) {
			continue;
		}

		const GameObjectComponentDispatch* componentDispatch = (const GameObjectComponentDispatch*) *(void**) component;
		Vector3 previousPosition;
		Quaternion previousOrientation;
		componentDispatch->m_getTransforms(component, NULL, &previousPosition, &previousOrientation);

		flags = object->m_flags;
		if ((flags & 0x02000000) != 0) {
			object->m_flags = flags & 0xfdffffff;
			void* orientation = componentDispatch->m_getOrientation(component, NULL);
			void* position = componentDispatch->m_getPosition(component, NULL);
			((const GameObjectTransformDispatch*) object->m_vtable)
				->m_applyTransform(object, NULL, (const Vector3*) position, (const Quaternion*) orientation);
			componentDispatch->m_finishTransform(component, NULL);
			continue;
		}

		if (interpolate) {
			const Vector3* position = (const Vector3*) componentDispatch->m_getPosition(component, NULL);
			Vector3 interpolatedPosition;
			interpolatedPosition.FUN_004a4ce0(&previousPosition, position, p_frameDelta);

			const Quaternion* orientation = (const Quaternion*) componentDispatch->m_getOrientation(component, NULL);
			Quaternion interpolatedOrientation;
			interpolatedOrientation.Interpolate(&previousOrientation, orientation, p_frameDelta);
			((const GameObjectTransformDispatch*) object->m_vtable)
				->m_applyTransform(object, NULL, &interpolatedPosition, &interpolatedOrientation);
		}
		else {
			void* previousOrientation = componentDispatch->m_getOrientation(component, NULL);
			const Vector3* position = (const Vector3*) componentDispatch->m_getPosition(component, NULL);
			((const GameObjectTransformDispatch*) object->m_vtable)
				->m_applyTransform(object, NULL, position, (const Quaternion*) previousOrientation);
		}
	}
}
// STUB: LEGORACERS2 0x004be460
LegoU32* __stdcall BuildAnimationHueColor(LegoU32* p_color, LegoS32 p_degrees, LegoS32 p_saturation, LegoS32 p_value)
{
	LegoS32 sector = (p_degrees / 60) * 1024;
	LegoS32 fraction = (p_degrees * 1024) / 60 - (p_degrees / 60) * 1024;
	LegoS32 valueComponent = FixedPointFloor(WrapMultiply(1024 - p_saturation, p_value));
	LegoS32 qComponent =
		FixedPointFloor(WrapMultiply(1024 - FixedPointFloor(WrapMultiply(fraction, p_saturation)), p_value));
	LegoS32 tComponent =
		FixedPointFloor(WrapMultiply(1024 - FixedPointFloor(WrapMultiply(1024 - fraction, p_saturation)), p_value));

	LegoS32 blue;
	LegoS32 green;
	LegoS32 red;
	if (sector == 0) {
		blue = valueComponent;
		green = qComponent;
		red = p_value;
	}
	else if (sector == 1024) {
		blue = qComponent;
		green = tComponent;
		red = p_value;
	}
	else if (sector == 2048) {
		blue = tComponent;
		green = p_value;
		red = valueComponent;
	}
	else if (sector == 3072) {
		blue = p_value;
		green = valueComponent;
		red = tComponent;
	}
	else if (sector == 4096) {
		blue = p_value;
		green = valueComponent;
		red = tComponent;
	}
	else if (sector == 5120) {
		blue = qComponent;
		green = valueComponent;
		red = p_value;
	}
	else {
		blue = p_saturation;
		green = p_saturation;
		red = p_saturation;
	}

	*p_color =
		((LegoU32) 0xff << 24) | ((LegoU32) (red >> 2) << 16) | ((LegoU32) (green >> 2) << 8) | (LegoU32) (blue >> 2);
	return p_color;
}
