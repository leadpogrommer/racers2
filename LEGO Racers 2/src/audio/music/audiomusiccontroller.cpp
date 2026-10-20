#include "audio/music/audiomusiccontroller.h"

#include "decomp.h"

#include <string.h>
#include <windows.h>

// GLOBAL: LEGORACERS2 0x0054b6b0
const LegoS32 g_msAdpcmAdaptation[16] =
	{230, 230, 230, 230, 307, 409, 512, 614, 768, 614, 512, 409, 307, 230, 230, 230};

// GLOBAL: LEGORACERS2 0x0054b6f0
const LegoS32 g_msAdpcmCoefficient1[7] = {256, 512, 0, 192, 240, 460, 392};

// GLOBAL: LEGORACERS2 0x0054b70c
const LegoS32 g_msAdpcmCoefficient2[7] = {0, -256, 0, 64, 0, -208, -232};

static LegoS32 ClampAdpcmSample(LegoS32 p_sample)
{
	if (p_sample > 32767) {
		return 32767;
	}
	if (p_sample < -32768) {
		return -32768;
	}
	return p_sample;
}

// STUB: LEGORACERS2 0x004f1860
LegoU32 DigitalMusicController::StartPlayback(LegoU32, LegoU32)
{
	if (m_playbackBuffer != NULL && m_playing == 0) {
		UpdatePlaybackBuffer(1);
		m_wasPlaying = 1;
		m_playing = 1;
		m_playbackBuffer->SetCurrentPosition(0);
		m_playbackBuffer->Play(0, 0, DSBPLAY_LOOPING);
	}
	return 1;
}

// STUB: LEGORACERS2 0x004f18c0
LegoU32 DigitalMusicController::StopPlaybackIfActive()
{
	if (m_playing == 1) {
		if (m_playbackBuffer != NULL) {
			m_playbackBuffer->Stop();
		}
		m_wasPlaying = 0;
		m_playing = 0;
	}
	return 1;
}

// STUB: LEGORACERS2 0x004f1900
LegoU32 DigitalMusicController::SetEnabled(LegoU32 p_enabled)
{
	if (p_enabled == 1) {
		m_wasPlaying = m_playing;
		if (m_playing == 1) {
			if (m_playbackBuffer != NULL) {
				m_playbackBuffer->Stop();
			}
			m_playing = 0;
		}
	}
	else if (m_wasPlaying == 1) {
		StartPlayback(0, (LegoU32) -1);
	}
	return 1;
}

// STUB: LEGORACERS2 0x004f1970
LegoU32 DigitalMusicController::ResetFileAndStreamState()
{
	if (m_streamHeader == NULL) {
		return 0;
	}

	m_streamHeader->m_blockCount = 0;
	m_streamHeader->m_pcmReadPosition = 0;
	m_streamHeader->m_fileSamplePosition = 0;
	if (m_filePosition != m_fileDataOffset) {
		SetFilePointer(m_file, m_fileDataOffset, NULL, FILE_BEGIN);
	}
	return 1;
}

// STUB: LEGORACERS2 0x004f19b0
LegoU32 DigitalMusicController::StopAndReset()
{
	StopPlaybackIfActive();
	ResetFileAndStreamState();
	return 1;
}

// STUB: LEGORACERS2 0x004f19d0
LegoU32 DigitalMusicController::UpdatePlaybackBuffer(LegoU32 p_mode)
{
	LONG* state = (LONG*) &m_workerState;
	if (InterlockedExchange(state, 1) != 0) {
		if (p_mode == 2) {
			InterlockedIncrement((LONG*) &m_pendingChunkCount);
		}
		return 0;
	}

	LegoU32 bytesToLock = 0;
	LegoU32 playCursor = 0;
	if (m_playbackBuffer != NULL) {
		if (m_playing == 1) {
			if (p_mode == 1) {
				bytesToLock = m_bufferByteCount;
				m_playbackCursor = 0;
				m_pendingChunkCount = 0;
			}
			else if (p_mode == 0) {
				DWORD writeCursor = 0;
				if (m_playbackBuffer->GetCurrentPosition((DWORD*) &playCursor, &writeCursor) != DS_OK) {
					goto done;
				}
				if (playCursor > m_playbackCursor) {
					bytesToLock = playCursor - m_playbackCursor;
				}
				else {
					bytesToLock = m_bufferByteCount - m_playbackCursor + playCursor;
				}
			}
			else if (p_mode == 2) {
				LegoU32 chunks = ((LegoU32) (m_pendingChunkCount + 1)) & 0x80000007;
				if ((LegoS32) chunks < 0) {
					chunks = (chunks - 1 | 0xfffffff8) + 1;
				}
				bytesToLock = m_chunkByteCount * chunks;
				for (LegoU32 i = 1; i < chunks; ++i) {
					InterlockedDecrement((LONG*) &m_pendingChunkCount);
				}
			}
			else {
				goto done;
			}
		}
		else if (p_mode == 1) {
			bytesToLock = m_bufferByteCount;
			m_playbackCursor = 0;
			m_pendingChunkCount = 0;
		}
		else {
			goto done;
		}

		if ((LegoS32) bytesToLock > 0) {
			void* firstRegion = NULL;
			void* secondRegion = NULL;
			DWORD firstBytes = 0;
			DWORD secondBytes = 0;
			if (m_playbackBuffer
					->Lock(m_playbackCursor, bytesToLock, &firstRegion, &firstBytes, &secondRegion, &secondBytes, 0) ==
				DS_OK) {
				if (firstRegion != NULL) {
					CopyDecodedSamples((LegoS16*) firstRegion, firstBytes >> 1);
				}
				if (secondRegion != NULL) {
					CopyDecodedSamples((LegoS16*) secondRegion, secondBytes >> 1);
				}
				HRESULT unlockResult = m_playbackBuffer->Unlock(firstRegion, firstBytes, secondRegion, secondBytes);
				if (p_mode == 0 && unlockResult == DS_OK) {
					m_playbackCursor = playCursor;
					InterlockedExchange(state, 0);
					return 1;
				}
				if (p_mode == 2) {
					m_playbackCursor += bytesToLock;
					if (m_playbackCursor >= m_bufferByteCount) {
						m_playbackCursor -= m_bufferByteCount;
					}
				}
			}
		}
	}

done:
	InterlockedExchange(state, 0);
	return 1;
}

// STUB: LEGORACERS2 0x004f1bc0
void DigitalMusicController::StopAndReleasePlaybackBuffer()
{
	StopAndReset();
	while (m_workerState == 1) {
	}
	if (m_playbackBuffer != NULL) {
		m_playbackBuffer->Release();
		m_playbackBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004f1d70
void DigitalMusicController::DecodeAdpcmBlock()
{
	const LegoS32 channels = m_waveFormat.nChannels;
	DigitalMusicPcmRing* ring = m_streamHeader;
	LegoS32 sampleCount = (LegoS32) (m_decodedSampleLimit - ring->m_fileSamplePosition);
	if (sampleCount > (LegoS32) m_samplesPerBlockTwice) {
		sampleCount = m_samplesPerBlockTwice;
	}

	ReadFileBytes(ring->m_adpcmBlock, (LegoU32) (sampleCount / channels + m_blockHeaderBytes));

	LegoU8* block = ring->m_adpcmBlock;
	LegoS16* output = ring->m_pcmSamples;
	LegoS32 predictor[2] = {0, 0};
	LegoS32 delta[2] = {0, 0};
	LegoS32 sample1[2] = {0, 0};
	LegoS32 sample2[2] = {0, 0};
	if (channels == 1) {
		predictor[0] = block[0];
		delta[0] = *(LegoS16*) (block + 1);
		sample1[0] = *(LegoS16*) (block + 3);
		sample2[0] = *(LegoS16*) (block + 5);
		output[0] = (LegoS16) sample2[0];
		output[1] = (LegoS16) sample1[0];
	}
	else {
		predictor[0] = block[0];
		predictor[1] = block[1];
		delta[0] = *(LegoS16*) (block + 2);
		delta[1] = *(LegoS16*) (block + 4);
		sample1[0] = *(LegoS16*) (block + 6);
		sample1[1] = *(LegoS16*) (block + 8);
		sample2[0] = *(LegoS16*) (block + 10);
		sample2[1] = *(LegoS16*) (block + 12);
		output[0] = (LegoS16) sample2[0];
		output[1] = (LegoS16) sample2[1];
		output[2] = (LegoS16) sample1[0];
		output[3] = (LegoS16) sample1[1];
	}

	LegoS32 sampleIndex = channels * 2;
	LegoU8* compressed = block + m_blockHeaderBytes;
	for (; sampleIndex < sampleCount; ++sampleIndex) {
		LegoS32 channel = sampleIndex & (channels - 1);
		LegoS32 nibbleIndex = sampleIndex - channels * 2;
		LegoU8 packed = compressed[nibbleIndex >> 1];
		LegoS32 nibble = (nibbleIndex & 1) == 0 ? (packed >> 4) : (packed & 0x0f);
		if (nibble >= 8) {
			nibble -= 16;
		}

		LegoS32 adaptedDelta = (g_msAdpcmAdaptation[nibble & 0x0f] * delta[channel]) >> 8;
		if (adaptedDelta < 16) {
			adaptedDelta = 16;
		}
		LegoS32 prediction = (sample1[channel] * g_msAdpcmCoefficient1[predictor[channel]] +
							  sample2[channel] * g_msAdpcmCoefficient2[predictor[channel]]) >>
							 8;
		LegoS32 sample = ClampAdpcmSample(prediction + adaptedDelta * nibble);
		output[sampleIndex] = (LegoS16) sample;
		sample1[channel] = sample2[channel];
		sample2[channel] = sample;
		delta[channel] = adaptedDelta;
	}

	ring->m_blockCount++;
	ring->m_fileSamplePosition += sampleCount;
	ring->m_pcmCapacity = sampleCount;
}

// STUB: LEGORACERS2 0x004f1fa0
LegoS32 DigitalMusicController::CopyDecodedSamples(LegoS16* p_destination, LegoS32 p_sampleCount)
{
	LegoS32 copied = 0;
	while (copied < p_sampleCount) {
		if (m_streamHeader->m_fileSamplePosition >= m_decodedSampleLimit) {
			if (m_loopEnabled == 0) {
				memset(p_destination + copied, 0, (p_sampleCount - copied) * sizeof(LegoS16));
				return copied;
			}
			ResetFileAndStreamState();
		}
		if (m_streamHeader->m_pcmReadPosition == 0) {
			DecodeAdpcmBlock();
		}
		LegoU32 available = m_streamHeader->m_pcmCapacity - m_streamHeader->m_pcmReadPosition;
		LegoU32 count = (LegoU32) (p_sampleCount - copied);
		if (available <= count) {
			count = available;
		}
		memcpy(
			p_destination + copied,
			m_streamHeader->m_pcmSamples + m_streamHeader->m_pcmReadPosition,
			count * sizeof(LegoS16)
		);
		copied += count;
		m_streamHeader->m_pcmReadPosition += count;
		if (m_streamHeader->m_pcmReadPosition >= m_streamHeader->m_pcmCapacity) {
			m_streamHeader->m_pcmReadPosition -= m_streamHeader->m_pcmCapacity;
		}
	}
	return copied;
}

// STUB: LEGORACERS2 0x004f2070
LegoU32 DigitalMusicController::ReadFileBytes(void* p_destination, LegoU32 p_bytes)
{
	DWORD bytesRead = 0;
	if (!ReadFile(m_file, p_destination, p_bytes, &bytesRead, NULL)) {
		return 0;
	}
	if (bytesRead != p_bytes) {
		return 0;
	}
	m_filePosition += bytesRead;
	return 1;
}

// STUB: LEGORACERS2 0x004f20c0
void DigitalMusicController::SkipFileBytes(LegoU32 p_bytes)
{
	SetFilePointer(m_file, p_bytes, NULL, FILE_CURRENT);
	m_filePosition += p_bytes;
}

void __fastcall SetDigitalMusicControllerEnabled(DigitalMusicController* p_controller, void*, LegoU32 p_enabled)
{
	p_controller->SetEnabled(p_enabled);
}
