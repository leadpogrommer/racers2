#ifndef LR2_MODELRESOURCE_H
#define LR2_MODELRESOURCE_H

#include "decomp.h"
#include "math/vector3.h"
#include "render/models/modelresourcereferences.h"
#include "types.h"

#include <stddef.h>

class RendererIndexBuffer;
class RendererVertexBuffer;
struct ModelBounds;
struct ModelSkin;

struct ModelPrimitiveTextureSlot {
	LegoS32 m_unk0x00;
	LegoS16 m_texture;
	LegoU8 m_coordinateSet;
	LegoU8 m_flags;
};
struct ModelPrimitiveTextureRecord {
	LegoU16 m_primitiveCount;
	LegoU16 m_vertexCount;
	LegoS16 m_material;
	LegoU16 m_flags;
	void* m_heapData;
	RendererVertexBuffer* m_vertexBuffer;
	RendererIndexBuffer* m_indexBuffer;
	LegoU32 m_unk0x14;
	LegoS16 m_slotCount;
	LegoU8 m_coordinateFlags;
	LegoU8 m_coordinateSetCount;
	ModelPrimitiveTextureSlot m_slots[4];

	ModelPrimitiveTextureRecord();
};
struct ModelTextureGroup {
	LegoU32 m_type;
	LegoU32 m_unk0x04;
	LegoS32 m_primitiveCount;
	Vector3* m_primitiveVectors;
	ModelPrimitiveTextureRecord* m_primitives;

	ModelTextureGroup();
	void ReleaseStorage();
};

// Retail loader 4a7d40 allocates 0x48 bytes and initializes these fields.
struct ModelResource {
	LegoU32 m_signature;
	LegoU32 m_hasSpecialPrimitives;
	LegoU32 m_unk0x08;
	ModelTextureGroup* m_groups;
	ModelTextureGroup* m_specialGroup;
	ModelBounds* m_bounds;
	ModelSkin* m_skin;
	void* m_unk0x1c;
	LegoS32 m_groupCount;
	LegoU32 m_unk0x24;
	void* m_unk0x28;
	ModelResourceReferences m_resources;
	Vector3 m_origin;
	void* m_collisionData;
	LegoU32 m_collisionDataSize;
	~ModelResource();
	void SetCollisionData(void* p_data, LegoU32 p_bytes);
	void RemapTextureIndices(LegoS32 p_oldIndex, LegoS16 p_newIndex);
};
DECOMP_SIZE_ASSERT(ModelPrimitiveTextureSlot, 8);
DECOMP_SIZE_ASSERT(ModelPrimitiveTextureRecord, 0x3c);
DECOMP_SIZE_ASSERT(ModelTextureGroup, 0x14);
DECOMP_SIZE_ASSERT(ModelResource, 0x48);
DECOMP_STATIC_ASSERT(offsetof(ModelPrimitiveTextureRecord, m_slots) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(ModelPrimitiveTextureRecord, m_heapData) == 8);
DECOMP_STATIC_ASSERT(offsetof(ModelPrimitiveTextureRecord, m_vertexBuffer) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ModelPrimitiveTextureRecord, m_indexBuffer) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_groups) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_groupCount) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_resources) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_origin) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_collisionData) == 0x40);

#endif
