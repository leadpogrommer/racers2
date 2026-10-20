#include "render/pipeline/rendererstagebuilders.h"

#include "decomp.h"
#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004e4e80
RendererStageRecordLink* __fastcall InitializeTextureStageRecord(
	RendererStageRecordLink* p_entry,
	void*,
	LegoU32 p_value0,
	LegoU32 p_value1,
	LegoU32 p_value2
)
{
	p_entry->m_values[0] = 1;
	p_entry->m_values[1] = p_value0;
	p_entry->m_values[2] = p_value1;
	p_entry->m_values[3] = p_value2;
	return p_entry;
}

// FUNCTION: LEGORACERS2 0x004e4ea0
void __fastcall SetTextureStageProfileValue(
	TextureStageProfileIndexMapView* p_map,
	void*,
	LegoU32 p_value,
	LegoS32 p_index
)
{
	p_map->m_values[p_index] = p_value;
	if (p_map->m_highestIndex < p_index) {
		p_map->m_highestIndex = p_index;
	}
}

// STUB: LEGORACERS2 0x004e5e70
RendererStageRecordLink* __fastcall InitializeAndLinkTextureStageRecord(
	RendererSubrecordGroup::Subrecord* p_group,
	void*,
	RendererStageRecordLink* p_entry,
	LegoU32 p_value0,
	LegoU32 p_value1,
	LegoU32 p_value2
)
{
	p_entry->m_values[0] = 1;
	p_entry->m_values[1] = p_value1;
	p_entry->m_values[2] = p_value2;
	p_entry->m_values[3] = p_value0;
	LinkTextureStageRecord(p_group, NULL, p_entry);
	return p_entry;
}

// FUNCTION: LEGORACERS2 0x004e5ea0
void __fastcall LinkTextureStageRecord(
	RendererSubrecordGroup::Subrecord* p_group,
	void*,
	RendererStageRecordLink* p_entry
)
{
	p_entry->m_next = (RendererStageRecordLink*) p_group->m_unk0x20;
	p_group->m_unk0x20 = (LegoU32) p_entry;
}

// STUB: LEGORACERS2 0x004e5eb0
void __fastcall AddTextureStageSubrecords(
	RendererTextureStageManager* p_manager,
	void*,
	RendererSubrecordGroup::Subrecord* p_groupBase,
	LegoU32 p_groupIndex,
	const RendererArrayMarker* p_sourceRecords,
	LegoS32 p_count
)
{
	RendererSubrecordGroup::Subrecord* group = p_groupBase + p_groupIndex;
	const LegoU32* source = (const LegoU32*) p_sourceRecords;

	for (LegoS32 index = 0; index < p_count; index++) {
		LegoU32 recordIndex = p_manager->m_unk0x3484;
		RendererStageRecordLink* entry = (RendererStageRecordLink*) &p_manager->m_stageRecords[recordIndex];

		entry->m_values[0] = source[0];
		entry->m_values[1] = source[1];
		entry->m_values[2] = source[2];
		entry->m_values[3] = source[3];

		// Retail reloads the counter after copying, then uses that reloaded
		// value for both the increment and the node address.
		recordIndex = p_manager->m_unk0x3484;
		p_manager->m_unk0x3484 = recordIndex + 1;
		entry = (RendererStageRecordLink*) &p_manager->m_stageRecords[recordIndex];
		LinkTextureStageRecord(group, NULL, entry);
		source += 4;
	}
}

// STUB: LEGORACERS2 0x004e5f30
LegoU32 __fastcall ApplyTextureStagePlanForManager(
	RendererTextureStageManager* p_manager,
	void*,
	RendererSubrecordGroup* p_group,
	RendererStagePlanRequest* p_request,
	LegoU32 p_mode
)
{
	DisplayRenderer* renderer =
		(DisplayRenderer*) ((BYTE*) p_manager - offsetof(DisplayRenderer, m_textureStageManager));
	const LegoU32 result = ApplyRendererTextureStagePlan(renderer, NULL, p_group, p_request, p_mode);
	if (result == 0) {
		p_manager->m_unk0x3480 = p_manager->m_unk0x3488;
		p_manager->m_unk0x3484 = p_manager->m_unk0x348c;
		p_manager->m_recordIndexCache[p_request->m_defaults.m_unk0x00] = 0xff;
		return 0;
	}
	return 1;
}

// STUB: LEGORACERS2 0x004e5f90
LegoU32 __fastcall ApplyRendererTextureStagePlan(
	DisplayRenderer* p_renderer,
	void*,
	RendererSubrecordGroup* p_group,
	RendererStagePlanRequest* p_request,
	LegoU32 p_mode
)
{
	if (p_mode == 1) {
		return 1;
	}

	IDirect3DVertexBuffer8* temporaryVertices = NULL;
	IDirect3DIndexBuffer8* temporaryIndices = NULL;
	D3DMATERIAL8 material;
	p_renderer->InitializeDefaultRenderStates();
	material.Diffuse.r = 1.0f;
	material.Diffuse.g = 1.0f;
	material.Diffuse.b = 1.0f;
	material.Diffuse.a = 0.5f;
	material.Ambient.r = 0.5f;
	material.Ambient.g = 0.5f;
	material.Ambient.b = 0.5f;
	material.Ambient.a = 0.5f;
	material.Specular.r = 0.2f;
	material.Specular.g = 0.2f;
	material.Specular.b = 0.2f;
	material.Specular.a = 0.5f;
	material.Emissive.r = 0.1f;
	material.Emissive.g = 0.1f;
	material.Emissive.b = 0.1f;
	material.Emissive.a = 0.5f;
	material.Power = 0.3f;

	p_renderer->m_device->SetMaterial(&material);
	RendererVertexBuffer* source = (RendererVertexBuffer*) p_request->m_unk0x0c;
	const LegoU32 stride = source->GetStride();
	const LegoU32 vertexShader = source->m_storage->m_format;
	p_renderer->m_device
		->CreateVertexBuffer(stride * 3, D3DUSAGE_WRITEONLY, vertexShader, D3DPOOL_DEFAULT, &temporaryVertices);
	p_renderer->m_device
		->CreateIndexBuffer(6, D3DUSAGE_WRITEONLY, (D3DFORMAT) 0x65, D3DPOOL_DEFAULT, &temporaryIndices);

	if (temporaryVertices == NULL) {
		p_renderer->m_cachedStreamBuffer = NULL;
		if (p_renderer->m_cachedVertexShader != vertexShader) {
			p_renderer->m_cachedVertexShader = vertexShader;
			p_renderer->m_device->SetVertexShader(vertexShader);
		}
	}
	else if (p_renderer->m_cachedStreamBuffer != temporaryVertices) {
		p_renderer->m_cachedStreamBuffer = temporaryVertices;
		p_renderer->m_device->SetStreamSource(0, temporaryVertices, stride);
		if (p_renderer->m_cachedVertexShader != vertexShader) {
			p_renderer->m_cachedVertexShader = vertexShader;
			p_renderer->m_device->SetVertexShader(vertexShader);
		}
	}
	p_renderer->m_device->SetIndices(temporaryIndices, 0);

	IDirect3DBaseTexture8* loadedTextures[4];
	for (LegoS32 slotIndex = 0; slotIndex < (LegoS16) p_request->m_defaults.m_unk0x04; slotIndex++) {
		RendererDefaultSlot* slot = &p_request->m_defaults.m_slots[slotIndex];
		loadedTextures[slotIndex] = NULL;
		if ((LegoS16) slot->m_mask == -1) {
			continue;
		}

		const LegoS32 textureIndex = (LegoS16) slot->m_mask;
		const LegoU32 recordIndex = p_renderer->m_textureIndexMap[textureIndex];
		RendererTextureRecord* record = &p_renderer->m_textureRecords[recordIndex];
		loadedTextures[slotIndex] = record->m_texture;
		if (record->m_texture == NULL) {
			p_renderer->CreateSecondaryTexture(record);
			record->UpdateGpuTexture();
		}
	}

	RendererTextureStageManager* manager = &p_renderer->m_textureStageManager;
	LegoU32 valid = 1;
	for (LegoS32 index = 0; manager->ApplyStagePlanSubrecord(&p_request->m_defaults, index) == 1; index++) {
		DWORD passCount = 0;
		HRESULT result = p_renderer->m_device->ValidateDevice(&passCount);
		result = PassThroughStageValidationResult(result);
		if (result != D3D_OK) {
			for (LegoS32 subrecord = 0; subrecord < (LegoS32) p_group->m_unk0xc0; subrecord++) {
				ResetTextureStageCoordinateIndices(&p_group->m_records[subrecord], NULL);
			}
			result = p_renderer->m_device->ValidateDevice(&passCount);
			result = PassThroughStageValidationResult(result);
			if (result != D3D_OK) {
				p_renderer->InitializeDefaultRenderStates();
				valid = 0;
				break;
			}
		}
	}

	const LegoS16 cleanupCount = (LegoS16) p_request->m_defaults.m_unk0x04;
	for (LegoS32 cleanupIndex = (LegoS32) cleanupCount - 1; cleanupIndex >= 0; cleanupIndex--) {
		RendererDefaultSlot* slot = &p_request->m_defaults.m_slots[cleanupIndex];
		if ((LegoS16) slot->m_mask == -1 || loadedTextures[cleanupIndex] != NULL) {
			continue;
		}
		const LegoS32 textureIndex = (LegoS16) slot->m_mask;
		const LegoU32 recordIndex = p_renderer->m_textureIndexMap[textureIndex];
		RendererTextureRecord* record = &p_renderer->m_textureRecords[recordIndex];
		if (record->m_texture != NULL) {
			record->m_texture->Release();
			record->m_texture = NULL;
		}
	}

	temporaryIndices->Release();
	temporaryVertices->Release();
	p_renderer->m_cachedStreamBuffer = NULL;
	if (p_renderer->m_cachedVertexShader != 0x1c4) {
		p_renderer->m_cachedVertexShader = 0x1c4;
		p_renderer->m_device->SetVertexShader(0x1c4);
	}
	p_renderer->m_device->SetIndices(NULL, 0);
	return valid;
}
