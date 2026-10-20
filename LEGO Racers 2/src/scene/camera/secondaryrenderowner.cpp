#include "scene/camera/secondaryrenderowner.h"

#include "frontend/preview/cardisplay.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectrenderprep.h"
#include "input/controller/inputcontrollermanager.h"
#include "memory/memoryheap.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/device/displayconfig.h"

#include <new>

struct SecondaryRenderOwnerDispatch {
	GameObjectDispatchView m_object;
	GameObjectDispatchView::Value m_size;
	GameObjectDispatchView::Value m_followMode;
	LegoFloat(__fastcall* m_orientationBlend)(GameObject*, void*);
	const Quaternion*(__fastcall* m_fixedOrientation)(GameObject*, void*);
	LegoFloat(__fastcall* m_fixedOrientationBlend)(GameObject*, void*);
	void(__stdcall* m_chaseOffsets)(LegoFloat*, LegoFloat*);
	void(__stdcall* m_followOffsets)(LegoFloat*, LegoFloat*);
};
DECOMP_SIZE_ASSERT(SecondaryRenderOwnerDispatch, 0x6c);

static GameObject* __fastcall DestroySecondaryRenderOwner(GameObject*, void*, LegoU32);
static void __fastcall RemoveSecondaryRenderOwner(SecondaryRenderOwner*, void*);
static void __fastcall InitializeSecondaryRenderOwner(GameObject*, void*);
void __fastcall ReleaseSecondaryRenderOwner(GameObject*, void*);
static void __fastcall ApplySecondaryRenderOwnerTransform(GameObject*, void*, LegoU32, LegoU32);
static LegoU32 __fastcall UpdateSecondaryRenderOwner(GameObject*, void*);
static LegoU32 __fastcall GetSecondaryRenderOwnerSize(GameObject*, void*);
static LegoU32 __fastcall GetSecondaryRenderOwnerCategory(GameObject*, void*);
static LegoU32 __fastcall GetSecondaryRenderFollowMode(GameObject*, void*);
static LegoFloat __fastcall GetSecondaryRenderZero(GameObject*, void*);
static void __stdcall GetSecondaryFollowOffsets(LegoFloat*, LegoFloat*);
static void __stdcall GetSecondaryChaseOffsets(LegoFloat*, LegoFloat*);
static LegoU32 __fastcall AttachSecondaryRenderOwner(SecondaryRenderOwner*, void*, PhysicalLifecycleState*);
static LegoU32 __fastcall FollowSecondaryRenderOwner(SecondaryRenderOwner*, void*, PhysicalLifecycleState*);
static LegoU32 __fastcall ChaseSecondaryRenderOwner(SecondaryRenderOwner*, void*, PhysicalLifecycleState*);
static LegoU32 __fastcall FlySecondaryRenderOwner(SecondaryRenderOwner*, void*);
static LegoU32 __fastcall TrackSecondaryRenderOwner(
	SecondaryRenderOwner*,
	void*,
	PhysicalLifecycleState*,
	PhysicalLifecycleState*
);
static LegoU32 __fastcall UpdateSecondaryRenderLifecycle(PhysicalLifecycleState*, void*);
static GameObject* __fastcall DestroySecondaryRenderLifecycle(PhysicalLifecycleState*, void*, LegoU32);

// Retail object dispatch 0x00536050, all 27 slots.
extern const SecondaryRenderOwnerDispatch g_secondaryRenderOwnerDispatch;
const SecondaryRenderOwnerDispatch g_secondaryRenderOwnerDispatch = {
	{&DestroySecondaryRenderOwner,
	 &ApplySecondaryRenderOwnerTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeSecondaryRenderOwner,
	 &GameObjectBaseTrue,
	 &GameObjectBaseZeroOneArgument,
	 &ReleaseSecondaryRenderOwner,
	 &UpdateSecondaryRenderOwner,
	 &GameObjectBaseZero,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,
	 &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,
	 &GameObjectBaseNoOpTwoArguments,
	 &GetSecondaryRenderOwnerCategory,
	 &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion,
	 &GameObjectBaseNoOpOneArgument},
	&GetSecondaryRenderOwnerSize,
	&GetSecondaryRenderFollowMode,
	&GetSecondaryRenderZero,
	(const Quaternion*(__fastcall*) (GameObject*, void*) ) & GameObjectBaseZero,
	&GetSecondaryRenderZero,
	&GetSecondaryChaseOffsets,
	&GetSecondaryFollowOffsets
};
#define RENDER_PHYSICAL_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
// Retail component dispatch 0x00536000, all 20 slots.
extern const PhysicalLifecycleDispatch g_secondaryRenderPhysicalDispatch;
const PhysicalLifecycleDispatch g_secondaryRenderPhysicalDispatch = {
	{RENDER_PHYSICAL_CALLBACK(&IsLifecycleActive),
	 RENDER_PHYSICAL_CALLBACK(&GetLifecycleParentValue),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseTrue),
	 RENDER_PHYSICAL_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 RENDER_PHYSICAL_CALLBACK(&GetLifecyclePosition),
	 RENDER_PHYSICAL_CALLBACK(&GetLifecycleRotation),
	 RENDER_PHYSICAL_CALLBACK(&GetLifecycleOrientation),
	 RENDER_PHYSICAL_CALLBACK(&SetLifecyclePosition),
	 RENDER_PHYSICAL_CALLBACK(&SetLifecycleOrientation),
	 RENDER_PHYSICAL_CALLBACK(&GetLifecycleParent),
	 RENDER_PHYSICAL_CALLBACK(&UpdateSecondaryRenderLifecycle),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseZero),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseZeroOneArgument),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 RENDER_PHYSICAL_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 RENDER_PHYSICAL_CALLBACK(&CaptureLifecycleTransform),
	 RENDER_PHYSICAL_CALLBACK(&GetCapturedLifecycleTransform),
	 RENDER_PHYSICAL_CALLBACK(&DestroySecondaryRenderLifecycle),
	 RENDER_PHYSICAL_CALLBACK(&ClearLifecycleMotion)}
};
#undef RENDER_PHYSICAL_CALLBACK

// GLOBAL: LEGORACERS2 0x00679eb8
static PhysicalLifecycleParameters g_secondaryRenderPhysicalParameters;

static const SecondaryRenderOwnerDispatch* RenderDispatch(SecondaryRenderOwner* p_owner)
{
	return (const SecondaryRenderOwnerDispatch*) p_owner->m_object.m_vtable;
}
static const LifecycleTransformDispatch* TransformDispatch(PhysicalLifecycleState* p_state)
{
	return p_state->GetTransformDispatch();
}

static void SetRenderPhysicalPosition(SecondaryRenderOwner* p_owner, const Vector3* p_position)
{
	PhysicalLifecycleState* physical = &p_owner->m_physical;
	physical->GetTransformDispatch()->m_setPosition(physical, NULL, p_position);
}
static void SetRenderPhysicalOrientation(SecondaryRenderOwner* p_owner, const Quaternion* p_orientation)
{
	PhysicalLifecycleState* physical = &p_owner->m_physical;
	typedef void(__fastcall * SetOrientation)(PhysicalLifecycleState*, void*, const Quaternion*);
	SetOrientation setOrientation = *(SetOrientation*) ((const LegoU8*) physical->m_base.m_vtable + 0x24);
	setOrientation(physical, NULL, p_orientation);
}

// STUB: LEGORACERS2 0x004b9d50
static void InitializeSecondaryRenderParameters()
{
	g_secondaryRenderPhysicalParameters.m_position.SetValues(0.0f, 0.0f, 0.0f);
	g_secondaryRenderPhysicalParameters.m_orientation.m_x = 0.0f;
	g_secondaryRenderPhysicalParameters.m_orientation.m_y = 0.0f;
	g_secondaryRenderPhysicalParameters.m_orientation.m_z = 0.0f;
	g_secondaryRenderPhysicalParameters.m_orientation.m_w = 1.0f;
	g_secondaryRenderPhysicalParameters.m_contactFriction = 0.5f;
	g_secondaryRenderPhysicalParameters.m_contactResponse = 0.5f;
	g_secondaryRenderPhysicalParameters.m_propertyIndex = 0;
	g_secondaryRenderPhysicalParameters.m_mass = 1.0f;
	g_secondaryRenderPhysicalParameters.m_inertia.SetValues(1.0f, 1.0f, 1.0f);
}
struct SecondaryRenderParameterInitializer {
	SecondaryRenderParameterInitializer() { InitializeSecondaryRenderParameters(); }
};
static SecondaryRenderParameterInitializer g_secondaryRenderParameterInitializer;

// STUB: LEGORACERS2 0x004b9e40
GameObject* __fastcall ConstructSecondaryRenderOwner(
	GameObject* p_object,
	void*,
	const SecondaryRenderOwnerTemplate* p_template
)
{
	SecondaryRenderOwner* owner = (SecondaryRenderOwner*) p_object;
	PhysicalLifecycleState* physical = owner != NULL ? &owner->m_physical : NULL;
	new (p_object) GameObject(0x300, (GameObjectTemplate*) &p_template->m_base, physical);
	ConstructPhysicalLifecycleState(physical, NULL, p_object, &g_secondaryRenderPhysicalParameters);
	owner->m_target = NULL;
	owner->m_position = p_template->m_position;
	owner->m_orientation = p_template->m_orientation;
	for (LegoU32 i = 0; i < 4; ++i) {
		((LegoS32*) &owner->m_viewportLeft)[i] = -1;
	}
	owner->m_shadowState = 0;
	owner->m_updateProjectionRatio = 1;
	owner->m_planePointPointer = owner->m_planePoints;
	owner->m_planeCount = 6;
	owner->m_worldPlanePointPointer = owner->m_worldPlanePoints;
	owner->m_worldPlaneCount = 6;
	owner->m_alternateProjectionScale = 1;
	owner->m_unk0x34c = 0;
	p_object->m_vtable = &g_secondaryRenderOwnerDispatch.m_object;
	physical->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_secondaryRenderPhysicalDispatch;
	owner->m_fieldOfView = 90.0f;
	owner->m_nearDistance = 0.1f;
	owner->m_farDistance = 8192.0f;
	owner->m_viewportLeft = p_template->m_modelIds[0];
	owner->m_viewportTop = p_template->m_modelIds[1];
	owner->m_viewportWidth = p_template->m_modelIds[2];
	owner->m_viewportHeight = p_template->m_modelIds[3];
	SetLifecyclePosition(physical, NULL, &p_template->m_position);
	SetLifecycleOrientation(physical, NULL, &p_template->m_orientation);
	physical->m_savedPosition = *physical->GetTransformDispatch()->m_position(physical, NULL);
	physical->m_savedOrientation = *physical->GetTransformDispatch()->m_orientation(physical, NULL);
	owner->m_geometryUsable = 1;
	owner->m_projectionMode = (p_template->m_flags >> 7) & 1;
	owner->m_orthographicScale = p_template->m_nearDistance;
	owner->m_angles.SetValues(0.0f, -1.0f, 0.0f);
	owner->m_offset.SetValues(0.0f, 0.0f, 0.0f);
	owner->m_secondOffset.SetValues(0.0f, 0.0f, 0.0f);
	owner->m_movementSpeed = 0.0f;
	return p_object;
}

// FUNCTION: LEGORACERS2 0x004ba100
static LegoU32 __fastcall GetSecondaryRenderOwnerSize(GameObject*, void*)
{
	return 0x350;
}
// FUNCTION: LEGORACERS2 0x004ba110
static LegoU32 __fastcall GetSecondaryRenderOwnerCategory(GameObject*, void*)
{
	return 3;
}
// STUB: LEGORACERS2 0x004ba120
static LegoU32 __fastcall GetSecondaryRenderFollowMode(GameObject* p_object, void*)
{
	SecondaryRenderOwner* owner = (SecondaryRenderOwner*) p_object;
	return owner->m_target->m_vtable->m_value0x40(owner->m_target, NULL);
}
// FUNCTION: LEGORACERS2 0x004ba130
static LegoFloat __fastcall GetSecondaryRenderZero(GameObject*, void*)
{
	return 0.0f;
}
// FUNCTION: LEGORACERS2 0x004ba140
static GameObject* __fastcall DestroySecondaryRenderOwner(GameObject* p_object, void*, LegoU32 p_flags)
{
	RemoveSecondaryRenderOwner((SecondaryRenderOwner*) p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}
// STUB: LEGORACERS2 0x004ba160
static void __fastcall RemoveSecondaryRenderOwner(SecondaryRenderOwner* p_owner, void*)
{
	p_owner->m_object.m_vtable = &g_secondaryRenderOwnerDispatch.m_object;
	p_owner->m_physical.m_base.m_vtable = (const GameLifecycleDispatchView*) &g_secondaryRenderPhysicalDispatch;
	if ((p_owner->m_object.m_flags & 1) == 0) {
		DisplayRenderer* renderer = g_displayRenderer;
		((const RendererMainDispatch*) renderer->m_vtable)->m_releaseViewport(renderer, NULL, p_owner->m_viewportIndex);
	}
	if (g_unk0x00679ef0 == p_owner) {
		g_unk0x00679ef0 = NULL;
	}
	p_owner->m_physical.m_base.m_vtable = (const GameLifecycleDispatchView*) &g_carDisplayBaseTransformDispatch;
	p_owner->m_object.~GameObject();
}
// STUB: LEGORACERS2 0x004ba1c0
static void __fastcall InitializeSecondaryRenderOwner(GameObject* p_object, void*)
{
	SecondaryRenderOwner* owner = (SecondaryRenderOwner*) p_object;
	if ((p_object->m_flags & 0x800000) == 0) {
		if (owner->m_viewportWidth == 0) {
			owner->m_viewportLeft = 0;
			owner->m_viewportTop = 0;
			owner->m_viewportWidth = g_displayConfiguration->m_width;
			owner->m_viewportHeight = g_displayConfiguration->m_height;
		}
		DisplayRenderer* renderer = g_displayRenderer;
		owner->m_viewportIndex = ((const RendererMainDispatch*) renderer->m_vtable)
									 ->m_addViewport(
										 renderer,
										 NULL,
										 owner->m_viewportLeft,
										 owner->m_viewportTop,
										 owner->m_viewportWidth,
										 owner->m_viewportHeight
									 );
	}
	p_object->Initialize();
	owner->m_settleFrames = 0;
	if (owner->m_geometryUsable != 0) {
		UpdateGameObjectRenderGeometry(owner, NULL);
	}
}
// The shared base-remove bridge at retail 0x004ba270.
void __fastcall ReleaseSecondaryRenderOwner(GameObject* p_object, void*)
{
	p_object->Remove();
}
// STUB: LEGORACERS2 0x004ba870
static void __fastcall ApplySecondaryRenderOwnerTransform(
	GameObject* p_object,
	void*,
	LegoU32 p_position,
	LegoU32 p_orientation
)
{
	SecondaryRenderOwner* owner = (SecondaryRenderOwner*) p_object;
	Vector3 position = *(const Vector3*) p_position;
	position.Add(&owner->m_offset);
	Matrix3 rotation;
	rotation.FUN_004a4d30((const LegoFloat*) p_orientation);
	owner->m_orientation = *(const Quaternion*) p_orientation;
	owner->m_position = position;
	owner->m_forward.m_x = rotation.m_values[0][2];
	owner->m_forward.m_y = rotation.m_values[1][2];
	owner->m_forward.m_z = rotation.m_values[2][2];
}
// STUB: LEGORACERS2 0x004ba930
static LegoU32 __fastcall UpdateSecondaryRenderOwner(GameObject* p_object, void*)
{
	SecondaryRenderOwner* owner = (SecondaryRenderOwner*) p_object;
	if (owner->m_target != NULL) {
		LegoU32 result = 0;
		if (owner->m_target->m_vtable->m_value0x44(owner->m_target, NULL, (LegoU32) owner, (LegoU32) &result) != 0) {
			return result;
		}
		PhysicalLifecycleState* target = (PhysicalLifecycleState*) owner->m_target->m_lifecycle0x54;
		if (target != NULL) {
			switch (RenderDispatch(owner)->m_followMode(p_object, NULL)) {
			case 0:
				return AttachSecondaryRenderOwner(owner, NULL, target);
			case 1:
				return FollowSecondaryRenderOwner(owner, NULL, target);
			case 2:
				return ChaseSecondaryRenderOwner(owner, NULL, target);
			case 3:
				return FlySecondaryRenderOwner(owner, NULL);
			case 4:
				return TrackSecondaryRenderOwner(
					owner,
					NULL,
					(PhysicalLifecycleState*) owner->m_secondTarget->m_lifecycle0x54,
					target
				);
			}
		}
	}
	return 0;
}
// STUB: LEGORACERS2 0x004baa90
static LegoU32 __fastcall AttachSecondaryRenderOwner(
	SecondaryRenderOwner* p_owner,
	void*,
	PhysicalLifecycleState* p_target
)
{
	const SecondaryRenderOwnerDispatch* dispatch = RenderDispatch(p_owner);
	const Quaternion* fixed = dispatch->m_fixedOrientation(&p_owner->m_object, NULL);
	Quaternion orientation;
	if (fixed == NULL) {
		orientation = *TransformDispatch(p_target)->m_orientation(p_target, NULL);
	}
	else {
		orientation = *TransformDispatch(&p_owner->m_physical)->m_orientation(&p_owner->m_physical, NULL);
		orientation.Interpolate(fixed, &orientation, dispatch->m_fixedOrientationBlend(&p_owner->m_object, NULL));
	}
	SetRenderPhysicalOrientation(p_owner, &orientation);
	const Vector3* position = TransformDispatch(p_target)->m_position(p_target, NULL);
	SetRenderPhysicalPosition(p_owner, position);
	return 1;
}
// FUNCTION: LEGORACERS2 0x004bab40
static void __stdcall GetSecondaryFollowOffsets(LegoFloat* p_distance, LegoFloat* p_height)
{
	*p_distance = -4.0f;
	*p_height = 1.5f;
}
// STUB: LEGORACERS2 0x004bab60
static LegoU32 __fastcall FollowSecondaryRenderOwner(
	SecondaryRenderOwner* p_owner,
	void*,
	PhysicalLifecycleState* p_target
)
{
	const Matrix3* rotation = TransformDispatch(p_target)->m_rotation(p_target, NULL);
	Vector3 forward = {rotation->m_values[0][2], rotation->m_values[1][2], rotation->m_values[2][2]};
	Vector3 up = {rotation->m_values[0][1], rotation->m_values[1][1], rotation->m_values[2][1]};
	LegoFloat distance, height;
	RenderDispatch(p_owner)->m_followOffsets(&distance, &height);
	forward.Scale(distance);
	up.Scale(height);
	forward.Add(&up);
	forward.Add(TransformDispatch(p_target)->m_position(p_target, NULL));
	SetRenderPhysicalPosition(p_owner, &forward);
	SetRenderPhysicalOrientation(p_owner, TransformDispatch(p_target)->m_orientation(p_target, NULL));
	return 1;
}
// FUNCTION: LEGORACERS2 0x004bac60
static void __stdcall GetSecondaryChaseOffsets(LegoFloat* p_distance, LegoFloat* p_height)
{
	*p_distance = -7.0f;
	*p_height = 2.5f;
}
// STUB: LEGORACERS2 0x004bac80
static LegoU32 __fastcall ChaseSecondaryRenderOwner(
	SecondaryRenderOwner* p_owner,
	void*,
	PhysicalLifecycleState* p_target
)
{
	const Matrix3* rotation = TransformDispatch(p_target)->m_rotation(p_target, NULL);
	Vector3 position = {rotation->m_values[0][2], rotation->m_values[1][2], rotation->m_values[2][2]};
	LegoFloat distance, height;
	RenderDispatch(p_owner)->m_chaseOffsets(&distance, &height);
	position.Scale(distance);
	position.m_x += 0.0f;
	position.m_y += height;
	position.m_z += 0.0f;
	position.Add(TransformDispatch(p_target)->m_position(p_target, NULL));
	SetRenderPhysicalPosition(p_owner, &position);
	Vector3 target = *TransformDispatch(p_target)->m_position(p_target, NULL);
	target.m_y += 0.8f;
	Quaternion orientation = *TransformDispatch(&p_owner->m_physical)->m_orientation(&p_owner->m_physical, NULL);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	orientation
		.LookAt(&target, TransformDispatch(&p_owner->m_physical)->m_position(&p_owner->m_physical, NULL), &up, 0);
	SetRenderPhysicalOrientation(p_owner, &orientation);
	return 1;
}
// STUB: LEGORACERS2 0x004bade0
static LegoU32 __fastcall FlySecondaryRenderOwner(SecondaryRenderOwner* p_owner, void*)
{
	LegoU32 input = 0;
	g_inputControllerManager->GetControllerInputForOwner((LegoU32) p_owner, &input, 0, 0);
	const LegoU8* bytes = (const LegoU8*) &input;
	LegoFloat yaw = (LegoS8) bytes[2] * 0.0001230314956046641f;
	// Retail computes the second axis here but applies the first axis to both angles below.
	LegoFloat pitch = (LegoS8) bytes[3] * 0.0001230314956046641f;
	(void) pitch;
	const Matrix3* rotation = TransformDispatch(&p_owner->m_physical)->m_rotation(&p_owner->m_physical, NULL);
	Vector3 forward = {rotation->m_values[0][2], rotation->m_values[1][2], rotation->m_values[2][2]};
	LegoFloat direction = (input & 0x100) != 0 ? 1.0f : 0.0f;
	if ((input & 0x200) != 0) {
		direction -= 1.0f;
	}
	if (direction == 0.0f) {
		p_owner->m_movementSpeed = 0.0f;
	}
	else {
		p_owner->m_movementSpeed += 0.002f;
	}
	forward.Scale(direction * p_owner->m_movementSpeed);
	forward.Add(TransformDispatch(&p_owner->m_physical)->m_position(&p_owner->m_physical, NULL));
	SetRenderPhysicalPosition(p_owner, &forward);
	p_owner->m_angles.m_y -= yaw;
	p_owner->m_angles.m_x += yaw;
	Matrix3 angles, transposed;
	angles.FUN_004a4f60(&p_owner->m_angles.m_x);
	transposed.FUN_004a5040(&angles);
	Quaternion orientation;
	orientation.FromMatrix(&transposed);
	SetRenderPhysicalOrientation(p_owner, &orientation);
	return 1;
}
// STUB: LEGORACERS2 0x004baf90
static LegoU32 __fastcall TrackSecondaryRenderOwner(
	SecondaryRenderOwner* p_owner,
	void*,
	PhysicalLifecycleState* p_positionTarget,
	PhysicalLifecycleState* p_lookTarget
)
{
	Vector3 position = *TransformDispatch(p_positionTarget)->m_position(p_positionTarget, NULL);
	PhysicalLifecycleState* second = (PhysicalLifecycleState*) p_owner->m_secondTarget->m_lifecycle0x54;
	TransformDispatch(second)->m_position(second, NULL);
	if (p_positionTarget != &p_owner->m_physical) {
		Vector3 offset = p_owner->m_secondOffset;
		Matrix3 rotation;
		rotation.FUN_004a4d30(&TransformDispatch(p_positionTarget)->m_orientation(p_positionTarget, NULL)->m_x);
		offset.FUN_004a4e00(&rotation);
		position.Add(&offset);
	}
	SetRenderPhysicalPosition(p_owner, &position);
	Vector3 target = *TransformDispatch(p_lookTarget)->m_position(p_lookTarget, NULL);
	Quaternion orientation = *TransformDispatch(&p_owner->m_physical)->m_orientation(&p_owner->m_physical, NULL);
	Quaternion previous = *TransformDispatch(&p_owner->m_physical)->m_orientation(&p_owner->m_physical, NULL);
	Vector3 up = {0.0f, 1.0f, 0.0f};
	orientation.LookAt(&target, &position, &up, 0);
	orientation
		.Interpolate(&orientation, &previous, RenderDispatch(p_owner)->m_orientationBlend(&p_owner->m_object, NULL));
	SetRenderPhysicalOrientation(p_owner, &orientation);
	return 1;
}
// STUB: LEGORACERS2 0x004bb190
static LegoU32 __fastcall UpdateSecondaryRenderLifecycle(PhysicalLifecycleState* p_state, void*)
{
	return UpdateSecondaryRenderOwner((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}
// STUB: LEGORACERS2 0x004bb1a0
static GameObject* __fastcall DestroySecondaryRenderLifecycle(PhysicalLifecycleState* p_state, void*, LegoU32 p_flags)
{
	return DestroySecondaryRenderOwner((GameObject*) ((LegoU8*) p_state - 0x58), NULL, p_flags);
}
