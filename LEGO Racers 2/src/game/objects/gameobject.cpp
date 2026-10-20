#include "audio/service/audioservice.h"
#include "core/application/application.h"
#include "game/objects/gamemanager.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/objects/gameobjectgeometry.h"
#include "game/objects/gameobjectrenderprep.h"
#include "math/matrix3.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "physics/collision/collisionproxy.h"
#include "physics/collision/collisionservice.h"
#include "physics/dynamics/physicallifecycle.h"

GameObject* __cdecl FindGameObjectByName(const LegoChar* p_name, LegoU32 p_unused, const LegoU32* p_typeMask);
void __fastcall SetLifecycleParent(
	GameLifecycleComponent* p_lifecycle,
	void*,
	GameLifecycleComponent* p_parent,
	LegoU32 p_flag
);
void __fastcall ClearLifecycleParent(GameLifecycleComponent* p_lifecycle, void*, LegoU32 p_flag);

struct GameObjectChildMesh {
	undefined m_unk0x00[0x08];
	LegoFloat m_radius; // 0x08
	undefined m_unk0x0c[0x08];
	Vector3* m_vertices;
};

struct GameObjectChildDispatchView {
	undefined m_unk0x00[0x04];
	void(__fastcall* m_draw)(void*, void*); // +0x04, child draw callback
};

struct GameObjectChildBuffer {
	undefined m_unk0x00[0x08];
	GameObjectChildMesh* m_mesh;
	undefined m_unk0x0c[0x80];
	LegoFloat m_rotation[4];
	Vector3 m_position;
	undefined m_unk0xa8[0x10];
	LegoFloat m_projectedDepth; // 0xb8
	undefined m_unk0xbc[0x04];
	LegoFloat m_visibility;         // 0xc0
	LegoFloat m_visibilityScale;    // 0xc4
	LegoFloat m_visibilityDistance; // 0xc8
	LegoFloat m_visibilityRadius;   // 0xcc
	LegoFloat* m_transformedVertices;
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_mesh) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_rotation) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_position) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_projectedDepth) == 0xb8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_visibility) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_visibilityScale) == 0xc4);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_visibilityDistance) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_visibilityRadius) == 0xcc);
DECOMP_STATIC_ASSERT(offsetof(GameObjectChildBuffer, m_transformedVertices) == 0xd0);

struct GameObjectRegionKey {
	LegoU8 m_level;
	LegoU8 m_cell;
	LegoS16 m_index;
};
DECOMP_SIZE_ASSERT(GameObjectRegionKey, 0x04);

struct GameObjectRegionRecord {
	LegoFloat m_center[3];
	LegoFloat m_unk0x0c;
	LegoFloat m_halfExtent[3];
	LegoFloat m_unk0x1c;
};
DECOMP_SIZE_ASSERT(GameObjectRegionRecord, 0x20);
DECOMP_STATIC_ASSERT(offsetof(GameObjectRegionRecord, m_halfExtent) == 0x10);

struct GameObjectRegion {
	undefined m_unk0x00[0x04];
	LegoU32 m_mode;              // 0x04
	LegoU32 m_transformPosition; // 0x08
	undefined m_unk0x0c[0x04];
	Vector3 m_transformOrigin; // 0x10
	Vector3 m_gridOrigin;      // 0x1c
	Vector3 m_gridCenter;      // 0x28
	undefined m_unk0x34[0x10];
	Matrix3 m_transform;               // 0x44
	GameObjectRegionRecord* m_records; // 0x68
	undefined m_unk0x6c[0x3c];
	LegoU32 m_gridShift;    // 0xa8
	LegoU8* m_levelEntries; // 0xac
	undefined m_unk0xb0[0x10];
	LegoS32 m_levelCount; // 0xc0
	undefined m_unk0xc4[0x04];
	LegoFloat m_unk0xc8;
	LegoFloat m_verticalPadding; // 0xcc
	LegoS32 m_gridWidth;         // 0xd0
	LegoS32 m_gridDepth;         // 0xd4
	undefined m_unk0xd8[0x08];
	LegoFloat m_inverseCellWidth; // 0xe0
	LegoFloat m_inverseCellDepth; // 0xe4
};
DECOMP_STATIC_ASSERT(offsetof(GameObjectRegion, m_records) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(GameObjectRegion, m_levelCount) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(GameObjectRegion, m_inverseCellWidth) == 0xe0);

struct GameObjectVisibilityRegion {
	undefined m_unk0x00[0x14];
};

struct GameObjectVisibilityContextDispatch {
	undefined m_unk0x00[0x14];
	const Vector3*(__fastcall* m_getPosition)(void*, void*); // 0x14
};

struct GameObjectVisibilityContext {
	const GameObjectVisibilityContextDispatch* m_vtable;
	undefined m_unk0x04[0x54];
};

struct GameObjectShadowDispatch {
	undefined m_unk0x00[0x4c];
	void(__fastcall* m_updateObject)(void*, void*, LegoU32); // 0x4c
};

struct GameObjectShadowState {
	const GameObjectShadowDispatch* m_vtable;
	undefined m_unk0x04[0xac];
	struct MapRecord {
		LegoU8* m_bytes;
		LegoU32 m_unk0x04;
	};
	MapRecord* m_objectMaps; // 0xb0, eight-byte records
};

struct GameObjectCullingVolume {
	LegoFloat m_points[8][3]; // 0x00..0x5f
	LegoU32 m_skipPointTest;  // 0x60
	Vector3 m_center;         // 0x64
	LegoFloat m_radius;       // 0x70
};

struct GameObjectRenderPlane {
	Vector3 m_normal;
	Vector3 m_point;
};
DECOMP_SIZE_ASSERT(GameObjectRenderPlane, 0x18);

LegoFloat __fastcall GetGameObjectViewDepth(void* p_camera, void*, const Vector3* p_point);
static LegoBool32 __fastcall IsPointWithinGameObjectView(
	void* p_volume,
	void*,
	const Vector3* p_point,
	LegoFloat p_radius
);
static LegoBool32 __fastcall TestGameObjectRegion(
	GameObjectRegion* p_region,
	void*,
	const Vector3* p_position,
	LegoFloat p_radius,
	GameObjectRegionKey* p_key
);

GameObject* __fastcall GameObjectBaseDestroy(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseApplyChildTransform(GameObject*, void*, LegoU32, LegoU32);
void __fastcall GameObjectBaseNoOp(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseTrue(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseZero(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseZeroOneArgument(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseNoOpOneArgument(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseNoOpTwoArguments(GameObject*, void*, LegoU32, LegoU32);
LegoU32 __fastcall GameObjectBaseZeroTwoArguments(GameObject*, void*, LegoU32, LegoU32);
void __fastcall GameObjectBaseDrawChild(GameObject*, void*);
void __fastcall GameObjectBaseUpdateRegion(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseInitialize(GameObject*, void*);
void __fastcall GameObjectBaseRemove(GameObject*, void*);

// These entries mirror the base object's actual 20-slot table. The functions
// at 0x4038d0/0x403900/0x403910/0x403920/0x403930/0x4038e0/0x4038f0 are
// linker-folded compiler leaves elsewhere in the image.
const GameObjectDispatchView g_gameObjectBaseDispatch = GAME_OBJECT_BASE_DISPATCH_INITIALIZER;

struct GameObjectTransformedBuffer {
	undefined m_unk0x00[0x60];
	LegoU32 m_usable;
	Vector3 m_center;
	LegoFloat m_unk0x70;
};

DECOMP_STATIC_ASSERT(offsetof(GameObjectTransformedBuffer, m_usable) == 0x60);
DECOMP_STATIC_ASSERT(offsetof(GameObjectTransformedBuffer, m_center) == 0x64);
DECOMP_STATIC_ASSERT(offsetof(GameObjectTransformedBuffer, m_unk0x70) == 0x70);

static void RefreshChildGeometry(GameObjectChildBuffer* p_child)
{
	GameObjectTransformedBuffer* output = (GameObjectTransformedBuffer*) p_child->m_transformedVertices;
	if (output == NULL) {
		return;
	}

	Vector3* source = p_child->m_mesh->m_vertices;
	Vector3* vertices = (Vector3*) output;
	Matrix3 rotation;
	rotation.FUN_004a4d30(p_child->m_rotation);

	for (LegoU32 vertexIndex = 0; vertexIndex < 8; vertexIndex++) {
		vertices[vertexIndex] = source[vertexIndex];
		vertices[vertexIndex].FUN_004a4e00(&rotation);
		vertices[vertexIndex].m_x += p_child->m_position.m_x;
		vertices[vertexIndex].m_y += p_child->m_position.m_y;
		vertices[vertexIndex].m_z += p_child->m_position.m_z;
	}

	output->m_center.m_x = vertices[6].m_x;
	output->m_center.m_y = vertices[6].m_y;
	output->m_center.m_z = vertices[6].m_z;
	output->m_center.m_x = output->m_center.m_x - vertices[1].m_x;
	output->m_center.m_y = output->m_center.m_y - vertices[1].m_y;
	output->m_center.m_z = output->m_center.m_z - vertices[1].m_z;
	output->m_center.m_x = output->m_center.m_x * 0.5f;
	output->m_center.m_y = output->m_center.m_y * 0.5f;
	output->m_center.m_z = output->m_center.m_z * 0.5f;
	output->m_center.m_x = vertices[1].m_x + output->m_center.m_x;
	output->m_center.m_y = vertices[1].m_y + output->m_center.m_y;
	output->m_center.m_z = vertices[1].m_z + output->m_center.m_z;
	output->m_unk0x70 = *(LegoFloat*) ((LegoU8*) source + 0x70);
	output->m_usable = 1;

	const LegoFloat initialBound = 2147483648.0f;
	LegoFloat minX = initialBound;
	LegoFloat minY = initialBound;
	LegoFloat minZ = initialBound;
	LegoFloat maxX = -initialBound;
	LegoFloat maxY = -initialBound;
	LegoFloat maxZ = -initialBound;
	for (LegoU32 boundIndex = 0; boundIndex < 8; boundIndex++) {
		if (vertices[boundIndex].m_x < minX) {
			minX = vertices[boundIndex].m_x;
		}
		if (vertices[boundIndex].m_y < minY) {
			minY = vertices[boundIndex].m_y;
		}
		if (vertices[boundIndex].m_z < minZ) {
			minZ = vertices[boundIndex].m_z;
		}
		if (maxX < vertices[boundIndex].m_x) {
			maxX = vertices[boundIndex].m_x;
		}
		if (maxY < vertices[boundIndex].m_y) {
			maxY = vertices[boundIndex].m_y;
		}
		if (maxZ < vertices[boundIndex].m_z) {
			maxZ = vertices[boundIndex].m_z;
		}
	}

	LegoFloat xRange = maxX - minX;
	LegoFloat zRange = maxZ - minZ;
	if (zRange <= xRange) {
		if (xRange > 0.0f && zRange / xRange > 0.5f) {
			output->m_usable = 0;
		}
	}
	else if (zRange > 0.0f && xRange / zRange > 0.5f) {
		output->m_usable = 0;
	}
}

// STUB: LEGORACERS2 0x004534e0
GameObject* __fastcall GameObjectBaseDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->~GameObject();
	if (p_flags & 1) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004a2e20
GameObject::GameObject(LegoU32 p_type, GameObjectTemplate* p_template, void* p_lifecycle)
	: m_vtable(&g_gameObjectBaseDispatch), m_type(p_type), m_flags(0), m_unk0x0c(0), m_first(), m_second(),
	  m_parent(NULL), m_template(NULL), m_unk0x48(NULL), m_unk0x4c(0xff), m_unk0x4d(0), m_unk0x4e(0),
	  m_unk0x50(2147483648.0f), m_lifecycle0x54((GameLifecycleComponent*) p_lifecycle)
{
	if (p_template != NULL) {
		m_template = p_template;
		p_template->NormalizeDevelopmentFlags();
		m_flags = p_template->m_flags;
		m_first = p_template->m_first;
		m_second = p_template->m_second;
	}

	if ((m_flags & 1) == 0) {
		m_flags = (m_flags & ~1) ^ 1;
	}

	GetGameManager()->RegisterObject(this);
}

// STUB: LEGORACERS2 0x004a2ed0
GameObject::~GameObject()
{
	m_vtable = &g_gameObjectBaseDispatch;
	if (GetMemoryHeap()->m_unk0x0c == 0) {
		AudioService* service = GetAudioService(FALSE);
		if (service != NULL) {
			service->m_vtable->m_removeSource(service, NULL, (LegoU32) this);
		}
	}

	GetGameManager()->UnregisterObject(this);
}

LegoBool32 __fastcall IsGameObjectFlagSelected(const LegoU32* p_mask, void*, LegoU32 p_bitIndex);

// STUB: LEGORACERS2 0x004a2f10
void GameObject::Initialize()
{
	g_gameLifecycleHooksNeedRefresh = 1;
	LegoBool32 shouldCreateProxy = (m_flags & 0x800000) == 0;
	if (shouldCreateProxy != FALSE) {
		m_unk0x0c = m_flags;
	}
	else {
		m_flags = m_unk0x0c;
	}

	GameLifecycleComponent* lifecycle = m_lifecycle0x54;
	if (lifecycle != NULL) {
		lifecycle->m_vtable->m_prepare(lifecycle, NULL);
	}

	GameObjectChildBuffer* child = (GameObjectChildBuffer*) m_unk0x48;
	if (child != NULL && lifecycle != NULL && child->m_mesh != NULL) {
		CollisionModelView* model = *(CollisionModelView**) ((LegoU8*) child->m_mesh + 0x40);
		if (shouldCreateProxy != FALSE) {
			lifecycle->m_collisionProxy = CreateCollisionProxy(model, (LegoU32) lifecycle);
		}
		if (model != NULL) {
			AttachCollisionProxy(lifecycle->m_collisionProxy);
			RefreshCollisionState();
		}
	}

	m_parent = NULL;
	if (m_second.m_data[0] != '\0') {
		GameObject* parent = FindGameObjectByName(m_second.m_data, 1, NULL);
		SetParent(parent, 0);
	}

	LegoU32 flags = m_flags | 0x800000;
	m_flags = flags;
	if ((flags & 1) != 0) {
		m_flags = flags & 0xfffffffe;
	}
}

// STUB: LEGORACERS2 0x004a2fe0
void GameObject::Remove()
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) m_unk0x48;
	GameLifecycleComponent* lifecycle = m_lifecycle0x54;
	if (child != NULL && lifecycle != NULL && child->m_mesh != NULL && lifecycle->m_collisionProxy != NULL) {
		CollisionProxy* proxy = lifecycle->m_collisionProxy;
		DestroyCollisionProxy(&proxy);
	}
}

// STUB: LEGORACERS2 0x004a3010
void GameObject::ApplyChildTransform(const LegoFloat* p_position, const LegoFloat* p_rotation)
{
	GameObjectChildGeometry* child = (GameObjectChildGeometry*) m_unk0x48;
	child->SetPosition(p_position);
	child->SetRotation(p_rotation);
}

// STUB: LEGORACERS2 0x004a3040
void GameObject::SetParent(GameObject* p_parent, LegoU32 p_flag)
{
	GameLifecycleComponent* lifecycle = m_lifecycle0x54;
	if (lifecycle != NULL && lifecycle->m_vtable->m_isActive(lifecycle, NULL, 0) != FALSE) {
		if (p_parent->m_lifecycle0x54 != NULL) {
			SetLifecycleParent(lifecycle, NULL, p_parent->m_lifecycle0x54, p_flag);
		}
	}
	m_parent = p_parent;
	if (p_flag == 0) {
		m_vtable->m_action0x08(this, NULL);
	}
}

// STUB: LEGORACERS2 0x004a3090
void GameObject::ClearParent(LegoU32 p_flag)
{
	GameLifecycleComponent* lifecycle = m_lifecycle0x54;
	if (lifecycle != NULL && lifecycle->m_vtable->m_isActive(lifecycle, NULL, 0) != FALSE) {
		GameLifecycleComponent* parentLifecycle = m_parent->m_lifecycle0x54;
		if (parentLifecycle != NULL) {
			ClearLifecycleParent(lifecycle, NULL, p_flag);
		}
	}
	m_parent = NULL;
	if (p_flag == 0) {
		m_vtable->m_action0x0c(this, NULL);
	}
}

// STUB: LEGORACERS2 0x004a30e0
void GameObject::UpdateRegion(void* p_region)
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) m_unk0x48;
	if (child != NULL && child->m_mesh != NULL) {
		GameObjectRegionKey* key = (GameObjectRegionKey*) &m_unk0x4c;
		m_unk0x50 = child->m_mesh->m_radius;
		TestGameObjectRegion((GameObjectRegion*) p_region, NULL, &child->m_position, m_unk0x50, key);
	}
}

// STUB: LEGORACERS2 0x004a3150
void GameObjectTemplate::NormalizeDevelopmentFlags()
{
	if (GetApplication()->IsDevelopmentMode() == FALSE) {
		m_flags &= ~0x10;
	}
}

// STUB: LEGORACERS2 0x004a3780
GameObject* __fastcall FindRegisteredGameObjectByName(
	GameManager*,
	void*,
	const LegoChar* p_name,
	LegoU32,
	const LegoU32* p_typeMask
)
{
	for (LegoS32 objectIndex = 0; objectIndex < g_gameObjectCount; objectIndex++) {
		GameObject* object = g_gameObjects[objectIndex];
		LegoBool32 nameMatches = TRUE;
		for (LegoU32 characterIndex = 0; characterIndex < sizeof(object->m_first.m_data); characterIndex++) {
			LegoChar character = object->m_first.m_data[characterIndex];
			if (p_name[characterIndex] != character) {
				nameMatches = FALSE;
				break;
			}
			if (character == '\0') {
				break;
			}
		}

		if (nameMatches != FALSE) {
			LegoU32 bitIndex = object->m_flags >> 5;
			if (p_typeMask == NULL || IsGameObjectFlagSelected(p_typeMask, NULL, bitIndex) != FALSE) {
				return object;
			}
		}
	}

	return NULL;
}

// STUB: LEGORACERS2 0x004a37f0
GameObject* __cdecl FindGameObjectByName(const LegoChar* p_name, LegoU32 p_unused, const LegoU32* p_typeMask)
{
	return FindRegisteredGameObjectByName(GetGameManager(), NULL, p_name, p_unused, p_typeMask);
}

// STUB: LEGORACERS2 0x004a3a20
LegoBool32 __fastcall IsGameObjectFlagSelected(const LegoU32* p_mask, void*, LegoU32 p_bitIndex)
{
	LegoU32 normalizedIndex = p_bitIndex & 0xff;
	return (p_mask[normalizedIndex >> 5] & ((LegoU32) 1 << (normalizedIndex & 31))) != 0;
}

// STUB: LEGORACERS2 0x004a6bb0
void __fastcall SetLifecycleParent(
	GameLifecycleComponent* p_lifecycle,
	void*,
	GameLifecycleComponent* p_parent,
	LegoU32 p_flag
)
{
	PhysicalLifecycleState* lifecycle = (PhysicalLifecycleState*) p_lifecycle;
	if (p_flag != 0) {
		lifecycle->m_parent = (PhysicalLifecycleState*) p_parent;
		return;
	}

	const LifecycleTransformDispatch* lifecycleDispatch = (const LifecycleTransformDispatch*) p_lifecycle->m_vtable;
	const Vector3* position = lifecycleDispatch->m_position((PhysicalLifecycleState*) p_lifecycle, NULL);
	lifecycle->m_parentOffset = *position;

	const Quaternion* orientation = lifecycleDispatch->m_orientation((PhysicalLifecycleState*) p_lifecycle, NULL);
	lifecycle->m_parentOrientation = *orientation;

	const LifecycleTransformDispatch* parentDispatch = (const LifecycleTransformDispatch*) p_parent->m_vtable;
	LegoU32 parentValue = parentDispatch->m_value0x04((PhysicalLifecycleState*) p_parent, NULL);
	lifecycle->m_savedParentValue = lifecycle->m_parentValue;
	lifecycle->m_parent = (PhysicalLifecycleState*) p_parent;
	lifecycle->m_parentValue = parentValue;
}

// STUB: LEGORACERS2 0x004a9d90
void GameObjectChildGeometry::SetPosition(const LegoFloat* p_position)
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) this;
	child->m_position.m_x = p_position[0];
	child->m_position.m_y = p_position[1];
	child->m_position.m_z = p_position[2];
	RefreshChildGeometry(child);
}

// STUB: LEGORACERS2 0x004a9ff0
void GameObjectChildGeometry::SetRotation(const LegoFloat* p_rotation)
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) this;
	child->m_rotation[0] = p_rotation[0];
	child->m_rotation[1] = p_rotation[1];
	child->m_rotation[2] = p_rotation[2];
	child->m_rotation[3] = p_rotation[3];
	RefreshChildGeometry(child);
}

// STUB: LEGORACERS2 0x004aa540
LegoBool32 GameObject::IsVisible()
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) m_unk0x48;
	void* camera = g_unk0x00679ef0;
	LegoFloat depth = GetGameObjectViewDepth(camera, NULL, &child->m_position);
	child->m_projectedDepth = depth;
	if (!(m_unk0x50 + depth > 0.1f)) {
		return FALSE;
	}

	LegoFloat depthFromCamera = depth - m_unk0x50;
	if (!(depthFromCamera >= g_renderProjectionRatio * child->m_visibilityRadius)) {
		return FALSE;
	}

	if (IsPointWithinGameObjectView((LegoU8*) camera + 0x270, NULL, &child->m_position, m_unk0x50) == FALSE) {
		return FALSE;
	}

	GameObjectShadowState* shadowState = (GameObjectShadowState*) g_unk0x0067bafc;
	if (shadowState != NULL) {
		m_vtable->m_action0x4c(this, NULL, (LegoU32) shadowState);
		if ((LegoS8) m_unk0x4c >= 0) {
			LegoU8* map = shadowState->m_objectMaps[(LegoS8) m_unk0x4c].m_bytes;
			LegoU8 state = map[(LegoS16) m_unk0x4e];
			state >>= m_unk0x4d;
			if ((state & 3) == 2) {
				return FALSE;
			}
		}
	}

	GameObjectTransformedBuffer* transformed = (GameObjectTransformedBuffer*) child->m_transformedVertices;
	if (transformed != NULL) {
		if (IsPointWithinGameObjectView(
				(LegoU8*) camera + 0x270,
				NULL,
				&transformed->m_center,
				transformed->m_unk0x70
			) == FALSE) {
			return FALSE;
		}

		if (transformed->m_usable == 0) {
			LegoU32 commonPlanes = 0xffffffff;
			const LegoFloat* point = (const LegoFloat*) transformed;
			for (LegoU32 vertexIndex = 0; vertexIndex < 8; vertexIndex++) {
				LegoU32 behindPlanes = 0;
				LegoS32 planeCount = *(LegoS32*) ((LegoU8*) camera + 0x304);
				for (LegoS32 planeIndex = 0; planeIndex < planeCount; planeIndex++) {
					const LegoFloat* normal = (const LegoFloat*) ((LegoU8*) camera + 0x270 + planeIndex * 0x0c);
					const LegoFloat* planePoint = (const LegoFloat*) ((LegoU8*) camera + 0x2b8 + planeIndex * 0x0c);
					LegoFloat side = (point[2] - planePoint[2]) * normal[2];
					side += (point[1] - planePoint[1]) * normal[1];
					side += (point[0] - planePoint[0]) * normal[0];
					if ((*(LegoU32*) &side & 0x80000000) != 0) {
						behindPlanes |= (LegoU32) 1 << (planeIndex & 0x1f);
					}
				}
				commonPlanes &= behindPlanes;
				point += 3;
			}
			if (commonPlanes == 0) {
				return FALSE;
			}
		}
	}

	LegoFloat distanceThreshold = g_renderProjectionRatio * child->m_visibilityDistance;
	if (distanceThreshold >= depthFromCamera) {
		child->m_visibility = 0.0f;
	}
	else {
		child->m_visibility = (depthFromCamera - child->m_visibilityDistance) * child->m_visibilityScale;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004aa730
static LegoBool32 __fastcall IsPointWithinGameObjectView(
	void* p_volume,
	void*,
	const Vector3* p_point,
	LegoFloat p_radius
)
{
	LegoU8* volume = (LegoU8*) p_volume;
	const LegoU8* end = *(const LegoU8**) (volume + 0x90);
	const LegoFloat negativeRadius = -p_radius;
	for (const LegoU8* pointPlane = volume + 0x14; pointPlane - 0x44 < end; pointPlane += 0x0c) {
		const LegoFloat* normal = (const LegoFloat*) (pointPlane - 0x48);
		const LegoFloat* planePoint = (const LegoFloat*) pointPlane;
		LegoFloat side = (p_point->m_z - planePoint[2]) * normal[2];
		side += (p_point->m_y - planePoint[1]) * normal[1];
		side += (p_point->m_x - planePoint[0]) * normal[0];
		if (!(side > negativeRadius)) {
			return FALSE;
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004aa790
void GameObject::DrawChild()
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) m_unk0x48;
	if (child != NULL && IsVisible() != FALSE) {
		const GameObjectChildDispatchView* dispatch = *(const GameObjectChildDispatchView**) child;
		dispatch->m_draw(child, NULL);
	}
}

// FUNCTION: LEGORACERS2 0x004ba270
void __fastcall GameObjectBaseRemove(GameObject* p_object, void*)
{
	p_object->Remove();
}

// STUB: LEGORACERS2 0x004ba370
LegoFloat __fastcall GetGameObjectViewDepth(void* p_camera, void*, const Vector3* p_point)
{
	GameObjectVisibilityContext* context = (GameObjectVisibilityContext*) ((LegoU8*) p_camera + 0x58);
	const Vector3* origin = context->m_vtable->m_getPosition(context, NULL);
	const LegoFloat* direction = (const LegoFloat*) ((LegoU8*) p_camera + 0x308);
	LegoFloat depth = (p_point->m_z - origin->m_z) * direction[2];
	depth += (p_point->m_y - origin->m_y) * direction[1];
	depth += (p_point->m_x - origin->m_x) * direction[0];
	return depth;
}

// STUB: LEGORACERS2 0x004bcf30
LegoBool32 __fastcall UpdateFoyerModelRenderRegion(GameObject* p_object, void*, void* p_region)
{
	GameObjectChildBuffer* child = (GameObjectChildBuffer*) p_object->m_unk0x48;
	return TestGameObjectRegion(
		(GameObjectRegion*) p_region,
		NULL,
		&child->m_position,
		p_object->m_unk0x50,
		(GameObjectRegionKey*) &p_object->m_unk0x4c
	);
}

// STUB: LEGORACERS2 0x004d40a0
static LegoBool32 __fastcall TestGameObjectRegion(
	GameObjectRegion* p_region,
	void*,
	const Vector3* p_position,
	LegoFloat p_radius,
	GameObjectRegionKey* p_key
)
{
	p_key->m_level = 0xfe;
	p_key->m_cell = 0;
	p_key->m_index = 0;
	if (p_region->m_mode != 1) {
		return FALSE;
	}

	Vector3 position = *p_position;
	if (p_region->m_transformPosition == 1) {
		position.m_x -= p_region->m_transformOrigin.m_x;
		position.m_y -= p_region->m_transformOrigin.m_y;
		position.m_z -= p_region->m_transformOrigin.m_z;
		position.FUN_004a4e00(&p_region->m_transform);
	}
	position.m_x -= p_region->m_gridOrigin.m_x;
	position.m_y -= p_region->m_gridOrigin.m_y;
	position.m_z -= p_region->m_gridOrigin.m_z;

	LegoS32 cellX = (LegoS32) (position.m_x * p_region->m_inverseCellWidth - 0.5f);
	LegoS32 cellZ = (LegoS32) (position.m_z * p_region->m_inverseCellDepth - 0.5f);
	if (cellX < 0 || cellZ < 0 || cellX >= p_region->m_gridWidth || cellZ >= p_region->m_gridDepth) {
		return FALSE;
	}

	position.m_x -= p_region->m_gridCenter.m_x;
	position.m_z -= p_region->m_gridCenter.m_z;
	LegoFloat xMinimum = position.m_x - p_radius;
	LegoFloat xMaximum = position.m_x + p_radius;
	LegoFloat zMinimum = position.m_z - p_radius;
	LegoFloat zMaximum = position.m_z + p_radius;
	LegoFloat yMaximum = position.m_y + p_radius;
	if (p_region->m_levelCount <= 0) {
		return FALSE;
	}

	LegoU32 indexShift = p_region->m_gridShift;
	LegoS32 levelX = cellX;
	LegoS32 levelZ = cellZ;
	for (LegoS32 level = 0; level < p_region->m_levelCount; level++) {
		LegoS32 recordIndex = (levelZ << (indexShift & 31)) + levelX;
		GameObjectRegionRecord* record =
			(GameObjectRegionRecord*) ((LegoU8*) p_region->m_records + recordIndex * sizeof(GameObjectRegionRecord));
		LegoFloat left = record->m_center[0] - record->m_halfExtent[0];
		LegoFloat right = record->m_center[0] + record->m_halfExtent[0];
		LegoFloat back = record->m_center[2] - record->m_halfExtent[2];
		LegoFloat front = record->m_center[2] + record->m_halfExtent[2];
		LegoFloat bottom = record->m_center[1] - record->m_halfExtent[1];
		LegoFloat top = record->m_center[1] + record->m_halfExtent[1] + p_region->m_verticalPadding;
		if (xMinimum >= left && xMaximum <= right && zMinimum >= back && zMaximum <= front && yMaximum >= bottom &&
			yMaximum <= top) {
			LegoS32 linearIndex = (levelZ << (p_region->m_levelEntries[level * 8 + 5] & 31)) + levelX;
			p_key->m_level = (LegoU8) level;
			p_key->m_cell = (LegoU8) (((LegoU8) linearIndex & 3) << 1);
			p_key->m_index = (LegoS16) (linearIndex >> 2);
			return TRUE;
		}

		indexShift--;
		levelX >>= 1;
		levelZ >>= 1;
	}
	return FALSE;
}

void __fastcall GameObjectBaseApplyChildTransform(GameObject* p_object, void*, LegoU32 p_position, LegoU32 p_rotation)
{
	p_object->ApplyChildTransform((const LegoFloat*) p_position, (const LegoFloat*) p_rotation);
}

void __fastcall GameObjectBaseNoOp(GameObject*, void*)
{
}

LegoU32 __fastcall GameObjectBaseTrue(GameObject*, void*)
{
	return TRUE;
}

LegoU32 __fastcall GameObjectBaseZero(GameObject*, void*)
{
	return 0;
}

LegoU32 __fastcall GameObjectBaseZeroOneArgument(GameObject*, void*, LegoU32)
{
	return 0;
}

void __fastcall GameObjectBaseNoOpOneArgument(GameObject*, void*, LegoU32)
{
}

void __fastcall GameObjectBaseNoOpTwoArguments(GameObject*, void*, LegoU32, LegoU32)
{
}

LegoU32 __fastcall GameObjectBaseZeroTwoArguments(GameObject*, void*, LegoU32, LegoU32)
{
	return 0;
}

void __fastcall GameObjectBaseDrawChild(GameObject* p_object, void*)
{
	p_object->DrawChild();
}

void __fastcall GameObjectBaseUpdateRegion(GameObject* p_object, void*, LegoU32 p_region)
{
	p_object->UpdateRegion((void*) p_region);
}

void __fastcall GameObjectBaseInitialize(GameObject* p_object, void*)
{
	p_object->Initialize();
}
