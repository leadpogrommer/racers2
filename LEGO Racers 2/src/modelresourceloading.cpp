#include "modelresourceloading.h"

#include "application.h"
#include "diagnostics.h"
#include "memoryheap.h"
#include "modelbounds.h"
#include "modelresourcecache.h"
#include "modelresourcechunks.h"
#include "modelresourceparser.h"
#include "modelskin.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererstageplan.h"
#include "winfile.h"

#include <new.h>

namespace
{
void InitializeLoadedModel(ModelResource* p_model)
{
	p_model->m_signature = 0x324c444d;
	p_model->m_hasSpecialPrimitives = 0;
	p_model->m_unk0x08 = 0;
	p_model->m_groups = NULL;
	p_model->m_specialGroup = NULL;
	p_model->m_bounds = NULL;
	p_model->m_skin = NULL;
	p_model->m_unk0x1c = NULL;
	p_model->m_groupCount = 0;
	p_model->m_unk0x24 = 1;
	p_model->m_unk0x28 = NULL;
	p_model->m_resources.Reset();
	p_model->m_collisionData = NULL;
	p_model->m_collisionDataSize = 0;
}

void CenterSkinnedModelBounds(ModelBounds* p_bounds)
{
	Vector3 minimum;
	Vector3 maximum;
	minimum.m_x = minimum.m_y = minimum.m_z = 2147483648.0f;
	maximum.m_x = maximum.m_y = maximum.m_z = -2147483648.0f;
	LegoS32 index;
	for (index = 0; index < 8; ++index) {
		const Vector3* point = &p_bounds->m_corners[index];
		if (point->m_x < minimum.m_x) {
			minimum.m_x = point->m_x;
		}
		if (point->m_y < minimum.m_y) {
			minimum.m_y = point->m_y;
		}
		if (point->m_z < minimum.m_z) {
			minimum.m_z = point->m_z;
		}
		if (maximum.m_x < point->m_x) {
			maximum.m_x = point->m_x;
		}
		if (maximum.m_y < point->m_y) {
			maximum.m_y = point->m_y;
		}
		if (maximum.m_z < point->m_z) {
			maximum.m_z = point->m_z;
		}
	}
	Vector3 center;
	center.m_x = (maximum.m_x - minimum.m_x) * 0.5f + minimum.m_x;
	center.m_y = (maximum.m_y - minimum.m_y) * 0.5f + minimum.m_y;
	center.m_z = minimum.m_z + (maximum.m_z - minimum.m_z) * 0.5f;
	for (index = 0; index < 8; ++index) {
		p_bounds->m_corners[index].m_x -= center.m_x;
		p_bounds->m_corners[index].m_y -= center.m_y;
		p_bounds->m_corners[index].m_z -= center.m_z;
	}
}
} // namespace

// STUB: LEGORACERS2 0x004a7d00
void ModelResource::SetCollisionData(void* p_data, LegoU32 p_bytes)
{
	if (m_collisionData != NULL) {
		ReleaseCollisionResource(&m_collisionData);
	}
	m_collisionDataSize = p_bytes;
	m_collisionData = p_data;
}

// STUB: LEGORACERS2 0x004a7d40
ModelResource* __cdecl LoadModelResource(
	const LegoChar* p_path,
	LegoS32 p_loadCollision,
	LegoS32 p_uncached,
	LegoU32 p_flags
)
{
	if (p_uncached == 0) {
		ModelResource* cached = g_modelResourceCache.Find(p_path);
		if (cached != NULL) {
			g_modelResourceCache.AddReference(cached);
			return cached;
		}
	}
	ModelResource* model = (ModelResource*) AllocateHeapMemory(sizeof(ModelResource));
	if (model != NULL) {
		InitializeLoadedModel(model);
	}
	BOOL opened;
	WinFile file(p_path, 0, &opened);
	if (opened == FALSE) {
		// Retail leaves the allocation unreleased on an open failure.
		return NULL;
	}
	g_displayRenderer->SetResourceLoadMode(1);
	ReadModelResourceGeometry(model, NULL, p_path, &file, p_uncached, p_flags);
	LegoBool32 regenerateCollision = FALSE;
	LegoU32 tag;
	LegoU32 bytes;
	while (file.Read(&tag, 4, 1) != FALSE && tag != 0 && file.Read(&bytes, 4, 1) != FALSE) {
		if (tag == 0x30414853) {
			ReadModelShadowChunk(model, NULL, &file, tag, bytes);
		}
		else if (tag == 0x304e4b53) {
			void* storage = AllocateHeapMemory(sizeof(ModelSkin));
			model->m_skin = storage == NULL ? NULL : new (storage) ModelSkin(model, &file);
			if (model->m_bounds != NULL) {
				CenterSkinnedModelBounds(model->m_bounds);
			}
		}
		else if (tag == 0x444c4f43 && p_loadCollision == 1) {
			if (ReadModelCollisionChunk(model, NULL, &file, tag, bytes) == 0) {
				regenerateCollision = TRUE;
			}
		}
		else {
			file.SeekManagedCurrent(bytes);
		}
	}
	file.CloseManaged();
	g_displayRenderer->SetResourceLoadMode(0);
	FinalizeModelDrawPlans(model, NULL);
	if (regenerateCollision != FALSE) {
		PrintDiagnostic("Regenerate Collision for Model %s.", p_path);
		Application* application = GetApplication();
		application->m_vtable->m_action0x8c(application, NULL, (LegoU32) p_path, (LegoU32) model);
	}
	if (p_uncached == 0) {
		g_modelResourceCache.Insert(model, p_path);
	}
	return model;
}

// STUB: LEGORACERS2 0x004a80c0
LegoU8 __fastcall ReadModelCollisionChunk(
	ModelResource* p_model,
	void*,
	WinFile* p_file,
	LegoU32 p_tag,
	LegoU32 p_bytes
)
{
	p_model->SetCollisionData(NULL, 0);
	if (p_tag != 0x444c4f43) {
		return 0;
	}
	p_model->m_collisionDataSize = p_bytes;
	if (p_bytes == 0) {
		p_model->m_collisionData = NULL;
		return 2;
	}
	p_model->m_collisionData = GetMemoryHeap()->Allocate(p_model->m_collisionDataSize, "New Collision Data");
	p_file->Read(p_model->m_collisionData, p_model->m_collisionDataSize, 1);
	return LoadCollisionResource(p_model->m_collisionData) != NULL;
}

// STUB: LEGORACERS2 0x004a9020
void __fastcall FinalizeModelDrawPlans(ModelResource* p_model, void*)
{
	for (LegoS32 group = 0; group < p_model->m_groupCount; ++group) {
		ModelTextureGroup* record = &p_model->m_groups[group];
		ModelPrimitiveTextureRecord* primitive = record->m_primitives;
		LegoS32 remaining = record->m_primitiveCount;
		while (remaining > 0) {
			((const RendererMainDispatch*) g_displayRenderer->m_vtable)
				->m_prepareDrawPlan(g_displayRenderer, NULL, (RendererStagePlanRequest*) primitive);
			++primitive;
			--remaining;
		}
	}
}
