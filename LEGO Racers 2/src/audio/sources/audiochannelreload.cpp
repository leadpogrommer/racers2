#include "audio/sources/audiochannelreload.h"

#include "io/files/winfile.h"
#include "memory/memoryheap.h"

#include <string.h>

// STUB: LEGORACERS2 0x004b0ad0
LegoBool32 __fastcall LoadAudioChannelFile(AudioChannelRecord* p_channel, void*, CharBuffer128 p_path)
{
	memcpy(p_channel->m_fileName, p_path.m_data, sizeof(p_path.m_data));
	BOOL opened = FALSE;
	WinFile file(p_path.m_data, 0, &opened);
	if (opened == FALSE) {
		return FALSE;
	}
	p_channel->m_unk0x88 = file.GetManagedSize();
	if ((LegoS32) p_channel->m_unk0x88 < 1) {
		return FALSE;
	}
	GetMemoryHeap()->m_useCRT = TRUE;
	p_channel->m_cachedBuffer = AllocateHeapMemory(p_channel->m_unk0x88);
	GetMemoryHeap()->m_useCRT = FALSE;
	if (p_channel->m_cachedBuffer == NULL) {
		return FALSE;
	}
	if (file.Read(p_channel->m_cachedBuffer, p_channel->m_unk0x88, 1) == FALSE) {
		return FALSE;
	}
	file.CloseManaged();
	return TRUE;
}
