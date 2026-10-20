#include "render/models/modelrendering.h"

#include "game/objects/gameobjectmodelbinding.h"
#include "game/objects/gameobjectrenderprep.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/models/modelresource.h"
#include "render/queue/rendererlookupstate.h"

// GLOBAL: LEGORACERS2 0x00549bd8
LegoFloat g_modelLodDepthThreshold = 0.2f;

namespace
{
void InsertModelDraw(
	LegoS32 p_table,
	LegoU32 p_key,
	GameObjectModelBinding* p_binding,
	ModelPrimitiveTextureRecord* p_primitive,
	LegoS32 p_sortKey
)
{
	RendererLookupTableState* table = g_displayRenderer->m_drawLookupTables[p_table];
	((const RendererLookupDispatch*) table->m_vtable)
		->m_insert(table, NULL, p_key, (LegoU32) p_binding, (LegoU32) p_primitive, p_sortKey);
}
} // namespace

// STUB: LEGORACERS2 0x004a7250
void __fastcall DrawModelResource(ModelResource* p_model, void*, GameObjectModelBinding* p_binding)
{
	LegoBool32 selected = IsLookupPointWithinRadius(
		(LegoU8*) g_unk0x00679ef0 + 0x270,
		NULL,
		p_binding->m_position,
		*(LegoFloat*) &p_model->m_unk0x08
	);
	p_binding->m_flags = (p_binding->m_flags & ~8u) | ((selected & 1) << 3);
	LegoS32 sortKey;
	if (p_binding->m_bounds != NULL) {
		sortKey = GetModelPointSortKey(
			g_displayRenderer,
			NULL,
			(const Vector3*) GetLookupResourceOrigin((LegoU8*) p_binding, NULL)
		);
	}
	else {
		sortKey = GetModelDepthSortKey(g_displayRenderer, NULL, p_binding->m_unk0xB8);
	}
	p_binding->m_owner->m_vtable->m_action0x2c(p_binding->m_owner, NULL);
	LegoS32 groupIndex = p_binding->m_unk0xBC < g_modelLodDepthThreshold ? 1 : 0;
	if (g_unk0x0058a170.m_mode == 1 && *(LegoFloat*) &p_model->m_unk0x08 > 25.0f) {
		groupIndex = 0;
	}
	if (groupIndex >= p_model->m_groupCount) {
		groupIndex = p_model->m_groupCount - 1;
	}
	ModelTextureGroup* group = &p_model->m_groups[groupIndex];
	if (p_binding->m_unk0xC0 != 0) {
		if (p_model->m_specialGroup != NULL) {
			group = p_model->m_specialGroup;
		}
		Vector3* vectors = group->m_primitiveVectors;
		ModelPrimitiveTextureRecord* primitive = group->m_primitives;
		ModelPrimitiveTextureRecord* end = primitive + group->m_primitiveCount;
		for (; primitive < end; ++primitive) {
			LegoS32 key = sortKey;
			if (vectors != NULL) {
				Vector3 position;
				position.m_x = p_binding->m_position[0] + vectors->m_x;
				position.m_y = vectors->m_y + p_binding->m_position[1];
				position.m_z = vectors->m_z + p_binding->m_position[2];
				LegoFloat depth = GetGameObjectViewDepth(g_unk0x00679ef0, NULL, &position);
				key = GetModelDepthSortKey(g_displayRenderer, NULL, depth);
				++vectors;
			}
			InsertModelDraw(0, 0, p_binding, primitive, key);
		}
	}
	else {
		ModelPrimitiveTextureRecord* primitive = group->m_primitives;
		ModelPrimitiveTextureRecord* end = primitive + group->m_primitiveCount;
		for (; primitive < end; ++primitive) {
			RendererAuxiliaryResourceRecord* material = g_displayRenderer->GetAuxiliaryResource(primitive->m_material);
			if (material->m_mode != 0) {
				InsertModelDraw(0, 0, p_binding, primitive, sortKey);
			}
			else {
				LegoS32 texture = 0;
				for (LegoS16 slot = 0; slot < primitive->m_slotCount; ++slot) {
					if (primitive->m_slots[slot].m_unk0x00 == 0) {
						if (primitive->m_slots[slot].m_texture != -1) {
							texture = primitive->m_slots[slot].m_texture;
						}
						break;
					}
				}
				InsertModelDraw(1, 0, p_binding, primitive, texture);
			}
		}
	}
}

// STUB: LEGORACERS2 0x004a9680
void __fastcall UpdateModelResourceVisibility(ModelResource* p_model, void*, GameObjectModelBinding* p_binding)
{
	if (p_model->m_unk0x28 != NULL) {
		LegoU8* camera = (LegoU8*) g_unk0x00679ef0;
		LegoU8* end = *(LegoU8**) (camera + 0x300);
		LegoFloat limit = -(*(LegoFloat*) &p_model->m_unk0x08 + 50.0f);
		for (LegoU8* normal = camera + 0x270; normal < end; normal += 0x0c) {
			const LegoFloat* direction = (const LegoFloat*) normal;
			const LegoFloat* point = (const LegoFloat*) (normal + 0x48);
			LegoFloat distance = (p_binding->m_position[0] - point[0]) * direction[0];
			distance += (p_binding->m_position[1] - point[1]) * direction[1];
			distance += (p_binding->m_position[2] - point[2]) * direction[2];
			if (!(distance > limit)) {
				return;
			}
		}
		InsertModelDraw(2, 1, p_binding, NULL, 0);
	}
}
