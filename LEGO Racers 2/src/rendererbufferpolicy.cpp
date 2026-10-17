#include "rendererbufferobjects.h"
#include "renderervertexdata.h"
#include "vector3.h"

#include <string.h>

struct VertexBufferCopyDispatch {
	typedef LegoBool32(__fastcall* Lock)(RendererVertexBuffer*, void*, LegoS32, LegoS32, LegoU32, LegoU32);
	typedef void(__fastcall* Unlock)(RendererVertexBuffer*, void*, LegoU32);
	typedef LegoU32(__fastcall* Value)(RendererVertexBuffer*, void*);
	typedef LegoS32(__fastcall* SignedValue)(RendererVertexBuffer*, void*);
	typedef LegoU8*(__fastcall* GetVertex)(RendererVertexBuffer*, void*, LegoS32);
	typedef void(__fastcall* SetVector)(RendererVertexBuffer*, void*, const Vector3*);
	typedef void(__fastcall* SetColor)(RendererVertexBuffer*, void*, const LegoU32*);
	typedef void(__fastcall* SetTexture)(RendererVertexBuffer*, void*, LegoS32, const RendererTextureCoordinate*);
	typedef const Vector3*(__fastcall* GetCurrentVector)(RendererVertexBuffer*, void*);
	typedef const LegoU32*(__fastcall* GetCurrentColor)(RendererVertexBuffer*, void*);
	typedef const RendererTextureCoordinate*(__fastcall* GetCurrentTexture)(RendererVertexBuffer*, void*, LegoS32);
	typedef void(__fastcall* AdvanceCursor)(RendererVertexBuffer*, void*);
	const void* m_slot0x00;
	Lock m_lock;     // 0x04
	Unlock m_unlock; // 0x08
	undefined m_unk0x0c[0x1c - 0x0c];
	void(__fastcall* m_setPolicy)(RendererVertexBuffer*, void*, LegoU32); // 0x1c
	undefined m_unk0x20[0x28 - 0x20];
	Value m_getAttributes;         // 0x28
	Value m_getStride;             // 0x2c
	Value m_getCount;              // 0x30
	SignedValue m_getTextureCount; // 0x34
	GetVertex m_getVertex;         // 0x38
	undefined m_unk0x3c[0x4c - 0x3c];
	typedef void(__fastcall* SetCursor)(RendererVertexBuffer*, void*, LegoS32);
	SetCursor m_setCursor;         // 0x4c
	AdvanceCursor m_advanceCursor; // 0x50
	undefined m_unk0x54[0x58 - 0x54];
	GetCurrentVector m_getCurrentPosition; // 0x58
	undefined m_unk0x5c[0x60 - 0x5c];
	GetCurrentVector m_getCurrentNormal; // 0x60
	undefined m_unk0x64[0x68 - 0x64];
	GetCurrentColor m_getCurrentColor; // 0x68
	undefined m_unk0x6c[0x70 - 0x6c];
	GetCurrentTexture m_getCurrentTexture; // 0x70
	undefined m_unk0x74[0x78 - 0x74];
	SetVector m_setCurrentPosition; // 0x78
	undefined m_unk0x7c[0x80 - 0x7c];
	SetVector m_setCurrentNormal; // 0x80
	undefined m_unk0x84[0x88 - 0x84];
	SetColor m_setCurrentColor; // 0x88
	undefined m_unk0x8c[0x90 - 0x8c];
	SetTexture m_setCurrentTexture; // 0x90
};

DECOMP_STATIC_ASSERT(offsetof(VertexBufferCopyDispatch, m_lock) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferCopyDispatch, m_getAttributes) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferCopyDispatch, m_advanceCursor) == 0x50);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferCopyDispatch, m_setCurrentTexture) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(VertexBufferCopyDispatch, m_setCursor) == 0x4c);

static const VertexBufferCopyDispatch* GetVertexBufferDispatch(const RendererVertexBuffer* p_buffer)
{
	return (const VertexBufferCopyDispatch*) p_buffer->m_vtable;
}

// STUB: LEGORACERS2 0x004e9960
void RendererVertexBuffer::CopyFromVertexData(RendererVertexDataView* p_source, LegoS32 p_start)
{
	const VertexBufferCopyDispatch* dispatch = GetVertexBufferDispatch(this);
	if (dispatch->m_lock(this, NULL, p_start, p_source->m_vertexCount, 1, 0) != 1) {
		return;
	}

	if ((LegoU32) p_source->m_attributes == dispatch->m_getAttributes(this, NULL) &&
		p_source->m_textureCount == dispatch->m_getTextureCount(this, NULL)) {
		LegoU32 byteCount = p_source->m_stride * p_source->m_vertexCount;
		memcpy(dispatch->m_getVertex(this, NULL, p_start), p_source->m_data, byteCount);
		dispatch->m_unlock(this, NULL, 1);
		return;
	}

	LegoU32 attributes = p_source->m_attributes & dispatch->m_getAttributes(this, NULL);
	LegoS32 textureCount = p_source->m_textureCount;
	LegoS32 destinationTextureCount = dispatch->m_getTextureCount(this, NULL);
	if (destinationTextureCount < textureCount) {
		textureCount = destinationTextureCount;
	}
	dispatch->m_setCursor(this, NULL, p_start);
	p_source->m_cursor = p_source->m_data;

	for (LegoS32 vertex = 0; vertex < (LegoS32) p_source->m_vertexCount; vertex++) {
		if ((attributes & 1) != 0) {
			dispatch
				->m_setCurrentPosition(this, NULL, (const Vector3*) (p_source->m_cursor + p_source->m_positionOffset));
		}
		if ((attributes & 2) != 0) {
			dispatch->m_setCurrentNormal(this, NULL, (const Vector3*) (p_source->m_cursor + p_source->m_normalOffset));
		}
		if ((attributes & 0x14) != 0) {
			dispatch->m_setCurrentColor(this, NULL, (const LegoU32*) (p_source->m_cursor + p_source->m_colorOffset));
		}
		if ((attributes & 8) != 0) {
			for (LegoS32 stage = 0; stage < textureCount; stage++) {
				const RendererTextureCoordinate* coordinate =
					(const RendererTextureCoordinate*) (p_source->m_cursor + p_source->m_textureOffset +
														stage * sizeof(RendererTextureCoordinate));
				dispatch->m_setCurrentTexture(this, NULL, stage, coordinate);
			}
		}
		dispatch->m_advanceCursor(this, NULL);
		p_source->m_cursor += p_source->m_stride;
	}
	dispatch->m_unlock(this, NULL, 1);
}

// STUB: LEGORACERS2 0x004e9b20
void RendererVertexBuffer::CopyVerticesFrom(RendererVertexBuffer* p_source, LegoS32 p_start)
{
	const VertexBufferCopyDispatch* destinationDispatch = GetVertexBufferDispatch(this);
	const VertexBufferCopyDispatch* sourceDispatch = GetVertexBufferDispatch(p_source);
	LegoS32 sourceCount = (LegoS32) sourceDispatch->m_getCount(p_source, NULL);
	if (destinationDispatch->m_lock(this, NULL, p_start, sourceCount, 1, 0) != 1) {
		return;
	}

	if (sourceDispatch->m_getAttributes(p_source, NULL) == destinationDispatch->m_getAttributes(this, NULL) &&
		sourceDispatch->m_getTextureCount(p_source, NULL) == destinationDispatch->m_getTextureCount(this, NULL)) {
		LegoU32 byteCount = sourceDispatch->m_getStride(p_source, NULL) * sourceDispatch->m_getCount(p_source, NULL);
		memcpy(
			destinationDispatch->m_getVertex(this, NULL, p_start),
			sourceDispatch->m_getVertex(p_source, NULL, 0),
			byteCount
		);
		destinationDispatch->m_unlock(this, NULL, 1);
		return;
	}

	LegoU32 attributes =
		sourceDispatch->m_getAttributes(p_source, NULL) & destinationDispatch->m_getAttributes(this, NULL);
	LegoS32 textureCount = sourceDispatch->m_getTextureCount(p_source, NULL);
	LegoS32 destinationTextureCount = destinationDispatch->m_getTextureCount(this, NULL);
	if (destinationTextureCount < textureCount) {
		textureCount = destinationTextureCount;
	}
	destinationDispatch->m_setCursor(this, NULL, p_start);
	sourceDispatch->m_setCursor(p_source, NULL, 0);
	LegoS32 vertexCount = (LegoS32) sourceDispatch->m_getCount(p_source, NULL);
	for (LegoS32 vertex = 0; vertex < vertexCount; vertex++) {
		if ((attributes & 1) != 0) {
			destinationDispatch->m_setCurrentPosition(this, NULL, sourceDispatch->m_getCurrentPosition(p_source, NULL));
		}
		if ((attributes & 2) != 0) {
			destinationDispatch->m_setCurrentNormal(this, NULL, sourceDispatch->m_getCurrentNormal(p_source, NULL));
		}
		if ((attributes & 0x14) != 0) {
			destinationDispatch->m_setCurrentColor(this, NULL, sourceDispatch->m_getCurrentColor(p_source, NULL));
		}
		if ((attributes & 8) != 0) {
			for (LegoS32 stage = 0; stage < textureCount; stage++) {
				destinationDispatch->m_setCurrentTexture(
					this,
					NULL,
					stage,
					sourceDispatch->m_getCurrentTexture(p_source, NULL, stage)
				);
			}
		}
		destinationDispatch->m_advanceCursor(this, NULL);
		sourceDispatch->m_advanceCursor(p_source, NULL);
	}
	destinationDispatch->m_unlock(this, NULL, 1);
}

// STUB: LEGORACERS2 0x004e9d00
void RendererVertexBuffer::CopyToVertexData(RendererVertexDataView* p_target)
{
	const VertexBufferCopyDispatch* dispatch = GetVertexBufferDispatch(this);
	if (dispatch->m_lock(this, NULL, 0, dispatch->m_getCount(this, NULL), 0, 0) == 1) {
		LegoU32 byteCount = dispatch->m_getCount(this, NULL) * dispatch->m_getStride(this, NULL);
		memcpy(p_target->m_data, dispatch->m_getVertex(this, NULL, 0), byteCount);
		dispatch->m_unlock(this, NULL, 0);
	}
}

// FUNCTION: LEGORACERS2 0x004e9df0
void RendererVertexBuffer::SetMode(LegoS32 p_mode)
{
	if (m_unk0x04 != 1 || p_mode != 0) {
		m_unk0x04 = p_mode;
	}
}

// Recovered flag transformations from the vertex/index policy callbacks.
// The original wrappers also read DisplayRenderer+0xc7ed0. They remain
// unannotated until an actual embedded-buffer owner can be recovered.
void ApplyVertexBufferPolicy(RendererVertexBuffer& p_buffer, LegoU32 p_requested, LegoU32 p_rendererPolicy)
{
	LegoU32 value = (p_buffer.m_flags & ~3) | (p_requested & 1) | ((p_requested & 1) << 1);
	if (p_rendererPolicy == 0) {
		value &= ~2;
	}
	p_buffer.m_flags = value;
}

void ApplyIndexBufferPolicy(RendererIndexBuffer& p_buffer, LegoU32 p_requested, LegoU32 p_rendererPolicy)
{
	LegoU32 value = (p_buffer.m_flags & ~6) | ((p_requested & 1) << 1) | ((p_requested & 1) << 2);
	if (p_rendererPolicy == 0) {
		value &= ~4;
	}
	p_buffer.m_flags = value;
}
