#include "decomp.h"
#include "game/objects/gameobjectrenderprep.h"
#include "math/quaternion.h"
#include "math/vector3.h"

#include <stddef.h>

struct CameraTransform;
struct CameraTransformDispatch {
	undefined m_prefix[0x1c];
	const Quaternion*(__fastcall* m_orientation)(CameraTransform*, void*);
	void(__fastcall* m_setPosition)(CameraTransform*, void*, const Vector3*);
	void(__fastcall* m_setOrientation)(CameraTransform*, void*, const Quaternion*);
};
struct CameraTransform {
	const CameraTransformDispatch* m_vtable;
};
DECOMP_STATIC_ASSERT(offsetof(CameraTransformDispatch, m_setPosition) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(CameraTransformDispatch, m_setOrientation) == 0x24);

// STUB: LEGORACERS2 0x004bb120
LegoBool32 __fastcall SetGameObjectLookAt(
	void* p_object,
	void*,
	const Vector3* p_position,
	const Vector3* p_target,
	const Vector3* p_up
)
{
	CameraTransform* transform = (CameraTransform*) ((LegoU8*) p_object + 0x58);
	transform->m_vtable->m_setPosition(transform, NULL, p_position);
	Quaternion orientation = *transform->m_vtable->m_orientation(transform, NULL);
	orientation.LookAt(p_position, p_target, p_up, 0);
	transform->m_vtable->m_setOrientation(transform, NULL, &orientation);
	return TRUE;
}
