#include "game/objects/gameobjectmodelbinding.h"

#include "core/application/application.h"
#include "game/objects/gameobjectgeometry.h"
#include "math/clipplanes.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/models/modelbounds.h"
#include "render/models/modelresource.h"
#include "render/models/modelresourcecache.h"
#include "render/models/modelresourceloading.h"
#include "render/models/modelshadow.h"

#include <string.h>

extern void __fastcall UpdateModelResourceVisibility(ModelResource*, void*, GameObjectModelBinding*);
extern void __fastcall DrawModelResource(ModelResource*, void*, GameObjectModelBinding*);
extern const LegoFloat* __fastcall GetLookupResourceOrigin(LegoU8*, void*);

// GLOBAL: LEGORACERS2 0x00535470
LegoFloat g_modelBindingDepthThreshold = 0.0266666673f;

const GameObjectModelBindingDispatch g_gameObjectModelBindingDispatch = {
	&DestroyGameObjectModelBinding,
	&UpdateGameObjectModelBinding,
};

// FUNCTION: LEGORACERS2 0x0043b8a0
GameObjectModelBinding* __fastcall DestroyGameObjectModelBinding(
	GameObjectModelBinding* p_binding,
	void*,
	LegoU32 p_flags
)
{
	ResetGameObjectModelBindingDispatch(p_binding, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_binding);
	}
	return p_binding;
}

// STUB: LEGORACERS2 0x004a9870
void __fastcall ResetGameObjectModelBindingDispatch(GameObjectModelBinding* p_binding, void*)
{
	p_binding->m_vtable = &g_gameObjectModelBindingDispatch;
}

// STUB: LEGORACERS2 0x004a9880
void __fastcall InitializeGameObjectModelBinding(GameObjectModelBinding* p_binding, void*)
{
	if (p_binding->m_model != NULL || p_binding->m_path[0] == '\0') {
		return;
	}
	ModelResource* model = LoadModelResource(p_binding->m_path, 1, p_binding->m_flags & 1, 0);
	p_binding->m_model = model;
	if (model->m_unk0x28 != NULL) {
		p_binding->m_flags |= 6;
		ConfigureModelShadow(model, NULL, 1, NULL, 1);
	}
	if (model->m_unk0x24 == 0 || GetApplication()->IsDevelopmentMode()) {
		SetClipPlanesForFoyerTarget(p_binding, NULL, 0);
	}
	if (model->m_bounds != NULL) {
		ModelBounds* bounds = (ModelBounds*) AllocateHeapMemory(sizeof(ModelBounds));
		if (bounds != NULL) {
			bounds->m_radius = 0.0f;
			bounds->m_isElongatedXZ = 1;
		}
		p_binding->m_bounds = bounds;
		((GameObjectChildGeometry*) p_binding)->SetPosition(p_binding->m_position);
	}
}

// STUB: LEGORACERS2 0x004a9b40
void __fastcall ReleaseGameObjectModelBindingResources(GameObjectModelBinding* p_binding, void*)
{
	if (p_binding->m_bounds != NULL) {
		FreeHeapMemory(p_binding->m_bounds);
		p_binding->m_bounds = NULL;
	}
	if ((p_binding->m_flags & 4) != 0) {
		p_binding->m_flags &= ~6u;
		ConfigureModelShadow((ModelResource*) p_binding->m_model, NULL, 0, NULL, 1);
	}
	if (p_binding->m_model != NULL) {
		ModelResource* model = (ModelResource*) p_binding->m_model;
		if ((p_binding->m_flags & 1) == 0) {
			g_modelResourceCache.Release(model);
		}
		else {
			model->~ModelResource();
			FreeHeapMemory(model);
		}
		p_binding->m_model = NULL;
	}
}

// STUB: LEGORACERS2 0x004a9bd0
void __fastcall UpdateGameObjectModelBinding(GameObjectModelBinding* p_binding, void*)
{
	ModelResource* model = (ModelResource*) p_binding->m_model;
	if (model == NULL) {
		return;
	}

	RendererMainDispatch* renderer = (RendererMainDispatch*) g_displayRenderer->m_vtable;
	p_binding->m_unk0xBC = renderer->m_scaleProjectionCoordinate(
		g_displayRenderer,
		NULL,
		*(LegoFloat*) &model->m_unk0x08,
		p_binding->m_unk0xB8
	);
	p_binding->m_unk0xD4 = 0xffffffff;

	const Vector3* center = (const Vector3*) p_binding->m_position;
	LegoFloat radius = *(LegoFloat*) &model->m_unk0x08;
	if (p_binding->m_bounds != NULL) {
		center = (const Vector3*) GetLookupResourceOrigin((LegoU8*) p_binding, NULL);
		radius = ((ModelBounds*) p_binding->m_bounds)->m_radius;
	}
	renderer
		->m_selectLights(g_displayRenderer, NULL, center, radius, model->m_hasSpecialPrimitives, &p_binding->m_unk0xD4);

	if ((p_binding->m_flags & 2) != 0 && g_modelBindingDepthThreshold < p_binding->m_unk0xBC) {
		UpdateModelResourceVisibility(model, NULL, p_binding);
	}
	DrawModelResource(model, NULL, p_binding);
}

// STUB: LEGORACERS2 0x004a9cb0
void __fastcall UpdateGameObjectModelBindingVisibility(GameObjectModelBinding* p_binding, void*)
{
	ModelResource* model = (ModelResource*) p_binding->m_model;
	if (model != NULL) {
		RendererMainDispatch* renderer = (RendererMainDispatch*) g_displayRenderer->m_vtable;
		LegoFloat depth = renderer->m_scaleProjectionCoordinate(
			g_displayRenderer,
			NULL,
			*(LegoFloat*) &model->m_unk0x08,
			p_binding->m_unk0xB8
		);
		p_binding->m_unk0xBC = depth;
		if ((p_binding->m_flags & 2) == 2 && depth > g_modelBindingDepthThreshold) {
			UpdateModelResourceVisibility(model, NULL, p_binding);
		}
	}
}

// STUB: LEGORACERS2 0x004aa410
void __fastcall ConstructGameObjectModelBinding(
	GameObject* p_owner,
	void*,
	const LegoU8* p_path,
	const LegoFloat* p_position,
	const LegoFloat* p_rotation,
	LegoU32 p_flags
)
{
	GameObjectModelBinding* binding = (GameObjectModelBinding*) AllocateHeapMemory(0xe4);
	if (binding == NULL) {
		p_owner->m_unk0x48 = NULL;
		return;
	}

	LegoU32 previousFlags;
	memcpy(&previousFlags, &binding->m_flags, sizeof(previousFlags));
	binding->m_owner = p_owner;
	binding->m_model = NULL;
	memcpy(binding->m_path, p_path, sizeof(binding->m_path));
	for (LegoS32 rotationIndex = 0; rotationIndex < 4; ++rotationIndex) {
		binding->m_rotation[rotationIndex] = p_rotation[rotationIndex];
	}
	for (LegoS32 positionIndex = 0; positionIndex < 3; ++positionIndex) {
		binding->m_position[positionIndex] = p_position[positionIndex];
	}
	binding->m_boundMinimumX = -1;
	binding->m_boundMinimumY = -1;
	binding->m_boundMinimumZ = 0xffff;
	binding->m_layer = 0x0f;
	binding->m_alpha = 3;
	binding->m_unk0xC0 = 0;
	binding->m_flags = (previousFlags & 0xffffffc8) ^ (p_flags & 1) | 8;
	binding->m_clipScale = 1.0f / (g_modelClipFar - g_modelClipNear);
	binding->m_clipNear = g_modelClipNear;
	binding->m_clipFar = g_modelClipFar;
	binding->m_bounds = NULL;
	binding->m_unk0xD4 = 0xffffffff;
	binding->m_unk0xD8 = 0;
	binding->m_unk0xDC = 0;
	binding->m_unk0xE0 = 0;
	binding->m_vtable = &g_gameObjectModelBindingDispatch;
	p_owner->m_unk0x48 = binding;
}

// STUB: LEGORACERS2 0x004aa530
void __fastcall DestroyGameObjectModelBindingForOwner(GameObject* p_owner, void*)
{
	GameObjectModelBinding* binding = (GameObjectModelBinding*) p_owner->m_unk0x48;
	if (binding != NULL) {
		binding->m_vtable->m_destroy(binding, NULL, 1);
	}
}
