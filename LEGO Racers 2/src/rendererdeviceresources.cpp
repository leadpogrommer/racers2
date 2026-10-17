#include "renderer.h"

#include <string.h>

static LegoBool32 EnsureVertexStorageDeviceBuffer(DisplayRenderer* p_renderer, RendererVertexStorage* p_storage)
{
	if (p_storage->m_deviceBuffer != NULL) {
		return TRUE;
	}

	DWORD usage;
	if ((p_storage->m_flags & 2) != 0) {
		usage = p_renderer->m_vertexBufferUsage | D3DUSAGE_DYNAMIC;
	}
	else {
		usage = D3DUSAGE_WRITEONLY;
	}
	return p_renderer->m_device->CreateVertexBuffer(
			   p_storage->m_capacity,
			   usage,
			   p_storage->m_format,
			   D3DPOOL_DEFAULT,
			   &p_storage->m_deviceBuffer
		   ) == D3D_OK
			   ? TRUE
			   : FALSE;
}

static void UploadVertexStorage(DisplayRenderer*, RendererVertexStorage* p_storage)
{
	BYTE* mapped = p_storage->Lock(0, p_storage->m_nextVertex, 1, 0);
	if (mapped != NULL) {
		memcpy(mapped, p_storage->m_base, p_storage->m_stride * p_storage->m_nextVertex);
		p_storage->Unlock();
	}
}

static LegoBool32 EnsureIndexDeviceBuffer(DisplayRenderer* p_renderer, RendererIndexBuffer* p_buffer)
{
	if (p_buffer->m_deviceBuffer == NULL) {
		p_renderer->m_device->CreateIndexBuffer(
			p_buffer->m_count * 2,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_DEFAULT,
			&p_buffer->m_deviceBuffer
		);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004db9c0
void DisplayRenderer::ReleaseDeviceResources()
{
	for (LegoS32 releaseTextureIndex = 0; releaseTextureIndex < sizeOfArray(m_textureRecords); releaseTextureIndex++) {
		RendererTextureRecord& record = m_textureRecords[releaseTextureIndex];
		if ((record.m_flags0x06 & 1) != 0 && record.m_texture != NULL) {
			record.m_texture->Release();
			record.m_texture = NULL;
		}
	}

	for (LegoS32 releaseStorageIndex = 0; releaseStorageIndex < sizeOfArray(m_vertexStorage); releaseStorageIndex++) {
		RendererVertexStorage& storage = m_vertexStorage[releaseStorageIndex];
		if ((storage.m_flags & 1) != 0) {
			storage.ReleaseDeviceBuffer();
		}
	}

	for (LegoS32 releaseIndexBuffer = 0; releaseIndexBuffer < sizeOfArray(m_indexRecords); releaseIndexBuffer++) {
		RendererIndexBuffer& record = m_indexRecords[releaseIndexBuffer];
		if ((record.m_flags & 1) != 0) {
			record.ReleaseDeviceBuffer();
			record.m_flags &= ~4;
		}
	}

	for (LegoS32 releaseVertexIndex = 0; releaseVertexIndex < sizeOfArray(m_vertexRecords); releaseVertexIndex++) {
		m_vertexRecords[releaseVertexIndex].m_flags &= ~2;
	}

	if (m_normalVertexBuffer != NULL) {
		m_normalVertexBuffer->Release();
		m_normalVertexBuffer = NULL;
	}
	if (m_transformedVertexBuffer != NULL) {
		m_transformedVertexBuffer->Release();
		m_transformedVertexBuffer = NULL;
	}
	if (m_specularVertexBuffer != NULL) {
		m_specularVertexBuffer->Release();
		m_specularVertexBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004dc000
LegoBool32 DisplayRenderer::RestoreDeviceResources()
{
	for (LegoS32 restoreFirstTexture = 0; restoreFirstTexture < sizeOfArray(m_textureRecords); restoreFirstTexture++) {
		RendererTextureRecord& record = m_textureRecords[restoreFirstTexture];
		if ((record.m_flags0x06 & 1) != 0) {
			if ((record.m_flags0x9c & 4) != 0) {
				if (CreateSecondaryTexture(&record) == FALSE) {
					return FALSE;
				}
			}
			else if ((record.m_flags0x9c & 1) != 0) {
				if (CreateSecondaryTexture(&record) == FALSE) {
					return FALSE;
				}
				record.UpdateGpuTexture();
			}
		}
	}

	for (LegoS32 restoreDynamicStorage = 0; restoreDynamicStorage < sizeOfArray(m_vertexStorage);
		 restoreDynamicStorage++) {
		RendererVertexStorage& storage = m_vertexStorage[restoreDynamicStorage];
		if ((storage.m_flags & 3) == 3) {
			if (EnsureVertexStorageDeviceBuffer(this, &storage) == FALSE) {
				return FALSE;
			}
			UploadVertexStorage(this, &storage);
		}
	}

	for (LegoS32 restoreDynamicIndex = 0; restoreDynamicIndex < sizeOfArray(m_indexRecords); restoreDynamicIndex++) {
		RendererIndexBuffer& record = m_indexRecords[restoreDynamicIndex];
		if ((record.m_flags & 0x11) == 0x11) {
			if (EnsureIndexDeviceBuffer(this, &record) == FALSE) {
				return FALSE;
			}
			record.Upload();
			record.m_flags = (record.m_flags & 2) << 1 | (record.m_flags & ~4);
		}
	}

	for (LegoS32 restoreStaticTexture = 0; restoreStaticTexture < sizeOfArray(m_textureRecords);
		 restoreStaticTexture++) {
		RendererTextureRecord& record = m_textureRecords[restoreStaticTexture];
		if ((record.m_flags0x06 & 1) != 0 && (record.m_flags0x9c & 5) == 0) {
			if (CreateSecondaryTexture(&record) == FALSE) {
				return FALSE;
			}
			record.UpdateGpuTexture();
		}
	}

	for (LegoS32 restoreStaticStorage = 0; restoreStaticStorage < sizeOfArray(m_vertexStorage);
		 restoreStaticStorage++) {
		RendererVertexStorage& storage = m_vertexStorage[restoreStaticStorage];
		if ((storage.m_flags & 1) != 0 && (storage.m_flags & 2) == 0) {
			if (EnsureVertexStorageDeviceBuffer(this, &storage) == FALSE) {
				return FALSE;
			}
			UploadVertexStorage(this, &storage);
		}
	}

	for (LegoS32 restoreStaticIndex = 0; restoreStaticIndex < sizeOfArray(m_indexRecords); restoreStaticIndex++) {
		RendererIndexBuffer& record = m_indexRecords[restoreStaticIndex];
		if ((record.m_flags & 1) != 0 && (record.m_flags & 0x10) == 0) {
			if (EnsureIndexDeviceBuffer(this, &record) == FALSE) {
				return FALSE;
			}
			record.Upload();
			record.m_flags = (record.m_flags & 2) << 1 | (record.m_flags & ~4);
		}
	}

	for (LegoS32 restoreVertexIndex = 0; restoreVertexIndex < sizeOfArray(m_vertexRecords); restoreVertexIndex++) {
		RendererVertexBuffer& record = m_vertexRecords[restoreVertexIndex];
		record.m_flags = (record.m_flags & 1) << 1 | (record.m_flags & ~2);
	}
	return TRUE;
}
