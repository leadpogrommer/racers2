#include "audio/streaming/audiocommentary.h"

#include "audio/service/audioservice.h"
#include "core/debug/diagnostics.h"
#include "memory/memoryheap.h"

#include <new.h>

// GLOBAL: LEGORACERS2 0x006e47d8
LegoU8 g_audioPcmInitialFill;

namespace
{
AudioCommentaryState& CommentaryState(AudioService* p_service)
{
	return *(AudioCommentaryState*) ((LegoU8*) p_service + 0x24ba0);
}

void ClearCommentaryQueue(AudioCommentaryState& p_state)
{
	p_state.m_head = 0;
	p_state.m_count = 0;
	for (LegoS32 index = 0; index < 8; ++index) {
		p_state.m_byteCounts[index] = 0;
		p_state.m_fileOffsets[index] = 0;
		p_state.m_tags[index] = 0;
	}
}
} // namespace

// STUB: LEGORACERS2 0x004ee700
void __fastcall InitializeAudioCommentary(AudioService* p_service, void*, LPCSTR p_path)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_active == 1) {
		PrintDiagnostic("SSFX Commentary already running");
		return;
	}
	state.m_head = 0;
	state.m_count = 0;
	state.m_stream = NULL;
	ClearCommentaryQueue(state);
	void* memory = AllocateHeapMemory(sizeof(CommentaryAudioStream));
	state.m_stream =
		memory == NULL ? NULL : new (memory) CommentaryAudioStream(p_path, g_audioBackend->m_device, 22050, 1);
	if (state.m_stream != NULL) {
		state.m_active = 1;
	}
}

// STUB: LEGORACERS2 0x004ee7a0
void __fastcall ShutdownAudioCommentary(AudioService* p_service, void*)
{
	typedef void(__fastcall * StopCallback)(AudioService*, void*, LegoU32);
	((StopCallback) p_service->m_vtable->m_unk0x90)(p_service, NULL, 1);
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_stream != NULL) {
		state.m_stream->~CommentaryAudioStream();
		FreeHeapMemory(state.m_stream);
	}
	state.m_stream = NULL;
}

// STUB: LEGORACERS2 0x004ee7e0
void __fastcall SetAudioCommentaryGain(AudioService* p_service, void*, LegoFloat p_gain)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_active != 0) {
		state.m_stream->SetGain(p_gain);
	}
}

// STUB: LEGORACERS2 0x004ee800
void __fastcall QueueAudioCommentary(AudioService* p_service, void*, LONG p_offset, DWORD p_bytes)
{
	if (CommentaryState(p_service).m_active != 0) {
		EnqueueAudioCommentary(p_service, NULL, p_offset, p_bytes);
	}
}

// STUB: LEGORACERS2 0x004ee820
void __fastcall StopAudioCommentary(AudioService* p_service, void*, LegoU32 p_clearQueue)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_active != 0) {
		state.m_stream->Stop();
		if (p_clearQueue != 0) {
			ClearCommentaryQueue(state);
		}
		else {
			PlayNextAudioCommentary(p_service, NULL);
		}
	}
}

// STUB: LEGORACERS2 0x004ee880
LegoBool32 __fastcall EnqueueAudioCommentary(AudioService* p_service, void*, LONG p_offset, DWORD p_bytes)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_count > 7) {
		return FALSE;
	}
	LegoS32 index = state.m_head + state.m_count;
	if (index > 7) {
		index -= 8;
	}
	state.m_byteCounts[index] = p_bytes;
	state.m_fileOffsets[index] = p_offset;
	state.m_tags[index] = 1;
	++state.m_count;
	if (state.m_stream->IsReading() == 0 && state.m_stream->m_state == 0) {
		PlayNextAudioCommentary(p_service, NULL);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004ee900
LegoBool32 __fastcall PlayNextAudioCommentary(AudioService* p_service, void*)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (state.m_count == 0 || state.m_stream->IsReading() != 0) {
		return FALSE;
	}
	--state.m_count;
	state.m_stream->Play(state.m_fileOffsets[state.m_head], state.m_byteCounts[state.m_head]);
	state.m_byteCounts[state.m_head] = 0;
	state.m_fileOffsets[state.m_head] = 0;
	state.m_tags[state.m_head] = 0;
	LegoS32 oldHead = state.m_head;
	state.m_head = oldHead + 1;
	if (state.m_head > 7) {
		state.m_head = oldHead - 7;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004ee9b0
void __fastcall InitializeAudioPcmBuffer(AudioService* p_service, void*, WAVEFORMATEX* p_format)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	state.m_pcmBufferSize = 0x15888;
	state.m_pcmWritePosition = 0;
	state.m_pcmPlaying = 0;
	state.m_pcmPhase = 1;
	g_audioPcmInitialFill = 1;
	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = 0x10180;
	description.dwBufferBytes = state.m_pcmBufferSize;
	description.lpwfxFormat = p_format;
	if (g_audioBackend->m_device->CreateSoundBuffer(&description, &state.m_pcmBuffer, NULL) == DS_OK) {
		void* data = NULL;
		DWORD bytes = 0;
		HRESULT result = state.m_pcmBuffer->Lock(0, 0, &data, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER);
		CheckAudioMediaResult(p_service, NULL, result);
		ClearAudioBytes(data, bytes);
		result = state.m_pcmBuffer->Unlock(data, bytes, NULL, 0);
		CheckAudioMediaResult(p_service, NULL, result);
		g_audioBackend->m_device->Compact();
	}
}

// STUB: LEGORACERS2 0x004eeaa0
void __fastcall ShutdownAudioPcmBuffer(AudioService* p_service, void*)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	state.m_pcmBuffer->Stop();
	state.m_pcmBuffer->Release();
	state.m_pcmPlaying = 0;
	state.m_pcmWritePosition = 0;
}

// STUB: LEGORACERS2 0x004eead0
void __fastcall AppendAudioPcmData(AudioService* p_service, void*, const void* p_data, LegoS32 p_bytes)
{
	AudioCommentaryState& state = CommentaryState(p_service);
	if (g_audioPcmInitialFill == 0) {
		if (state.m_pcmWritePosition + p_bytes > 0xac43) {
			state.m_pcmWritePosition = 0;
		}
	}
	else if (state.m_pcmWritePosition + p_bytes > 0x15887) {
		void* first;
		void* second;
		DWORD firstBytes;
		DWORD secondBytes;
		g_audioPcmInitialFill = 0;
		state.m_pcmBuffer
			->Lock(0, state.m_pcmWritePosition, &first, &firstBytes, &second, &secondBytes, DSBLOCK_ENTIREBUFFER);
		CopyAudioBytes(first, state.m_pcmStaging, firstBytes);
		if (second != NULL) {
			CopyAudioBytes(second, state.m_pcmStaging + firstBytes, secondBytes);
		}
		state.m_pcmBuffer->Unlock(first, firstBytes, second, secondBytes);
		state.m_pcmBuffer->Play(0, 0, DSBPLAY_LOOPING);
		state.m_pcmPlaying = 1;
		return;
	}
	CopyAudioBytes(state.m_pcmStaging + state.m_pcmWritePosition, p_data, p_bytes);
	state.m_pcmWritePosition += p_bytes;
}
