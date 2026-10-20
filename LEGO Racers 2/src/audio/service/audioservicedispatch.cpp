#include "audio/output/audioprovider.h"
#include "audio/service/audioinitializationcallbacks.h"
#include "audio/service/audioservice.h"
#include "audio/streaming/audiocommentary.h"
#include "memory/memoryheap.h"

#include <stdlib.h>

extern "C" int __cdecl _purecall(void);

void __fastcall AddAudioSource(
	AudioService* p_service,
	void*,
	const LegoU32* p_parameters,
	LegoU32 p_sourceId,
	LegoU32 p_unused
);
void __fastcall RemoveAudioSource(AudioService* p_service, void*, LegoU32 p_sourceId);
AudioSourceRecord* __fastcall FindAudioSource(AudioService* p_service, void*, LegoU32 p_sourceId, LPCSTR p_name);
LegoU32 __fastcall ValidateAudioSource(AudioService* p_service, void*, LegoU32 p_sourceIndex);
void __fastcall ReleaseAudioSourceRecord(AudioService* p_service, void*, AudioSourceRecord* p_source);

static AudioService* __fastcall DeleteBaseAudioService(AudioService* p_service, void*, LegoU32 p_delete)
{
	p_service->m_vtable = &g_audioServiceBaseDispatch;
	if ((p_delete & 1) != 0) {
		FreeHeapMemory(p_service);
	}
	return p_service;
}

static AudioService* __fastcall DeleteOperationalAudioService(AudioService* p_service, void*, LegoU32 p_delete)
{
	p_service->Shutdown();
	if ((p_delete & 1) != 0) {
		FreeHeapMemory(p_service);
	}
	return p_service;
}

static void __fastcall NoAudioServiceAction(AudioService*, void*)
{
}

static LegoU32 __fastcall NoAudioServiceResult(AudioService*, void*)
{
	return FALSE;
}

static void __fastcall NoAudioServiceOneWord(AudioService*, void*, LegoU32)
{
}

static void __fastcall NoAudioServiceTwoWords(AudioService*, void*, LegoU32, LegoU32)
{
}

static void __fastcall NoAudioServiceThreeWords(AudioService*, void*, LegoU32, LegoU32, LegoU32)
{
}

static void __fastcall SetAudioServiceParameters(AudioService* p_service, void*, LegoFloat p_first, LegoFloat p_second)
{
	SetAudioParameters(p_service, NULL, p_first, p_second);
}

static void __fastcall ShutdownOperationalAudioService(AudioService* p_service, void*)
{
	p_service->Shutdown();
}

static void __fastcall RestartOperationalAudioService(AudioService* p_service, void*)
{
	p_service->Restart();
}

static void __fastcall UpdateOperationalAudioService(AudioService* p_service, void*, LegoU32 p_frame)
{
	p_service->Update(p_frame);
}

static void __fastcall GetAudioQualityMode(AudioService* p_service, void*, LegoU32* p_mode)
{
	p_service->GetQualityMode(p_mode);
}

static void __fastcall SetAudioQualityMode(AudioService* p_service, void*, LegoU32 p_mode)
{
	p_service->SetQualityMode(p_mode);
}

static void __fastcall SetAudioServiceVolumes(AudioService* p_service, void*, LegoFloat p_first, LegoFloat p_second)
{
	p_service->SetVolumes(p_first, p_second);
}

static void __fastcall GetAudioServiceVolumes(AudioService* p_service, void*, LegoFloat* p_first, LegoFloat* p_second)
{
	p_service->GetVolumes(p_first, p_second);
}

static void __fastcall SetAudioServiceMusicEnabled(AudioService* p_service, void*, LegoU32 p_enabled)
{
	p_service->SetMusicEnabled(p_enabled);
}

static void __fastcall PlayAudioCdTrack(AudioService* p_service, void*, LegoS32 p_track)
{
	p_service->PlayCdTrack(p_track);
}

static void __fastcall SetListenerField0x04(AudioService* p_service, void*, LegoU32 p_id, LegoU32 p_value)
{
	p_service->SetListenerField0x04(p_id, p_value);
}

static void __fastcall SetListenerField0x30(AudioService* p_service, void*, LegoU32 p_id, LegoU32 p_value)
{
	p_service->SetListenerField0x30(p_id, p_value);
}

static void __fastcall SetListenerGains(
	AudioService* p_service,
	void*,
	LegoU32 p_id,
	LegoFloat p_gainA,
	LegoFloat p_gainB
)
{
	p_service->SetListenerGains(p_id, p_gainA, p_gainB);
}

static void __fastcall AddAudioListener(
	AudioService* p_service,
	void*,
	const LegoU32* p_listenerData,
	LegoU32 p_listenerId
)
{
	p_service->AddListener(p_listenerData, p_listenerId);
}

static void __fastcall RemoveAudioListener(AudioService* p_service, void*, LegoU32 p_listenerId)
{
	p_service->RemoveListener(p_listenerId);
}

static void __fastcall SetAudioCommentaryPath(AudioService* p_service, void*, LPCSTR p_path)
{
	InitializeAudioCommentary(p_service, NULL, p_path);
}

static void __fastcall InvokeSetAudioCommentaryGain(AudioService* p_service, void*, LegoFloat p_gain)
{
	SetAudioCommentaryGain(p_service, NULL, p_gain);
}

static void __fastcall InvokeQueueAudioCommentary(AudioService* p_service, void*, LONG p_offset, DWORD p_bytes)
{
	QueueAudioCommentary(p_service, NULL, p_offset, p_bytes);
}

static void __fastcall InvokeStopAudioCommentary(AudioService* p_service, void*, LegoU32 p_clearQueue)
{
	StopAudioCommentary(p_service, NULL, p_clearQueue);
}

static void __fastcall InvokeShutdownAudioCommentary(AudioService* p_service, void*)
{
	ShutdownAudioCommentary(p_service, NULL);
}

static void __fastcall InvokeInitializeAudioPcmBuffer(AudioService* p_service, void*, WAVEFORMATEX* p_format)
{
	InitializeAudioPcmBuffer(p_service, NULL, p_format);
}

static void __fastcall InvokeShutdownAudioPcmBuffer(AudioService* p_service, void*)
{
	ShutdownAudioPcmBuffer(p_service, NULL);
}

static void __fastcall InvokeAppendAudioPcmData(AudioService* p_service, void*, const void* p_data, LegoS32 p_bytes)
{
	AppendAudioPcmData(p_service, NULL, p_data, p_bytes);
}

static void __fastcall SetDigitalAudioPath(AudioService* p_service, void*, LPCSTR p_path, LegoU32)
{
	p_service->SetDigitalAudioPath(p_path);
}

static void __fastcall SetMixerVolume(AudioService* p_service, void*, LegoFloat p_volume, LegoU32 p_unused)
{
	p_service->SetMixerVolume(p_volume, p_unused);
}

static void __fastcall GetMixerVolume(AudioService* p_service, void*, LegoFloat* p_volumeA, LegoFloat* p_volumeB)
{
	p_service->GetMixerVolume(p_volumeA, p_volumeB);
}

static void __fastcall SetCdPlayerMode(AudioService* p_service, void*, LegoU32 p_mode)
{
	p_service->SetCdPlayerMode(p_mode);
}

// Retail table 0x005359f4; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioServiceDispatch g_audioServiceOperationalDispatch = {
	DeleteOperationalAudioService,
	ShutdownOperationalAudioService,
	GetAudioServiceShutdownStatus,
	RestartOperationalAudioService,
	GetAudioQualityMode,
	SetAudioQualityMode,
	NoAudioServiceAction,
	UpdateOperationalAudioService,
	SetAudioServiceVolumes,
	GetAudioServiceVolumes,
	SetAudioServiceParameters,
	SetAudioServiceMusicEnabled,
	NoAudioServiceTwoWords,
	AddAudioSource,
	NoAudioServiceAction,
	ReleaseAudioSourceRecord,
	RemoveAudioSource,
	FindAudioSource,
	ValidateAudioSource,
	NoAudioServiceThreeWords,
	NoAudioServiceOneWord,
	NoAudioServiceOneWord,
	SetDigitalAudioPath,
	PlayAudioCdTrack,
	NoAudioServiceAction,
	SetCdPlayerMode,
	SetMixerVolume,
	GetMixerVolume,
	AddAudioListener,
	RemoveAudioListener,
	SetListenerField0x04,
	SetListenerField0x30,
	SetListenerGains,
	SetAudioCommentaryPath,
	InvokeSetAudioCommentaryGain,
	InvokeQueueAudioCommentary,
	InvokeStopAudioCommentary,
	InvokeShutdownAudioCommentary,
	NoAudioServiceAction,
	InvokeInitializeAudioPcmBuffer,
	InvokeShutdownAudioPcmBuffer,
	InvokeAppendAudioPcmData,
};

// Retail table 0x00535a9c; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioServiceDispatch g_audioServiceBaseDispatch = {
	DeleteBaseAudioService,
	NoAudioServiceAction,
	NoAudioServiceResult,
	NoAudioServiceAction,
	(AudioServiceDispatch::GetValueOut) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	NoAudioServiceAction,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetFloatPair) _purecall,
	(AudioServiceDispatch::GetFloatPair) _purecall,
	(AudioServiceDispatch::SetFloatPair) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetTwoWords) _purecall,
	(AudioServiceDispatch::AddAudioSource) _purecall,
	(AudioServiceDispatch::Action) _purecall,
	(AudioServiceDispatch::ReleaseSourceRecord) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::FindAudioSource) _purecall,
	(AudioServiceDispatch::ValidateAudioSource) _purecall,
	(AudioServiceDispatch::SetThreeWords) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetPathAndUnused) _purecall,
	(AudioServiceDispatch::SetSignedValue) _purecall,
	(AudioServiceDispatch::Action) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetMixerVolume) _purecall,
	(AudioServiceDispatch::GetFloatPair) _purecall,
	(AudioServiceDispatch::AddListener) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::SetTwoWords) _purecall,
	(AudioServiceDispatch::SetTwoWords) _purecall,
	(AudioServiceDispatch::SetListenerGains) _purecall,
	(AudioServiceDispatch::SetPath) _purecall,
	(AudioServiceDispatch::SetFloat) _purecall,
	(AudioServiceDispatch::SetOffsetAndLength) _purecall,
	(AudioServiceDispatch::SetValue) _purecall,
	(AudioServiceDispatch::Action) _purecall,
	NoAudioServiceAction,
	(AudioServiceDispatch::SetWaveFormat) _purecall,
	(AudioServiceDispatch::Action) _purecall,
	(AudioServiceDispatch::AppendAudioData) _purecall,
};
