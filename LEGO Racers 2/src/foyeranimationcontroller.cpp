#include "foyeranimationcontroller.h"

#include "foyeranimation.h"
#include "foyertemplateinstance.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"
#include "winfile.h"

#include <new>
#include <string.h>

static GameObject* __fastcall DestroyFoyerAnimationController(GameObject*, void*, LegoU32);
static LegoU32 __fastcall GetFoyerAnimationControllerSize(GameObject*, void*);
static void __fastcall InitializeFoyerAnimationController(GameObject* p_object, void*)
{
	((FoyerAnimationController*) p_object)->Initialize();
}
static void __fastcall RemoveFoyerAnimationController(GameObject* p_object, void*)
{
	((FoyerAnimationController*) p_object)->Remove();
}
static LegoU32 __fastcall UpdateFoyerAnimationController(GameObject* p_object, void*, LegoFloat p_step)
{
	return ((FoyerAnimationController*) p_object)->Update(p_step);
}

// Retail object dispatch: 0x00536604, all 21 slots.
const FoyerAnimationControllerDispatch g_foyerAnimationControllerDispatch = {
	{&DestroyFoyerAnimationController,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeFoyerAnimationController,
	 &GameObjectBaseTrue,
	 (GameObjectDispatchView::ValueArgument) &UpdateFoyerAnimationController,
	 &RemoveFoyerAnimationController,
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
	 &GameObjectBaseNoOpOneArgument},
	&GetFoyerAnimationControllerSize
};

// STUB: LEGORACERS2 0x004c51b0
FoyerAnimationController::FoyerAnimationController(FoyerTemplateInstance* p_parent, void* p_parsed)
	: GameObject(0x700, NULL, NULL)
{
	m_vtable = &g_foyerAnimationControllerDispatch.m_object;
	m_parent = p_parent;
	p_parent->m_animationChild = this;
	m_parsedTemplate = p_parsed;
}

// FUNCTION: LEGORACERS2 0x004c51f0
static LegoU32 __fastcall GetFoyerAnimationControllerSize(GameObject*, void*)
{
	return 0x1ac;
}

// FUNCTION: LEGORACERS2 0x004c5200
static GameObject* __fastcall DestroyFoyerAnimationController(GameObject* p_object, void*, LegoU32 p_flags)
{
	((FoyerAnimationController*) p_object)->~FoyerAnimationController();
	if (p_flags & 1) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004c5220
FoyerAnimationController::~FoyerAnimationController()
{
	m_vtable = &g_foyerAnimationControllerDispatch.m_object;
}

// STUB: LEGORACERS2 0x004c5230
void FoyerAnimationController::Initialize()
{
	GameObject::Initialize();
	m_skeletonCount = m_parent->m_skeletonCount;
	m_pose.m_quaternions =
		(Quaternion*) GetMemoryHeap()->Allocate(m_skeletonCount * sizeof(Quaternion), "QSpace for anim master");
	for (LegoS32 i = 0; i < 4; ++i) {
		void* storage = AllocateHeapMemory(sizeof(FoyerAnimationPlayer));
		m_players[i] = storage == NULL ? NULL : new (storage) FoyerAnimationPlayer(m_skeletonCount);
	}
	m_cache = AcquireFoyerAnimationMasterCache();
	LoadTracks();
	RetainBindings();
	m_mode = 2;
	m_defaultAnimationBound = 0;
	m_rate = 1.0f;
}

// STUB: LEGORACERS2 0x004c52e0
void FoyerAnimationController::Remove()
{
	ReleaseBindings();
	ReleaseFoyerAnimationMasterCache();
	for (LegoS32 i = 3; i >= 0; --i) {
		if (m_players[i] != NULL) {
			m_players[i]->~FoyerAnimationPlayer();
			FreeHeapMemory(m_players[i]);
		}
	}
	GetMemoryHeap()->Free(m_pose.m_quaternions);
	GameObject::Remove();
}

// STUB: LEGORACERS2 0x004c5340
LegoU32 FoyerAnimationController::Update(LegoFloat p_step)
{
	if (m_mode != 1) {
		if (m_mode != 2) {
			return 0;
		}
		EnsureRunLoop();
	}
	LegoU32 quaternions = (m_parent->m_state & 0x4000) != 0;
	if (quaternions) {
		m_parent->m_state &= ~0x4000u;
	}
	AdvancePlayers(p_step, quaternions);
	ApplyBlendedPose(quaternions);
	return 1;
}

// STUB: LEGORACERS2 0x004c53a0
void FoyerAnimationController::LoadTracks()
{
	void* parsed = m_parsedTemplate;
	const LegoChar* path = (const LegoChar*) m_parent->m_templatePayload;
	if (parsed == NULL) {
		WinFile file(path, 0, NULL);
		LegoU32 length = file.GetManagedSize();
		parsed = GetMemoryHeap()->Allocate(length, "ANIMMASTER buffer");
		file.Read(parsed, length, 1);
	}
	m_trackCount = *(LegoS32*) ((LegoU8*) parsed + 0x18);
	const LegoU8* records = (LegoU8*) parsed + *(LegoU32*) ((LegoU8*) parsed + 0x1c);
	LegoS32 lastSlash = 0;
	for (LegoS32 i = 0; i < (LegoS32) strlen(path); ++i) {
		if (path[i] == '\\') {
			lastSlash = i;
		}
	}
	lastSlash = lastSlash > 0 ? lastSlash + 1 : 0;
	LegoChar directory[256];
	strcpy(directory, path);
	directory[lastSlash] = '\0';
	for (LegoS32 track = 0; track < m_trackCount; ++track) {
		CharBuffer24 descriptor((const LegoChar*) records);
		m_tracks[track] = m_cache->FindOrAdd(directory, &descriptor, 0);
		records += 0x20;
	}
	if (m_parsedTemplate == NULL) {
		GetMemoryHeap()->Free(parsed);
	}
	m_parsedTemplate = NULL;
}

// FUNCTION: LEGORACERS2 0x004c54f0
void FoyerAnimationController::RetainBindings()
{
	for (LegoS32 i = 0; i < m_trackCount; ++i) {
		m_cache->Retain(m_tracks[i]);
	}
}

// FUNCTION: LEGORACERS2 0x004c5530
void FoyerAnimationController::ReleaseBindings()
{
	for (LegoS32 i = m_trackCount - 1; i >= 0; --i) {
		m_cache->Release(m_tracks[i]);
	}
}

// FUNCTION: LEGORACERS2 0x004c5750
void FoyerAnimationController::AdvancePlayers(LegoFloat p_step, LegoU32 p_quaternions)
{
	for (LegoS32 i = 0; i < 4; ++i) {
		if (m_players[i]->m_state != 0) {
			m_players[i]->Update(p_step, p_quaternions);
		}
	}
}

// STUB: LEGORACERS2 0x004c57d0
void FoyerAnimationController::ApplyBlendedPose(LegoU32 p_quaternions)
{
	FoyerAnimationPlayer* players[4];
	LegoFloat weights[4];
	LegoS32 count = 0;
	for (LegoS32 i = 0; i < 4; ++i) {
		if (m_players[i]->m_state != 0 && m_players[i]->m_weight > 0.0f) {
			players[count] = m_players[i];
			weights[count++] = m_players[i]->m_weight;
		}
	}
	if (count == 0) {
		return;
	}
	if (count == 1) {
		ApplyPose(p_quaternions, players[0]);
		return;
	}
	LegoFloat totalWeight = weights[0];
	for (LegoS32 index = 1; index < count; ++index) {
		totalWeight += weights[index];
		const FoyerAnimationPose* first = index == 1 ? &players[0]->m_pose : &m_pose;
		BlendPoses(first, &players[index]->m_pose, 1.0f - (1.0f / totalWeight) * weights[index], p_quaternions);
	}
	ApplyPose(p_quaternions, NULL);
}

// STUB: LEGORACERS2 0x004c58c0
void FoyerAnimationController::ApplyPose(LegoU32 p_quaternions, const FoyerAnimationPlayer* p_player)
{
	m_parent->m_state |= 1;
	const FoyerAnimationPose* pose = p_player == NULL ? &m_pose : &p_player->m_pose;
	m_parent->m_skeletonOffset = pose->m_position;
	m_parent->m_localVelocity = pose->m_velocity;
	if (p_quaternions) {
		for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
			m_parent->m_skeletons[i].m_localOrientation = pose->m_quaternions[i];
		}
	}
}

// The retail AH40 test treats unordered comparisons like equality.
static LegoU32 EqualOrUnordered(LegoFloat p_first, LegoFloat p_second)
{
	LegoU32 status;
	__asm {
		fld p_first
		fcomp p_second
		fnstsw ax
		movzx eax, ah
		and eax, 40h
		mov status, eax
	}
	return status;
}

// STUB: LEGORACERS2 0x004c59b0
void FoyerAnimationController::BlendPoses(
	const FoyerAnimationPose* p_first,
	const FoyerAnimationPose* p_second,
	LegoFloat p_alpha,
	LegoU32 p_quaternions
)
{
	LegoFloat secondWeight = 1.0f - p_alpha;
	// Read both inputs before writing: the first pose may be our scratch pose.
	Vector3 firstPosition = p_first->m_position;
	Vector3 secondPosition = p_second->m_position;
	Vector3 firstVelocity = p_first->m_velocity;
	Vector3 secondVelocity = p_second->m_velocity;
	m_pose.m_position.m_x = secondWeight * secondPosition.m_x + p_alpha * firstPosition.m_x;
	m_pose.m_position.m_y = secondWeight * secondPosition.m_y + p_alpha * firstPosition.m_y;
	m_pose.m_position.m_z = secondWeight * secondPosition.m_z + p_alpha * firstPosition.m_z;
	m_pose.m_velocity.m_x = secondWeight * secondVelocity.m_x + p_alpha * firstVelocity.m_x;
	m_pose.m_velocity.m_y = secondWeight * secondVelocity.m_y + p_alpha * firstVelocity.m_y;
	m_pose.m_velocity.m_z = secondWeight * secondVelocity.m_z + p_alpha * firstVelocity.m_z;
	if (p_quaternions) {
		if (EqualOrUnordered(p_alpha, 1.0f)) {
			for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
				m_pose.m_quaternions[i] = p_first->m_quaternions[i];
			}
		}
		else if (EqualOrUnordered(p_alpha, 0.0f)) {
			for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
				m_pose.m_quaternions[i] = p_second->m_quaternions[i];
			}
		}
		else {
			for (LegoS32 i = 0; i < m_skeletonCount; ++i) {
				m_pose.m_quaternions[i].Interpolate(&p_first->m_quaternions[i], &p_second->m_quaternions[i], p_alpha);
			}
		}
	}
}

// STUB: LEGORACERS2 0x004c5b50
void FoyerAnimationController::EnsureRunLoop()
{
	if (m_defaultAnimationBound != 1) {
		FoyerAnimationControllerView* view = (FoyerAnimationControllerView*) this;
		StartFoyerAnimation(view, NULL, "RUN_LOOP", 0);
		LegoU32 rate;
		memcpy(&rate, &m_rate, sizeof(rate));
		SetFoyerAnimationLayerValue(view, NULL, 0, rate);
		m_defaultAnimationBound = 1;
	}
}
