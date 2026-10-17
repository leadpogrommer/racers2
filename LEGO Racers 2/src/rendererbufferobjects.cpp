#include "rendererbufferobjects.h"

#include "application.h"
#include "displayconfig.h"
#include "memoryheap.h"

#include <string.h>

// Offset view of the retail vertex-buffer table. The complete table has 44
// entries; this view is used for calls only and is never emitted as a vtable.
struct VertexBufferDispatchView {
	typedef void(__fastcall* Policy)(RendererVertexBuffer*, void*, LegoU32);
	typedef LegoU32(__fastcall* Value)(RendererVertexBuffer*, void*);
	typedef LegoU8*(__fastcall* Vertex)(RendererVertexBuffer*, void*, LegoS32);
	const void* m_slots0x00[7];
	Policy m_setPolicy; // 0x1c
	const void* m_slots0x20[3];
	Value m_getStride; // 0x2c
	Value m_getCount;  // 0x30
	const void* m_slot0x34;
	Vertex m_getVertex; // 0x38
};
DECOMP_STATIC_ASSERT(offsetof(VertexBufferDispatchView, m_setPolicy) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferDispatchView, m_getStride) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferDispatchView, m_getVertex) == 0x38);

// FUNCTION: LEGORACERS2 0x004ab860
void RendererIndexBuffer::Release()
{
	if (m_storage != NULL) {
		m_storage->ReleaseReference();
		m_storage = NULL;
	}
	ReleaseDeviceBuffer();
	m_flags &= ~1;
}

// FUNCTION: LEGORACERS2 0x004ab890
LegoBool32 RendererIndexBuffer::Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode)
{
	if (p_count == 0) {
		p_count = m_count - p_start;
	}
	m_lockStart = p_start;
	m_lockCount = p_count;
	if ((m_flags & 4) == 4) {
		m_savedData = m_data;
		BYTE* mapped;
		// Retail passes the start directly as a byte offset, then adjusts the
		// returned base by twice that start. Preserve that observed convention.
		if (m_deviceBuffer->Lock(p_start, p_count * 2, &mapped, 0) != D3D_OK) {
			return FALSE;
		}
		m_data = mapped - m_lockStart * 2;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004ab900
void RendererIndexBuffer::Unlock(LegoU32 p_commit, LegoU32 p_mode)
{
	if (p_commit == 1) {
		LegoS32 end = m_lockCount + m_lockStart;
		if (((LegoS32) (m_writtenEnd - m_data) >> 1) < end) {
			m_writtenEnd = m_data + end * 2;
		}
	}
	if ((m_flags & 4) == 4) {
		m_deviceBuffer->Unlock();
		m_data = m_savedData;
		return;
	}
	if (p_commit == 1 && m_deviceBuffer != NULL) {
		BYTE* mapped;
		if (m_deviceBuffer->Lock(m_lockStart, m_lockCount << 1, &mapped, 0) == D3D_OK) {
			memcpy(mapped, m_data + m_lockStart * 2, (LegoU32) m_lockCount << 1);
			m_deviceBuffer->Unlock();
		}
	}
}

// FUNCTION: LEGORACERS2 0x004ab9c0
void RendererIndexBuffer::SetFlag4(LegoU32 p_enabled)
{
	m_flags = ((p_enabled & 1) << 4) | (m_flags & ~0x10);
}

// Shared source factoring of the layout writes in 4e9560 and 4e96a0.
// These helpers have no retail address annotations.
void RendererVertexBuffer::ConfigureLayout(
	LegoU32 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode
)
{
	m_stride = 0;
	m_count = (LegoU16) p_count;
	m_textureCount = p_textureCount;
	m_attributes = p_attributes;
	if ((p_attributes & 1) != 0) {
		m_positionOffset = 0;
		m_stride = 12;
	}
	m_stride = (m_stride + 3) & ~3;
	if ((p_attributes & 2) != 0) {
		m_normalOffset = m_stride;
		m_stride += 12;
	}
	m_stride = (m_stride + 3) & ~3;
	if ((p_attributes & 0x14) != 0) {
		m_colorOffset = m_stride;
		m_stride += 4;
	}
	m_stride = (m_stride + 3) & ~3;
	if ((p_attributes & 8) != 0 && p_textureCount > 0) {
		m_textureOffset = m_stride;
		m_stride += p_textureCount * 8;
	}
	m_stride = (m_stride + 3) & ~3;
	m_unk0x04 = p_mode;
}

void RendererVertexBuffer::AttachStorage(RendererVertexStorage* p_storage, LegoS32 p_count)
{
	m_storage = p_storage;
	m_storage->Reserve(p_count, &m_baseVertex, &m_data);
	m_generation = 0;
	const VertexBufferDispatchView* dispatch = (const VertexBufferDispatchView*) m_vtable;
	dispatch->m_setPolicy(this, NULL, 0);
}

// STUB: LEGORACERS2 0x004e96a0
LegoBool32 RendererVertexBuffer::ConfigureFromStorage(
	RendererVertexStorage* p_storage,
	LegoU16 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode
)
{
	ConfigureLayout(p_attributes, p_count, p_textureCount, p_mode);
	AttachStorage(p_storage, p_count);
	return TRUE;
}

// STUB: LEGORACERS2 0x004e9770
LegoBool32 RendererVertexBuffer::Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode, LegoU32 p_discard)
{
	if ((m_flags & 0x30) != 0) {
		return FALSE;
	}
	const VertexBufferDispatchView* dispatch = (const VertexBufferDispatchView*) m_vtable;
	if (p_count == 0) {
		p_count = dispatch->m_getCount(this, NULL) - p_start;
	}
	m_lockStart = p_start;
	m_lockCount = p_count;
	m_flags = ((p_mode == 1 ? 2 : 1) << 4) | (m_flags & ~0x30);
	if ((m_flags & 2) == 2) {
		m_savedData = m_data;
		m_savedStride = dispatch->m_getStride(this, NULL);
		m_data = m_storage->Lock(m_baseVertex + p_start, p_count, p_mode, p_discard);
		m_stride = m_storage->m_stride;
		// Retail adjusts the result even when the storage lock returned NULL.
		m_data -= m_stride * p_start;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004e9810
void RendererVertexBuffer::Unlock(LegoU32 p_commit)
{
	if ((m_flags & 0x30) == 0) {
		return;
	}
	if ((m_flags & 2) == 2) {
		m_storage->Unlock();
		m_data = m_savedData;
		m_stride = m_savedStride;
		m_flags &= ~0x30;
		return;
	}
	if (p_commit == 1 && m_storage->m_deviceBuffer != NULL) {
		BYTE* mapped = m_storage->Lock(m_baseVertex + m_lockStart, m_lockCount, 1, 0);
		if (mapped != NULL) {
			const VertexBufferDispatchView* dispatch = (const VertexBufferDispatchView*) m_vtable;
			if (dispatch->m_getStride(this, NULL) == (LegoU32) m_storage->m_stride) {
				LegoU32 bytes = dispatch->m_getStride(this, NULL) * m_lockCount;
				memcpy(mapped, dispatch->m_getVertex(this, NULL, m_lockStart), bytes);
			}
			else {
				BYTE* source = dispatch->m_getVertex(this, NULL, m_lockStart);
				LegoU32 sourceStride = dispatch->m_getStride(this, NULL);
				LegoS32 destinationStride = m_storage->m_stride;
				for (LegoS32 i = 0; i < m_lockCount; i++) {
					memcpy(mapped, source, sourceStride);
					source += sourceStride;
					mapped += destinationStride;
				}
			}
			m_storage->Unlock();
		}
		// The original increments the generation even if mapping fails.
		m_generation++;
	}
	m_flags &= ~0x30;
}

// FUNCTION: LEGORACERS2 0x004e9f20
void RendererIndexBuffer::ReleaseDeviceBuffer()
{
	if (m_deviceBuffer != NULL) {
		m_deviceBuffer->Release();
		m_deviceBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004e9f40
void RendererIndexBuffer::Upload()
{
	BYTE* mapped;
	if (m_deviceBuffer->Lock(0, m_count * 2, &mapped, 0) == D3D_OK) {
		memcpy(mapped, m_data, (LegoU32) m_count * 2);
		m_deviceBuffer->Unlock();
	}
}

// FUNCTION: LEGORACERS2 0x004e9fd0
LegoU32 WINAPI GetVertexFormat(LegoU8 p_attributes, LegoU32 p_textureCount)
{
	LegoU32 format = p_textureCount << 8;
	if ((p_attributes & 1) != 0) {
		format |= 2;
	}
	if ((p_attributes & 2) != 0) {
		format |= 0x10;
	}
	if ((p_attributes & 0x14) != 0) {
		format |= 0x40;
	}
	return format;
}

// FUNCTION: LEGORACERS2 0x004ea000
LegoS32 WINAPI GetVertexFormatStride(LegoU32 p_format)
{
	LegoS32 bytes = 0;
	if ((p_format & 2) != 0) {
		bytes = 12;
	}
	if ((p_format & 0x10) != 0) {
		bytes += 12;
	}
	if ((p_format & 0x40) != 0) {
		bytes += 4;
	}
	// Retail tests these masks in this order; they are not equality checks.
	if ((p_format & 0x100) != 0) {
		return bytes + 8;
	}
	if ((p_format & 0x200) != 0) {
		return bytes + 16;
	}
	if ((p_format & 0x300) != 0) {
		return bytes + 24;
	}
	if ((p_format & 0x400) != 0) {
		return bytes + 32;
	}
	if ((p_format & 0x500) != 0) {
		return bytes + 40;
	}
	if ((p_format & 0x600) != 0) {
		return bytes + 48;
	}
	if ((p_format & 0x700) != 0) {
		return bytes + 56;
	}
	if ((p_format & 0x800) != 0) {
		bytes += 64;
	}
	return bytes;
}

// STUB: LEGORACERS2 0x004ea080
LegoBool32 RendererVertexStorage::Initialize(
	LegoU32 p_attributes,
	LegoS32 p_count,
	LegoS32 p_textureCount,
	LegoU32 p_mode
)
{
	m_flags |= 1;
	m_references = 0;
	m_format = GetVertexFormat((LegoU8) p_attributes, p_textureCount);
	m_stride = GetVertexFormatStride(m_format);
	m_flags = ((p_mode & 1) << 1) | (m_flags & ~2);
	LegoS32 bytes = m_stride * p_count;
	LegoS32 minimum = g_displayConfiguration->m_largeVBs == 1 ? 0x40000 : 0x10000;
	if (bytes <= minimum) {
		bytes = minimum;
	}
	m_capacity = bytes;
	m_available = bytes;
	m_nextVertex = 0;
	Application* application = GetApplication();
	application->m_vtable->m_action0x58(application, NULL);
	m_base = (LegoU8*) GetMemoryHeap()->Allocate(m_capacity, "Local copy of vertex buffer");
	return TRUE;
}

// STUB: LEGORACERS2 0x004ea1e0
void RendererVertexStorage::Reserve(LegoS32 p_vertices, LegoS32* p_baseVertex, LegoU8** p_data)
{
	*p_baseVertex = m_nextVertex;
	*p_data = m_base + m_nextVertex * m_stride;
	m_nextVertex += p_vertices;
	m_available -= p_vertices * m_stride;
	m_references++;
}

// STUB: LEGORACERS2 0x004ea220
BYTE* RendererVertexStorage::Lock(LegoS32 p_start, LegoS32 p_count, LegoU32 p_mode, LegoU32 p_discard)
{
	if ((m_flags & 0xc) != 0) {
		return NULL;
	}
	m_flags = ((p_mode == 1 ? 2 : 1) << 2) | (m_flags & ~0xc);
	DWORD flags = (p_discard == 1 && p_mode == 1) ? D3DLOCK_DISCARD : 0;
	BYTE* mapped;
	if (m_deviceBuffer->Lock(m_stride * p_start, m_stride * p_count, &mapped, flags) == D3D_OK) {
		return mapped;
	}
	// Retail retries without DISCARD and leaves the lock-state bits set even
	// if both calls fail. Its callers handle that state through Unlock().
	flags &= ~D3DLOCK_DISCARD;
	if (m_deviceBuffer->Lock(m_stride * p_start, m_stride * p_count, &mapped, flags) == D3D_OK) {
		return mapped;
	}
	return NULL;
}

// FUNCTION: LEGORACERS2 0x004ea2d0
void RendererVertexStorage::Unlock()
{
	if ((m_flags & 0xc) != 0) {
		m_deviceBuffer->Unlock();
		m_flags &= ~0xc;
	}
}

// STUB: LEGORACERS2 0x004ea2f0
void RendererVertexStorage::ReleaseReference()
{
	if (m_deviceBuffer != NULL && --m_references < 1) {
		ReleaseDeviceBuffer();
		Application* application = GetApplication();
		application->m_vtable->m_action0x58(application, NULL);
		GetMemoryHeap()->Free(m_base);
		m_base = NULL;
		m_flags &= ~1;
	}
}

// FUNCTION: LEGORACERS2 0x004ea340
void RendererVertexStorage::ReleaseDeviceBuffer()
{
	if (m_deviceBuffer != NULL) {
		m_deviceBuffer->Release();
		m_deviceBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004ea360
LegoBool32 RendererIndexStorage::Initialize(LegoU32 p_bytes)
{
	m_references = 0;
	m_available = p_bytes;
	m_capacity = p_bytes;
	Application* application = GetApplication();
	application->m_vtable->m_action0x58(application, NULL);
	m_base = (LegoU8*) GetMemoryHeap()->Allocate(p_bytes, "D3D buffer allocation");
	m_cursor = m_base;
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004ea3b0
void RendererIndexStorage::Reserve(LegoU32 p_bytes, LegoU8** p_result)
{
	*p_result = m_cursor;
	m_cursor += p_bytes;
	m_available -= p_bytes;
	m_references++;
}

// STUB: LEGORACERS2 0x004ea3e0
void RendererIndexStorage::ReleaseReference()
{
	if (m_base != NULL && --m_references < 1) {
		Application* application = GetApplication();
		application->m_vtable->m_action0x58(application, NULL);
		GetMemoryHeap()->Free(m_base);
		m_base = NULL;
	}
}
