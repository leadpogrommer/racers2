#include "audio/service/audioinitializationcallbacks.h"

// STUB: LEGORACERS2 0x004af7b0
void __fastcall SetAudioParameters(AudioService* p_service, void*, LegoFloat p_paramA, LegoFloat p_paramB)
{
	p_service->m_hasAudioParameters = TRUE;
	p_service->m_unk0x2c = p_paramA * 4000.0f - 2000.0f;
	p_service->m_unk0x30 = p_paramB * 0.1f;
}
