#include "audio/music/audiomusiccontroller.h"
#include "audio/sources/audiochannelreload.h"

// This projection fills the previously untyped PCM-format/device interval.
// Existing controller members outside that interval are accessed by name.
struct DigitalMusicOutputPrefix {
	undefined m_unk0x00[0x40];
	WAVEFORMATEX m_format;
	undefined m_unk0x52[0x6c - 0x52];
	IDirectSound8* m_device;
};
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicOutputPrefix, m_device) == 0x6c);

// STUB: LEGORACERS2 0x004f1c00
void __fastcall RestoreDigitalMusicOutput(
	DigitalMusicController* p_music,
	void*,
	IDirectSound8* p_device,
	LegoU32,
	LegoU32
)
{
	DigitalMusicOutputPrefix* output = (DigitalMusicOutputPrefix*) p_music;
	output->m_device = p_device;
	ZeroMemory(&output->m_format, sizeof(output->m_format));
	WAVEFORMATEX& format = output->m_format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = p_music->m_waveFormat.nChannels;
	format.nSamplesPerSec = p_music->m_waveFormat.nSamplesPerSec;
	format.nBlockAlign = (WORD) (format.nChannels * 2);
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
	format.wBitsPerSample = 16;
	format.cbSize = 0;
	p_music->m_chunkByteCount = (format.nSamplesPerSec >> 4) * format.nChannels * 2;
	p_music->m_bufferByteCount = p_music->m_chunkByteCount * 8;

	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = 0x10180;
	description.dwBufferBytes = p_music->m_bufferByteCount;
	description.lpwfxFormat = &format;
	p_device->CreateSoundBuffer(&description, &p_music->m_playbackBuffer, NULL);
	IDirectSoundNotify* notification = NULL;
	p_music->m_playbackBuffer->QueryInterface(IID_IDirectSoundNotify, (void**) &notification);
	DSBPOSITIONNOTIFY positions[8];
	for (LegoU32 i = 0; i < 8; ++i) {
		positions[i].dwOffset = p_music->m_chunkByteCount * (i + 1) - 1;
		positions[i].hEventNotify = p_music->m_workerEvent;
	}
	notification->SetNotificationPositions(8, positions);
	notification->Release();
}

// STUB: LEGORACERS2 0x004f1d40
void __fastcall SetDigitalMusicBufferGain(DigitalMusicController* p_music, void*, LegoFloat p_gain)
{
	if (p_music->m_playbackBuffer != NULL) {
		LegoS32 volume = (LegoS32) ((p_gain * 4.0f - 4.0f) * 1000.0f);
		p_music->m_playbackBuffer->SetVolume(volume);
	}
}

// STUB: LEGORACERS2 0x004f2ec0
void __fastcall RestoreCommentaryPlaybackBuffer(
	CommentaryAudioStream* p_stream,
	void*,
	IDirectSound8* p_device,
	LegoU32 p_rate,
	LegoU32 p_channels
)
{
	WAVEFORMATEX format;
	ZeroMemory(&format, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = (WORD) p_channels;
	format.nSamplesPerSec = p_rate;
	format.nBlockAlign = (WORD) (p_channels * 2);
	format.nAvgBytesPerSec = format.nBlockAlign * p_rate;
	format.wBitsPerSample = 16;
	format.cbSize = 0;
	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = 0x10080;
	description.dwBufferBytes = 0x5622;
	description.lpwfxFormat = &format;
	if (p_device->CreateSoundBuffer(&description, &p_stream->m_buffer, NULL) != DS_OK) {
		DebugBreak();
	}
	void* data = NULL;
	DWORD bytes = 0;
	if (p_stream->m_buffer->Lock(0, 0, &data, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER) == DS_OK) {
		ClearAudioBytes(data, bytes);
		p_stream->m_buffer->Unlock(data, bytes, NULL, 0);
	}
	p_stream->Play(0, (DWORD) -1);
}
