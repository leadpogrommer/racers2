#include "modelresourceparser.h"

#include "matrix3.h"
#include "memoryheap.h"
#include "modelbounds.h"
#include "modelresource.h"
#include "modelskin.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererdrawpacketview.h"
#include "rendererstageplan.h"
#include "renderervertexdata.h"
#include "winfile.h"

#include <math.h>
#include <new>
#include <string.h>

namespace
{
const LegoU32 kModelVersion1 = 0x314c444d;          // MDL1
const LegoU32 kModelVersion2 = 0x324c444d;          // MDL2
const LegoU32 kLegacyModelVersion = 0x314c444c;     // LDL1
const LegoU32 kGeometryVersion0 = 0x304f4547;       // GEO0
const LegoU32 kGeometryVersion1 = 0x314f4547;       // GEO1
const LegoU32 kGeometryPacketVersion0 = 0x30473250; // P2G0

struct ModelBitmapRecord {
	LegoChar m_path[0x100];
	LegoU32 m_resourceKind;
	LegoU32 m_sourceIndex;
};
DECOMP_SIZE_ASSERT(ModelBitmapRecord, 0x108);

LegoS32 GetLegacyModelIndexCount(LegoS32 p_type, LegoS32 p_primitiveCount)
{
	switch (p_type) {
	case 0:
		return p_primitiveCount * 3;
	case 1:
	case 2:
		return p_primitiveCount + 2;
	case 3:
		return p_primitiveCount * 2;
	case 4:
		return p_primitiveCount + 1;
	case 5:
		return p_primitiveCount;
	default:
		return p_primitiveCount;
	}
}

LegoS32 GetModelPrimitiveCount(LegoS32 p_type, LegoS32 p_indexCount)
{
	switch (p_type) {
	case 0:
		return p_indexCount / 3;
	case 1:
	case 2:
		return p_indexCount - 2;
	case 3:
		return p_indexCount / 2;
	case 4:
		return p_indexCount - 1;
	case 5:
	default:
		return p_indexCount;
	}
}

void SetModelBoundsFromFile(ModelBounds* p_bounds, const LegoFloat* p_box, LegoFloat p_yaw)
{
	Matrix3 rotation;
	LegoFloat angles[3] = {0.0f, p_yaw, 0.0f};
	rotation.FUN_004a4f60(angles);
	LegoFloat minimumX = p_box[0];
	LegoFloat minimumY = p_box[1];
	LegoFloat minimumZ = p_box[2];
	LegoFloat maximumX = p_box[3];
	LegoFloat maximumY = p_box[4];
	LegoFloat maximumZ = p_box[5];
	LegoFloat x[8] = {minimumX, minimumX, maximumX, maximumX, minimumX, minimumX, maximumX, maximumX};
	LegoFloat y[8] = {maximumY, minimumY, maximumY, minimumY, maximumY, minimumY, maximumY, minimumY};
	LegoFloat z[8] = {minimumZ, minimumZ, minimumZ, minimumZ, maximumZ, maximumZ, maximumZ, maximumZ};
	Vector3 translation;
	translation.SetValues(p_box[6], p_box[7], p_box[8]);
	LegoS32 index;
	for (index = 0; index < 8; ++index) {
		p_bounds->m_corners[index].SetValues(x[index], y[index], z[index]);
		p_bounds->m_corners[index].FUN_004a4e00(&rotation);
		p_bounds->m_corners[index].Add(&translation);
	}
	Vector3& center = p_bounds->m_center;
	center.m_x = (p_bounds->m_corners[6].m_x - p_bounds->m_corners[1].m_x) * 0.5f;
	center.m_y = (p_bounds->m_corners[6].m_y - p_bounds->m_corners[1].m_y) * 0.5f;
	center.m_z = (p_bounds->m_corners[6].m_z - p_bounds->m_corners[1].m_z) * 0.5f;
	p_bounds->m_radius = center.Dot(&center) > 0.0f ? (LegoFloat) sqrt(center.Dot(&center)) : 0.0f;
	center.m_x += p_bounds->m_corners[1].m_x;
	center.m_y += p_bounds->m_corners[1].m_y;
	center.m_z += p_bounds->m_corners[1].m_z;
	p_bounds->ClassifyXZAspectRatio();
}

RendererMainDispatch* GetModelRendererDispatch(DisplayRenderer* p_renderer)
{
	return (RendererMainDispatch*) p_renderer->m_vtable;
}

} // namespace

// STUB: LEGORACERS2 0x004a8140
void __fastcall ReadModelResourceGeometry(
	ModelResource* p_model,
	void* p_unusedEdx,
	const LegoChar* p_path,
	WinFile* p_file,
	LegoU32 p_uncached,
	LegoU32 p_flags
)
{
	ModelResource* model = p_model;
	(void) p_unusedEdx;
	DisplayRenderer* renderer = g_displayRenderer;
	RendererMainDispatch* rendererDispatch = GetModelRendererDispatch(renderer);
	LegoU32 legacyHeader = 0;
	LegoU32 geometrySignature = 0;
	LegoU32 geometryBytes = 0;
	LegoS32 bitmapCount = 0;
	LegoS32 materialCount = 0;
	LegoS32 index;
	LegoS32 slotIndex;
	LegoS32 groupIndex;
	LegoS32 primitiveIndex;
	LegoS32 search;
	LegoBool32 usePacketVertexData = FALSE;
	(void) p_path;
	(void) p_flags;

	p_file->Read(&model->m_signature, 4, 1);
	if (model->m_signature > kLegacyModelVersion) {
		p_file->Read(&legacyHeader, 4, 1);
	}
	p_file->Read(&model->m_origin, 0x0c, 1);
	p_file->Read(&model->m_unk0x08, 4, 1);
	if (model->m_signature < kModelVersion1) {
		model->m_unk0x24 = 0;
	}
	else {
		LegoU32 boundsPresent;
		LegoFloat boxAndOrigin[9];
		LegoFloat boundsYaw;
		p_file->Read(&model->m_unk0x24, 4, 1);
		p_file->Read(&boundsPresent, 4, 1);
		if (boundsPresent != 0) {
			model->m_bounds = (ModelBounds*) AllocateHeapMemory(sizeof(ModelBounds));
			if (model->m_bounds != NULL) {
				model->m_bounds->m_isElongatedXZ = 1;
				model->m_bounds->m_radius = 0.0f;
			}
			p_file->Read(boxAndOrigin, sizeof(boxAndOrigin), 1);
			p_file->Read(&boundsYaw, 4, 1);
			SetModelBoundsFromFile(model->m_bounds, boxAndOrigin, boundsYaw);
		}
	}
	RendererAuxiliaryResourceRecord material;
	if (model->m_signature >= kModelVersion1) {
		p_file->Read(&material, 0x40, 1);
	}

	p_file->Read(&bitmapCount, 4, 1);
	ModelBitmapRecord* bitmaps = (ModelBitmapRecord*) GetMemoryHeap()->AllocateFromEnd(
		(LegoU32) bitmapCount * sizeof(ModelBitmapRecord),
		"Temporary model bitmaps"
	);
	LegoS32* textureIds = (LegoS32*) GetMemoryHeap()->AllocateFromEnd(
		(LegoU32) bitmapCount * sizeof(LegoS32),
		"Temporary model bitmap indices"
	);
	p_file->Read(bitmaps, (DWORD) bitmapCount * sizeof(ModelBitmapRecord), 1);
	for (index = 0; index < bitmapCount; ++index) {
		ModelBitmapRecord* bitmap = &bitmaps[index];
		if (rendererDispatch->m_acceptUnusedRendererPolicy(renderer, NULL, bitmap->m_resourceKind) == TRUE) {
			LegoS32 textureIndex =
				bitmap->m_sourceIndex == (LegoU32) index
					? renderer->CreateTextureResource(bitmap->m_path, bitmap->m_resourceKind, -1, 1, 1)
					: renderer->CreateTextureResource(
						  bitmap->m_path,
						  bitmap->m_resourceKind,
						  textureIds[bitmap->m_sourceIndex],
						  1,
						  1
					  );
			textureIds[index] = textureIndex;
		}
		else {
			textureIds[index] = -1;
		}
	}

	p_file->Read(&materialCount, 4, 1);
	model->m_resources.Allocate((LegoS16) bitmapCount, (LegoS16) materialCount);
	for (index = 0; index < bitmapCount; ++index) {
		model->m_resources.SetTexture(index, (LegoS16) textureIds[index]);
	}
	GetMemoryHeap()->Free(textureIds);
	GetMemoryHeap()->Free(bitmaps);

	for (index = 0; index < materialCount; ++index) {
		if (model->m_signature < kModelVersion2) {
			LegoU32 data[7];
			p_file->Read(data, sizeof(data), 1);
			material.InitializeMaterial(
				data[0],
				data[1],
				data[2],
				data[3],
				*(LegoFloat*) &data[4],
				*(LegoFloat*) &data[5],
				*(LegoFloat*) &data[6]
			);
		}
		else {
			p_file->Read(&material, 0x58, 1);
		}
		model->m_resources.SetMaterial(index, (LegoS16) renderer->AcquireAuxiliaryResource(&material, 1));
	}

	if (model->m_signature < kModelVersion1) {
		geometrySignature = kGeometryVersion0;
	}
	else {
		p_file->Read(&geometrySignature, 4, 1);
		p_file->Read(&geometryBytes, 4, 1);
		if (geometrySignature == kGeometryPacketVersion0) {
			// This option dispatch is the renderer's existing six-argument
			// geometry policy. A rejected packet chunk is skipped verbatim.
			usePacketVertexData = rendererDispatch->m_supportsUnknownDrawFeature(
									  renderer,
									  NULL,
									  (LegoU32) p_file,
									  (LegoU32) model,
									  geometrySignature,
									  geometryBytes,
									  p_uncached,
									  p_flags
								  ) != FALSE;
			if (usePacketVertexData == FALSE) {
				p_file->SeekManagedCurrent((LONG) geometryBytes);
			}
			p_file->Read(&geometrySignature, 4, 1);
			p_file->Read(&geometryBytes, 4, 1);
		}
		if (geometrySignature != kGeometryVersion0 && geometrySignature != kGeometryVersion1) {
			goto selectSpecialGroup;
		}
	}

	p_file->Read(&model->m_groupCount, 4, 1);
	model->m_groups = (ModelTextureGroup*) AllocateHeapMemory(model->m_groupCount * sizeof(ModelTextureGroup));
	if (model->m_groups != NULL) {
		for (groupIndex = 0; groupIndex < model->m_groupCount; ++groupIndex) {
			new (&model->m_groups[groupIndex]) ModelTextureGroup();
		}
	}
	for (groupIndex = 0; groupIndex < model->m_groupCount; ++groupIndex) {
		model->m_groups[groupIndex].m_type = groupIndex + 1;
	}
	for (groupIndex = 0; groupIndex < model->m_groupCount; ++groupIndex) {
		ModelTextureGroup* group = &model->m_groups[groupIndex];
		LegoS32 groupVertexCount = 0;
		if (geometrySignature == kGeometryVersion1) {
			p_file->Read(group, sizeof(ModelTextureGroup), 1);
			groupVertexCount = group->m_primitiveCount;
			if (group->m_primitiveVectors != NULL) {
				group->m_primitiveVectors = (Vector3*) AllocateHeapMemory(groupVertexCount * sizeof(Vector3));
				p_file->Read(group->m_primitiveVectors, groupVertexCount * sizeof(Vector3), 1);
			}
		}
		else {
			p_file->Read(&groupVertexCount, 4, 1);
			group->m_primitiveCount = groupVertexCount;
		}
		ModelPrimitiveTextureRecord* allocation = (ModelPrimitiveTextureRecord*) AllocateHeapMemory(
			groupVertexCount * sizeof(ModelPrimitiveTextureRecord) + sizeof(LegoU32)
		);
		if (allocation != NULL) {
			*((LegoU32*) allocation) = groupVertexCount;
			group->m_primitives = (ModelPrimitiveTextureRecord*) ((LegoU32*) allocation + 1);
			for (primitiveIndex = 0; primitiveIndex < groupVertexCount; ++primitiveIndex) {
				new (&group->m_primitives[primitiveIndex]) ModelPrimitiveTextureRecord();
			}
		}
		else {
			group->m_primitives = NULL;
		}

		for (primitiveIndex = 0; primitiveIndex < groupVertexCount; ++primitiveIndex) {
			ModelPrimitiveTextureRecord* primitive = &group->m_primitives[primitiveIndex];
			p_file->Read(primitive, sizeof(ModelPrimitiveTextureRecord), 1);
			primitive->m_heapData = NULL;
			primitive->m_vertexBuffer = NULL;
			primitive->m_indexBuffer = NULL;

			LegoU32 vertexBufferMode = p_uncached;
			for (slotIndex = 0; slotIndex < primitive->m_slotCount; ++slotIndex) {
				ModelPrimitiveTextureSlot* slot = &primitive->m_slots[slotIndex];
				if (rendererDispatch->m_isTextureResourceKindSpecial(renderer, NULL, slot->m_unk0x00) == TRUE) {
					vertexBufferMode = 1;
				}
				if (slot->m_texture != -1) {
					slot->m_texture = (LegoS16) model->m_resources.GetTexture(slot->m_texture);
				}
			}
			primitive->m_material = (LegoS16) model->m_resources.GetMaterial(primitive->m_material);
			LegoU8 maximumCoordinateSet = 0;
			for (slotIndex = 0; slotIndex < primitive->m_slotCount; ++slotIndex) {
				ModelPrimitiveTextureSlot* slot = &primitive->m_slots[slotIndex];
				if (rendererDispatch->m_isTextureResourceKindAccepted(renderer, NULL, slot->m_unk0x00) == FALSE) {
					slot->m_coordinateSet = 0x0f;
				}
				else if (
					(slot->m_coordinateSet & 0x0f) != 0x0f && maximumCoordinateSet < (slot->m_coordinateSet & 0x0f)
				) {
					maximumCoordinateSet = slot->m_coordinateSet & 0x0f;
				}
			}
			primitive->m_coordinateSetCount = maximumCoordinateSet + 1;
			for (slotIndex = 0; slotIndex < primitive->m_slotCount; ++slotIndex) {
				ModelPrimitiveTextureSlot* slot = &primitive->m_slots[slotIndex];
				LegoU8 coordinateSet = 0;
				if (rendererDispatch->m_isTextureResourceKindAccepted(renderer, NULL, slot->m_unk0x00) == TRUE) {
					coordinateSet = slot->m_coordinateSet & 0x0f;
					if (coordinateSet == 0x0f) {
						for (coordinateSet = 0; coordinateSet < 4; ++coordinateSet) {
							LegoBool32 found = FALSE;
							for (search = 0; search < primitive->m_slotCount; ++search) {
								if ((primitive->m_slots[search].m_coordinateSet & 0x0f) == coordinateSet) {
									found = TRUE;
									break;
								}
							}
							if (found == FALSE) {
								break;
							}
						}
						if (primitive->m_coordinateSetCount <= coordinateSet) {
							primitive->m_coordinateSetCount = (LegoU8) (coordinateSet + 1);
						}
					}
				}
				slot->m_coordinateSet = coordinateSet;
			}

			RendererVertexDataView vertexData;
			memset(&vertexData, 0, sizeof(vertexData));
			p_file->Read(&vertexData, sizeof(vertexData), 1);
			*(LegoU16*) vertexData.m_unk0x1c = 0;
			LegoU32 vertexCount = vertexData.m_vertexCount;
			LegoU32 vertexBytes = vertexCount * vertexData.m_stride;
			if (usePacketVertexData == TRUE) {
				p_file->SeekManagedCurrent((LONG) vertexBytes);
				primitive->m_heapData = NULL;
				vertexData.m_data = NULL;
			}
			else {
				vertexData.m_data = (LegoU8*) GetMemoryHeap()->AllocateFromEnd(vertexBytes, "Temporary vertex buffer");
				p_file->Read(vertexData.m_data, vertexBytes, 1);
				LegoU32 vertexAttributes =
					vertexData.m_attributes |
					rendererDispatch->m_getDotProductStageFlag(renderer, NULL, primitive->m_unk0x14);
				primitive->m_vertexBuffer = rendererDispatch->m_createVertexBuffer(
					renderer,
					NULL,
					vertexAttributes,
					vertexData.m_vertexCount,
					primitive->m_coordinateSetCount,
					vertexBufferMode
				);
				primitive->m_vertexBuffer->CopyFromVertexData(&vertexData, 0);
				GetMemoryHeap()->Free(vertexData.m_data);
			}

			LegoS32 indexType = 0;
			LegoS32 indexCount;
			if (model->m_signature < kModelVersion1) {
				p_file->Read(&indexType, 4, 1);
				indexCount = GetLegacyModelIndexCount(indexType, primitive->m_primitiveCount);
			}
			else {
				LegoS32 indexInfo[3];
				p_file->Read(indexInfo, sizeof(indexInfo), 1);
				indexType = indexInfo[1];
				indexCount = indexInfo[2];
				primitive->m_primitiveCount = (LegoU16) GetModelPrimitiveCount(indexType, indexCount);
			}
			if (usePacketVertexData == TRUE) {
				LegoU32 specialData = rendererDispatch->m_rejectUnknownDrawRequestAgain(
					renderer,
					NULL,
					(LegoU32) model->m_unk0x1c,
					(LegoU32) groupIndex,
					(LegoU32) primitiveIndex
				);
				p_file->SeekManagedCurrent(indexCount * 2);
				primitive->m_indexBuffer = rendererDispatch->m_createIndexBuffer(renderer, NULL, 6, 0);
				primitive->m_indexBuffer->m_data = (LegoU8*) specialData;
			}
			else {
				primitive->m_indexBuffer = rendererDispatch->m_createIndexBuffer(renderer, NULL, indexType, indexCount);
				if (primitive->m_indexBuffer->Lock(0, indexCount, 1) == TRUE) {
					DWORD byteCount = (DWORD) indexCount * 2;
					p_file->Read(primitive->m_indexBuffer->m_writtenEnd, byteCount, 1);
					primitive->m_indexBuffer->m_writtenEnd += byteCount;
					primitive->m_indexBuffer->Unlock(1, 1);
				}
			}
			rendererDispatch->m_prepareDrawPlan(renderer, NULL, (RendererStagePlanRequest*) primitive);
			if ((primitive->m_unk0x14 & 8) != 0) {
				model->m_hasSpecialPrimitives = 1;
			}
		}
	}
selectSpecialGroup:
	model->m_specialGroup = NULL;
	for (index = 0; index < model->m_groupCount; ++index) {
		if (model->m_groups[index].m_type == 3) {
			model->m_specialGroup = &model->m_groups[index];
			break;
		}
	}
}
