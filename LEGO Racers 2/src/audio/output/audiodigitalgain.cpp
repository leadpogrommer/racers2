#include "audio/music/audiomusiccontroller.h"
#include "audio/sources/audiochannelreload.h"

#include <math.h>

// STUB: LEGORACERS2 0x004ef560
void __fastcall SetDigitalMusicGain(AudioService* p_service, void*, LegoFloat p_gain, LegoU32)
{
	p_service->m_unk0x249f8 = MapGameAudioGain(p_gain);
	if (p_service->m_unk0x24870 != 0 && p_service->m_unk0x249f4 != NULL) {
		SetDigitalMusicBufferGain((DigitalMusicController*) p_service->m_unk0x249f4, NULL, p_gain);
	}
}

// FUNCTION: LEGORACERS2 0x004ef5a0
void __fastcall GetDigitalMusicGain(AudioService* p_service, void*, LegoFloat* p_first, LegoFloat* p_second)
{
	LegoFloat value = p_service->m_unk0x249f8;
	*p_second = value;
	*p_first = value;
}

// STUB: LEGORACERS2 0x00503340
LegoFloat __cdecl MapGameAudioGain(LegoFloat p_gain)
{
	if (!(p_gain > 0.1f)) {
		return 0.0f;
	}
	if (p_gain >= 0.95f) {
		return 1.0f;
	}
	return log10((double) p_gain * 10.0);
}
