#include "memory/memoryheap.h"
#include "render/draw/rendererdrawpacketview.h"
#include "render/models/modelresource.h"

// STUB: LEGORACERS2 0x004a7c60
void ModelTextureGroup::ReleaseStorage()
{
	if (m_primitives != NULL) {
		// The retail allocation has a VC6 array cookie before its first record.
		LegoU32* cookie = (LegoU32*) m_primitives - 1;
		for (LegoS32 index = (LegoS32) *cookie - 1; index >= 0; --index) {
			ReleaseRendererDrawPacket((RendererDrawPacketView*) &m_primitives[index], NULL);
		}
		FreeHeapMemory(cookie);
		m_primitives = NULL;
	}
	if (m_primitiveVectors != NULL) {
		FreeHeapMemory(m_primitiveVectors);
		m_primitiveVectors = NULL;
	}
	m_primitiveCount = 0;
}
