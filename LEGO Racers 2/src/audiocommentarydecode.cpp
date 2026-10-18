#include "audiocommentary.h"
#include "audioreadrequests.h"

#include <stdlib.h>

namespace
{
LegoS32 RoundPcmSample(LegoFloat p_sample)
{
	double scaled = p_sample * 32767.0f;
	LegoS32 value;
	__asm {
  fld scaled
  fistp value
	}
	if (value > 32767)
	{
		value = 32767;
	}
	else if (value < -32768)
	{
		value = -32768;
	}
	return value;
}

LegoBool32 DrainDecodedPackets(CommentaryAudioStream* p_stream, void* p_output, LegoS32 p_capacity, LegoS32& p_total)
{
	while (true) {
		int packetResult = ogg_stream_packetout(&p_stream->m_oggStream, &p_stream->m_packet);
		if (packetResult == 0) {
			return FALSE;
		}
		if (packetResult < 0) {
			continue;
		}
		if (vorbis_synthesis(&p_stream->m_block, &p_stream->m_packet) == 0) {
			vorbis_synthesis_blockin(&p_stream->m_decoder, &p_stream->m_block);
		}
		int available = vorbis_synthesis_pcmout(&p_stream->m_decoder, NULL);
		int samples =
			available < (LegoS32) p_stream->m_readBufferSize ? available : (LegoS32) p_stream->m_readBufferSize;
		bool full = p_capacity < p_total + samples * p_stream->m_info.channels * 2;
		float** pcm;
		available = vorbis_synthesis_pcmout(&p_stream->m_decoder, &pcm);
		if (full) {
			return TRUE;
		}
		while (available > 0) {
			samples =
				available < (LegoS32) p_stream->m_readBufferSize ? available : (LegoS32) p_stream->m_readBufferSize;
			for (int channel = 0; channel < p_stream->m_info.channels; ++channel) {
				short* output = p_stream->m_pcm + channel;
				float* input = pcm[channel];
				for (int sample = 0; sample < samples; ++sample) {
					*output = (short) RoundPcmSample(*input++);
					output += p_stream->m_info.channels;
				}
			}
			int bytes = samples * p_stream->m_info.channels * 2;
			CopyAudioBytes((LegoU8*) p_output + p_total, p_stream->m_pcm, bytes);
			p_total += bytes;
			vorbis_synthesis_read(&p_stream->m_decoder, samples);
			available = vorbis_synthesis_pcmout(&p_stream->m_decoder, &pcm);
		}
	}
}
} // namespace

// STUB: LEGORACERS2 0x004f2740
void CommentaryAudioStream::StartRead()
{
	ogg_sync_init(&m_sync);
	m_input = ogg_sync_buffer(&m_sync, 0x1000);
	DWORD bytesRead;
	LegoS32 request = BeginAudioRead(m_file, m_filePosition, m_input, 0x1000);
	if (!WaitForAudioRead(request, &bytesRead)) {
		return;
	}
	m_filePosition += bytesRead;
	ogg_sync_wrote(&m_sync, bytesRead);
	if (ogg_sync_pageout(&m_sync, &m_page) != 1) {
		if (bytesRead < 0x1000) {
			return;
		}
		exit(1);
	}
	ogg_stream_init(&m_oggStream, ogg_page_serialno(&m_page));
	vorbis_info_init(&m_info);
	vorbis_comment_init(&m_comment);
	if (ogg_stream_pagein(&m_oggStream, &m_page) < 0) {
		exit(1);
	}
	if (ogg_stream_packetout(&m_oggStream, &m_packet) != 1) {
		exit(1);
	}
	if (vorbis_synthesis_headerin(&m_info, &m_comment, &m_packet) < 0) {
		exit(1);
	}
	int headers = 0;
	do {
		while (headers < 2) {
			int pageResult = ogg_sync_pageout(&m_sync, &m_page);
			if (pageResult == 0) {
				break;
			}
			if (pageResult == 1) {
				ogg_stream_pagein(&m_oggStream, &m_page);
				while (headers < 2) {
					int packetResult = ogg_stream_packetout(&m_oggStream, &m_packet);
					if (packetResult == 0) {
						break;
					}
					if (packetResult < 0) {
						exit(1);
					}
					vorbis_synthesis_headerin(&m_info, &m_comment, &m_packet);
					++headers;
				}
			}
		}
		m_input = ogg_sync_buffer(&m_sync, 0x1000);
		request = BeginAudioRead(m_file, m_filePosition, m_input, 0x1000);
		if (!WaitForAudioRead(request, &bytesRead)) {
			DebugBreak();
			return;
		}
		m_filePosition += bytesRead;
		if (bytesRead == 0 && headers < 2) {
			exit(1);
		}
		ogg_sync_wrote(&m_sync, bytesRead);
	} while (headers < 2);
	m_readBufferSize = 0x1000 / m_info.channels;
	vorbis_synthesis_init(&m_decoder, &m_info);
	vorbis_block_init(&m_decoder, &m_block);
	m_state = 1;
}

// STUB: LEGORACERS2 0x004f2990
void CommentaryAudioStream::FinishRead()
{
	m_state = 0;
	ogg_stream_clear(&m_oggStream);
	vorbis_block_clear(&m_block);
	vorbis_dsp_clear(&m_decoder);
	vorbis_comment_clear(&m_comment);
	vorbis_info_clear(&m_info);
	ogg_sync_clear(&m_sync);
}

// STUB: LEGORACERS2 0x004f29f0
LegoS32 CommentaryAudioStream::DecodePcm(void* p_output, LegoS32 p_capacity)
{
	LegoS32 total = 0;
	bool end = false;
	if (ogg_stream_packetpeek(&m_oggStream, NULL) == 1 && DrainDecodedPackets(this, p_output, p_capacity, total)) {
		return total;
	}
	while (!end) {
		int pageResult = ogg_sync_pageout(&m_sync, &m_page);
		if (pageResult == 0) {
			if (CountPendingAudioReads() < 1) {
				BeginAudioRead(m_file, m_filePosition, NULL, 0x1000);
			}
			m_filePosition += 0x1000;
			LegoS32 index = 0;
			while (FindCompletedAudioRead(&index) == TRUE) {
				AudioReadRequest* request = GetAudioReadRequest(index);
				if (request->m_bytesRead == 0) {
					end = true;
				}
				else {
					m_input = ogg_sync_buffer(&m_sync, 0x1000);
					CopyAudioBytes(m_input, request->m_buffer, request->m_bytesRead);
					ogg_sync_wrote(&m_sync, request->m_bytesRead);
				}
			}
		}
		else if (pageResult > 0) {
			ogg_stream_pagein(&m_oggStream, &m_page);
			if (DrainDecodedPackets(this, p_output, p_capacity, total)) {
				return total;
			}
			if (ogg_page_eos(&m_page)) {
				break;
			}
		}
	}
	return total == 0 ? -1 : total;
}

// FUNCTION: LEGORACERS2 0x004f2ea0
void CommentaryAudioStream::ReleasePlaybackBuffer()
{
	Stop();
	m_buffer->Release();
	m_buffer = NULL;
}
