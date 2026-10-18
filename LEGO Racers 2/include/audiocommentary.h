#ifndef LR2_AUDIOCOMMENTARY_H
#define LR2_AUDIOCOMMENTARY_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>
#include <vorbis/codec.h>

class AudioService;

class CommentaryAudioStream {
public:
	CommentaryAudioStream(LPCSTR p_path, IDirectSound8* p_device, LegoU32 p_rate, LegoU32 p_channels);
	~CommentaryAudioStream();
	void Play(LONG p_offset, DWORD p_bytes);
	void Stop();
	void SetGain(LegoFloat p_gain);
	LegoU32 IsReading();
	void UpdateBuffer();
	void StartRead();
	void FinishRead();
	LegoS32 DecodePcm(void* p_output, LegoS32 p_capacity);
	void ReleasePlaybackBuffer();

	LegoU32 m_state;              // 0x00
	LegoU32 m_playing;            // 0x04
	LONG m_startOffset;           // 0x08
	DWORD m_byteLength;           // 0x0c
	IDirectSoundBuffer* m_buffer; // 0x10
	LegoU32 m_reading;            // 0x14
	HANDLE m_file;                // 0x18
	DWORD m_filePosition;         // 0x1c
	LegoU32 m_unk0x20;
	LegoU32 m_unk0x24;
	LegoU8 m_decodeBuffer[0x2000];
	ogg_sync_state m_sync; // 0x2028
	LegoU32 m_unk0x2044;
	ogg_stream_state m_oggStream; // 0x2048
	ogg_page m_page;              // 0x21b0
	ogg_packet m_packet;          // 0x21c0
	vorbis_info m_info;           // 0x21e0
	vorbis_comment m_comment;     // 0x2200
	vorbis_dsp_state m_decoder;   // 0x2210
	vorbis_block m_block;         // 0x2280
	char* m_input;                // 0x22f0
	short m_pcm[0x1000];          // 0x22f4
	DWORD m_readBufferSize;       // 0x42f4
};
DECOMP_SIZE_ASSERT(CommentaryAudioStream, 0x42f8);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_decodeBuffer) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_sync) == 0x2028);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_oggStream) == 0x2048);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_page) == 0x21b0);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_packet) == 0x21c0);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_info) == 0x21e0);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_comment) == 0x2200);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_decoder) == 0x2210);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_block) == 0x2280);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_input) == 0x22f0);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_pcm) == 0x22f4);
DECOMP_STATIC_ASSERT(offsetof(CommentaryAudioStream, m_readBufferSize) == 0x42f4);

// Evidenced AudioService tail beginning at +0x24ba0.
struct AudioCommentaryState {
	LegoU32 m_active;
	CommentaryAudioStream* m_stream;
	LegoS32 m_head;
	LegoS32 m_count;
	DWORD m_byteCounts[8];
	LONG m_fileOffsets[8];
	LegoU32 m_tags[8];
	IDirectSoundBuffer* m_pcmBuffer; // +0x70
	LegoU32 m_pcmPlaying;
	LegoU32 m_pcmPhase;
	DWORD m_pcmBufferSize;
	LegoS32 m_pcmWritePosition;
	LegoU8 m_pcmStaging[0x5a00];
};
DECOMP_STATIC_ASSERT(offsetof(AudioCommentaryState, m_pcmBuffer) == 0x70);
DECOMP_STATIC_ASSERT(offsetof(AudioCommentaryState, m_pcmStaging) == 0x84);
DECOMP_SIZE_ASSERT(AudioCommentaryState, 0x5a84);

void __fastcall InitializeAudioCommentary(AudioService*, void*, LPCSTR p_path);
void __fastcall ShutdownAudioCommentary(AudioService*, void*);
void __fastcall SetAudioCommentaryGain(AudioService*, void*, LegoFloat p_gain);
void __fastcall QueueAudioCommentary(AudioService*, void*, LONG p_offset, DWORD p_bytes);
void __fastcall StopAudioCommentary(AudioService*, void*, LegoU32 p_clearQueue);
LegoBool32 __fastcall EnqueueAudioCommentary(AudioService*, void*, LONG p_offset, DWORD p_bytes);
LegoBool32 __fastcall PlayNextAudioCommentary(AudioService*, void*);
void __fastcall InitializeAudioPcmBuffer(AudioService*, void*, WAVEFORMATEX* p_format);
void __fastcall ShutdownAudioPcmBuffer(AudioService*, void*);
void __fastcall AppendAudioPcmData(AudioService*, void*, const void* p_data, LegoS32 p_bytes);
LegoBool32 __fastcall CheckAudioMediaResult(AudioService*, void*, HRESULT p_result);

void CopyAudioBytes(void* p_destination, const void* p_source, DWORD p_bytes);
void ClearAudioBytes(void* p_destination, DWORD p_bytes);

#endif // LR2_AUDIOCOMMENTARY_H
