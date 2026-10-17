#include "application.h"
#include "renderer.h"

// These helpers replace retail's reads of fields in the renderer singleton
// (4ea130 and 4e9ef0). Pass the evidenced owning renderer instead, preserving
// its actual device and usage policy without inventing separate globals.
static LegoBool32 CreateVertexDeviceBuffer(DisplayRenderer* p_renderer, RendererVertexStorage* p_storage)
{
	DWORD usage =
		(p_storage->m_flags & 2) == 2 ? p_renderer->m_vertexBufferUsage | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY;
	if (p_storage->m_deviceBuffer != NULL) {
		return TRUE;
	}
	return p_renderer->m_device->CreateVertexBuffer(
			   p_storage->m_capacity,
			   usage,
			   p_storage->m_format,
			   D3DPOOL_DEFAULT,
			   &p_storage->m_deviceBuffer
		   ) == D3D_OK;
}

static void CreateIndexDeviceBuffer(DisplayRenderer* p_renderer, RendererIndexBuffer* p_buffer)
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
}

// STUB: LEGORACERS2 0x004e9420
LegoBool32 DisplayRenderer::TryReuseVertexStorage(
	LegoU32 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode,
	RendererVertexBuffer* p_buffer
)
{
	LegoU32 format = GetVertexFormat((LegoU8) p_attributes, p_textureCount);
	LegoS32 bytes = GetVertexFormatStride(format) * p_count;
	for (LegoS32 i = 0; i < 128; i++) {
		RendererVertexStorage* storage = &m_vertexStorage[i];
		if ((storage->m_flags & 1) != 0 && storage->m_available >= bytes && p_mode == ((storage->m_flags >> 1) & 1) &&
			storage->m_format == format) {
			p_buffer->ConfigureFromStorage(storage, (LegoU16) p_attributes, p_count, p_textureCount, p_mode);
			return TRUE;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004e94b0
RendererVertexBuffer* DisplayRenderer::CreateVertexBuffer(
	LegoU32 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode
)
{
	Application* application = GetApplication();
	if (application->m_vtable->m_developmentMode(application, NULL) != 0) {
		p_mode = 1;
	}
	for (LegoS32 i = 0; i < 5000; i++) {
		RendererVertexBuffer* buffer = &m_vertexRecords[i];
		if (buffer->m_storage == NULL) {
			if (!TryReuseVertexStorage(p_attributes, p_count, p_textureCount, p_mode, buffer)) {
				// The original 4e9560 sets the layout before the pool allocator's
				// application callback, then attaches and reserves the new storage.
				buffer->ConfigureLayout(p_attributes, p_count, p_textureCount, p_mode);
				RendererVertexStorage* storage = AllocateVertexStorage(p_attributes, p_count, p_textureCount, p_mode);
				buffer->AttachStorage(storage, p_count);
			}
			return buffer;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004e9640
RendererVertexStorage* DisplayRenderer::AllocateVertexStorage(
	LegoU32 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode
)
{
	LegoS32 i;
	for (i = 0; i < 128; i++) {
		if ((m_vertexStorage[i].m_flags & 1) == 0) {
			break;
		}
	}
	// The retail routine does not handle a full storage array here. Preserve
	// its selected address; the exhaustion case remains an unresolved limit.
	RendererVertexStorage* storage = &m_vertexStorage[i];
	storage->Initialize(p_attributes, p_count, p_textureCount, p_mode);
	if (m_createDeviceBuffers == 1) {
		CreateVertexDeviceBuffer(this, storage);
	}
	return storage;
}

// STUB: LEGORACERS2 0x004e9e10
RendererIndexBuffer* DisplayRenderer::CreateIndexBuffer(LegoU32 p_kind, LegoS32 p_count)
{
	LegoS32 i;
	RendererIndexStorage* storage = NULL;
	for (i = 0; i < 64; i++) {
		if (m_indexStorage[i].m_base != NULL && p_count * 2 <= (LegoS32) m_indexStorage[i].m_available) {
			storage = &m_indexStorage[i];
			break;
		}
	}
	if (storage == NULL) {
		for (i = 0; i < 64; i++) {
			if (m_indexStorage[i].m_base == NULL) {
				storage = &m_indexStorage[i];
				storage->Initialize(0x80000);
				break;
			}
		}
		if (storage == NULL) {
			return NULL;
		}
	}
	LegoU8* data;
	storage->Reserve(p_count * 2, &data);
	for (i = 0; i < 5000; i++) {
		RendererIndexBuffer* buffer = &m_indexRecords[i];
		if ((buffer->m_flags & 1) == 0) {
			buffer->m_unk0x0c = p_kind;
			buffer->m_count = p_count;
			buffer->m_writtenEnd = data;
			buffer->m_data = data;
			buffer->m_deviceBuffer = NULL;
			buffer->m_storage = storage;
			buffer->m_flags = (buffer->m_flags & ~6) | 1;
			if (m_createDeviceBuffers == 1) {
				CreateIndexDeviceBuffer(this, buffer);
			}
			return buffer;
		}
	}
	return NULL;
}
