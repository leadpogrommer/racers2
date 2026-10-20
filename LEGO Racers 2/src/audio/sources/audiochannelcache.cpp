#include "audio/output/audioprovider.h"
#include "audio/service/audioservice.h"
#include "audio/sources/audiochannelreload.h"
#include "core/strings/charbuffer128.h"

#include <string.h>

extern void __fastcall ReleaseAudioChannelRecordBuffer(AudioChannelRecord* p_channel, void* p_edx);

static void BuildAudioChannelFileName(CharBuffer128* p_output, const LegoChar* p_path)
{
	memcpy(p_output->m_data, p_path, sizeof(p_output->m_data));
	LegoChar* extension = NULL;
	for (LegoU32 i = 0; i < 0x7e && p_output->m_data[i] != '\0'; ++i) {
		if (p_output->m_data[i] == '.') {
			extension = p_output->m_data + i + 1;
		}
	}
	if (extension != NULL) {
		memcpy(extension, "aif", 4);
	}
	p_output->Uppercase();
}

static AudioChannelRecord* FindAudioChannelByName(AudioService* p_service, const LegoChar* p_name)
{
	for (LegoU32 i = 0; i < 256; ++i) {
		AudioChannelRecord* channel = &p_service->m_channels[i];
		if (channel->m_unk0x84 != 0 && strcmp(channel->m_fileName, p_name) == 0) {
			return channel;
		}
	}
	return NULL;
}

static AudioChannelRecord* FindAvailableAudioChannel(AudioService* p_service)
{
	for (LegoU32 i = 0; i < 256; ++i) {
		AudioChannelRecord* channel = &p_service->m_channels[i];
		if (channel->m_unk0x84 == 0) {
			return channel;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b08a0
AudioChannelRecord* __fastcall FindOrCreateAudioChannel(AudioService* p_service, void*, const LegoChar* p_path)
{
	if (p_service->m_initializationFailed != 0) {
		return NULL;
	}

	CharBuffer128 normalizedPath;
	BuildAudioChannelFileName(&normalizedPath, p_path);
	AudioChannelRecord* channel = FindAudioChannelByName(p_service, normalizedPath.m_data);
	if (channel != NULL) {
		return channel;
	}

	channel = FindAvailableAudioChannel(p_service);
	if (channel == NULL || !LoadAudioChannelFile(channel, NULL, normalizedPath)) {
		return NULL;
	}

	IDirectSoundBuffer* playbackBuffer =
		g_audioBackend->m_vtable
			->m_createSourceBuffer(g_audioBackend, NULL, channel->m_cachedBuffer, channel->m_unk0x88);
	if (playbackBuffer == NULL) {
		DestroyAudioChannelRecordResources(channel, NULL);
		return NULL;
	}

	channel->m_playbackBuffer = playbackBuffer;
	channel->m_unk0x94 =
		g_audioBackend->m_vtable->m_getSourceSampleRate(g_audioBackend, NULL, channel->m_cachedBuffer, 0);
	ReleaseAudioChannelRecordBuffer(channel, NULL);
	channel->m_referenceCount = 0;
	channel->m_unk0x84 = TRUE;
	strcpy(channel->m_fileName, normalizedPath.m_data);
	return channel;
}
