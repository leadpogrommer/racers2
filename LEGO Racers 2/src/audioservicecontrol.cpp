#include "audiochannelreload.h"
#include "audiocommentary.h"
#include "audiomusiccontroller.h"
#include "audioprovider.h"
#include "audioservice.h"
#include "diagnostics.h"
#include "memoryheap.h"
#include "stereoaudioprovider.h"
#include "threedimensionalaudioprovider.h"

#include <new.h>
#include <string.h>

void __fastcall ReleaseAudioChannelRecordBuffer(AudioChannelRecord*, void*);

// STUB: LEGORACERS2 0x004b1020
void AudioService::Shutdown()
{
	if (m_shutdown != 0 || m_initializationFailed != 0) {
		return;
	}
	AudioCommentaryState& commentary = *(AudioCommentaryState*) &m_unk0x24ba0;
	if (commentary.m_active != 0 && commentary.m_stream != NULL) {
		commentary.m_stream->ReleasePlaybackBuffer();
	}
	if (m_usesDigitalAudio != 0 && m_unk0x249f4 != NULL) {
		((DigitalMusicController*) m_unk0x249f4)->StopAndReleasePlaybackBuffer();
	}

	LegoU32 i;
	for (i = 0; i < 512; ++i) {
		AudioSourceRecord* source = &m_sources[i];
		if (source->m_unk0x2c != 0) {
			g_audioBackend->m_vtable->m_releaseSourceBuffer(g_audioBackend, NULL, source);
		}
	}
	for (i = 0; i < 256; ++i) {
		AudioChannelRecord* channel = &m_channels[i];
		if (channel->m_unk0x84 != 0) {
			g_audioBackend->m_vtable->m_releaseBufferPointer(g_audioBackend, NULL, &channel->m_playbackBuffer);
		}
	}
	for (i = 0; i < 512; ++i) {
		AudioSourceRecord& source = m_sources[i];
		if (source.m_unk0xc4 != 0 || source.m_playbackBuffer != NULL) {
			PrintDiagnostic("Monkeys on fire: %s", source.m_unkName0x4c);
		}
	}
	for (i = 0; i < 256; ++i) {
		AudioChannelRecord& channel = m_channels[i];
		if (channel.m_playbackBuffer != NULL) {
			PrintDiagnostic("Monkeys exploding: %s %x", channel.m_fileName, channel.m_playbackBuffer);
		}
	}
	GetMemoryHeap()->m_useCRT = TRUE;
	if (g_audioBackend != NULL) {
		g_audioBackend->m_vtable->m_destroy(g_audioBackend, NULL, 1);
		g_audioBackend = NULL;
	}
	GetMemoryHeap()->m_useCRT = FALSE;
	m_shutdown = 1;
}

// STUB: LEGORACERS2 0x004b1150
void AudioService::Restart()
{
	GetMemoryHeap()->m_useCRT = TRUE;
	void* storage;
	switch (m_qualityMode) {
	case 0:
		storage = AllocateHeapMemory(0x10);
		g_audioBackend =
			storage != NULL ? (AudioBackendStateView*) new (storage) MonoDirectSoundProvider(g_audioWindow) : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_initializationFailed = 1;
			if (g_audioBackend != NULL) {
				g_audioBackend->m_vtable->m_destroy(g_audioBackend, NULL, 1);
				g_audioBackend = NULL;
			}
		}
		break;
	case 1:
		storage = AllocateHeapMemory(0x10);
		g_audioBackend =
			storage != NULL ? (AudioBackendStateView*) ConstructStereoAudioProvider(storage, g_audioWindow) : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 0);
		}
		break;
	case 2:
		storage = AllocateHeapMemory(0x24);
		g_audioBackend = storage != NULL
							 ? (AudioBackendStateView*) ConstructThreeDimensionalAudioProvider(storage, g_audioWindow)
							 : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 1);
		}
		break;
	case 3:
		m_initializationFailed = 1;
		if (g_audioBackend != NULL) {
			g_audioBackend->m_vtable->m_destroy(g_audioBackend, NULL, 1);
			g_audioBackend = NULL;
		}
		break;
	default:
		storage = AllocateHeapMemory(0x10);
		g_audioBackend =
			storage != NULL ? (AudioBackendStateView*) ConstructStereoAudioProvider(storage, g_audioWindow) : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 0);
		}
		break;
	}
	GetMemoryHeap()->m_useCRT = FALSE;
	if (m_initializationFailed != 0) {
		return;
	}
	LegoU32 i;
	for (i = 0; i < 256; ++i) {
		AudioChannelRecord* channel = &m_channels[i];
		if (channel->m_unk0x84 != 0 && channel != NULL) {
			CharBuffer128 path;
			memcpy(path.m_data, channel->m_fileName, sizeof(path.m_data));
			LoadAudioChannelFile(channel, NULL, path);
			channel->m_playbackBuffer =
				g_audioBackend->m_vtable
					->m_createSourceBuffer(g_audioBackend, NULL, channel->m_cachedBuffer, channel->m_unk0x88);
			ReleaseAudioChannelRecordBuffer(channel, NULL);
		}
	}
	for (i = 0; i < 512; ++i) {
		AudioSourceRecord& source = m_sources[i];
		if (source.m_channelResource != NULL) {
			source.m_playbackBuffer = g_audioBackend->m_vtable->m_duplicatePlaybackBuffer(
				g_audioBackend,
				NULL,
				source.m_channelResource->m_playbackBuffer
			);
		}
		if (source.m_unk0x38 != 0) {
			source.m_vtable->m_resetPlaybackState(&source, NULL);
		}
	}
	AudioCommentaryState& commentary = *(AudioCommentaryState*) &m_unk0x24ba0;
	if (commentary.m_active != 0 && commentary.m_stream != NULL) {
		RestoreCommentaryPlaybackBuffer(commentary.m_stream, NULL, g_audioBackend->m_device, 0x5622, 1);
	}
	if (m_usesDigitalAudio != 0 && m_unk0x249f4 != NULL) {
		RestoreDigitalMusicOutput((DigitalMusicController*) m_unk0x249f4, NULL, g_audioBackend->m_device, 0x5622, 2);
	}
	m_shutdown = 0;
}
