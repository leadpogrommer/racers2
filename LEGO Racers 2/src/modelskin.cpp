#include "modelskin.h"

#include "memoryheap.h"
#include "winfile.h"

// STUB: LEGORACERS2 0x004e7f10
ModelSkin::ModelSkin(ModelResource* p_model, WinFile* p_file)
{
	m_unk0x00 = 0;
	m_model = p_model;
	m_unk0x08 = 1;
	p_file->Read(&m_unk0x0c, 4, 1);
	LegoU32 bytes;
	p_file->Read(&bytes, 4, 1);
	m_blendVertices = GetMemoryHeap()->Allocate(bytes, "blend vertices");
	p_file->Read(m_blendVertices, bytes, 1);
	p_file->Read(&m_groupCount, 4, 1);
	m_groups = (ModelSkinGroup*) AllocateHeapMemory(m_groupCount * sizeof(ModelSkinGroup));
	for (LegoS32 index = 0; index < m_groupCount; ++index) {
		ModelSkinGroup* group = &m_groups[index];
		p_file->Read(&group->m_vertexCount, 4, 1);
		p_file->Read(&bytes, 4, 1);
		group->m_lookupData = GetMemoryHeap()->Allocate(bytes, "BlendVertex lookup array");
		p_file->Read(group->m_lookupData, bytes, 1);
		group->m_positions =
			(Vector3*) GetMemoryHeap()->Allocate(group->m_vertexCount * sizeof(Vector3), "Vector positions");
	}
}

// STUB: LEGORACERS2 0x004e8030
void ModelSkin::ReleaseStorage()
{
	for (LegoS32 index = m_groupCount - 1; index >= 0; --index) {
		GetMemoryHeap()->Free(m_groups[index].m_positions);
		GetMemoryHeap()->Free(m_groups[index].m_lookupData);
	}
	FreeHeapMemory(m_groups);
	GetMemoryHeap()->Free(m_blendVertices);
}
