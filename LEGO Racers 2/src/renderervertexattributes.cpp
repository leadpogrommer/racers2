#include "rendererbufferobjects.h"
#include "vector3.h"

// FUNCTION: LEGORACERS2 0x004ab590
IDirect3DVertexBuffer8* RendererVertexBuffer::GetDeviceBuffer()
{
	return m_storage->m_deviceBuffer;
}

// FUNCTION: LEGORACERS2 0x004ab5a0
LegoU32 RendererVertexBuffer::GetAttributes()
{
	return m_attributes;
}

// FUNCTION: LEGORACERS2 0x004ab5b0
LegoU32 RendererVertexBuffer::GetCount()
{
	return m_count;
}

// FUNCTION: LEGORACERS2 0x004ab5c0
LegoS32 RendererVertexBuffer::GetTextureCount()
{
	return m_textureCount;
}

// FUNCTION: LEGORACERS2 0x004ab5d0
LegoU8* RendererVertexBuffer::GetVertex(LegoS32 p_index)
{
	return m_data + m_stride * p_index;
}

// FUNCTION: LEGORACERS2 0x004ab5e0 FOLDED
LegoU32 RendererVertexBuffer::GetPositionOffset()
{
	return m_positionOffset;
}

// FUNCTION: LEGORACERS2 0x004ab5f0
LegoU32 RendererVertexBuffer::GetNormalOffset()
{
	return m_normalOffset;
}

// FUNCTION: LEGORACERS2 0x004ab600 FOLDED
LegoU32 RendererVertexBuffer::GetTextureOffset()
{
	return m_textureOffset;
}

// FUNCTION: LEGORACERS2 0x004ab610
void RendererVertexBuffer::SetCursor(LegoS32 p_index)
{
	m_cursor = m_data + m_stride * p_index;
}

// FUNCTION: LEGORACERS2 0x004ab630
void RendererVertexBuffer::AdvanceCursor()
{
	m_cursor += m_stride;
}

// FUNCTION: LEGORACERS2 0x004ab640
Vector3* RendererVertexBuffer::GetCurrentPosition()
{
	return (Vector3*) (m_cursor + m_positionOffset);
}

// FUNCTION: LEGORACERS2 0x004ab650
Vector3* RendererVertexBuffer::GetPosition(LegoS32 p_index)
{
	return (Vector3*) (m_data + m_stride * p_index + m_positionOffset);
}

// FUNCTION: LEGORACERS2 0x004ab670
Vector3* RendererVertexBuffer::GetCurrentNormal()
{
	return (Vector3*) (m_cursor + m_normalOffset);
}

// FUNCTION: LEGORACERS2 0x004ab680
Vector3* RendererVertexBuffer::GetNormal(LegoS32 p_index)
{
	return (Vector3*) (m_data + m_stride * p_index + m_normalOffset);
}

// FUNCTION: LEGORACERS2 0x004ab6a0
LegoU32* RendererVertexBuffer::GetCurrentColor()
{
	return (LegoU32*) (m_cursor + m_colorOffset);
}

// FUNCTION: LEGORACERS2 0x004ab6b0
LegoU32* RendererVertexBuffer::GetColor(LegoS32 p_index)
{
	return (LegoU32*) (m_data + m_stride * p_index + m_colorOffset);
}

// STUB: LEGORACERS2 0x004ab6d0
RendererTextureCoordinate* RendererVertexBuffer::GetCurrentTextureCoordinate(LegoS32 p_stage)
{
	return (RendererTextureCoordinate*) (m_cursor + m_textureOffset + p_stage * 8);
}

// STUB: LEGORACERS2 0x004ab6f0
RendererTextureCoordinate* RendererVertexBuffer::GetTextureCoordinate(LegoS32 p_index, LegoS32 p_stage)
{
	return (RendererTextureCoordinate*) (m_data + m_stride * p_index + m_textureOffset + p_stage * 8);
}

// FUNCTION: LEGORACERS2 0x004ab710
void RendererVertexBuffer::SetCurrentPosition(const Vector3* p_value)
{
	*((Vector3*) (m_cursor + m_positionOffset)) = *p_value;
}

// FUNCTION: LEGORACERS2 0x004ab730
void RendererVertexBuffer::SetPosition(LegoS32 p_index, const Vector3* p_value)
{
	*((Vector3*) (m_data + m_stride * p_index + m_positionOffset)) = *p_value;
}

// FUNCTION: LEGORACERS2 0x004ab760
void RendererVertexBuffer::SetCurrentNormal(const Vector3* p_value)
{
	*((Vector3*) (m_cursor + m_normalOffset)) = *p_value;
}

// FUNCTION: LEGORACERS2 0x004ab780
void RendererVertexBuffer::SetNormal(LegoS32 p_index, const Vector3* p_value)
{
	*((Vector3*) (m_data + m_stride * p_index + m_normalOffset)) = *p_value;
}

// FUNCTION: LEGORACERS2 0x004ab7b0
void RendererVertexBuffer::SetCurrentColor(const LegoU32* p_value)
{
	*((LegoU32*) (m_cursor + m_colorOffset)) = *p_value;
}

// FUNCTION: LEGORACERS2 0x004ab7d0
void RendererVertexBuffer::SetColor(LegoS32 p_index, const LegoU32* p_value)
{
	*((LegoU32*) (m_data + m_stride * p_index + m_colorOffset)) = *p_value;
}

// STUB: LEGORACERS2 0x004ab7f0
void RendererVertexBuffer::SetCurrentTextureCoordinate(LegoS32 p_stage, const RendererTextureCoordinate* p_value)
{
	*((RendererTextureCoordinate*) (m_cursor + m_textureOffset + p_stage * 8)) = *p_value;
}

// STUB: LEGORACERS2 0x004ab820
void RendererVertexBuffer::SetTextureCoordinate(
	LegoS32 p_index,
	LegoS32 p_stage,
	const RendererTextureCoordinate* p_value
)
{
	*((RendererTextureCoordinate*) (m_data + m_stride * p_index + m_textureOffset + p_stage * 8)) = *p_value;
}

// Shares the original field-at-0x18 leaf with the index getter below.
// FUNCTION: LEGORACERS2 0x004ab850 FOLDED
LegoU32 RendererVertexBuffer::GetStride()
{
	return m_stride;
}

// FUNCTION: LEGORACERS2 0x004ab850 FOLDED
IDirect3DIndexBuffer8* RendererIndexBuffer::GetDeviceBuffer()
{
	return m_deviceBuffer;
}

// FUNCTION: LEGORACERS2 0x004e9540
void RendererVertexBuffer::Release()
{
	if (m_storage != NULL) {
		m_storage->ReleaseReference();
		m_storage = NULL;
	}
}

// Same original leaf as Application::GetUnknownFlag at 0x0049a6a0.
// FUNCTION: LEGORACERS2 0x0049a6a0 FOLDED
LegoU32 RendererVertexBuffer::GetColorOffset()
{
	return m_colorOffset;
}
