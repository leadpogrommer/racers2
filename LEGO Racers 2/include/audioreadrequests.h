#ifndef LR2_AUDIOREADREQUESTS_H
#define LR2_AUDIOREADREQUESTS_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>
#include <windows.h>

struct AudioReadRequest {
	LegoU32 m_active;
	LegoU32 m_overlapped;
	HANDLE m_file;
	DWORD m_requestedBytes;
	DWORD m_bytesRead;
	void* m_buffer;
	void* m_destination;
	OVERLAPPED m_operation;
};
DECOMP_SIZE_ASSERT(AudioReadRequest, 0x30);
DECOMP_STATIC_ASSERT(offsetof(AudioReadRequest, m_operation) == 0x1c);
extern AudioReadRequest g_audioReadRequests[32];
LegoS32 BeginAudioRead(HANDLE p_file, LONG p_offset, void* p_destination, DWORD p_bytes);
LegoBool32 WaitForAudioRead(LegoS32 p_index, DWORD* p_bytesRead);
LegoS32 CountPendingAudioReads();
LegoBool32 FindCompletedAudioRead(LegoS32* p_index);
AudioReadRequest* GetAudioReadRequest(LegoS32 p_index);
#endif
