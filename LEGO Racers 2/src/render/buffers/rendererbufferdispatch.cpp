#include "render/buffers/rendererbufferdispatch.h"

#include "memory/memoryheap.h"
#include "render/core/renderer.h"

extern "C" int __cdecl _purecall(void);

// The original callback at 0x004ab560 conditionally frees the DWORD at +0x10
// only when the DWORD at +0x08 equals one. Its unusual shared-table role and
// field meaning are not inferred as an ordinary buffer destructor.
// STUB: LEGORACERS2 0x004ab560
void __fastcall RendererVertexBufferLegacyConditionalFree(RendererVertexBuffer* p_buffer, void*)
{
	if (p_buffer->m_positionOffset == 1 && p_buffer->m_colorOffset != 0) {
		GetMemoryHeap()->Free((void*) p_buffer->m_colorOffset);
		p_buffer->m_colorOffset = 0;
	}
}

// STUB: LEGORACERS2 0x004abde0
RendererVertexBuffer::RendererVertexBuffer()
{
	m_unk0x24 = 0;
	m_count = 0;
	m_textureCount = 1;
	m_attributes = 0x0b;
	m_positionOffset = 0;
	m_normalOffset = 12;
	m_textureOffset = 24;
	m_stride = 32;
	m_data = NULL;
	m_generation = 0;
	m_storage = NULL;
	m_vtable = &g_rendererVertexBufferDispatch;
	m_flags &= 0xffffffcc;
}

// STUB: LEGORACERS2 0x004abe30
RendererIndexBuffer::RendererIndexBuffer()
{
	m_unk0x08 = 0;
	m_deviceBuffer = NULL;
	m_storage = NULL;
	m_vtable = &g_rendererIndexBufferDispatch;
	m_flags &= 0xffffff98;
}

// STUB: LEGORACERS2 0x004e9d80
void RendererVertexBuffer::SetBufferPolicy(LegoU32 p_enabled)
{
	LegoU32 oldFlags = m_flags & 0xfffffffc;
	LegoU32 newFlags = oldFlags ^ (p_enabled & 1);
	newFlags |= (p_enabled & 1) << 1;
	m_flags = newFlags;
	if (GetStaticDisplayRenderer()->m_bufferStoragePolicy == 0) {
		m_flags = (oldFlags & 0xffffff00) | (newFlags & 0xff);
		m_flags &= 0xfffffffd;
	}
}

// STUB: LEGORACERS2 0x004e9fa0
void RendererIndexBuffer::SetBufferPolicy(LegoU32 p_enabled)
{
	LegoU32 oldFlags = m_flags & 0xfffffff9;
	LegoU32 requested = p_enabled & 1;
	LegoU32 newFlags = ((requested | (requested * 2)) << 1) | oldFlags;
	m_flags = newFlags;
	if (GetStaticDisplayRenderer()->m_bufferStoragePolicy == 0) {
		m_flags = (oldFlags & 0xffffff00) | (newFlags & 0xff);
		m_flags &= 0xfffffffb;
	}
}

void __fastcall RendererVertexBufferDispatchRelease(RendererVertexBuffer* p_buffer, void*)
{
	p_buffer->Release();
}

LegoBool32 __fastcall RendererVertexBufferDispatchLock(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_start,
	LegoS32 p_count,
	LegoU32 p_mode,
	LegoU32 p_discard
)
{
	return p_buffer->Lock(p_start, p_count, p_mode, p_discard);
}

void __fastcall RendererVertexBufferDispatchUnlock(RendererVertexBuffer* p_buffer, void*, LegoU32 p_commit)
{
	p_buffer->Unlock(p_commit);
}

void __fastcall RendererVertexBufferDispatchCopyVertices(
	RendererVertexBuffer* p_buffer,
	void*,
	RendererVertexBuffer* p_source,
	LegoS32 p_start
)
{
	p_buffer->CopyVerticesFrom(p_source, p_start);
}

void __fastcall RendererVertexBufferDispatchCopyFromData(
	RendererVertexBuffer* p_buffer,
	void*,
	RendererVertexDataView* p_source,
	LegoS32 p_start
)
{
	p_buffer->CopyFromVertexData(p_source, p_start);
}

void __fastcall RendererVertexBufferDispatchCopyToData(
	RendererVertexBuffer* p_buffer,
	void*,
	RendererVertexDataView* p_target
)
{
	p_buffer->CopyToVertexData(p_target);
}

void __fastcall RendererVertexBufferDispatchSetMode(RendererVertexBuffer* p_buffer, void*, LegoU32 p_mode)
{
	p_buffer->SetMode((LegoS32) p_mode);
}

void __fastcall RendererVertexBufferDispatchSetPolicy(RendererVertexBuffer* p_buffer, void*, LegoU32 p_policy)
{
	p_buffer->SetBufferPolicy(p_policy);
}

IDirect3DVertexBuffer8* __fastcall RendererVertexBufferDispatchGetDeviceBuffer(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetDeviceBuffer();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetAttributes(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetAttributes();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetStride(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetStride();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetCount(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetCount();
}

LegoS32 __fastcall RendererVertexBufferDispatchGetTextureCount(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetTextureCount();
}

LegoU8* __fastcall RendererVertexBufferDispatchGetVertex(RendererVertexBuffer* p_buffer, void*, LegoS32 p_index)
{
	return p_buffer->GetVertex(p_index);
}

LegoU32 __fastcall RendererVertexBufferDispatchGetPositionOffset(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetPositionOffset();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetNormalOffset(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetNormalOffset();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetColorOffset(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetColorOffset();
}

LegoU32 __fastcall RendererVertexBufferDispatchGetTextureOffset(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetTextureOffset();
}

void __fastcall RendererVertexBufferDispatchSetCursor(RendererVertexBuffer* p_buffer, void*, LegoS32 p_index)
{
	p_buffer->SetCursor(p_index);
}

void __fastcall RendererVertexBufferDispatchAdvanceCursor(RendererVertexBuffer* p_buffer, void*)
{
	p_buffer->AdvanceCursor();
}

Vector3* __fastcall RendererVertexBufferDispatchGetPosition(RendererVertexBuffer* p_buffer, void*, LegoS32 p_index)
{
	return p_buffer->GetPosition(p_index);
}

Vector3* __fastcall RendererVertexBufferDispatchGetCurrentPosition(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetCurrentPosition();
}

Vector3* __fastcall RendererVertexBufferDispatchGetNormal(RendererVertexBuffer* p_buffer, void*, LegoS32 p_index)
{
	return p_buffer->GetNormal(p_index);
}

Vector3* __fastcall RendererVertexBufferDispatchGetCurrentNormal(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetCurrentNormal();
}

LegoU32* __fastcall RendererVertexBufferDispatchGetColor(RendererVertexBuffer* p_buffer, void*, LegoS32 p_index)
{
	return p_buffer->GetColor(p_index);
}

LegoU32* __fastcall RendererVertexBufferDispatchGetCurrentColor(RendererVertexBuffer* p_buffer, void*)
{
	return p_buffer->GetCurrentColor();
}

RendererTextureCoordinate* __fastcall RendererVertexBufferDispatchGetTextureCoordinate(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_index,
	LegoS32 p_stage
)
{
	return p_buffer->GetTextureCoordinate(p_index, p_stage);
}

RendererTextureCoordinate* __fastcall RendererVertexBufferDispatchGetCurrentTextureCoordinate(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_stage
)
{
	return p_buffer->GetCurrentTextureCoordinate(p_stage);
}

void __fastcall RendererVertexBufferDispatchSetPosition(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_index,
	const Vector3* p_value
)
{
	p_buffer->SetPosition(p_index, p_value);
}

void __fastcall RendererVertexBufferDispatchSetCurrentPosition(
	RendererVertexBuffer* p_buffer,
	void*,
	const Vector3* p_value
)
{
	p_buffer->SetCurrentPosition(p_value);
}

void __fastcall RendererVertexBufferDispatchSetNormal(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_index,
	const Vector3* p_value
)
{
	p_buffer->SetNormal(p_index, p_value);
}

void __fastcall RendererVertexBufferDispatchSetCurrentNormal(
	RendererVertexBuffer* p_buffer,
	void*,
	const Vector3* p_value
)
{
	p_buffer->SetCurrentNormal(p_value);
}

void __fastcall RendererVertexBufferDispatchSetColor(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_index,
	const LegoU32* p_value
)
{
	p_buffer->SetColor(p_index, p_value);
}

void __fastcall RendererVertexBufferDispatchSetCurrentColor(
	RendererVertexBuffer* p_buffer,
	void*,
	const LegoU32* p_value
)
{
	p_buffer->SetCurrentColor(p_value);
}

void __fastcall RendererVertexBufferDispatchSetTextureCoordinate(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_index,
	LegoS32 p_stage,
	const RendererTextureCoordinate* p_value
)
{
	p_buffer->SetTextureCoordinate(p_index, p_stage, p_value);
}

void __fastcall RendererVertexBufferDispatchSetCurrentTextureCoordinate(
	RendererVertexBuffer* p_buffer,
	void*,
	LegoS32 p_stage,
	const RendererTextureCoordinate* p_value
)
{
	p_buffer->SetCurrentTextureCoordinate(p_stage, p_value);
}

void __fastcall RendererIndexBufferDispatchRelease(RendererIndexBuffer* p_buffer, void*)
{
	p_buffer->Release();
}

LegoBool32 __fastcall RendererIndexBufferDispatchLock(
	RendererIndexBuffer* p_buffer,
	void*,
	LegoS32 p_start,
	LegoS32 p_count,
	LegoU32 p_mode
)
{
	return p_buffer->Lock(p_start, p_count, p_mode);
}

void __fastcall RendererIndexBufferDispatchUnlock(
	RendererIndexBuffer* p_buffer,
	void*,
	LegoU32 p_commit,
	LegoU32 p_mode
)
{
	p_buffer->Unlock(p_commit, p_mode);
}

void __fastcall RendererIndexBufferDispatchSetFlag4(RendererIndexBuffer* p_buffer, void*, LegoU32 p_enabled)
{
	p_buffer->SetFlag4(p_enabled);
}

void __fastcall RendererIndexBufferDispatchSetPolicy(RendererIndexBuffer* p_buffer, void*, LegoU32 p_policy)
{
	p_buffer->SetBufferPolicy(p_policy);
}

IDirect3DIndexBuffer8* __fastcall RendererIndexBufferDispatchGetDeviceBuffer(RendererIndexBuffer* p_buffer, void*)
{
	return p_buffer->GetDeviceBuffer();
}

const RendererVertexBufferDispatch g_rendererVertexBufferDispatch = {
	RendererVertexBufferDispatchRelease,
	RendererVertexBufferDispatchLock,
	RendererVertexBufferDispatchUnlock,
	RendererVertexBufferDispatchCopyVertices,
	RendererVertexBufferDispatchCopyFromData,
	RendererVertexBufferDispatchCopyToData,
	RendererVertexBufferDispatchSetMode,
	RendererVertexBufferDispatchSetPolicy,
	_purecall,
	RendererVertexBufferDispatchGetDeviceBuffer,
	RendererVertexBufferDispatchGetAttributes,
	RendererVertexBufferDispatchGetStride,
	RendererVertexBufferDispatchGetCount,
	RendererVertexBufferDispatchGetTextureCount,
	RendererVertexBufferDispatchGetVertex,
	RendererVertexBufferDispatchGetPositionOffset,
	RendererVertexBufferDispatchGetNormalOffset,
	RendererVertexBufferDispatchGetColorOffset,
	RendererVertexBufferDispatchGetTextureOffset,
	RendererVertexBufferDispatchSetCursor,
	RendererVertexBufferDispatchAdvanceCursor,
	RendererVertexBufferDispatchGetPosition,
	RendererVertexBufferDispatchGetCurrentPosition,
	RendererVertexBufferDispatchGetNormal,
	RendererVertexBufferDispatchGetCurrentNormal,
	RendererVertexBufferDispatchGetColor,
	RendererVertexBufferDispatchGetCurrentColor,
	RendererVertexBufferDispatchGetTextureCoordinate,
	RendererVertexBufferDispatchGetCurrentTextureCoordinate,
	RendererVertexBufferDispatchSetPosition,
	RendererVertexBufferDispatchSetCurrentPosition,
	RendererVertexBufferDispatchSetNormal,
	RendererVertexBufferDispatchSetCurrentNormal,
	RendererVertexBufferDispatchSetColor,
	RendererVertexBufferDispatchSetCurrentColor,
	RendererVertexBufferDispatchSetTextureCoordinate,
	RendererVertexBufferDispatchSetCurrentTextureCoordinate,
	RendererVertexBufferLegacyConditionalFree,
	_purecall,
	_purecall,
	_purecall,
	_purecall,
	_purecall,
	_purecall,
};

const RendererIndexBufferDispatch g_rendererIndexBufferDispatch = {
	RendererIndexBufferDispatchRelease,
	RendererIndexBufferDispatchLock,
	RendererIndexBufferDispatchUnlock,
	RendererIndexBufferDispatchSetFlag4,
	RendererIndexBufferDispatchSetPolicy,
	_purecall,
	RendererIndexBufferDispatchGetDeviceBuffer,
};
