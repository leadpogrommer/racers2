#ifndef LR2_AUDIOMUSICCONTROLLER_H
#define LR2_AUDIOMUSICCONTROLLER_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <mmsystem.h>
#include <stddef.h>
#include <windows.h>

struct DigitalMusicPcmRing {
	LegoU32 m_blockCount;
	LegoU32 m_pcmReadPosition;    // 0x04; sample index consumed by the playback buffer
	LegoU32 m_fileSamplePosition; // 0x08; decoded position in the WAVE data chunk
	LegoU32 m_pcmCapacity;        // 0x0c; sample capacity
	LegoU8* m_adpcmBlock;         // 0x10; compressed block scratch
	LegoS16* m_pcmSamples;        // 0x14; decoded sample ring
};

DECOMP_STATIC_ASSERT(offsetof(DigitalMusicPcmRing, m_pcmReadPosition) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicPcmRing, m_fileSamplePosition) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicPcmRing, m_pcmCapacity) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicPcmRing, m_adpcmBlock) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicPcmRing, m_pcmSamples) == 0x14);

// This object is allocated with a 0xb0-byte request by AudioService. The
// projection names only fields whose uses are established by the controller
// and its file/stream setup code.
class DigitalMusicController {
public:
	LegoU32 StartPlayback(LegoU32 p_ignored0, LegoU32 p_ignored1);
	LegoU32 StopPlaybackIfActive();
	LegoU32 ResetFileAndStreamState();
	LegoU32 SetEnabled(LegoU32 p_enabled);
	LegoU32 StopAndReset();
	LegoU32 UpdatePlaybackBuffer(LegoU32 p_mode);
	LegoS32 CopyDecodedSamples(LegoS16* p_destination, LegoS32 p_sampleCount);
	void DecodeAdpcmBlock();
	LegoU32 ReadFileBytes(void* p_destination, LegoU32 p_bytes);
	void SkipFileBytes(LegoU32 p_bytes);
	void StopAndReleasePlaybackBuffer();

	HANDLE m_file;                        // 0x00
	LegoU8 m_unknown0x04[4];              // 0x04 through 0x07
	LegoU32 m_filePosition;               // 0x08
	WAVEFORMATEX m_waveFormat;            // 0x0c
	LegoU8 m_formatExtension[0x22];       // 0x1e through 0x3f
	LegoU8 m_unknown0x40[0x10];           // 0x40 through 0x4f
	LegoU8 m_unknown0x50[4];              // 0x50 through 0x53
	DigitalMusicPcmRing* m_streamHeader;  // 0x54; allocated per WAVE format
	LegoU32 m_factSampleCount;            // 0x58; `fact` chunk sample count
	LegoU32 m_fileDataOffset;             // 0x5c
	LegoU32 m_formatExtensionSize;        // 0x60
	void* m_formatExtensionData;          // 0x64
	LegoU32 m_waveDataBytes;              // 0x68
	LegoU8 m_unknown0x6c[4];              // 0x6c through 0x6f
	IDirectSoundBuffer* m_playbackBuffer; // 0x70
	LegoU32 m_bufferByteCount;            // 0x74
	LegoU32 m_chunkByteCount;             // 0x78
	volatile LegoS32 m_pendingChunkCount; // 0x7c; incremented by the worker if the update lock is held
	HANDLE m_workerEvent;                 // 0x80
	HANDLE m_workerThread;                // 0x84
	DWORD m_workerThreadId;               // 0x88
	LegoU32 m_playbackCursor;             // 0x8c
	volatile LegoU32 m_workerState;       // 0x90; worker-shared, shutdown waits while equal to 1
	LegoU32 m_playing;                    // 0x94
	LegoU32 m_wasPlaying;                 // 0x98
	LegoU32 m_loopEnabled;                // 0x9c
	LegoU32 m_blockHeaderBytes;           // 0xa0
	LegoU32 m_samplesPerBlockTwice;       // 0xa4
	LegoU32 m_dataBlockCount;             // 0xa8
	LegoU32 m_decodedSampleLimit;         // 0xac
};

DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_streamHeader) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_playbackBuffer) == 0x70);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_workerState) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_playing) == 0x94);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_wasPlaying) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_loopEnabled) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_blockHeaderBytes) == 0xa0);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_samplesPerBlockTwice) == 0xa4);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_dataBlockCount) == 0xa8);
DECOMP_STATIC_ASSERT(offsetof(DigitalMusicController, m_decodedSampleLimit) == 0xac);
DECOMP_SIZE_ASSERT(DigitalMusicController, 0xb0);

void __fastcall SetDigitalMusicControllerEnabled(DigitalMusicController* p_controller, void* p_edx, LegoU32 p_enabled);

#endif // LR2_AUDIOMUSICCONTROLLER_H
