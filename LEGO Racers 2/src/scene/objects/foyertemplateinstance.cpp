#include "scene/objects/foyertemplateinstance.h"

#include "core/strings/charbuffer24.h"
#include "game/character/charactervisual.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectmodelbinding.h"
#include "io/files/winfile.h"
#include "math/matrix3.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "physics/dynamics/physicallifecycle.h"
#include "scene/animation/foyeranimationcontroller.h"
#include "scene/animation/foyerskeleton.h"

#include <new>
#include <string.h>

// Retail object dispatch: 0x00536550. Complete inventory is verified when linked.
const FoyerTemplateInstanceDispatch g_foyerTemplateInstanceDispatch = {
	{&DestroyCharacterVisualHead,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeFoyerTemplateInstance,
	 &GameObjectBaseTrue,
	 (GameObjectDispatchView::ValueArgument) &UpdateFoyerTemplateInstance,
	 &RemoveFoyerTemplateInstance,
	 &IntegrateFoyerTemplateMotion,
	 &ApplyFoyerTemplateForces,
	 &GameObjectBaseNoOp,
	 &UpdateFoyerTemplateAttachment,
	 &DrawFoyerModelRenderObject,
	 &GameObjectBaseNoOpOneArgument,
	 &GameObjectBaseZero,
	 &GameObjectBaseNoOpTwoArguments,
	 &GetFoyerTemplateObjectCategory,
	 &GameObjectBaseZeroTwoArguments,
	 &GameObjectBaseUpdateRegion,
	 (GameObjectDispatchView::Update) &UpdateFoyerModelRenderRegion},
	&GetFoyerTemplateInstanceObjectSize
};

// STUB: LEGORACERS2 0x004c1ee0
FoyerTemplateInstance::FoyerTemplateInstance(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, (LegoU8*) this + 0x58)
{
	InitializeCharacterVisualHeadBase(this, p_template);
	m_animationChild = NULL;
	memcpy(m_templatePayload, (const LegoU8*) p_template + 0xf0, 0x80);
	m_state = 0;
	m_vtable = &g_foyerTemplateInstanceDispatch.m_object;
	m_physical.m_base.m_vtable = (const GameLifecycleDispatchView*) &g_foyerTemplateLifecycleDispatch;
}

// FUNCTION: LEGORACERS2 0x004c1f40
LegoU32 __fastcall GetFoyerTemplateInstanceObjectSize(GameObject*, void*)
{
	return 0x2a4;
}

// STUB: LEGORACERS2 0x004c1f50
void FoyerTemplateInstance::Initialize()
{
	MemoryHeap* heap = GetMemoryHeap();
	if ((m_flags & 0x800000) == 0) {
		WinFile file((const LegoChar*) m_templatePayload, 0, NULL);
		LegoU32 length = file.GetManagedSize();
		void* parsed = heap->AllocateFromEnd(length, "skeleton_buffer_space");
		file.Read(parsed, length, 1);
		m_skeletonCount = *(LegoS32*) ((LegoU8*) parsed + 0x10);
		m_attachmentSkeletonIndex = *(LegoS32*) ((LegoU8*) parsed + 0x20);
		m_skeletons = (FoyerSkeletonRecord*) AllocateHeapMemory(m_skeletonCount * sizeof(FoyerSkeletonRecord));
		if (m_skeletons != NULL) {
			for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
				ConstructFoyerSkeletonRecord(m_skeletons + i, NULL);
			}
		}
		for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
			const FoyerSkeletonDefinition* definition =
				(const FoyerSkeletonDefinition*) ((const LegoU8*) parsed + *(LegoU32*) ((LegoU8*) parsed + 0x14) +
												  i * 0x20);
			BindFoyerSkeletonRecord(m_skeletons + i, NULL, definition, this);
		}
		void* childStorage = AllocateHeapMemory(0x1ac);
		m_animationChild =
			childStorage == NULL ? NULL : (GameObject*) new (childStorage) FoyerAnimationController(this, parsed);
		m_animationChild->m_vtable->m_initialize(m_animationChild, NULL);
		m_animationChild->m_flags |= 0x400000;
		heap->Free(parsed);
	}
	InitializeFoyerModelRenderObject(this);
	m_state &= ~1u;
	Vector3 zero = {0.0f, 0.0f, 0.0f};
	m_skeletonOffset = zero;
	m_localVelocity = zero;
	RebuildFoyerSkeletonTransforms(this, NULL);
	GameObject* model = *(GameObject**) ((LegoU8*) m_unk0x48 + 8);
	m_attachment = *(void**) ((LegoU8*) model + 0x18);
	if (m_attachment != NULL) {
		*(GameObject**) m_attachment = this;
		*(LegoU32*) ((LegoU8*) m_attachment + 0x14) = 1;
		m_state |= 0x1000;
	}
	if (m_attachmentSkeletonIndex > 0) {
		FoyerSkeletonRecord* record = m_skeletons + m_attachmentSkeletonIndex;
		Matrix3 rotation;
		rotation.FUN_004a4d30(&record->m_worldOrientation.m_x);
		// Retail takes the three diagonal matrix entries here, not world position.
		Vector3 diagonal = {rotation.m_values[0][0], rotation.m_values[1][1], rotation.m_values[2][2]};
		Quaternion inverse = record->m_parent->m_worldOrientation;
		inverse.m_x = -inverse.m_x;
		inverse.m_y = -inverse.m_y;
		inverse.m_z = -inverse.m_z;
		rotation.FUN_004a4d30(&inverse.m_x);
		diagonal.FUN_004a4e00(&rotation);
		m_lookBlend = 0.0f;
		m_lookUp = diagonal;
		m_lookThreshold = 0.0f;
		m_lookEnabled = 0;
		m_lookFlags = 0;
		Vector3 zero = {0.0f, 0.0f, 0.0f};
		m_lookTarget = zero;
		Quaternion identity = {0.0f, 0.0f, 0.0f, 1.0f};
		m_lookOrientation = identity;
	}
}

// STUB: LEGORACERS2 0x004c2280
void FoyerTemplateInstance::Remove()
{
	RemoveFoyerModelRenderObject(this);
	GameObject* child = m_animationChild;
	child->m_vtable->m_remove(child, NULL);
	child = m_animationChild;
	if (child != NULL) {
		child->m_vtable->m_destroy(child, NULL, 1);
	}
	FreeHeapMemory(m_skeletons);
}

void __fastcall InitializeFoyerTemplateInstance(GameObject* p_object, void*)
{
	((FoyerTemplateInstance*) p_object)->Initialize();
}

void __fastcall RemoveFoyerTemplateInstance(GameObject* p_object, void*)
{
	((FoyerTemplateInstance*) p_object)->Remove();
}

FoyerTemplateInstance::~FoyerTemplateInstance()
{
	ReleaseFoyerModelRenderBase(this);
}
