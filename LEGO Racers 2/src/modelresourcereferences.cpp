#include "modelresourcereferences.h"

#include "memoryheap.h"
#include "renderer.h"

// STUB: LEGORACERS2 0x004a9710
void ModelResourceReferences::Allocate(LegoS16 p_textureCount, LegoS16 p_materialCount)
{
	m_textureCount = p_textureCount;
	m_materialCount = p_materialCount;
	m_ids = (LegoS16*) GetMemoryHeap()->Allocate((m_textureCount + m_materialCount) * 2, "Model resources");
}

// STUB: LEGORACERS2 0x004a9750
void ModelResourceReferences::FreeStorage()
{
	if (m_ids != NULL) {
		GetMemoryHeap()->Free(m_ids);
	}
	Reset();
}

// FUNCTION: LEGORACERS2 0x004a9770
void ModelResourceReferences::ReleaseRendererReferences()
{
	if (m_ids != NULL) {
		LegoS32 index;
		for (index = m_materialCount - 1; index >= 0; --index) {
			g_displayRenderer->ReleaseAuxiliaryResource(GetMaterial(index));
		}
		for (index = m_textureCount - 1; index >= 0; --index) {
			g_displayRenderer->ReleaseTextureResourceReference(GetTexture(index), 1);
		}
		m_materialCount = 0;
		m_textureCount = 0;
	}
}

// FUNCTION: LEGORACERS2 0x004a97d0
void ModelResourceReferences::Reset()
{
	m_ids = NULL;
	m_materialCount = 0;
	m_textureCount = 0;
}

// FUNCTION: LEGORACERS2 0x004a97e0
LegoS32 ModelResourceReferences::GetTexture(LegoS32 p_index) const
{
	return m_ids[p_index];
}

// FUNCTION: LEGORACERS2 0x004a97f0
LegoS32 ModelResourceReferences::GetMaterial(LegoS32 p_index) const
{
	return m_ids[m_textureCount + p_index];
}

// FUNCTION: LEGORACERS2 0x004a9810
void ModelResourceReferences::SetTexture(LegoS32 p_index, LegoS16 p_id)
{
	m_ids[p_index] = p_id;
}

// FUNCTION: LEGORACERS2 0x004a9830
void ModelResourceReferences::SetMaterial(LegoS32 p_index, LegoS16 p_id)
{
	m_ids[m_textureCount + p_index] = p_id;
}
