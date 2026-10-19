#include "audioservice.h"
#include "gamecontroller.h"
#include "gameobjectdispatch.h"
#include "gameobjectfactoryregistry.h"
#include "matrix3.h"
#include "memoryheap.h"
#include "reverbmanager.h"
#include "vector3.h"

#include <new>

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);
void __fastcall InitializeReverbSettingsObject(GameObject*, void*);
void __fastcall RemoveReverbSettingsObject(GameObject*, void*);
GameObject* __fastcall DestroyReverbArea(GameObject*, void*, LegoU32);
GameObject* __fastcall DestroyReverbGateway(GameObject*, void*, LegoU32);
GameObject* __fastcall ConstructReverbArea(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
GameObject* __fastcall ConstructReverbGateway(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
GameObject* __fastcall ConstructReverbInit(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);

struct ReverbGatewayTemplateView {
	GameObjectTemplate m_base;
	Vector3 m_reference;       // 0x34
	LegoFloat m_quaternion[4]; // 0x40
	LegoFloat m_halfWidth;     // 0x50
	LegoFloat m_halfDepth;     // 0x54
	LegoU32 m_mode;            // 0x58
};
DECOMP_SIZE_ASSERT(ReverbGatewayTemplateView, 0x5c);

typedef LegoU32(__fastcall* FactoryZero)(GameObjectFactoryEntry*, void*);
typedef GameObject*(__fastcall* FactoryConstruct)(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate*);
typedef LegoU32(__fastcall* FactoryFourArgumentZero)(
	GameObjectFactoryEntry*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);

struct ReverbFactoryDispatch {
	const LegoChar*(__fastcall* m_getClassName)(GameObjectFactoryEntry*, void*);
	FactoryConstruct m_construct;
	FactoryZero m_noOp;
	FactoryFourArgumentZero m_zeroForFourArguments;
};
DECOMP_SIZE_ASSERT(ReverbFactoryDispatch, 0x10);

static const LegoChar* __fastcall GetReverbFactoryClassName(GameObjectFactoryEntry* p_factory, void*)
{
	return p_factory->m_className.m_data;
}

static LegoU32 __fastcall ReverbGatewayIsActive(GameObject*, void*, LegoU32)
{
	return 1;
}

// Retail table VA: 0x00531188 (object) and 0x005311d8 (factory).
extern const GameObjectDispatchView g_reverbAreaObjectDispatch;
const GameObjectDispatchView g_reverbAreaObjectDispatch = {
	&DestroyReverbArea,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseInitialize,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&GameObjectBaseRemove,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument
};

// Retail table VA: 0x005311f8; slot +0x18 aliases the true predicate at 0x0049a640.
extern const GameObjectDispatchView g_reverbGatewayObjectDispatch;
const GameObjectDispatchView g_reverbGatewayObjectDispatch = {
	&DestroyReverbGateway,       &GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&GameObjectBaseInitialize,   &GameObjectBaseTrue,
	&ReverbGatewayIsActive,      &GameObjectBaseRemove,
	&GameObjectBaseZero,         &GameObjectBaseZero,
	&GameObjectBaseNoOp,         &GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,    &GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,         &GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,         &GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion, &GameObjectBaseNoOpOneArgument
};

extern const GameObjectDispatchView g_reverbInitObjectDispatch;
const GameObjectDispatchView g_reverbInitObjectDispatch = {
	&GameObjectBaseDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&InitializeReverbSettingsObject,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&RemoveReverbSettingsObject,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument
};

extern const ReverbFactoryDispatch g_reverbAreaFactoryDispatch;
const ReverbFactoryDispatch g_reverbAreaFactoryDispatch = {
	&GetReverbFactoryClassName,
	&ConstructReverbArea,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};
extern const ReverbFactoryDispatch g_reverbGatewayFactoryDispatch;
const ReverbFactoryDispatch g_reverbGatewayFactoryDispatch = {
	&GetReverbFactoryClassName,
	&ConstructReverbGateway,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};
extern const ReverbFactoryDispatch g_reverbInitFactoryDispatch;
const ReverbFactoryDispatch g_reverbInitFactoryDispatch = {
	&GetReverbFactoryClassName,
	&ConstructReverbInit,
	(FactoryZero) &GameObjectBaseZero,
	(FactoryFourArgumentZero) &GameObjectFactoryReturnZeroForFourArguments
};

struct ReverbFactoryRegistration : GameObjectFactoryEntry {
	ReverbFactoryRegistration(const LegoChar* p_name, const ReverbFactoryDispatch* p_dispatch)
		: GameObjectFactoryEntry(p_name)
	{
		m_vtable = (const GameObjectFactoryDispatch*) p_dispatch;
	}
};

struct ReverbAreaFactoryRegistration : ReverbFactoryRegistration {
	ReverbAreaFactoryRegistration() : ReverbFactoryRegistration("cReverbArea", &g_reverbAreaFactoryDispatch) {}
};
struct ReverbGatewayFactoryRegistration : ReverbFactoryRegistration {
	ReverbGatewayFactoryRegistration() : ReverbFactoryRegistration("cReverbGateway", &g_reverbGatewayFactoryDispatch) {}
};
struct ReverbInitFactoryRegistration : ReverbFactoryRegistration {
	ReverbInitFactoryRegistration() : ReverbFactoryRegistration("cReverbInit", &g_reverbInitFactoryDispatch) {}
};
DECOMP_SIZE_ASSERT(ReverbAreaFactoryRegistration, 0x1c);
DECOMP_SIZE_ASSERT(ReverbGatewayFactoryRegistration, 0x1c);
DECOMP_SIZE_ASSERT(ReverbInitFactoryRegistration, 0x1c);
// GLOBAL: LEGORACERS2 0x0056cd58
ReverbAreaFactoryRegistration g_reverbAreaFactoryRegistration;
// GLOBAL: LEGORACERS2 0x0056cd38
ReverbGatewayFactoryRegistration g_reverbGatewayFactoryRegistration;
// GLOBAL: LEGORACERS2 0x0056cd98
ReverbInitFactoryRegistration g_reverbInitFactoryRegistration;

void SetVector(Vector3* p_target, LegoFloat p_x, LegoFloat p_y, LegoFloat p_z)
{
	p_target->m_x = p_x;
	p_target->m_y = p_y;
	p_target->m_z = p_z;
}

// STUB: LEGORACERS2 0x0043ce00
ReverbVolumeView* __fastcall ConstructReverbAreaObject(
	ReverbVolumeView* p_object,
	void*,
	GameObjectTemplate* p_template
)
{
	new (p_object) GameObject(0x900, p_template, NULL);
	p_object->m_firstAudioParameter = *(LegoFloat*) ((LegoU8*) p_template + 0x34);
	p_object->m_secondAudioParameter = *(LegoFloat*) ((LegoU8*) p_template + 0x38);
	p_object->m_unk0x60 = *(LegoU32*) ((LegoU8*) p_template + 0x3c);
	p_object->m_underground = *(LegoU32*) ((LegoU8*) p_template + 0x40);
	p_object->m_planeCount = 0;
	p_object->m_vtable = &g_reverbAreaObjectDispatch;
	ReverbManager* manager = (ReverbManager*) g_gameController.m_reverbManager;
	manager->m_volumes[manager->m_volumeCount] = p_object;
	++manager->m_volumeCount;
	return p_object;
}

// STUB: LEGORACERS2 0x0043ce60
GameObject* __fastcall DestroyReverbArea(GameObject* p_object, void*, LegoU32 p_freeMemory)
{
	p_object->m_vtable = &g_reverbAreaObjectDispatch;
	((ReverbManager*) g_gameController.m_reverbManager)->Reset();
	return GameObjectBaseDestroy(p_object, NULL, p_freeMemory);
}

// STUB: LEGORACERS2 0x0043ced0
GameObject* __fastcall ConstructReverbArea(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate* p_template)
{
	p_template->m_flags |= 4;
	void* storage = AllocateHeapMemory(0x8c);
	if (storage == NULL) {
		return NULL;
	}
	return ConstructReverbAreaObject((ReverbVolumeView*) storage, NULL, p_template);
}

// STUB: LEGORACERS2 0x0043cf40
GameObject* __fastcall ConstructReverbGateway(
	GameObjectFactoryEntry*,
	void*,
	LegoU32,
	GameObjectTemplate* p_baseTemplate
)
{
	ReverbGatewayTemplateView* p_template = (ReverbGatewayTemplateView*) p_baseTemplate;
	p_template->m_base.m_flags |= 4;
	ReverbBoundaryPlane* plane = (ReverbBoundaryPlane*) AllocateHeapMemory(sizeof(ReverbBoundaryPlane));
	if (plane == NULL) {
		return NULL;
	}
	new (plane) GameObject(0x900, p_baseTemplate, NULL);
	plane->m_reference = p_template->m_reference;
	plane->m_mode = p_template->m_mode;
	plane->m_radiusSquared = -1.0f;
	plane->m_vtable = &g_reverbGatewayObjectDispatch;

	Matrix3 rotation;
	rotation.FUN_004a4d30(p_template->m_quaternion);
	Vector3 axisA = {
		rotation.m_values[0][0] * (p_template->m_halfWidth * 0.5f),
		rotation.m_values[1][0] * (p_template->m_halfWidth * 0.5f),
		rotation.m_values[2][0] * (p_template->m_halfWidth * 0.5f)
	};
	Vector3 axisB = {
		rotation.m_values[0][1] * (p_template->m_halfDepth * 0.5f),
		rotation.m_values[1][1] * (p_template->m_halfDepth * 0.5f),
		rotation.m_values[2][1] * (p_template->m_halfDepth * 0.5f)
	};
	SetVector(&plane->m_normal, -rotation.m_values[0][2], -rotation.m_values[1][2], -rotation.m_values[2][2]);
	plane->m_corners[0] = plane->m_reference;
	plane->m_corners[0].Add(&axisB);
	plane->m_corners[1] = plane->m_corners[0];
	plane->m_corners[0].Subtract(&axisA);
	plane->m_corners[1].Add(&axisA);
	plane->m_corners[2] = plane->m_reference;
	plane->m_corners[2].Subtract(&axisB);
	plane->m_corners[3] = plane->m_corners[2];
	plane->m_corners[2].Add(&axisA);
	plane->m_corners[3].Subtract(&axisA);

	Vector3 radiusOffset;
	radiusOffset.m_x = plane->m_corners[0].m_x - plane->m_reference.m_x;
	radiusOffset.m_y = plane->m_corners[0].m_y - plane->m_reference.m_y;
	radiusOffset.m_z = plane->m_corners[0].m_z - plane->m_reference.m_z;
	radiusOffset.Scale(1.5f);
	plane->m_radiusSquared = radiusOffset.Dot(&radiusOffset) + 5.0f;

	if (p_template->m_base.m_second.m_data[0] != '\0') {
		LegoU32 type = (p_template->m_base.m_flags >> 5) & 0xff;
		LegoU32 typeMask[8];
		for (LegoU32 index = 0; index < 8; ++index) {
			typeMask[index] = type == 0xfc ? 0xffffffff : 0;
		}
		if (type != 0xfc) {
			typeMask[type >> 5] = 1u << (type & 0x1f);
		}
		ReverbVolumeView* volume =
			(ReverbVolumeView*) FindGameObjectByName(p_template->m_base.m_second.m_data, 1, typeMask);
		volume->m_planes[volume->m_planeCount] = plane;
		++volume->m_planeCount;
	}
	return plane;
}

// STUB: LEGORACERS2 0x0043d290
GameObject* __fastcall DestroyReverbGateway(GameObject* p_object, void*, LegoU32 p_freeMemory)
{
	p_object->m_vtable = &g_reverbGatewayObjectDispatch;
	((ReverbManager*) g_gameController.m_reverbManager)->Reset();
	return GameObjectBaseDestroy(p_object, NULL, p_freeMemory);
}

// STUB: LEGORACERS2 0x0043dd20
GameObject* __fastcall ConstructReverbInit(GameObjectFactoryEntry*, void*, LegoU32, GameObjectTemplate* p_template)
{
	void* storage = AllocateHeapMemory(0x58);
	if (storage == NULL) {
		return NULL;
	}
	GameObject* object = new (storage) GameObject(0xa00, p_template, NULL);
	object->m_vtable = &g_reverbInitObjectDispatch;
	return object;
}

// STUB: LEGORACERS2 0x0043dd60
void __fastcall InitializeReverbSettingsObject(GameObject* p_object, void*)
{
	p_object->Initialize();
	GameObjectTemplate* p_template = p_object->m_template;
	((ReverbManager*) g_gameController.m_reverbManager)
		->Configure(
			*(LegoU32*) ((LegoU8*) p_template + 0x4c),
			*(LegoFloat*) ((LegoU8*) p_template + 0x50),
			*(LegoFloat*) ((LegoU8*) p_template + 0x54)
		);
	((ReverbManager*) g_gameController.m_reverbManager)->ResetCameraVolume(0, 1);
}

// STUB: LEGORACERS2 0x0043dda0
void __fastcall RemoveReverbSettingsObject(GameObject* p_object, void*)
{
	p_object->Remove();
	ReverbManager* manager = (ReverbManager*) g_gameController.m_reverbManager;
	if (manager != NULL) {
		manager->m_currentState = g_reverbDefaultState;
	}
	AudioService* audio = GetAudioService(TRUE);
	audio->m_vtable->m_setParameters(audio, NULL, 0.0f, 0.0f);
}
