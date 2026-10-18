#include "audiocommentary.h"

// GLOBAL: LEGORACERS2 0x0054b728
DWORD g_commentaryWriteCursor = 0x1000;

// STUB: LEGORACERS2 0x004f2110
CommentaryAudioStream::CommentaryAudioStream(LPCSTR p_path, IDirectSound8* p_device, LegoU32 p_rate, LegoU32 p_channels)
{
	m_reading = 0;
	m_startOffset = 0;
	m_byteLength = 0;
	m_state = 0;
	m_buffer = NULL;
	m_unk0x20 = 0;
	m_playing = 0;
	m_readBufferSize = 0x1000;
	m_file = CreateFile(p_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0x08000080, NULL);
	if (m_file == INVALID_HANDLE_VALUE) {
		DebugBreak();
		return;
	}
	m_filePosition = 0;
	WAVEFORMATEX format;
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
	if (p_device->CreateSoundBuffer(&description, &m_buffer, NULL) != DS_OK) {
		DebugBreak();
	}
	void* data = NULL;
	DWORD bytes = 0;
	if (m_buffer->Lock(0, 0, &data, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER) == DS_OK) {
		ClearAudioBytes(data, bytes);
		m_buffer->Unlock(data, bytes, NULL, 0);
	}
}

// FUNCTION: LEGORACERS2 0x004f2250
CommentaryAudioStream::~CommentaryAudioStream()
{
	Stop();
	m_buffer->Release();
	CloseHandle(m_file);
}

// STUB: LEGORACERS2 0x004f2270
void CommentaryAudioStream::UpdateBuffer()
{
	if (m_state == 0) {
		return;
	}
	DWORD writeCursor = 0;
	DWORD playCursor = 0;
	m_buffer->GetCurrentPosition(&playCursor, &writeCursor);
	if ((playCursor <= g_commentaryWriteCursor || g_commentaryWriteCursor + 0x2000 <= playCursor) &&
		(g_commentaryWriteCursor <= playCursor || g_commentaryWriteCursor + 0x2000 < 0x5623 ||
		 g_commentaryWriteCursor - 0x3622 <= playCursor)) {
		LegoS32 bytes = DecodePcm(m_decodeBuffer, 0x2000);
		bool silence = bytes < 1 || m_reading == 0;
		if (silence) {
			ClearAudioBytes(m_decodeBuffer, 0x2000);
			m_unk0x20 = 1;
			bytes = 0x2000;
		}
		void* first;
		void* second;
		DWORD firstBytes;
		DWORD secondBytes;
		HRESULT result = m_buffer->Lock(g_commentaryWriteCursor, bytes, &first, &firstBytes, &second, &secondBytes, 0);
		if (result == DS_OK) {
			CopyAudioBytes(first, m_decodeBuffer, firstBytes);
			g_commentaryWriteCursor += firstBytes;
			if (secondBytes != 0) {
				CopyAudioBytes(second, m_decodeBuffer + firstBytes, secondBytes);
				g_commentaryWriteCursor = secondBytes;
			}
			m_buffer->Unlock(first, firstBytes, second, secondBytes);
			if (g_commentaryWriteCursor >= 0x5622) {
				g_commentaryWriteCursor = 0;
			}
		}
		else if (!silence) {
			DebugBreak();
			return;
		}
		if (silence) {
			m_reading = 0;
		}
	}
}

// STUB: LEGORACERS2 0x004f2450
void CommentaryAudioStream::Play(LONG p_offset, DWORD p_bytes)
{
	if (p_bytes == 0xffffffff) {
		p_bytes = GetFileSize(m_file, NULL);
		if (p_bytes == INVALID_FILE_SIZE) {
			DebugBreak();
			return;
		}
	}
	m_reading = 1;
	m_unk0x20 = 0;
	m_startOffset = p_offset;
	m_byteLength = p_bytes;
	SetFilePointer(m_file, p_offset, NULL, FILE_BEGIN);
	m_filePosition = p_offset;
	if (m_state != 0) {
		FinishRead();
	}
	if (m_state == 0) {
		StartRead();
	}
	UpdateBuffer();
	if (m_buffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) {
		DebugBreak();
		return;
	}
	m_playing = 1;
}

// STUB: LEGORACERS2 0x004f24e0
void CommentaryAudioStream::Stop()
{
	m_playing = 0;
	m_buffer->Stop();
	void* data = NULL;
	DWORD bytes = 0;
	if (m_buffer->Lock(0, 0, &data, &bytes, NULL, NULL, DSBLOCK_ENTIREBUFFER) == DS_OK) {
		ClearAudioBytes(data, bytes);
		m_buffer->Unlock(data, bytes, NULL, 0);
	}
}

// STUB: LEGORACERS2 0x004f2560
void CommentaryAudioStream::SetGain(LegoFloat p_gain)
{
	m_buffer->SetVolume((LONG) (((p_gain * 4.0f) - 4.0f) * 1000.0f));
}

// FUNCTION: LEGORACERS2 0x004f2590 FOLDED
LegoU32 CommentaryAudioStream::IsReading()
{
	return m_reading;
}
