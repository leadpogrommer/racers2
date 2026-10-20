#include "frontend/secondary/secondarymanager.h"
#include "game/character/charactervisualbehavior.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameanimatedobject.h"
#include "game/session/gamecontrollersession.h"
#include "game/state/gamerandom.h"
#include "math/matrix3.h"
#include "physics/collision/collisionservice.h"
#include "physics/dynamics/physicallifecycle.h"
#include "scene/animation/foyeranimation.h"
#include "scene/camera/controllercamera.h"

template <class T>
static T& CharacterField(void* p_object, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_object + p_offset);
}

static LegoU32 CharacterFloatBits(LegoFloat p_value)
{
	return *(LegoU32*) &p_value;
}

static FoyerAnimationControllerView* CharacterAnimation(CharacterVisual* p_visual)
{
	return ((GameAnimatedObject*) p_visual->m_body)->m_animation;
}

static void SetCharacterPosition(GameObject* p_object, const Vector3* p_position)
{
	PhysicalLifecycleState* transform = (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
	transform->GetTransformDispatch()->m_setPosition(transform, NULL, p_position);
}

static void SetCharacterOrientation(GameObject* p_object, const Quaternion* p_orientation)
{
	PhysicalLifecycleState* transform = (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
	typedef void(__fastcall * SetOrientation)(PhysicalLifecycleState*, void*, const Quaternion*);
	const SetOrientation* dispatch = (const SetOrientation*) transform->m_base.m_vtable;
	dispatch[9](transform, NULL, p_orientation);
}

static LegoFloat CharacterRandom()
{
	GameFloatRandom* stream = &GetGameRandomState()->m_floatStreams[1];
	stream->m_frame = stream->m_frame * stream->m_multiplier + stream->m_increment;
	LegoU32 bits = (stream->m_mantissaMask & stream->m_frame) | stream->m_state;
	// This stream uses its rolling state at +24 and exponent bits at +1c.
	return *(LegoFloat*) &bits - 1.0f;
}

static void __fastcall EnterCharacterDrivingMode(CharacterVisual*, void*);
static void __fastcall EnterCharacterIdleMode(CharacterVisual*, void*);
static void __fastcall EnterCharacterSelectionMode(CharacterVisual*, void*);
static void __fastcall EnterCharacterRunningMode(CharacterVisual*, void*);

// STUB: LEGORACERS2 0x0044f6e0
void __fastcall SetCharacterVisualMode(CharacterVisual* p_visual, void*, LegoU32 p_mode)
{
	GameAnimatedObject* body = (GameAnimatedObject*) p_visual->m_body;
	body->SetFrozen(p_mode == 5);
	if (p_mode == 5) {
		p_visual->m_head->m_flags |= 8;
	}
	else if ((p_visual->m_head->m_flags & 8) != 0) {
		p_visual->m_head->m_flags &= ~8;
		p_visual->m_head->m_flags |= 0x02000000;
	}
	body->m_transform.m_stateFlags &= ~1;
	switch (p_mode) {
	case 1:
		EnterCharacterDrivingMode(p_visual, NULL);
		break;
	case 2:
		EnterCharacterRunningMode(p_visual, NULL);
		break;
	case 3:
		EnterCharacterIdleMode(p_visual, NULL);
		break;
	case 4:
		EnterCharacterSelectionMode(p_visual, NULL);
		break;
	case 6:
		p_visual->EnterIdlePreviewMode();
		break;
	default:
		p_visual->EnterHiddenMode();
		break;
	}
}

// STUB: LEGORACERS2 0x0044f820
static void __fastcall SynchronizeCharacterBody(CharacterVisual* p_visual, void*, const Vector3* p_position)
{
	void* controller = CharacterField<void*>(p_visual, 0x5c);
	typedef const Quaternion*(__fastcall * GetOrientation)(void*, void*);
	typedef const Vector3*(__fastcall * GetPosition)(void*, void*);
	const void** dispatch = *(const void***) controller;
	Quaternion orientation = ((GetOrientation) dispatch[0x58 / 4])(controller, NULL)[0];
	LegoU32 index = CharacterField<LegoU32>(controller, 0x58);
	Vector3 position = CharacterField<Vector3>(g_gameControllerSession, index * 0x344 + 0x360);
	position.m_y += 0.2f;
	position.m_z -= 0.1f;
	Matrix3 rotation;
	rotation.FUN_004a4d30(&orientation.m_x);
	position.FUN_004a4e00(&rotation);
	if (p_position == NULL) {
		p_position = ((GetPosition) dispatch[0x54 / 4])(controller, NULL);
	}
	position.Add(p_position);
	SetCharacterPosition(p_visual->m_body, &position);
	SetCharacterOrientation(p_visual->m_body, &orientation);
}

// STUB: LEGORACERS2 0x0044f920
static void __fastcall SynchronizeCharacterHead(CharacterVisual* p_visual, void*)
{
	Vector3 position;
	Quaternion orientation;
	GetCharacterBodyHeadTransform(p_visual->m_body, NULL, &position, &orientation);
	SetCharacterPosition(p_visual->m_head, &position);
	SetCharacterOrientation(p_visual->m_head, &orientation);
}

static void __fastcall UpdateCharacterLookTarget(CharacterVisual*, void*);
static void __fastcall StartCharacterIdleAnimation(CharacterVisual*, void*, const LegoChar*, LegoFloat, LegoU32);

// STUB: LEGORACERS2 0x0044f960
static void __fastcall EnterCharacterDrivingMode(CharacterVisual* p_visual, void*)
{
	p_visual->m_mode = 1;
	CharacterField<LegoU32>(p_visual, 0xec) = 0;
	CharacterField<LegoU32>(p_visual, 0xe4) = 0;
	CharacterField<LegoU32>(p_visual, 0xe8) = 0;
	CharacterField<LegoU32>(p_visual, 0x10c) = 0;
	CharacterField<LegoU32>(p_visual, 0x108) = 0;
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	animation->m_enabled = 1;
	CharacterField<LegoU32>(p_visual, 0xf8) = FindFoyerAnimationTrack(animation, NULL, "CRASH_FRONT", 1);
	CharacterField<LegoU32>(p_visual, 0xfc) = FindFoyerAnimationTrack(animation, NULL, "CRASH_REAR", 1);
	CharacterField<LegoU32>(p_visual, 0x100) = FindFoyerAnimationTrack(animation, NULL, "DRIVE_LEFT", 1);
	CharacterField<LegoU32>(p_visual, 0x104) = FindFoyerAnimationTrack(animation, NULL, "DRIVE_RIGHT", 1);
	CharacterField<LegoU32>(p_visual, 0x118) = FindFoyerAnimationTrack(animation, NULL, "RUN_LOOP", 1);
	CharacterField<LegoU32>(p_visual, 0x110) = FindFoyerAnimationTrack(animation, NULL, "RUN_LEFTTURN_LOOP", 1);
	CharacterField<LegoU32>(p_visual, 0x114) = FindFoyerAnimationTrack(animation, NULL, "RUN_RIGHTTURN_LOOP", 1);
	StartFoyerAnimation(animation, NULL, "DRIVE_CENTRE", 0);
	ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x104), 1);
	ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0xf8), 2);
	SetFoyerAnimationLayerValues(animation, NULL, 0, 0x3f800000, 0x3f800000, 0);
	for (LegoU32 i = 1; i < 4; ++i) {
		SetFoyerAnimationLayerValues(animation, NULL, i, 0x3f800000, 0, 0);
	}
	CharacterField<LegoU32>(p_visual, 0xf4) = 1;
	CharacterField<LegoU32>(p_visual, 0xf0) = 1;
	CharacterField<LegoU32>(p_visual->m_body, 0xf4) =
		CharacterField<LegoU32>(CharacterField<void*>(p_visual, 0x5c), 0x60);
	DetachCollisionProxy(((GameAnimatedObject*) p_visual->m_body)->m_transform.m_collisionProxy);
}

// STUB: LEGORACERS2 0x0044fac0
void __fastcall UpdateCharacterDrivingAnimation(CharacterVisual* p_visual, void*, LegoFloat p_elapsed)
{
	SynchronizeCharacterBody(p_visual, NULL, NULL);
	SynchronizeCharacterHead(p_visual, NULL);
	UpdateCharacterLookTarget(p_visual, NULL);
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	const LegoU32* input = GetCharacterControllerInput(CharacterField<void*>(p_visual, 0x5c), NULL, 0);
	LegoFloat steer = (LegoS8) (*input >> 16) * 0.007874015718698502f;
	LegoFloat& turn = CharacterField<LegoFloat>(p_visual, 0xe4);
	if (steer < 0.0f) {
		turn -= p_elapsed * 4.0f;
		if (turn < -1.0f) {
			turn = -1.0f;
		}
	}
	else if (steer > 0.0f) {
		turn += p_elapsed * 4.0f;
		if (turn > 1.0f) {
			turn = 1.0f;
		}
	}
	LegoS32& turnLayer = CharacterField<LegoS32>(p_visual, 0xf4);
	if (turn < 0.0f) {
		if (turnLayer != 0) {
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x100), 1);
			turnLayer = 0;
		}
		turn += p_elapsed;
		if (turn > 0.0f) {
			turn = 0.0f;
			ResetFoyerAnimationLayer(animation, NULL, 1);
			turnLayer = -1;
		}
	}
	else if (turn > 0.0f) {
		if (turnLayer != 1) {
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x104), 1);
			turnLayer = 1;
		}
		turn -= p_elapsed;
		if (turn < 0.0f) {
			turn = 0.0f;
			ResetFoyerAnimationLayer(animation, NULL, 1);
			turnLayer = -1;
		}
	}
	LegoFloat& collision = CharacterField<LegoFloat>(p_visual, 0xe8);
	LegoFloat& collisionTime = CharacterField<LegoFloat>(p_visual, 0x10c);
	if (collisionTime > 0.0f) {
		collisionTime -= p_elapsed;
		if (collisionTime < 0.0f) {
			collisionTime = 0.0f;
		}
		collision += CharacterField<LegoFloat>(p_visual, 0x108);
		if (collision < -1.0f) {
			collision = -1.0f;
		}
		else if (collision > 1.0f) {
			collision = 1.0f;
		}
	}
	LegoS32& collisionLayer = CharacterField<LegoS32>(p_visual, 0xf0);
	if (collision < 0.0f) {
		if (collisionLayer != 0) {
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0xfc), 2);
			collisionLayer = 0;
		}
		collision += p_elapsed * 4.0f;
		if (collision > 0.0f) {
			collision = 0.0f;
			ResetFoyerAnimationLayer(animation, NULL, 2);
			collisionLayer = -1;
		}
	}
	else if (collision > 0.0f) {
		if (collisionLayer != 1) {
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0xf8), 2);
			collisionLayer = 1;
		}
		collision -= p_elapsed * 4.0f;
		if (collision < 0.0f) {
			collision = 0.0f;
			ResetFoyerAnimationLayer(animation, NULL, 2);
			collisionLayer = -1;
		}
	}
	LegoFloat amount = turn < 0.0f ? -turn : turn;
	SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(1.0f - amount), 0);
	SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(amount), 1);
	amount = collision < 0.0f ? -collision : collision;
	if (amount > 0.0f) {
		LegoFloat first = GetFoyerAnimationLayerValue(animation, NULL, 0);
		LegoFloat second = GetFoyerAnimationLayerValue(animation, NULL, 1);
		SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits((1.0f - amount) * first), 0);
		SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(second * (1.0f - amount)), 1);
	}
	SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(amount), 2);
}

// STUB: LEGORACERS2 0x0044fec0
static void __fastcall EnterCharacterIdleMode(CharacterVisual* p_visual, void*)
{
	p_visual->m_mode = 3;
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	animation->m_enabled = 1;
	StartFoyerAnimation(animation, NULL, "IDLE_LOOP", 0);
	SetFoyerAnimationLayerValues(animation, NULL, 0, 0x3f800000, 0, 0);
	SetFoyerAnimationLayerValues(animation, NULL, 1, 0x3f800000, 0x3f800000, 0);
	SetFoyerAnimationBlend(animation, NULL, 0.0f);
	Vector3 zero;
	zero.m_x = zero.m_y = zero.m_z = 0.0f;
	SetCharacterPosition(p_visual->m_body, &zero);
	SynchronizeCharacterHead(p_visual, NULL);
	CharacterField<LegoU32>(p_visual, 0x148) = 0;
	CharacterField<const LegoChar*>(p_visual, 0x140) = NULL;
	CharacterField<LegoU32>(p_visual, 0x144) = 1;
	CharacterField<LegoFloat>(p_visual, 0x138) = 100.0f;
	CharacterField<LegoU32>(p_visual, 0x16c) = 0;
	StartCharacterIdleAnimation(p_visual, NULL, "IDLE_LOOP", 1.0f, 1);
	CharacterField<LegoU32>(p_visual, 0x14c) = 0;
	CharacterField<LegoFloat>(p_visual, 0x150) = 0.0f;
	CharacterField<LegoFloat>(p_visual, 0x154) = 1.0f;
	CharacterField<LegoFloat>(p_visual, 0x164) = 0.0f;
	CharacterField<LegoFloat>(p_visual, 0x168) = 1.3f;
	LegoFloat y = CharacterRandom();
	LegoFloat x = CharacterRandom();
	CharacterField<LegoFloat>(p_visual->m_body, 0x1cc) = 1.0f;
	CharacterField<LegoFloat>(p_visual->m_body, 0x19c) = x * 0.5f - 0.25f;
	CharacterField<LegoFloat>(p_visual->m_body, 0x1a0) = y * 0.3f - 0.25f;
	CharacterField<LegoFloat>(p_visual->m_body, 0x1a4) = 1.0f;
	DetachCollisionProxy(((GameAnimatedObject*) p_visual->m_body)->m_transform.m_collisionProxy);
}

// STUB: LEGORACERS2 0x00450070
void __fastcall UpdateCharacterIdleAnimation(CharacterVisual* p_visual, void*, LegoFloat p_elapsed)
{
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	LegoFloat& elapsed = CharacterField<LegoFloat>(p_visual, 0x138);
	elapsed += p_elapsed;
	if (elapsed > 0.25f) {
		if (elapsed >= CharacterField<LegoFloat>(p_visual, 0x13c)) {
			StartCharacterIdleAnimation(p_visual, NULL, "IDLE_LOOP", 1.0f, 1);
		}
		else if (CharacterField<const LegoChar*>(p_visual, 0x140) != NULL) {
			const LegoChar* queued = CharacterField<const LegoChar*>(p_visual, 0x140);
			CharacterField<const LegoChar*>(p_visual, 0x140) = NULL;
			StartCharacterIdleAnimation(p_visual, NULL, queued, 1.0f, 0);
		}
	}
	else {
		LegoFloat blend = elapsed * 4.0f;
		if (CharacterField<LegoU32>(p_visual, 0x144) != 1) {
			blend = 1.0f - blend;
		}
		SetFoyerAnimationBlend(animation, NULL, blend);
	}
	LegoFloat& blinkTime = CharacterField<LegoFloat>(p_visual, 0x150);
	blinkTime += p_elapsed;
	if (blinkTime > CharacterField<LegoFloat>(p_visual, 0x154)) {
		LegoU32& blink = CharacterField<LegoU32>(p_visual, 0x14c);
		blinkTime = 0.0f;
		blink = blink == 0;
		p_visual->SetFaceVariant(blink);
		if (blink == 1) {
			CharacterField<LegoFloat>(p_visual, 0x154) = CharacterRandom() * 0.5f + 0.25f;
		}
		else {
			LegoFloat random = CharacterRandom();
			CharacterField<LegoFloat>(p_visual, 0x154) = random + random + 1.0f;
		}
	}
	LegoFloat& lookTime = CharacterField<LegoFloat>(p_visual, 0x164);
	lookTime += p_elapsed;
	if (lookTime > CharacterField<LegoFloat>(p_visual, 0x168)) {
		lookTime = 0.0f;
		LegoFloat y = CharacterRandom();
		LegoFloat x = CharacterRandom();
		CharacterField<LegoFloat>(p_visual->m_body, 0x1cc) = 1.0f;
		CharacterField<LegoFloat>(p_visual->m_body, 0x19c) = x * 0.5f - 0.25f;
		CharacterField<LegoFloat>(p_visual->m_body, 0x1a0) = y * 0.3f - 0.25f;
		CharacterField<LegoFloat>(p_visual->m_body, 0x1a4) = 1.0f;
		CharacterField<LegoFloat>(p_visual, 0x168) = CharacterRandom() * 1.5f + 0.5f;
	}
	SynchronizeCharacterHead(p_visual, NULL);
}

// STUB: LEGORACERS2 0x00450300
static void __fastcall StartCharacterIdleAnimation(
	CharacterVisual* p_visual,
	void*,
	const LegoChar* p_name,
	LegoFloat p_speed,
	LegoU32 p_enableTransform
)
{
	if (CharacterField<LegoFloat>(p_visual, 0x138) <= 0.25f) {
		CharacterField<const LegoChar*>(p_visual, 0x140) = p_name;
		return;
	}
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	LegoU32 layer = CharacterField<LegoU32>(p_visual, 0x144);
	StartFoyerAnimation(animation, NULL, p_name, layer);
	SetFoyerAnimationBlend(animation, NULL, layer == 1 ? 1.0f : 0.0f);
	CharacterField<LegoFloat>(p_visual, 0x138) = 0.0f;
	const void* track = animation->m_layers[layer]->m_track;
	CharacterField<LegoFloat>(p_visual, 0x13c) = CharacterField<LegoFloat>((void*) track, 0x144) / p_speed - 0.25f;
	SetFoyerAnimationLayerValue(animation, NULL, layer, CharacterFloatBits(p_speed));
	CharacterField<LegoU32>(p_visual, 0x144) = layer == 0;
	if (p_enableTransform != CharacterField<LegoU32>(p_visual, 0x16c)) {
		((GameAnimatedObject*) p_visual->m_body)->SetAnimationTransformEnabled(p_enableTransform);
		CharacterField<LegoU32>(p_visual, 0x16c) = p_enableTransform;
	}
}

// STUB: LEGORACERS2 0x004503e0
static void __fastcall EnterCharacterSelectionMode(CharacterVisual* p_visual, void*)
{
	p_visual->m_mode = 4;
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	animation->m_enabled = 1;
	StartFoyerAnimation(animation, NULL, "DRIVE_CENTRE", 0);
	SetFoyerAnimationLayerValues(animation, NULL, 0, 0x3f800000, 0x3f800000, 0);
	for (LegoU32 i = 1; i < 4; ++i) {
		SetFoyerAnimationLayerValues(animation, NULL, i, 0x3f800000, 0, 0);
	}
	Vector3 zero;
	zero.m_x = zero.m_y = zero.m_z = 0.0f;
	SetCharacterPosition(p_visual->m_body, &zero);
	SynchronizeCharacterHead(p_visual, NULL);
	DetachCollisionProxy(((GameAnimatedObject*) p_visual->m_body)->m_transform.m_collisionProxy);
}

// STUB: LEGORACERS2 0x004504a0
void __fastcall UpdateCharacterSelectionAnimation(CharacterVisual* p_visual, void*, LegoFloat)
{
	Vector3 position;
	GetCharacterSelectionPosition(
		((SecondaryManager*) g_gameController.m_secondaryManager)->m_carSelection,
		NULL,
		&position
	);
	position.m_y += 0.16f;
	position.m_z -= 0.1f;
	SetCharacterPosition(p_visual->m_body, &position);
	SynchronizeCharacterHead(p_visual, NULL);
}

// STUB: LEGORACERS2 0x00450500
static void __fastcall EnterCharacterRunningMode(CharacterVisual* p_visual, void*)
{
	p_visual->m_mode = 2;
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	animation->m_enabled = 1;
	StartFoyerAnimation(animation, NULL, "IDLE_LOOP", 0);
	StartFoyerAnimation(animation, NULL, "INAIR_LOOP", 1);
	SetFoyerAnimationLayerValues(animation, NULL, 0, 0x3f800000, 0x3f800000, 0);
	SetFoyerAnimationLayerValues(animation, NULL, 1, 0x3f800000, 0, 0);
	SetFoyerAnimationBlend(animation, NULL, 0.0f);
	CharacterField<LegoU32>(p_visual, 0x11c) = 1;
	CharacterField<LegoFloat>(p_visual, 0x120) = 0.0f;
	CharacterField<LegoFloat>(p_visual, 0x128) = 0.0f;
	void* controller = CharacterField<void*>(p_visual, 0x5c);
	const void** dispatch = *(const void***) controller;
	typedef const Vector3*(__fastcall * GetPosition)(void*, void*);
	typedef const Quaternion*(__fastcall * GetOrientation)(void*, void*);
	Vector3 position = *((GetPosition) dispatch[0x54 / 4])(controller, NULL);
	position.m_y += 1.0f;
	SetCharacterPosition(p_visual->m_body, &position);
	Quaternion orientation = *((GetOrientation) dispatch[0x58 / 4])(controller, NULL);
	Matrix3 rotation;
	rotation.FUN_004a4d30(&orientation.m_x);
	Vector3 facing;
	facing.m_x = rotation.m_values[0][2];
	facing.m_y = 0.0f;
	facing.m_z = rotation.m_values[2][2];
	if (facing.m_x == 0.0f && facing.m_z == 0.0f) {
		facing.m_z = 1.0f;
	}
	facing.Normalize();
	CharacterField<Vector3>(p_visual->m_body, 0x2b8) = facing;
	CharacterField<LegoU32>(p_visual->m_body, 0x18c) |= 0x60000;
	Vector3 force;
	force.m_x = force.m_z = 0.0f;
	force.m_y = 20000.0f;
	CharacterField<Vector3>(p_visual->m_body, 0x288) = force;
	CharacterField<LegoU32>(p_visual->m_body, 0x18c) |= 0x20000;
	Vector3 zero;
	zero.m_x = zero.m_y = zero.m_z = 0.0f;
	CharacterField<Vector3>(p_visual->m_body, 0x118) = zero;
	CharacterField<Vector3>(p_visual->m_body, 0x100) = zero;
	AttachCollisionProxy(((GameAnimatedObject*) p_visual->m_body)->m_transform.m_collisionProxy);
}

// STUB: LEGORACERS2 0x00450720
void __fastcall UpdateCharacterRunningAnimation(CharacterVisual* p_visual, void*, LegoFloat p_elapsed)
{
	FoyerAnimationControllerView* animation = CharacterAnimation(p_visual);
	LegoU32 input = *GetCharacterControllerInput(CharacterField<void*>(p_visual, 0x5c), NULL, 0);
	LegoU32& state = CharacterField<LegoU32>(p_visual, 0x11c);
	if (state == 2) {
		LegoFloat& blend = CharacterField<LegoFloat>(p_visual, 0x120);
		blend += p_elapsed * 3.0f;
		SetFoyerAnimationBlend(animation, NULL, blend);
		if (blend > 1.0f) {
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x118), 1);
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x110), 2);
			ApplyFoyerAnimationTrack(animation, NULL, CharacterField<LegoU32>(p_visual, 0x114), 3);
			SetFoyerAnimationLayerValues(animation, NULL, 1, CharacterFloatBits(1.25f), 0, 0);
			SetFoyerAnimationLayerValues(animation, NULL, 2, CharacterFloatBits(1.25f), 0, 0);
			SetFoyerAnimationLayerValues(animation, NULL, 3, CharacterFloatBits(1.25f), 0, 0);
			SetFoyerAnimationBlend(animation, NULL, 1.0f);
			state = 3;
			blend = 0.0f;
			CharacterField<LegoU32>(p_visual->m_body, 0xf4) =
				CharacterField<LegoU32>(CharacterField<void*>(p_visual, 0x5c), 0x60) + 1;
		}
	}
	else if (state != 3) {
		if ((CharacterField<LegoU32>(p_visual->m_body, 0x2a8) & 4) != 0) {
			state = 2;
			CharacterField<LegoFloat>(p_visual, 0x124) = 0.0f;
			CharacterField<LegoFloat>(p_visual, 0x120) = 0.0f;
			CharacterField<LegoFloat>(p_visual, 0x12c) = 1.25f;
		}
	}
	else {
		LegoFloat& run = CharacterField<LegoFloat>(p_visual, 0x124);
		if ((input & 0x100) != 0) {
			run += 0.05f;
			if (run > 1.0f) {
				run = 1.0f;
			}
		}
		else {
			run -= 0.03f;
			if (run < 0.0f) {
				run = 0.0f;
			}
		}
		LegoFloat& turn = CharacterField<LegoFloat>(p_visual, 0x128);
		LegoFloat steer = (LegoS8) (input >> 16) * 0.007874015718698502f;
		LegoFloat angle = 0.0f;
		LegoFloat delta = p_elapsed * 3.0f;
		if (steer > 0.1f) {
			turn -= delta;
			angle = 100.0f;
			if (turn < -1.0f) {
				turn = -1.0f;
			}
		}
		else if (steer < -0.1f) {
			turn += delta;
			angle = -100.0f;
			if (turn > 1.0f) {
				turn = 1.0f;
			}
		}
		else if (turn < 0.0f) {
			turn += delta;
			if (turn > 0.0f) {
				turn = 0.0f;
			}
		}
		else if (turn > 0.0f) {
			turn -= delta;
			if (turn < 0.0f) {
				turn = 0.0f;
			}
		}
		LegoFloat left = run * turn;
		LegoFloat right = -left;
		if (left < 0.0f) {
			left = 0.0f;
		}
		if (right < 0.0f) {
			right = 0.0f;
		}
		SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(left), 2);
		SetFoyerAnimationLayerValueReordered(animation, NULL, CharacterFloatBits(right), 3);
		SetFoyerAnimationBlend(animation, NULL, 1.0f - run);
		if (angle != 0.0f) {
			TurnCharacterBody(p_visual->m_body, NULL, angle);
		}
		LegoFloat mode = CharacterField<LegoFloat>(CharacterField<void*>(p_visual, 0x5c), 0x194);
		LegoFloat& speed = CharacterField<LegoFloat>(p_visual, 0x12c);
		if (mode == 0.0f) {
			speed -= p_elapsed * 5.0f;
			if (speed < 1.25f) {
				speed = 1.25f;
			}
		}
		else {
			speed += p_elapsed + p_elapsed;
			LegoFloat maximum = mode == 0.25f ? 3.0f : 4.0f;
			if (speed > maximum) {
				speed = maximum;
			}
			Matrix3 rotation;
			rotation.FUN_004a4d30(&CharacterField<Quaternion>(p_visual->m_body->m_unk0x48, 0x8c).m_x);
			Vector3 direction;
			direction.m_x = rotation.m_values[0][1];
			direction.m_y = rotation.m_values[1][1];
			direction.m_z = rotation.m_values[2][1];
			if (direction.m_y > 0.25f) {
				Vector3 position = CharacterField<Vector3>(p_visual, 0x18c);
				position.m_y += 0.2f;
				AddCharacterFootstepTrail(
					CharacterField<void*>((void*) g_gameController.m_unk0xa4, 0x88),
					NULL,
					&position,
					&direction,
					p_visual->m_object.m_lifecycle0x54,
					1
				);
			}
		}
		SetFoyerAnimationLayerValue(animation, NULL, 1, CharacterFloatBits(speed));
		SetFoyerAnimationLayerValue(animation, NULL, 2, CharacterFloatBits(speed));
		SetFoyerAnimationLayerValue(animation, NULL, 3, CharacterFloatBits(speed));
		ApplyCharacterBodyMotion(p_visual->m_body, NULL, ((1.75f + 1.25f) / (1.75f + speed)) * 5.0f, 1);
		if (run > 0.05f) {
			UpdateCharacterFootstepAudio(p_visual->m_body, NULL, 1);
		}
	}
	SynchronizeCharacterHead(p_visual, NULL);
	UpdateCharacterLookTarget(p_visual, NULL);
}

// STUB: LEGORACERS2 0x00450d10
void __fastcall UpdateCharacterHiddenAnimation(CharacterVisual* p_visual, void*, LegoFloat)
{
	SynchronizeCharacterHead(p_visual, NULL);
	if (g_gameController.m_state == 6 && g_gameControllerSession->m_selectedMode == 0 &&
		CharacterField<void*>(p_visual, 0x5c) == NULL) {
		LegoU32 state = CharacterField<LegoU32>(g_gameController.m_unk0xbc, 0xf8);
		if (state == 1 || state == 2) {
			ApplyCharacterBodyMotion(p_visual->m_body, NULL, 1.0f, 1);
		}
	}
}

// STUB: LEGORACERS2 0x00450d60
static void __fastcall UpdateCharacterLookTarget(CharacterVisual* p_visual, void*)
{
	GameAnimatedObject* body = (GameAnimatedObject*) p_visual->m_body;
	body->SetAnimationTransformEnabled(0);
	Vector3 position;
	if (g_gameController.m_state == 3) {
		if (!GetCharacterRaceLookTarget(g_gameController.m_foyerManager, NULL, &position)) {
			return;
		}
		CharacterField<Vector3>(body, 0x19c) = position;
		CharacterField<LegoFloat>(body, 0x1cc) = 1.0f;
		body->SetAnimationTransformEnabled(1);
	}
	else if (g_gameController.m_state == 4) {
		void* action = CharacterField<void*>(CharacterField<void*>(p_visual, 0x5c), 0x120);
		if (action == NULL) {
			return;
		}
		void* race = CharacterField<void*>(g_gameController.m_raceManager, 0x64);
		LegoU32 index = CharacterField<LegoU32>(action, 0x14);
		if (index == 0) {
			index = CharacterField<LegoU32>(race, 0x134);
		}
		void* target = CharacterField<void*>(race, 0x68 + index * 4);
		position = CharacterField<Vector3>(target, 0x168);
		body->SetAnimationTransformEnabled(1);
		CharacterField<LegoFloat>(body, 0x1cc) = 1.0f;
		CharacterField<Vector3>(body, 0x19c) = position;
	}
}

// FUNCTION: LEGORACERS2 0x004ef550 FOLDED
void __fastcall UpdateCharacterPassiveAnimation(CharacterVisual*, void*, LegoFloat)
{
}
