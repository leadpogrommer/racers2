#include "foyermesh.h"

#include "foyertemplateinstance.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"
#include "modelresource.h"
#include "rendererbufferdispatch.h"

#include <stdlib.h>

struct FoyerMeshInfluence {
	Vector3 m_position;
	LegoS16 m_weight;
	LegoS16 m_bone;
};
DECOMP_SIZE_ASSERT(FoyerMeshInfluence, 0x10);
struct FoyerMeshVertex {
	FoyerMeshInfluence m_influences[2];
	Vector3 m_position;
	LegoS16 m_influenceCount;
	LegoS16 m_unk0x2e;
};
DECOMP_SIZE_ASSERT(FoyerMeshVertex, 0x30);
DECOMP_STATIC_ASSERT(offsetof(FoyerMeshVertex, m_position) == 0x20);
struct FoyerMeshPrimitive {
	LegoS32 m_vertexCount;
	const LegoS16* m_indices;
	LegoU32 m_unk0x08;
};
DECOMP_SIZE_ASSERT(FoyerMeshPrimitive, 0x0c);
struct FoyerMeshAttachment {
	FoyerTemplateInstance* m_owner;
	ModelResource* m_model;
	LegoU32 m_unk0x08;
	LegoS32 m_vertexCount;
	LegoS32 m_primitiveCount;
	LegoU32 m_dirty;
	FoyerMeshVertex* m_vertices;
	FoyerMeshPrimitive* m_primitives;
};
DECOMP_SIZE_ASSERT(FoyerMeshAttachment, 0x20);

struct FoyerMeshUpdateService;
struct FoyerMeshUpdateServiceDispatch {
	FoyerMeshUpdateService*(__fastcall* m_destroy)(FoyerMeshUpdateService*, void*, LegoU32);
	LegoBool32(__fastcall* m_update)(FoyerMeshUpdateService*, void*, FoyerMeshAttachment*);
};
DECOMP_SIZE_ASSERT(FoyerMeshUpdateServiceDispatch, 8);
struct FoyerMeshUpdateService {
	const FoyerMeshUpdateServiceDispatch* m_dispatch;
};

FoyerMeshUpdateService* GetFoyerMeshUpdateService();
FoyerMeshUpdateService* __fastcall DestroyFoyerMeshUpdateService(FoyerMeshUpdateService*, void*, LegoU32);
LegoBool32 __fastcall UpdateFoyerMeshAndUpload(FoyerMeshUpdateService*, void*, FoyerMeshAttachment*);
static void OnFoyerMeshUpdateServiceExit();

// Retail ABI function-pointer tables: 0x0053709c and 0x005370a4.
extern const FoyerMeshUpdateServiceDispatch g_foyerMeshUpdateServiceDispatch;
extern const FoyerMeshUpdateServiceDispatch g_foyerMeshUpdateServiceBaseDispatch;
const FoyerMeshUpdateServiceDispatch g_foyerMeshUpdateServiceDispatch = {
	&DestroyFoyerMeshUpdateService,
	&UpdateFoyerMeshAndUpload
};
const FoyerMeshUpdateServiceDispatch g_foyerMeshUpdateServiceBaseDispatch = {
	&DestroyFoyerMeshUpdateService,
	(LegoBool32(__fastcall*)(FoyerMeshUpdateService*, void*, FoyerMeshAttachment*)) & GameObjectBaseZeroOneArgument
};

// GLOBAL: LEGORACERS2 0x006e4e80
static LegoU8 g_foyerMeshUpdateServiceInitialized;
// GLOBAL: LEGORACERS2 0x006e4e88
static FoyerMeshUpdateService g_foyerMeshUpdateService;

static void GetFoyerMeshBoneTransforms(FoyerMeshAttachment* p_mesh, Matrix3* p_rotations, Vector3* p_positions)
{
	for (LegoS32 i = 0; i < p_mesh->m_owner->m_skeletonCount; ++i) {
		const FoyerSkeletonRecord* record = p_mesh->m_owner->m_skeletons + i;
		p_rotations[i].FUN_004a4d30(&record->m_worldOrientation.m_x);
		p_positions[i] = record->m_worldPosition;
	}
}

static Vector3 TransformFoyerMeshInfluence(
	const FoyerMeshInfluence* p_influence,
	const Matrix3* p_rotations,
	const Vector3* p_positions,
	LegoBool32 p_combined
)
{
	const LegoFloat(*m)[3] = p_rotations[p_influence->m_bone].m_values;
	const Vector3* translation = p_positions + p_influence->m_bone;
	const Vector3* position = &p_influence->m_position;
	Vector3 result;
	if (p_combined) {
		result.m_x = (position->m_z * m[0][2] + position->m_y * m[0][1]) + position->m_x * m[0][0] + translation->m_x;
		result.m_y = (position->m_z * m[1][2] + position->m_y * m[1][1]) + position->m_x * m[1][0] + translation->m_y;
		result.m_z = (position->m_z * m[2][2] + position->m_y * m[2][1]) + position->m_x * m[2][0] + translation->m_z;
	}
	else {
		result.m_x = (m[0][0] * position->m_x + m[0][2] * position->m_z) + m[0][1] * position->m_y + translation->m_x;
		result.m_y = (m[1][2] * position->m_z + m[1][1] * position->m_y) + m[1][0] * position->m_x + translation->m_y;
		result.m_z = (m[2][2] * position->m_z + m[2][1] * position->m_y) + m[2][0] * position->m_x + translation->m_z;
	}
	return result;
}

static void UploadFoyerMeshPrimitives(FoyerMeshAttachment* p_mesh, LegoBool32 p_combined)
{
	ModelPrimitiveTextureRecord* records = p_mesh->m_model->m_groups[0].m_primitives;
	for (LegoS32 i = 0; i < p_mesh->m_primitiveCount; ++i) {
		FoyerMeshPrimitive* primitive = p_mesh->m_primitives + i;
		RendererVertexBuffer* buffer = records[i].m_vertexBuffer;
		const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) buffer->m_vtable;
		dispatch->m_setBufferPolicy(buffer, NULL, 1);
		if (dispatch->m_lock(buffer, NULL, 0, primitive->m_vertexCount, 1, 0) == 1) {
			LegoU8* destination;
			if (p_combined) {
				destination = (LegoU8*) dispatch->m_getPosition(buffer, NULL, 0);
			}
			else {
				dispatch->m_setCursor(buffer, NULL, 0);
				destination = (LegoU8*) dispatch->m_getCurrentPosition(buffer, NULL);
			}
			LegoU32 stride = dispatch->m_getStride(buffer, NULL);
			for (LegoS32 j = 0; j < primitive->m_vertexCount; ++j) {
				*(Vector3*) destination = p_mesh->m_vertices[primitive->m_indices[j]].m_position;
				destination += stride;
			}
			dispatch->m_unlock(buffer, NULL, 1);
		}
		dispatch->m_setBufferPolicy(buffer, NULL, 0);
	}
}

LegoBool32 TryUpdateFoyerMeshWithRenderer(FoyerMeshAttachment* p_mesh)
{
	FoyerMeshUpdateService* service = GetFoyerMeshUpdateService();
	return service->m_dispatch->m_update(service, NULL, p_mesh);
}

// STUB: LEGORACERS2 0x004e8090
void __fastcall UpdateFoyerMeshVertices(FoyerMeshAttachment* p_mesh, void*)
{
	if (p_mesh->m_dirty != 0) {
		p_mesh->m_dirty = 0;
		Matrix3 rotations[64];
		Vector3 positions[64];
		GetFoyerMeshBoneTransforms(p_mesh, rotations, positions);
		for (LegoS32 i = 0; i < p_mesh->m_vertexCount; ++i) {
			FoyerMeshVertex* vertex = p_mesh->m_vertices + i;
			Vector3 position = TransformFoyerMeshInfluence(vertex->m_influences, rotations, positions, FALSE);
			if (vertex->m_influenceCount > 1) {
				LegoFloat scale = (LegoFloat) vertex->m_influences[0].m_weight * 0.00390625f;
				position.m_x *= scale;
				position.m_y *= scale;
				position.m_z *= scale;
				for (LegoS32 j = 1; j < vertex->m_influenceCount; ++j) {
					const FoyerMeshInfluence* influence =
						(const FoyerMeshInfluence*) ((const LegoU8*) vertex + j * 0x10);
					Vector3 next = TransformFoyerMeshInfluence(influence, rotations, positions, FALSE);
					scale = (LegoFloat) influence->m_weight * 0.00390625f;
					position.m_x = next.m_x * scale + position.m_x;
					position.m_y = next.m_y * scale + position.m_y;
					position.m_z = next.m_z * scale + position.m_z;
				}
			}
			vertex->m_position = position;
		}
	}
}

// STUB: LEGORACERS2 0x004e82b0
void __fastcall UploadFoyerMeshVertices(FoyerMeshAttachment* p_mesh, void*)
{
	UploadFoyerMeshPrimitives(p_mesh, FALSE);
}

// STUB: LEGORACERS2 0x004f5d60
FoyerMeshUpdateService* GetFoyerMeshUpdateService()
{
	if ((g_foyerMeshUpdateServiceInitialized & 1) == 0) {
		g_foyerMeshUpdateServiceInitialized |= 1;
		g_foyerMeshUpdateService.m_dispatch = &g_foyerMeshUpdateServiceDispatch;
		atexit(OnFoyerMeshUpdateServiceExit);
	}
	return &g_foyerMeshUpdateService;
}

// STUB: LEGORACERS2 0x004f5da0
FoyerMeshUpdateService* __fastcall DestroyFoyerMeshUpdateService(
	FoyerMeshUpdateService* p_service,
	void*,
	LegoU32 p_flags
)
{
	p_service->m_dispatch = &g_foyerMeshUpdateServiceBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_service);
	}
	return p_service;
}

// STUB: LEGORACERS2 0x004f5dc0
static void OnFoyerMeshUpdateServiceExit()
{
	g_foyerMeshUpdateService.m_dispatch = &g_foyerMeshUpdateServiceBaseDispatch;
}

// STUB: LEGORACERS2 0x004f5dd0
LegoBool32 __fastcall UpdateFoyerMeshAndUpload(FoyerMeshUpdateService*, void*, FoyerMeshAttachment* p_mesh)
{
	if (p_mesh->m_dirty != 0) {
		p_mesh->m_dirty = 0;
		Matrix3 rotations[64];
		Vector3 positions[64];
		GetFoyerMeshBoneTransforms(p_mesh, rotations, positions);
		for (LegoS32 i = 0; i < p_mesh->m_vertexCount; ++i) {
			FoyerMeshVertex* vertex = p_mesh->m_vertices + i;
			vertex->m_unk0x2e = 0;
			Vector3 position = TransformFoyerMeshInfluence(vertex->m_influences, rotations, positions, TRUE);
			if (vertex->m_influenceCount < 2) {
				vertex->m_position = position;
			}
			else {
				LegoFloat scale = (LegoFloat) vertex->m_influences[0].m_weight * 0.00390625f;
				vertex->m_position.m_x = position.m_x * scale;
				vertex->m_position.m_y = position.m_y * scale;
				vertex->m_position.m_z = position.m_z * scale;
				for (LegoS32 j = 1; j < (LegoU16) vertex->m_influenceCount; ++j) {
					const FoyerMeshInfluence* influence =
						(const FoyerMeshInfluence*) ((const LegoU8*) vertex + j * 0x10);
					Vector3 next = TransformFoyerMeshInfluence(influence, rotations, positions, TRUE);
					scale = (LegoFloat) influence->m_weight * 0.00390625f;
					vertex->m_position.m_x = next.m_x * scale + vertex->m_position.m_x;
					vertex->m_position.m_z = next.m_z * scale + vertex->m_position.m_z;
					vertex->m_position.m_y = next.m_y * scale + vertex->m_position.m_y;
				}
			}
		}
		UploadFoyerMeshPrimitives(p_mesh, TRUE);
	}
	return TRUE;
}
