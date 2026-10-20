#include "charactervisualbodylifecycle.h"

#include "cardisplay.h"
#include "charactercollisioneffects.h"
#include "charactervisual.h"
#include "collisioncontacts.h"
#include "diagnostics.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gameobjectdispatch.h"
#include "inputcontrollermanager.h"
#include "inputjoystick.h"
#include "screenprinter.h"
#include "terrain.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern LegoU32 g_unk0x00572f94;
extern LegoFloat g_currentSimulationStep;
struct CarDisplayIndexedRecordOwner;
extern CarDisplayIndexedRecordOwner* g_carDisplayIndexedRecordOwner;

template <class T>
static T& CharacterBodyField(void* p_object, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_object + p_offset);
}

struct CharacterContactMaterial {
	Vector3 m_values;
	LegoU32 m_color;
	LegoU16 m_kind;
	LegoU16 m_flags;
};
DECOMP_SIZE_ASSERT(CharacterContactMaterial, 0x14);

static void __fastcall UpdateCharacterBodyMotion(GameObject*, void*);
static LegoU32 __fastcall ApplyCharacterBodyForces(GameObject*, void*);
static void __fastcall HandleCharacterBodyImpact(PhysicalLifecycleState*, void*, CollisionContact*);
static void __fastcall UpdateCharacterBodyLifecycleMotion(PhysicalLifecycleState*, void*);
static LegoU32 __fastcall RegisterCharacterBodyContact(PhysicalLifecycleState*, void*, CollisionContact*);
static void __fastcall ForwardCharacterBodyEvent(GameObject*, void*, const CharBuffer24*, LegoU32*);
static LegoU32 __fastcall GetCharacterVisualBodyObjectSize(GameObject*, void*);
static LegoU32 __fastcall ApplyCharacterBodyLifecycleForces(PhysicalLifecycleState*, void*);
void __fastcall BeginCharacterShieldEffect(CharacterVisual*, void*, LegoFloat);

// Retail ABI function-pointer table: 0x00531b40.
#define CHARACTER_BODY_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
const PhysicalLifecycleDispatch g_characterVisualBodyLifecycleDispatch = {
	{CHARACTER_BODY_CALLBACK(&IsLifecycleActive),
	 CHARACTER_BODY_CALLBACK(&GetLifecycleParentValue),
	 CHARACTER_BODY_CALLBACK(&GameObjectBaseZero),
	 CHARACTER_BODY_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 CHARACTER_BODY_CALLBACK(&HandleCharacterBodyImpact),
	 CHARACTER_BODY_CALLBACK(&GetLifecyclePosition),
	 CHARACTER_BODY_CALLBACK(&GetLifecycleRotation),
	 CHARACTER_BODY_CALLBACK(&GetLifecycleOrientation),
	 CHARACTER_BODY_CALLBACK(&SetLifecyclePosition),
	 CHARACTER_BODY_CALLBACK(&SetLifecycleOrientation),
	 CHARACTER_BODY_CALLBACK(&GetLifecycleParent),
	 CHARACTER_BODY_CALLBACK(&UpdateCharacterBodyLifecycleMotion),
	 CHARACTER_BODY_CALLBACK(&ApplyCharacterBodyLifecycleForces),
	 CHARACTER_BODY_CALLBACK(&RegisterCharacterBodyContact),
	 CHARACTER_BODY_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 CHARACTER_BODY_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 CHARACTER_BODY_CALLBACK(&CaptureLifecycleTransform),
	 CHARACTER_BODY_CALLBACK(&GetCapturedLifecycleTransform),
	 CHARACTER_BODY_CALLBACK(&DestroyCharacterVisualHeadLifecycle),
	 CHARACTER_BODY_CALLBACK(&ClearLifecycleMotion)}
};
#undef CHARACTER_BODY_CALLBACK

// Retail ABI function-pointer table: 0x00531b90.
const FoyerTemplateInstanceDispatch g_characterVisualBodyDispatch = {
	{&DestroyCharacterVisualHead,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeFoyerTemplateInstance,
	 &GameObjectBaseTrue,
	 (GameObjectDispatchView::ValueArgument) &UpdateFoyerTemplateInstance,
	 &RemoveFoyerTemplateInstance,
	 (GameObjectDispatchView::Value) &UpdateCharacterBodyMotion,
	 &ApplyCharacterBodyForces,
	 &GameObjectBaseNoOp,
	 &UpdateFoyerTemplateAttachment,
	 &DrawFoyerModelRenderObject,
	 &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,
	 (GameObjectDispatchView::TwoArguments) &ForwardCharacterBodyEvent,
	 &GetFoyerTemplateObjectCategory,
	 &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion,
	 (GameObjectDispatchView::Update) &UpdateFoyerModelRenderRegion},
	&GetCharacterVisualBodyObjectSize
};

// STUB: LEGORACERS2 0x0043e9e0
static LegoU32 __fastcall ApplyCharacterBodyLifecycleForces(PhysicalLifecycleState* p_state, void*)
{
	return ApplyCharacterBodyForces((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

// STUB: LEGORACERS2 0x004403a0
static LegoFloat __fastcall GetCharacterCollisionSpeed(const Vector3* p_velocity, void*)
{
	return (LegoFloat) sqrt(p_velocity->Dot(p_velocity));
}

// FUNCTION: LEGORACERS2 0x004403d0
static LegoU32* __fastcall ReorderCharacterCollisionColor(LegoU32* p_output, void*, const LegoU32* p_input)
{
	LegoU8* output = (LegoU8*) p_output;
	const LegoU8* input = (const LegoU8*) p_input;
	output[2] = input[0];
	output[1] = input[1];
	output[0] = input[2];
	output[3] = input[3];
	return p_output;
}

// FUNCTION: LEGORACERS2 0x00445ee0
static LegoU32 __fastcall GetCharacterVisualBodyObjectSize(GameObject*, void*)
{
	return 0x3a0;
}

// STUB: LEGORACERS2 0x00451640
static void __fastcall HandleCharacterBodyImpact(PhysicalLifecycleState* p_state, void*, CollisionContact* p_contact)
{
	if (g_unk0x00572f94 != 0) {
		return;
	}
	CharacterContactMaterial material;
	material.m_values.m_x = 1.0f;
	material.m_values.m_y = material.m_values.m_z = 0.0f;
	material.m_color = 0;
	material.m_kind = 0;
	material.m_flags = 2;
	PhysicalLifecycleState* first = (PhysicalLifecycleState*) p_contact->m_first;
	PhysicalLifecycleState* second = (PhysicalLifecycleState*) p_contact->m_second;
	if (second == *(PhysicalLifecycleState**) ((LegoU8*) p_state - 4)) {
		PhysicalLifecycleState* other = first;
		first = second;
		second = other;
	}
	Vector3 position = p_contact->m_point;
	position.Add(first->GetTransformDispatch()->m_position(first, NULL));
	typedef void(__fastcall * GetMaterial)(
		PhysicalLifecycleState*,
		void*,
		CharacterContactMaterial*,
		const Vector3*,
		LegoFloat
	);
	GetMaterial getMaterial = *(GetMaterial*) ((const LegoU8*) second->m_base.m_vtable + 0x0c);
	getMaterial(second, NULL, &material, &position, 1.0f);
	LegoU32 color;
	ReorderCharacterCollisionColor(&color, NULL, &material.m_color);
	EmitCharacterCollisionDust(
		NULL,
		NULL,
		&position,
		GetCharacterCollisionSpeed(&p_contact->m_relativeVelocity, NULL),
		color,
		material.m_values.m_z
	);
	CharacterVisual* visual = CharacterBodyField<CharacterVisual*>(p_state, 0x344);
	if (visual != NULL) {
		if (second->GetTransformDispatch()->m_isActive(second, NULL, 0) &&
			p_contact->m_normal.Dot(&p_contact->m_relativeVelocity) < -10.0f) {
			BeginCharacterShieldEffect(visual, NULL, 5.0f);
		}
		if (p_contact->m_point.m_y < -0.4f) {
			position = p_contact->m_point;
			PhysicalLifecycleState* owner = (PhysicalLifecycleState*) p_contact->m_first;
			position.Add(owner->GetTransformDispatch()->m_position(owner, NULL));
			CharacterBodyField<Vector3>(visual, 0x18c) = position;
		}
	}
}

// STUB: LEGORACERS2 0x00451800
static void __fastcall UpdateCharacterBodyFeedback(GameObject* p_body, void*, LegoFloat)
{
	CharacterVisual* visual = CharacterBodyField<CharacterVisual*>(p_body, 0x39c);
	if (visual == NULL) {
		return;
	}
	void* controller = CharacterBodyField<void*>(visual, 0x5c);
	if (controller == NULL || CharacterBodyField<LegoU32>(controller, 0x68) != 1) {
		return;
	}
	InputController* input = g_inputControllerManager->FindOrCreateController((LegoU32) controller, 0, 0, 1);
	if (input == NULL) {
		return;
	}
	InputDeviceRecord* device = GetControllerInputDevice(input, NULL);
	if (device == NULL || !device->m_vtable->isActive(device, NULL)) {
		return;
	}
	device->m_vtable->VTable0x2c(device, NULL, 0x3f000000);
	device->m_vtable->VTable0x30(device, NULL, 0);
	controller = CharacterBodyField<void*>(visual, 0x5c);
	device = CharacterBodyField<InputDeviceRecord*>(controller, 0x1f0);
	if (device != NULL) {
		if (CharacterBodyField<LegoFloat>(controller, 0x194) <= 0.0f) {
			device->m_vtable->VTable0x54(device, NULL);
			CharacterBodyField<LegoU32>(controller, 0x1ec) = 0;
		}
		else if (CharacterBodyField<LegoU32>(controller, 0x1ec) == 0) {
			device->m_vtable->VTable0x38(device, NULL, 0x42c80000, 0x3f000000);
			CharacterBodyField<LegoU32>(controller, 0x1ec) = 1;
		}
	}
}

static void __fastcall FinishCharacterBodyMotion(GameObject*, void*);

// STUB: LEGORACERS2 0x004518d0
static void __fastcall UpdateCharacterBodyMotion(GameObject* p_body, void*)
{
	if (CharacterBodyField<LegoU32>(p_body, 0x300) == 0) {
		CharacterVisual* visual = CharacterBodyField<CharacterVisual*>(p_body, 0x39c);
		void* controller = CharacterBodyField<void*>(visual, 0x5c);
		if (controller != NULL && visual->m_mode == 2) {
			CharacterBodyField<LegoU16>(controller, 0xf4) = 0;
			CharacterBodyField<LegoFloat>(controller, 0xe8) = 1.0f;
			CharacterBodyField<LegoFloat>(controller, 0xe4) = 1.0f;
			controller = CharacterBodyField<void*>(visual, 0x5c);
			if (controller != NULL) {
				void* subject = CharacterBodyField<void*>(controller, 0x1e8);
				Vector3* velocity = &CharacterBodyField<Vector3>(subject, 0x118);
				UpdateCharacterBodyFeedback(
					p_body,
					NULL,
					(LegoFloat) sqrt(velocity->Dot(velocity)) * 2.237414598464966f
				);
			}
		}
	}
	FinishCharacterBodyMotion(p_body, NULL);
}

static void EnsureCharacterBodyEventNames();
static const CharBuffer24* GetCharacterBodyEventName(LegoU32);

// STUB: LEGORACERS2 0x00451960
static void __fastcall ForwardCharacterBodyEvent(
	GameObject* p_body,
	void*,
	const CharBuffer24* p_name,
	LegoU32* p_value
)
{
	EnsureCharacterBodyEventNames();
	if (strncmp(p_name->m_data, GetCharacterBodyEventName(0)->m_data, 24) == 0) {
		GameObject* controller =
			CharacterBodyField<GameObject*>(CharacterBodyField<CharacterVisual*>(p_body, 0x39c), 0x5c);
		typedef void(__fastcall * SetSurface)(GameObject*, void*, LegoU32);
		(*(SetSurface*) ((const LegoU8*) controller->m_vtable + 0x68))(controller, NULL, *p_value);
		return;
	}
	if (strncmp(p_name->m_data, GetCharacterBodyEventName(1)->m_data, 24) == 0) {
		return;
	}
	for (LegoU32 i = 2; i < 5; ++i) {
		const CharBuffer24* name = GetCharacterBodyEventName(i);
		if (strncmp(p_name->m_data, name->m_data, 24) == 0) {
			GameObject* controller =
				CharacterBodyField<GameObject*>(CharacterBodyField<CharacterVisual*>(p_body, 0x39c), 0x5c);
			if (i == 2 && controller == NULL) {
				return;
			}
			controller->m_vtable->m_action0x3c(controller, NULL, (LegoU32) name, (LegoU32) p_value);
			return;
		}
	}
}

// STUB: LEGORACERS2 0x00451a80
static void __fastcall UpdateCharacterBodyLifecycleMotion(PhysicalLifecycleState* p_state, void*)
{
	UpdateCharacterBodyMotion((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

static const CharacterContactMaterial* __fastcall GetCharacterContactMaterial(PhysicalLifecycleState*, void*);
static void __cdecl PrintCharacterContactStatus(ScreenPrinter*, LegoU32, const LegoChar*, ...);

// STUB: LEGORACERS2 0x00452b30
static LegoU32 __fastcall RegisterCharacterBodyContact(
	PhysicalLifecycleState* p_state,
	void*,
	CollisionContact* p_contact
)
{
	PhysicalLifecycleState* other = (PhysicalLifecycleState*) p_contact->m_first;
	if (other == *(PhysicalLifecycleState**) ((LegoU8*) p_state - 4)) {
		other = (PhysicalLifecycleState*) p_contact->m_second;
	}
	GameObject* object = other->m_base.m_owner;
	CharacterBodyField<GameObject*>(p_state, 0x2a8) = object;
	LegoU32& flags = CharacterBodyField<LegoU32>(p_state, 0x250);
	flags |= 1;
	Vector3& normal = CharacterBodyField<Vector3>(p_state, 0x254);
	normal.m_x = normal.m_z = 0.0f;
	normal.m_y = 1.0f;
	void* controller = CharacterBodyField<void*>(p_state, 0x24c);
	if (controller != NULL) {
		CharacterContactMaterial* material = &CharacterBodyField<CharacterContactMaterial>(controller, 0xe4);
		PrintCharacterContactStatus((ScreenPrinter*) g_gameController.m_screenPrinter, 0, "");
		GameObject* terrain = *(GameObject**) ((LegoU8*) g_gameControllerSession + 0x48);
		if (object == terrain) {
			Vector3 position = *p_state->GetTransformDispatch()->m_position(p_state, NULL);
			Vector3 projected;
			((Terrain*) terrain)->ProjectToSurface(&position, &projected, &normal);
			flags |= 2;
			Vector3& up = CharacterBodyField<Vector3>(p_state, 0x26c);
			if (CharacterBodyField<LegoU32>(p_state, 0x294) != 0) {
				up = normal;
			}
			else {
				up.m_x = up.m_z = 0.0f;
				up.m_y = 1.0f;
			}
			PhysicalLifecycleState* transform = (PhysicalLifecycleState*) ((LegoU8*) terrain + 0x58);
			typedef void(__fastcall * GetMaterial)(
				PhysicalLifecycleState*,
				void*,
				CharacterContactMaterial*,
				const Vector3*,
				LegoFloat
			);
			GetMaterial getMaterial = *(GetMaterial*) ((const LegoU8*) transform->m_base.m_vtable + 0x0c);
			getMaterial(transform, NULL, material, &p_state->m_position, 2.0f);
		}
		else if (object->m_lifecycle0x54 != NULL) {
			const CharacterContactMaterial* otherMaterial =
				GetCharacterContactMaterial((PhysicalLifecycleState*) object->m_lifecycle0x54, NULL);
			if (otherMaterial != NULL && otherMaterial->m_kind == 100) {
				CharacterBodyField<Vector3>(p_state, 0x26c) = p_contact->m_normal;
				normal = p_contact->m_normal;
				flags |= 2;
				if (CharacterBodyField<void*>(p_state, 0x24c) != NULL) {
					*material = *otherMaterial;
				}
			}
		}
	}
	CharacterBodyField<Vector3>(p_state, 0x284) = *p_state->GetTransformDispatch()->m_position(p_state, NULL);
	Vector3 up;
	up.m_x = up.m_z = 0.0f;
	up.m_y = 1.0f;
	controller = CharacterBodyField<void*>(p_state, 0x24c);
	if (controller != NULL && (CharacterBodyField<LegoU32>(controller, 0xd8) & 4) != 0) {
		up = normal;
	}
	LegoFloat alignment = up.Dot(&normal);
	if (alignment < 0.2f) {
		CharacterBodyField<LegoFloat>(p_state, 0x2a0) = 0.5f;
		if (alignment < 0.0f) {
			alignment = 0.0f;
		}
	}
	CharacterBodyField<LegoFloat>(p_state, 0x29c) = alignment;
	return 0;
}

// STUB: LEGORACERS2 0x00452dd0
static LegoU32 __fastcall ApplyCharacterBodyForces(GameObject* p_body, void*)
{
	Vector3 zero;
	zero.m_x = zero.m_y = zero.m_z = 0.0f;
	CharacterBodyField<Vector3>(p_body, 0x160) = zero;
	CharacterBodyField<Vector3>(p_body, 0x124) = zero;
	CharacterBodyField<Vector3>(p_body, 0x10c) = zero;
	if ((CharacterBodyField<LegoU32>(p_body, 0x2a8) & 2) != 0) {
		CharacterBodyField<LegoU32>(p_body, 0x18c) |= 0x40000;
		return 0;
	}
	void* controller = CharacterBodyField<void*>(p_body, 0x2a4);
	PhysicalLifecycleState* state = (PhysicalLifecycleState*) ((LegoU8*) p_body + 0x58);
	if (controller != NULL && (CharacterBodyField<LegoU32>(controller, 0xd8) & 4) != 0) {
		state->m_force = zero;
		LegoFloat blend = CharacterBodyField<LegoFloat>(controller, 0xfc);
		LegoFloat scale = g_physicalLifecycleNormalizedStep;
		state->m_torque.m_x = scale * state->m_angularMomentum.m_x;
		state->m_torque.m_y = scale * state->m_angularMomentum.m_y;
		state->m_torque.m_z = scale * state->m_angularMomentum.m_z;
		scale = g_physicalLifecycleNormalizedStep;
		state->m_force.m_x = scale * state->m_linearMomentum.m_x;
		state->m_force.m_y = scale * state->m_linearMomentum.m_y;
		state->m_force.m_z = scale * state->m_linearMomentum.m_z;
		LegoFloat gravity = g_physicalLifecycleStep * state->m_mass;
		LegoFloat directed = gravity * blend;
		Vector3& up = CharacterBodyField<Vector3>(p_body, 0x2c4);
		state->m_force.m_x = up.m_x * directed + state->m_force.m_x;
		state->m_force.m_y = up.m_y * directed + state->m_force.m_y;
		state->m_force.m_z = directed * up.m_z + state->m_force.m_z;
		state->m_force.m_y = (1.0f - blend) * gravity + state->m_force.m_y;
		return 0;
	}
	ApplyPhysicalLifecycleDampingAndGravity(state, NULL);
	return 0;
}

// STUB: LEGORACERS2 0x00452fc0
static void __fastcall FinishCharacterBodyMotion(GameObject* p_body, void*)
{
	Vector3 zero;
	zero.m_x = zero.m_y = zero.m_z = 0.0f;
	LegoU32& behavior = CharacterBodyField<LegoU32>(p_body, 0x18c);
	if ((behavior & 0x40000) != 0) {
		behavior &= ~0x40000;
		CharacterBodyField<Vector3>(p_body, 0x154) = zero;
	}
	if ((behavior & 0x20000) != 0) {
		CharacterBodyField<Vector3>(p_body, 0x154).Add(&CharacterBodyField<Vector3>(p_body, 0x288));
		behavior &= ~0x20000;
	}
	LegoU32& flags = CharacterBodyField<LegoU32>(p_body, 0x2a8);
	Vector3& velocity = CharacterBodyField<Vector3>(p_body, 0x118);
	if ((flags & 8) != 0 && velocity.Dot(&velocity) < 0.1f) {
		velocity = zero;
		CharacterBodyField<Vector3>(p_body, 0x100) = zero;
	}
	LegoU32 previous = flags;
	flags &= ~4;
	LegoU32& audioTicks = CharacterBodyField<LegoU32>(p_body, 0x2fc);
	if (audioTicks != 0) {
		--audioTicks;
	}
	if ((previous & 3) != 0) {
		audioTicks = 5;
		flags |= 4;
	}
	flags &= ~3;
	Vector3& up = CharacterBodyField<Vector3>(p_body, 0x2c4);
	Vector3& startUp = CharacterBodyField<Vector3>(p_body, 0x2d0);
	LegoFloat& elapsed = CharacterBodyField<LegoFloat>(p_body, 0x2f0);
	LegoU32& contact = CharacterBodyField<LegoU32>(p_body, 0x300);
	if (contact != 0) {
		elapsed = 0.0f;
		contact = 0;
		startUp = up;
		IntegratePhysicalLifecycleMotion((PhysicalLifecycleState*) ((LegoU8*) p_body + 0x58), NULL);
		return;
	}
	elapsed += g_currentSimulationStep;
	LegoFloat blend = elapsed;
	if (blend > 1.0f) {
		blend = 1.0f;
	}
	Vector3 vertical;
	vertical.m_x = vertical.m_z = 0.0f;
	vertical.m_y = 1.0f;
	up.FUN_004a4ce0(&vertical, &startUp, blend);
	contact = 0;
	IntegratePhysicalLifecycleMotion((PhysicalLifecycleState*) ((LegoU8*) p_body + 0x58), NULL);
}

// FUNCTION: LEGORACERS2 0x0045f950
static void __cdecl PrintCharacterContactStatus(
	ScreenPrinter* p_printer,
	LegoU32 p_index,
	const LegoChar* p_format,
	...
)
{
	LegoChar buffer[512];
	va_list arguments;
	va_start(arguments, p_format);
	vsprintf(buffer, p_format, arguments);
	va_end(arguments);
	if (strlen(buffer) > sizeof(buffer)) {
		ReportFatalError("TempString too small in ScreenPrinter()");
	}
	p_printer->SetText(buffer, p_index);
}

// The five buffers are initialized on first event forwarding, matching their
// original dynamic initializer contents without relying on CRT startup order.
// GLOBAL: LEGORACERS2 0x00576fb0
static CharBuffer24 g_characterSurfaceEventName;
// GLOBAL: LEGORACERS2 0x00577078
static CharBuffer24 g_characterPickupEventName;
// GLOBAL: LEGORACERS2 0x00577030
static CharBuffer24 g_characterRacerEventName;
// GLOBAL: LEGORACERS2 0x00577060
static CharBuffer24 g_characterGoldenBrickEventName;
// GLOBAL: LEGORACERS2 0x00576f50
static CharBuffer24 g_characterBonusVortexEventName;

// STUB: LEGORACERS2 0x00491830
static void InitializeCharacterSurfaceEventName()
{
	g_characterSurfaceEventName = CharBuffer24("PIT SURFACE CONTACT");
}

// STUB: LEGORACERS2 0x00491850
static void InitializeCharacterPickupEventName()
{
	g_characterPickupEventName = CharBuffer24("HIT A PICKUP");
}

// STUB: LEGORACERS2 0x00491870
static void InitializeCharacterGoldenBrickEventName()
{
	g_characterGoldenBrickEventName = CharBuffer24("HIT A GOLDEN BRICK");
}

// STUB: LEGORACERS2 0x00491890
static void InitializeCharacterBonusVortexEventName()
{
	g_characterBonusVortexEventName = CharBuffer24("HIT A BONUS VORTEX");
}

// STUB: LEGORACERS2 0x00491990
static void InitializeCharacterRacerEventName()
{
	g_characterRacerEventName = CharBuffer24("HIT A RACER");
}

static void EnsureCharacterBodyEventNames()
{
	static LegoBool32 initialized;
	if (!initialized) {
		InitializeCharacterSurfaceEventName();
		InitializeCharacterPickupEventName();
		InitializeCharacterGoldenBrickEventName();
		InitializeCharacterBonusVortexEventName();
		InitializeCharacterRacerEventName();
		initialized = TRUE;
	}
}

static const CharBuffer24* GetCharacterBodyEventName(LegoU32 p_index)
{
	const CharBuffer24* names[] = {
		&g_characterSurfaceEventName,
		&g_characterPickupEventName,
		&g_characterRacerEventName,
		&g_characterGoldenBrickEventName,
		&g_characterBonusVortexEventName
	};
	return names[p_index];
}

// STUB: LEGORACERS2 0x004a6440
static const CharacterContactMaterial* __fastcall GetCharacterContactMaterial(PhysicalLifecycleState* p_state, void*)
{
	typedef const CharacterContactMaterial*(__fastcall * GetMaterial)(CarDisplayIndexedRecordOwner*, void*, LegoU32);
	GetMaterial getMaterial = *(GetMaterial*) ((const LegoU8*) *(void**) g_carDisplayIndexedRecordOwner + 0x50);
	return getMaterial(g_carDisplayIndexedRecordOwner, NULL, p_state->m_unk0x58);
}
