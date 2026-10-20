#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004ac120
LegoBool32 DisplayRenderer::UsesHardwareVertexProcessing()
{
	return ((~m_deviceBehaviorFlags) >> 5) & 1;
}

// FUNCTION: LEGORACERS2 0x004acfb0
LegoBool32 DisplayRenderer::CreateVertexBuffers()
{
	m_device->CreateVertexBuffer(0x10000, m_vertexBufferUsage | 0x200, 0x1c4, D3DPOOL_DEFAULT, &m_specularVertexBuffer);

	m_device->CreateVertexBuffer(
		m_vertexBufferCapacity * 36,
		m_vertexBufferUsage | 0x200,
		0x152,
		D3DPOOL_DEFAULT,
		&m_normalVertexBuffer
	);

	if (m_vtable->m_usesHardwareVertexProcessing(this, NULL) == FALSE) {
		m_device->CreateVertexBuffer(
			m_vertexBufferCapacity * 28,
			m_vertexBufferUsage | 0x200,
			0x144,
			D3DPOOL_DEFAULT,
			&m_transformedVertexBuffer
		);
	}

	return TRUE;
}
