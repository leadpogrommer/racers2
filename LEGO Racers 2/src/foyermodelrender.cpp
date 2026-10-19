#include "foyermodelrender.h"

#include "cardisplay.h"
#include "gameobjectdispatch.h"
#include "gameobjectmodelbinding.h"
#include "gameobjectrenderprep.h"
#include "memoryheap.h"
#include "physicallifecycle.h"

#include <new>

struct FoyerModelRenderTemplate {
	GameObjectTemplate m_base;
	PhysicalLifecycleParameters m_physical; // 0x34
	LegoChar m_path[128];                   // 0x6c
	LegoU32 m_childFlags;                   // 0xec
};
DECOMP_SIZE_ASSERT(FoyerModelRenderTemplate, 0xf0);
DECOMP_STATIC_ASSERT(offsetof(FoyerModelRenderTemplate, m_physical) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(FoyerModelRenderTemplate, m_path) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(FoyerModelRenderTemplate, m_childFlags) == 0xec);

LegoU32 __fastcall IntegrateFoyerLifecycleMotion(PhysicalLifecycleState*, void*);
LegoU32 __fastcall ApplyFoyerLifecycleForces(PhysicalLifecycleState*, void*);
static GameObject* __fastcall DestroyFoyerLifecycleOwner(PhysicalLifecycleState*, void*, LegoU32);
LegoU32 __fastcall IntegrateFoyerModelRenderMotion(GameObject*, void*);
LegoU32 __fastcall ApplyFoyerModelRenderForces(GameObject*, void*);

static const void* __fastcall GetFoyerModelRenderTypeAddress(GameObject* p_object, void*)
{
	// Retail 0x40ec90 is the shared LEA EAX,[ECX+4]; RET body.
	return &p_object->m_type;
}

#define FOYER_CALLBACK(p_function) ((void(__cdecl*)())(p_function))
// Retail ABI function-pointer table: 0x00536264.
extern const PhysicalLifecycleDispatch g_foyerModelLifecycleDispatch;
const PhysicalLifecycleDispatch g_foyerModelLifecycleDispatch = {
	{FOYER_CALLBACK(&IsLifecycleActive),
	 FOYER_CALLBACK(&GetLifecycleParentValue),
	 FOYER_CALLBACK(&GameObjectBaseTrue),
	 FOYER_CALLBACK(&CarDisplayLifecycleCopyTargetData),
	 FOYER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 FOYER_CALLBACK(&GetLifecyclePosition),
	 FOYER_CALLBACK(&GetLifecycleRotation),
	 FOYER_CALLBACK(&GetLifecycleOrientation),
	 FOYER_CALLBACK(&SetLifecyclePosition),
	 FOYER_CALLBACK(&SetLifecycleOrientation),
	 FOYER_CALLBACK(&GetLifecycleParent),
	 FOYER_CALLBACK(&IntegrateFoyerLifecycleMotion),
	 FOYER_CALLBACK(&ApplyFoyerLifecycleForces),
	 FOYER_CALLBACK(&GameObjectBaseZeroOneArgument),
	 FOYER_CALLBACK(&GameObjectBaseNoOpOneArgument),
	 FOYER_CALLBACK(&GameObjectBaseNoOpTwoArguments),
	 FOYER_CALLBACK(&CaptureLifecycleTransform),
	 FOYER_CALLBACK(&GetCapturedLifecycleTransform),
	 FOYER_CALLBACK(&DestroyFoyerLifecycleOwner),
	 FOYER_CALLBACK(&ClearLifecycleMotion)}
};
#undef FOYER_CALLBACK

// Retail ABI function-pointer table: 0x005362b4.
const FoyerModelRenderDispatch g_foyerModelRenderDispatch = {
	{&DestroyFoyerModelRenderObject,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeFoyerModelRenderObject,
	 &GameObjectBaseTrue,
	 &GameObjectBaseZeroOneArgument,
	 &CarDisplayModelRemove,
	 &IntegrateFoyerModelRenderMotion,
	 &ApplyFoyerModelRenderForces,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseDrawChild,
	 &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,
	 &GameObjectBaseNoOpTwoArguments,
	 &GameObjectBaseZero,
	 &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion,
	 (GameObjectDispatchView::Update) &UpdateFoyerModelRenderRegion},
	&GetFoyerModelRenderTypeAddress
};

void InitializeFoyerModelRenderBase(GameObject* p_object, GameObjectTemplate* p_template)
{
	PhysicalLifecycleState* state = (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
	FoyerModelRenderTemplate* parameters = (FoyerModelRenderTemplate*) p_template;
	ConstructPhysicalLifecycleState(state, NULL, p_object, &parameters->m_physical);
	p_object->m_vtable = &g_foyerModelRenderDispatch.m_object;
	state->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_foyerModelLifecycleDispatch;
	const Quaternion* orientation = state->m_parent == NULL ? &state->m_orientation : state->GetWorldOrientation();
	const Vector3* position = state->m_parent == NULL ? &state->m_position : state->GetWorldPosition();
	ConstructGameObjectModelBinding(
		p_object,
		NULL,
		(const LegoU8*) parameters->m_path,
		&position->m_x,
		&orientation->m_x,
		parameters->m_childFlags & 1
	);
}

void ReleaseFoyerModelRenderBase(GameObject* p_object)
{
	p_object->m_vtable = &g_foyerModelRenderDispatch.m_object;
	PhysicalLifecycleState* state = (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
	state->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_foyerModelLifecycleDispatch;
	DestroyGameObjectModelBindingForOwner(p_object, NULL);
	state->m_base.m_vtable = (const GameLifecycleDispatchView*) &g_carDisplayBaseTransformDispatch;
}

void RemoveFoyerModelRenderObject(GameObject* p_object)
{
	CarDisplayModelRemove(p_object, NULL);
}

void ApplyFoyerModelRenderTransform(GameObject* p_object, const LegoFloat* p_position, const LegoFloat* p_rotation)
{
	PhysicalLifecycleState* state = (PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58);
	const LifecycleTransformDispatch* dispatch = state->GetTransformDispatch();
	dispatch->m_setPosition(state, NULL, (const Vector3*) p_position);
	typedef void(__fastcall * SetOrientation)(PhysicalLifecycleState*, void*, const Quaternion*);
	SetOrientation setOrientation = *(SetOrientation*) ((const LegoU8*) state->m_base.m_vtable + 0x24);
	setOrientation(state, NULL, (const Quaternion*) p_rotation);
}

// STUB: LEGORACERS2 0x00433480
LegoU32 __fastcall IntegrateFoyerLifecycleMotion(PhysicalLifecycleState* p_state, void*)
{
	return IntegrateFoyerModelRenderMotion((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

// STUB: LEGORACERS2 0x00433490
LegoU32 __fastcall ApplyFoyerLifecycleForces(PhysicalLifecycleState* p_state, void*)
{
	return ApplyFoyerModelRenderForces((GameObject*) ((LegoU8*) p_state - 0x58), NULL);
}

// FUNCTION: LEGORACERS2 0x00473b10
GameObject* __fastcall DestroyFoyerModelRenderObject(GameObject* p_object, void*, LegoU32 p_flags)
{
	DestructFoyerModelRenderObject(p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x0048da40
static GameObject* __fastcall DestroyFoyerLifecycleOwner(PhysicalLifecycleState* p_state, void*, LegoU32 p_flags)
{
	return DestroyFoyerModelRenderObject((GameObject*) ((LegoU8*) p_state - 0x58), NULL, p_flags);
}

// STUB: LEGORACERS2 0x004bce10
GameObject* __fastcall ConstructFoyerModelRenderObject(
	GameObject* p_object,
	void*,
	LegoU32 p_type,
	GameObjectTemplate* p_template
)
{
	new (p_object) GameObject(p_type, p_template, p_object == NULL ? NULL : (LegoU8*) p_object + 0x58);
	InitializeFoyerModelRenderBase(p_object, p_template);
	return p_object;
}

// STUB: LEGORACERS2 0x004bcea0
void __fastcall DestructFoyerModelRenderObject(GameObject* p_object, void*)
{
	ReleaseFoyerModelRenderBase(p_object);
	p_object->GameObject::~GameObject();
}

// STUB: LEGORACERS2 0x004bced0
void __fastcall InitializeFoyerModelRenderObject(GameObject* p_object, void*)
{
	InitializeGameObjectModelBinding((GameObjectModelBinding*) p_object->m_unk0x48, NULL);
	ResetPhysicalLifecycleState((PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58), NULL, p_object);
	p_object->GameObject::Initialize();
}

// STUB: LEGORACERS2 0x004bcf10
LegoU32 __fastcall IntegrateFoyerModelRenderMotion(GameObject* p_object, void*)
{
	return IntegratePhysicalLifecycleMotion((PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58), NULL);
}

// STUB: LEGORACERS2 0x004bcf20
LegoU32 __fastcall ApplyFoyerModelRenderForces(GameObject* p_object, void*)
{
	return ApplyPhysicalLifecycleDampingAndGravity((PhysicalLifecycleState*) ((LegoU8*) p_object + 0x58), NULL);
}

struct FoyerModelRenderDrawState {
	undefined m_prefix[0x180];
	LegoFloat m_visibilityOverride;
	LegoU32 m_anyView;
	void* m_view;
};
DECOMP_STATIC_ASSERT(offsetof(FoyerModelRenderDrawState, m_visibilityOverride) == 0x180);

// STUB: LEGORACERS2 0x004bcf60
void __fastcall DrawFoyerModelRenderObject(GameObject* p_object, void*)
{

	FoyerModelRenderDrawState* state = (FoyerModelRenderDrawState*) p_object;
	if (p_object->m_unk0x48 != NULL && (state->m_anyView != 0 || state->m_view == g_unk0x00679ef0)) {
		LegoBool32 visible = p_object->IsVisible();
		if (state->m_visibilityOverride > 0.0f) {
			((GameObjectModelBinding*) p_object->m_unk0x48)->m_unk0xC0 = *(LegoU32*) &state->m_visibilityOverride;
		}
		// Retail tests only x87 carry, accepting unordered overrides here.
		if (!(state->m_visibilityOverride >= 1.0f)) {
			GameObjectModelBinding* binding = (GameObjectModelBinding*) p_object->m_unk0x48;
			if (visible == 1) {
				binding->m_vtable->m_update(binding, NULL);
			}
			else {
				UpdateGameObjectModelBindingVisibility(binding, NULL);
			}
		}
	}
}
