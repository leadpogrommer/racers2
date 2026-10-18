#include "audioservice.h"

#include "application.h"
#include "audioprovider.h"
#include "audiosettings.h"
#include "diagnostics.h"
#include "memoryheap.h"
#include "nativewindow.h"

#include <new.h>
#include <stdlib.h>

extern LegoU32 g_audioServiceExitCallbackRegistered;

// This accessor is recovered in the application/configuration slice. The
// AudioService constructor currently consumes but does not read its argument.
LegoFloat __fastcall GetAudioFrameDuration(AudioService* p_service, void* p_edx);
extern void* ConstructStereoAudioProvider(void* p_storage, HWND p_window);
extern void* ConstructThreeDimensionalAudioProvider(void* p_storage, HWND p_window);
void __fastcall InitializeAudioChannelRecord(AudioChannelRecord* p_record, void* p_edx);

// STUB: LEGORACERS2 0x004af2f0
AudioService* GetAudioService(LegoBool32 p_create)
{
	if (g_audioService == NULL && p_create == TRUE) {
		g_audioWindow = g_viewWindow;
		void* storage = AllocateHeapMemory(sizeof(AudioService));
		if (storage != NULL) {
			g_audioService = new (storage) AudioService(GetConfiguredAudioServiceMode());
		}
		else {
			g_audioService = NULL;
		}

		if (g_audioServiceExitCallbackRegistered != FALSE) {
			g_audioServiceExitCallbackRegistered = FALSE;
			atexit(DestroyAudioService);
		}
	}

	return g_audioService;
}

// STUB: LEGORACERS2 0x004af360
void DestroyAudioService()
{
	if (g_audioService != NULL) {
		g_audioService->m_vtable->m_deleteObject(g_audioService, NULL, 1);
		g_audioService = NULL;
	}
}

// STUB: LEGORACERS2 0x004af380
AudioService::AudioService(LegoU32 p_initialMode)
{
	(void) p_initialMode;
	m_vtable = &g_audioServiceBaseDispatch;

	LegoU32 i;
	for (i = 0; i < 512; ++i) {
		ConstructAudioSourceRecord(&m_sources[i], NULL);
	}
	for (i = 0; i < 256; ++i) {
		ConstructAudioChannelRecord(&m_channels[i], NULL);
	}
	for (i = 0; i < 8; ++i) {
		ConstructAudioDeviceRecord(&m_devices[i], NULL);
	}

	m_vtable = &g_audioServiceOperationalDispatch;
	Initialize();
}

// STUB: LEGORACERS2 0x004af4e0
void AudioService::Initialize()
{
	m_unk0x1c = 12;
	m_unk0x24ba0 = 0;
	m_unk0x24ba4 = 0;
	m_unk0x249f4 = NULL;
	m_unk0x249f8 = 0;
	m_usesDigitalAudio = 0;
	m_digitalPlaybackStarted = 0;
	m_currentCdTrack = 0;
	m_listenerCount = 0;
	m_unk0x24870 = 0;
	m_shutdown = 0;
	m_initializationFailed = 0;
	m_vtable->m_setParameters(this, NULL, 0.0f, 0.0f);
	m_elapsedTime = GetAudioFrameDuration(this, NULL);
	GetMemoryHeap()->m_useCRT = TRUE;

	void* providerStorage;
	switch (m_qualityMode) {
	case 0:
		providerStorage = AllocateHeapMemory(0x10);
		g_audioBackend = providerStorage != NULL ? (AudioBackendStateView*) new (providerStorage)
													   MonoDirectSoundProvider(g_audioWindow)
												 : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_initializationFailed = 1;
			if (g_audioBackend != NULL) {
				((const AudioBackendStateDispatch*) g_audioBackend->m_vtable)->m_destroy(g_audioBackend, NULL, 1);
				g_audioBackend = NULL;
			}
		}
		break;
	case 1:
		providerStorage = AllocateHeapMemory(0x10);
		g_audioBackend = providerStorage != NULL
							 ? (AudioBackendStateView*) ConstructStereoAudioProvider(providerStorage, g_audioWindow)
							 : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 0);
		}
		break;
	case 2:
		providerStorage = AllocateHeapMemory(0x24);
		g_audioBackend =
			providerStorage != NULL
				? (AudioBackendStateView*) ConstructThreeDimensionalAudioProvider(providerStorage, g_audioWindow)
				: NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 1);
		}
		break;
	case 3:
		m_initializationFailed = 1;
		if (g_audioBackend != NULL) {
			((const AudioBackendStateDispatch*) g_audioBackend->m_vtable)->m_destroy(g_audioBackend, NULL, 1);
			g_audioBackend = NULL;
		}
		break;
	default:
		providerStorage = AllocateHeapMemory(0x10);
		g_audioBackend = providerStorage != NULL
							 ? (AudioBackendStateView*) ConstructStereoAudioProvider(providerStorage, g_audioWindow)
							 : NULL;
		if (g_audioBackend->m_initializationStatus == 0) {
			m_vtable->m_setQuality(this, NULL, 0);
		}
		break;
	}

	GetMemoryHeap()->m_useCRT = FALSE;
	InitializeAudioSources(this, NULL);
	InitializeAudioChannels(this, NULL);
	InitializeAudioDevices(this, NULL);
	m_vtable->m_setVolumes(this, NULL, 1.0f, 1.0f);
	m_vtable->m_setParameters(this, NULL, 0.0f, 0.0f);
	m_currentTrack = -1;
	m_vtable->m_setMusicEnabled(this, NULL, 0);
}

// STUB: LEGORACERS2 0x004af6c0
void AudioService::GetQualityMode(LegoU32* p_mode)
{
	*p_mode = m_qualityMode;
}

// STUB: LEGORACERS2 0x004af6d0
void AudioService::SetQualityMode(LegoU32 p_mode)
{
	if (m_qualityMode == p_mode) {
		return;
	}

	m_qualityMode = p_mode;
	if (m_initializationFailed == 0 && g_audioBackend->m_initializationStatus != 0) {
		m_vtable->m_shutdown(this, NULL);
	}
	m_vtable->m_restart(this, NULL);
}

// STUB: LEGORACERS2 0x004af710
void AudioService::SetVolumes(LegoFloat p_volumeA, LegoFloat p_volumeB)
{
	m_volumeA = 0.0f;
	if (0.0f <= p_volumeA) {
		m_volumeA = p_volumeA;
		if (1.0f < p_volumeA) {
			m_volumeA = 1.0f;
		}
	}

	if (p_volumeB < 0.0f) {
		m_volumeB = 0.0f;
	}
	else if (p_volumeB <= 1.0f) {
		m_volumeB = p_volumeB;
	}
	else {
		m_volumeB = 1.0f;
	}
}

// STUB: LEGORACERS2 0x004af790
void AudioService::GetVolumes(LegoFloat* p_volumeA, LegoFloat* p_volumeB)
{
	*p_volumeA = m_volumeA;
	*p_volumeB = m_volumeB;
}

// STUB: LEGORACERS2 0x004b02d0
void __fastcall InitializeAudioSources(AudioService* p_service, void*)
{
	for (LegoS32 i = 0; i < 512; ++i) {
		InitializeAudioSourceRecordFields(&p_service->m_sources[i], NULL);
		p_service->m_sources[i].m_recordIndex = i;
	}
}

// STUB: LEGORACERS2 0x004b0340
AudioSourceRecord* __fastcall ConstructAudioSourceRecord(AudioSourceRecord* p_record, void*)
{
	p_record->m_vtable = &g_audioSourceRecordBaseDispatch;
	ZeroMemory(p_record->m_unkName0x4c, sizeof(p_record->m_unkName0x4c));
	ZeroMemory(p_record->m_unkName0x64, sizeof(p_record->m_unkName0x64));
	p_record->m_vtable = &g_audioSourceRecordOperationalDispatch;
	InitializeAudioSourceRecordFields(p_record, NULL);
	return p_record;
}

// STUB: LEGORACERS2 0x004b03b0
void __fastcall InitializeAudioSourceRecordFields(AudioSourceRecord* p_record, void*)
{
	p_record->m_unk0xd0 = 0;
	p_record->m_unk0x2c = 0;
	p_record->m_unk0x30 = 0;
	p_record->m_unk0x34 = 0;
	p_record->m_unk0x3c = 0;
	p_record->m_unk0x40 = 1;
	p_record->m_unk0x48 = 1;
	p_record->m_unk0x44 = 0;
	p_record->m_unk0xb8 = 0;
	ZeroMemory(p_record->m_unkName0x4c, sizeof(p_record->m_unkName0x4c));
	ZeroMemory(p_record->m_unkName0x64, sizeof(p_record->m_unkName0x64));
	p_record->m_unk0x7c = 1.0f;
	p_record->m_unk0x80 = 1.0f;
	p_record->m_unk0x84 = 1.0f;
	ZeroMemory(p_record->m_unk0x90, sizeof(p_record->m_unk0x90));
	p_record->m_defaultRange = 150.0f;
	p_record->m_defaultFalloff = 30.0f;
	p_record->m_channelResource = NULL;
	p_record->m_playbackBuffer = NULL;
	p_record->m_unk0xc4 = 0;
	p_record->m_unk0xc8 = 0;
	p_record->m_unk0xcc = 0;
	p_record->m_unk0x24 = 0;
	p_record->m_unk0x20 = 0;
	p_record->m_unk0x1c = 0;
	p_record->m_unk0x18 = 0;
	p_record->m_unk0x14 = 0;
	p_record->m_unk0x10 = 0;
	p_record->m_unk0x0c = 0;
	p_record->m_unk0x08 = 0;
	p_record->m_unk0x04 = 0;
	p_record->m_unk0xac = 0;
	p_record->m_unk0xb0 = 0;
	p_record->m_unk0xb4 = 1.0f;
}

// STUB: LEGORACERS2 0x004b07c0
void __fastcall InitializeAudioChannels(AudioService* p_service, void*)
{
	for (LegoS32 i = 0; i < 256; ++i) {
		InitializeAudioChannelRecord(&p_service->m_channels[i], NULL);
	}
}

// STUB: LEGORACERS2 0x004b0a20
AudioChannelRecord* __fastcall ConstructAudioChannelRecord(AudioChannelRecord* p_record, void*)
{
	ZeroMemory(p_record, sizeof(*p_record));
	return p_record;
}

// STUB: LEGORACERS2 0x004b0a40
void __fastcall InitializeAudioChannelRecord(AudioChannelRecord* p_record, void*)
{
	ZeroMemory(p_record, sizeof(*p_record));
}

// STUB: LEGORACERS2 0x004b0c20
void AudioService::AddListener(const LegoU32* p_listenerData, LegoU32 p_listenerId)
{
	AudioDeviceRecord* listener = FindAvailableListener();
	if (listener != NULL) {
		for (LegoS32 i = 0; i < 6; ++i) {
			listener->m_unk0x0c[i] = p_listenerData[i];
		}
		listener->m_active = 1;
		listener->m_listenerId = p_listenerId;
		++m_listenerCount;
	}
}

// STUB: LEGORACERS2 0x004b0c60
void AudioService::RemoveListener(LegoU32 p_listenerId)
{
	AudioDeviceRecord* listener = FindListenerById(p_listenerId);
	if (listener != NULL) {
		listener->m_active = 0;
		--m_listenerCount;
		if (m_listenerCount < 0) {
			ReportFatalError("Invalid amount of listeners <0");
		}
	}
}

// STUB: LEGORACERS2 0x004b0ca0
void AudioService::SetListenerField0x04(LegoU32 p_listenerId, LegoU32 p_value)
{
	AudioDeviceRecord* listener = FindListenerById(p_listenerId);
	if (listener != NULL) {
		listener->m_unk0x04 = p_value;
	}
}

// STUB: LEGORACERS2 0x004b0cc0
void AudioService::SetListenerField0x30(LegoU32 p_listenerId, LegoU32 p_value)
{
	AudioDeviceRecord* listener = FindListenerById(p_listenerId);
	if (listener != NULL) {
		listener->m_unk0x30 = p_value;
	}
}

// STUB: LEGORACERS2 0x004b0ce0
void AudioService::SetListenerGains(LegoU32 p_listenerId, LegoFloat p_gainA, LegoFloat p_gainB)
{
	AudioDeviceRecord* listener = FindListenerById(p_listenerId);
	if (listener == NULL) {
		return;
	}

	if (!(p_gainA >= -1.0f)) {
		listener->m_gainA = -1.0f;
	}
	else if (p_gainA <= 1.0f) {
		listener->m_gainA = p_gainA;
	}
	else {
		listener->m_gainA = 1.0f;
	}

	if (!(p_gainB >= -1.0f)) {
		listener->m_gainB = -1.0f;
	}
	else if (p_gainB <= 1.0f) {
		listener->m_gainB = p_gainB;
	}
	else {
		listener->m_gainB = 1.0f;
	}
}

// STUB: LEGORACERS2 0x004b0d80
void __fastcall InitializeAudioDevices(AudioService* p_service, void*)
{
	for (LegoS32 i = 0; i < 8; ++i) {
		InitializeAudioDeviceRecord(&p_service->m_devices[i], NULL);
		p_service->m_devices[i].m_unk0x08 = i;
	}
}

// STUB: LEGORACERS2 0x004b0da0
AudioDeviceRecord* AudioService::FindAvailableListener()
{
	for (LegoS32 i = 0; i < 8; ++i) {
		if (m_devices[i].m_active == 0) {
			return &m_devices[i];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b0dc0
AudioDeviceRecord* AudioService::FindListenerById(LegoU32 p_listenerId)
{
	for (LegoS32 i = 0; i < 8; ++i) {
		if (m_devices[i].m_active != 0 && m_devices[i].m_listenerId == p_listenerId) {
			return &m_devices[i];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b0fa0
AudioDeviceRecord* __fastcall ConstructAudioDeviceRecord(AudioDeviceRecord* p_record, void*)
{
	InitializeAudioDeviceRecord(p_record, NULL);
	return p_record;
}

// STUB: LEGORACERS2 0x004b0fc0
void __fastcall InitializeAudioDeviceRecord(AudioDeviceRecord* p_record, void*)
{
	ZeroMemory(p_record, sizeof(*p_record));
}

// STUB: LEGORACERS2 0x004eec50
void AudioService::PlayCdTrack(LegoS32 p_track)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}

	if (m_usesDigitalAudio != 0) {
		// The digital-file path is handled by the separate 0x004ef3b0 helper;
		// its file/cache dependencies are still being recovered.
		return;
	}

	m_mciOpenParameters.lpstrDeviceType = "cdaudio";
	if (m_mciPlaybackStarted == 0) {
		if (mciSendCommandA(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD_PTR) &m_mciOpenParameters) != 0) {
			return;
		}

		MCI_SET_PARMS setTimeFormat;
		setTimeFormat.dwTimeFormat = MCI_FORMAT_TMSF;
		if (mciSendCommandA(m_mciOpenParameters.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR) &setTimeFormat) !=
			0) {
			mciSendCommandA(m_mciOpenParameters.wDeviceID, MCI_CLOSE, 0, 0);
			return;
		}
	}

	MCI_PLAY_PARMS play;
	play.dwCallback = (DWORD_PTR) g_audioWindow & 0xffff;
	if (p_track == -1) {
		play.dwFrom = (LegoU8) m_currentCdTrack;
	}
	else {
		play.dwFrom = (LegoU8) p_track;
	}
	play.dwTo = (LegoU8) (play.dwFrom + 1);
	if (mciSendCommandA(m_mciOpenParameters.wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_FROM | MCI_TO, (DWORD_PTR) &play) ==
		0) {
		m_mciPlaybackStarted = 1;
		if (p_track != -1) {
			m_currentCdTrack = p_track;
		}
	}
}

// STUB: LEGORACERS2 0x004f12e0
LegoFloat __fastcall GetAudioFrameDuration(AudioService*, void*)
{
	Application* app = GetApplication();
	LegoU32 frameRate = app->m_vtable->m_frameRate(app, NULL);
	return 1.0f / frameRate;
}
