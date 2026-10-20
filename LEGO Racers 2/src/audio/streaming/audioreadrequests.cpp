#include "audio/streaming/audioreadrequests.h"

#include "audio/streaming/audiocommentary.h"

// GLOBAL: LEGORACERS2 0x006e4810
AudioReadRequest g_audioReadRequests[32];

// STUB: LEGORACERS2 0x004f25a0
LegoS32 BeginAudioRead(HANDLE p_file, LONG p_offset, void* p_destination, DWORD p_bytes)
{
	// Retail ignores the supplied file offset; callers position the handle.
	(void) p_offset;
	LegoS32 index = 0;
	while (g_audioReadRequests[index].m_active != 0) {
		if (++index >= 32) {
			return -1;
		}
	}
	AudioReadRequest& request = g_audioReadRequests[index];
	request.m_file = p_file;
	request.m_requestedBytes = p_bytes;
	request.m_destination = p_destination;
	request.m_overlapped = 0;
	if (p_destination == NULL) {
		request.m_destination = request.m_buffer;
	}
	request.m_active = 1;
	if (!ReadFile(p_file, request.m_buffer, p_bytes, &request.m_bytesRead, NULL)) {
		return -1;
	}
	return index;
}

// FUNCTION: LEGORACERS2 0x004f2610
LegoBool32 WaitForAudioRead(LegoS32 p_index, DWORD* p_bytesRead)
{
	*p_bytesRead = 0;
	if (p_index < 0) {
		return FALSE;
	}
	AudioReadRequest& request = g_audioReadRequests[p_index];
	if (request.m_overlapped == 1 &&
		!GetOverlappedResult(request.m_file, &request.m_operation, &request.m_bytesRead, TRUE)) {
		request.m_active = 0;
		return FALSE;
	}
	if (request.m_destination != request.m_buffer) {
		CopyAudioBytes(request.m_destination, request.m_buffer, request.m_bytesRead);
	}
	*p_bytesRead = request.m_bytesRead;
	request.m_active = 0;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f2690
LegoS32 CountPendingAudioReads()
{
	LegoS32 count = 0;
	for (LegoS32 i = 0; i < 32; ++i) {
		if (g_audioReadRequests[i].m_active == 1) {
			++count;
		}
	}
	return count;
}

// FUNCTION: LEGORACERS2 0x004f26b0
LegoBool32 FindCompletedAudioRead(LegoS32* p_index)
{
	for (LegoS32 index = *p_index; index < 32; ++index) {
		AudioReadRequest& request = g_audioReadRequests[index];
		if (request.m_active == 1) {
			if (request.m_overlapped == 1 &&
				!GetOverlappedResult(request.m_file, &request.m_operation, &request.m_bytesRead, FALSE)) {
				if (GetLastError() == ERROR_IO_PENDING) {
					continue;
				}
				request.m_bytesRead = 0;
			}
			request.m_active = 0;
			*p_index = index;
			return TRUE;
		}
	}
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x004f2730
AudioReadRequest* GetAudioReadRequest(LegoS32 p_index)
{
	return &g_audioReadRequests[p_index];
}
