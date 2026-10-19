#include "modelshadow.h"

#include "memoryheap.h"
#include "modelresource.h"
#include "modelresourcechunks.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "renderershadowshared.h"
#include "winfile.h"

#include <string.h>

extern DisplayRenderer* g_displayRenderer;

// STUB: LEGORACERS2 0x004c6c90
void __fastcall ReleaseModelShadowData(ModelResource* p_model, void* p_unusedEdx)
{
	(void) p_unusedEdx;
	ModelShadowData* shadow = (ModelShadowData*) p_model->m_unk0x28;
	if (shadow != NULL) {
		--shadow->m_referenceCount;
		if ((LegoS32) shadow->m_referenceCount <= 0x1f) {
			LegoU32 mask = 1u << (shadow->m_referenceCount & 0x1f);
			if ((shadow->m_geometryReferenceMask & mask) != 0) {
				shadow->m_geometryReferenceMask &= ~mask;
				if (shadow->m_geometryReferenceMask == 0) {
					if (shadow->m_edgeRecords != NULL) {
						GetMemoryHeap()->Free(shadow->m_edgeRecords);
					}
					if (shadow->m_faceRecords != NULL) {
						GetMemoryHeap()->Free(shadow->m_faceRecords);
					}
				}
			}
			if ((shadow->m_textureReferenceMask & mask) != 0) {
				g_displayRenderer->ReleaseTextureResourceReference(shadow->m_textureId, 1);
			}
		}
		if (shadow->m_referenceCount == 0) {
			FreeHeapMemory(shadow);
			p_model->m_unk0x28 = NULL;
		}
		ReleaseSharedShadowGeometry(g_displayRenderer, NULL);
	}
}

// STUB: LEGORACERS2 0x004c6e80
void __fastcall ReadModelShadowChunk(
	ModelResource* p_model,
	void* p_unusedEdx,
	WinFile* p_file,
	LegoU32 p_tag,
	LegoU32 p_bytes
)
{
	(void) p_unusedEdx;
	if (p_tag != 0x30414853) {
		return;
	}

	DisplayRenderer* renderer = g_displayRenderer;
	RendererMainDispatch* dispatch = (RendererMainDispatch*) renderer->m_vtable;
	if (dispatch->m_supportsExtendedTextureStages(renderer, NULL) != TRUE) {
		p_file->SeekManagedCurrent((LONG) p_bytes);
		return;
	}

	EnsureSharedShadowGeometry(renderer, NULL);
	ModelShadowData* shadow = (ModelShadowData*) AllocateHeapMemory(sizeof(ModelShadowData));
	memset(shadow, 0, sizeof(*shadow));
	shadow->m_unk0x10 = 1;
	shadow->m_scaleX = 1.0f;
	shadow->m_scaleY = 1.0f;
	shadow->m_textureId = -1;
	p_model->m_unk0x28 = shadow;

	p_file->Read(&shadow->m_faceRecordCount, sizeof(shadow->m_faceRecordCount), 1);
	shadow->m_faceRecords = GetMemoryHeap()->Allocate(shadow->m_faceRecordCount * 0x0c, "Shadow face info");
	p_file->Read(shadow->m_faceRecords, shadow->m_faceRecordCount * 0x0c, 1);

	p_file->Read(&shadow->m_edgeRecordCount, sizeof(shadow->m_edgeRecordCount), 1);
	shadow->m_edgeRecords = GetMemoryHeap()->Allocate(shadow->m_edgeRecordCount * 0x24, "Shadow edge info");
	p_file->Read(shadow->m_edgeRecords, shadow->m_edgeRecordCount * 0x24, 1);

	shadow->m_geometryReferenceMask |= 1u << (shadow->m_referenceCount & 0x1f);
	++shadow->m_referenceCount;
	p_model->m_hasSpecialPrimitives = 1;
}
