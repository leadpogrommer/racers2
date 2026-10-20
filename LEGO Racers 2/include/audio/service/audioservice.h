#ifndef LR2_AUDIOSERVICE_H
#define LR2_AUDIOSERVICE_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>
#include <windows.h>
// The WinMM declarations depend on the Win32 types above.
#include <mmsystem.h>

// Embedded records are constructed in arrays at fixed strides. The source
// record's operational virtual slots are recovered below.
struct AudioSourceRecord;

struct AudioSourceRecordDispatch {
	typedef AudioSourceRecord*(__fastcall* DeleteObject)(AudioSourceRecord*, void*, LegoU32);
	typedef LegoU32(__fastcall* GetIndex)(AudioSourceRecord*, void*);
	typedef void(__fastcall* SetOneWord)(AudioSourceRecord*, void*, LegoU32);
	typedef void(__fastcall* SetTwoWords)(AudioSourceRecord*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* SetThreeWords)(AudioSourceRecord*, void*, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* SetOneFloat)(AudioSourceRecord*, void*, LegoFloat);
	typedef void(__fastcall* SetFloatWithUnusedFirst)(AudioSourceRecord*, void*, LegoU32, LegoFloat);
	typedef void(__fastcall* SetTwoFloats)(AudioSourceRecord*, void*, LegoFloat, LegoFloat);
	typedef void(__fastcall* Action)(AudioSourceRecord*, void*);

	DeleteObject m_deleteObject;          // 0x00
	SetOneWord m_setFlagsLow;             // 0x04
	SetOneWord m_setFlagsHigh;            // 0x08
	SetTwoWords m_setAudioValues0x90;     // 0x0c
	SetTwoWords m_setAudioValues0x98;     // 0x10
	SetTwoWords m_setAudioValues0xa0;     // 0x14
	SetTwoFloats m_setPlaybackGains;      // 0x18
	SetOneFloat m_setMaximumGain;         // 0x1c
	SetThreeWords m_setSourceVector;      // 0x20
	SetThreeWords m_setSourceRanges;      // 0x24
	SetThreeWords m_setSourceBounds;      // 0x28
	SetFloatWithUnusedFirst m_setFalloff; // 0x2c; first argument is unused by the body
	GetIndex m_getIndex;                  // 0x30
	Action m_resetPlaybackState;          // 0x34
	Action m_clearSourceFlag;             // 0x38
	SetOneWord m_setSourceEnabled;        // 0x3c
};

struct AudioSourceRecord {
	const AudioSourceRecordDispatch* m_vtable;
	LegoU32 m_unk0x04;
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	LegoU32 m_unk0x10;
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20;
	LegoU32 m_unk0x24;
	LegoU32 m_recordIndex; // 0x28, assigned by AudioService initialization
	LegoU32 m_unk0x2c;
	LegoU32 m_unk0x30;
	LegoU32 m_unk0x34;
	LegoU32 m_unk0x38;
	LegoU32 m_unk0x3c;
	LegoU32 m_unk0x40;
	LegoU32 m_unk0x44;
	LegoU32 m_unk0x48;
	LegoChar m_unkName0x4c[0x18];
	LegoChar m_unkName0x64[0x18];
	LegoFloat m_unk0x7c;
	LegoFloat m_unk0x80;
	LegoFloat m_unk0x84;
	LegoFloat m_defaultRange;   // 0x88, initialized to 150.0f
	LegoFloat m_defaultFalloff; // 0x8c, initialized to 30.0f
	LegoU32 m_unk0x90[6];
	LegoU8 m_unk0xa8[4];
	LegoU32 m_unk0xac;
	LegoU32 m_unk0xb0;
	LegoFloat m_unk0xb4;
	LegoU32 m_unk0xb8;
	struct AudioChannelRecord* m_channelResource; // 0xbc, reference-counted cleanup record
	IDirectSoundBuffer* m_playbackBuffer;         // 0xc0
	union {
		IDirectSound3DBuffer* m_spatialBuffer; // 0xc4, queried from the 3D source buffer
		LegoU32 m_unk0xc4;
	};
	LegoU32 m_unk0xc8;
	LegoU32 m_unk0xcc;
	LegoU32 m_unk0xd0;
	LegoU32 m_unk0xd4;
};

struct AudioChannelRecord {
	LegoU32 m_referenceCount;  // 0x00
	LegoChar m_fileName[0x80]; // 0x04, canonical uppercase .AIF path
	LegoU32 m_unk0x84;
	LegoU32 m_unk0x88;
	void* m_cachedBuffer;                 // 0x8c, source audio data owned through the heap
	IDirectSoundBuffer* m_playbackBuffer; // 0x90, released through provider slot +0x10
	LegoU32 m_unk0x94;
};

struct AudioDeviceRecord {
	LegoU32 m_active;     // 0x00
	LegoU32 m_unk0x04;    // 0x04
	LegoU32 m_unk0x08;    // 0x08
	LegoU32 m_unk0x0c[6]; // 0x0c through 0x20
	LegoU32 m_listenerId; // 0x24
	LegoFloat m_gainA;    // 0x28
	LegoFloat m_gainB;    // 0x2c
	LegoU32 m_unk0x30;    // 0x30
};

DECOMP_SIZE_ASSERT(AudioSourceRecord, 0xd8);
DECOMP_STATIC_ASSERT(offsetof(AudioSourceRecord, m_channelResource) == 0xbc);
DECOMP_STATIC_ASSERT(offsetof(AudioSourceRecord, m_playbackBuffer) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(AudioSourceRecord, m_spatialBuffer) == 0xc4);
DECOMP_SIZE_ASSERT(AudioSourceRecordDispatch, 0x40);
DECOMP_SIZE_ASSERT(AudioChannelRecord, 0x98);
DECOMP_STATIC_ASSERT(offsetof(AudioChannelRecord, m_unk0x84) == 0x84);
DECOMP_STATIC_ASSERT(offsetof(AudioChannelRecord, m_fileName) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(AudioChannelRecord, m_cachedBuffer) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(AudioChannelRecord, m_playbackBuffer) == 0x90);
DECOMP_SIZE_ASSERT(AudioDeviceRecord, 0x34);

class AudioService;

struct AudioServiceDispatch {
	// These are free-function bridge types for the retail thiscall slots:
	// ECX is the receiver, EDX is unused, and the remaining args stay on stack.
	typedef AudioService*(__fastcall* DeleteObject)(AudioService*, void*, LegoU32);
	typedef void(__fastcall* Action)(AudioService*, void*);
	typedef LegoU32(__fastcall* GetValue)(AudioService*, void*);
	typedef void(__fastcall* GetValueOut)(AudioService*, void*, LegoU32*);
	typedef void(__fastcall* SetValue)(AudioService*, void*, LegoU32);
	typedef void(__fastcall* SetSignedValue)(AudioService*, void*, LegoS32);
	typedef void(__fastcall* SetFloat)(AudioService*, void*, LegoFloat);
	typedef void(__fastcall* SetPointer)(AudioService*, void*, void*);
	typedef void(__fastcall* SetPointerAndSize)(AudioService*, void*, const void*, LegoU32);
	typedef void(__fastcall* SetPath)(AudioService*, void*, LPCSTR);
	typedef void(__fastcall* SetPathAndUnused)(AudioService*, void*, LPCSTR, LegoU32);
	typedef void(__fastcall* SetFloatPair)(AudioService*, void*, LegoFloat, LegoFloat);
	typedef void(__fastcall* GetFloatPair)(AudioService*, void*, LegoFloat*, LegoFloat*);
	typedef void(__fastcall* SetOffsetAndLength)(AudioService*, void*, LONG, DWORD);
	typedef void(__fastcall* SetWaveFormat)(AudioService*, void*, WAVEFORMATEX*);
	typedef void(__fastcall* AppendAudioData)(AudioService*, void*, const void*, LegoS32);
	typedef void(__fastcall* SetTwoWords)(AudioService*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* SetThreeWords)(AudioService*, void*, LegoU32, LegoU32, LegoU32);
	typedef void(__fastcall* AddAudioSource)(AudioService*, void*, const LegoU32*, LegoU32, LegoU32);
	typedef void(__fastcall* ReleaseSourceRecord)(AudioService*, void*, AudioSourceRecord*);
	typedef AudioSourceRecord*(__fastcall* FindAudioSource)(AudioService*, void*, LegoU32, LPCSTR);
	typedef LegoU32(__fastcall* ValidateAudioSource)(AudioService*, void*, LegoU32);
	typedef void(__fastcall* SetMixerVolume)(AudioService*, void*, LegoFloat, LegoU32);
	typedef void(__fastcall* AddListener)(AudioService*, void*, const LegoU32*, LegoU32);
	typedef void(__fastcall* SetListenerGains)(AudioService*, void*, LegoU32, LegoFloat, LegoFloat);

	DeleteObject m_deleteObject;            // 0x00
	Action m_shutdown;                      // 0x04
	GetValue m_getStatus;                   // 0x08
	Action m_restart;                       // 0x0c
	GetValueOut m_getQuality;               // 0x10
	SetValue m_setQuality;                  // 0x14
	Action m_unk0x18;                       // 0x18, pure virtual in the base table
	SetValue m_update;                      // 0x1c
	SetFloatPair m_setVolumes;              // 0x20
	GetFloatPair m_getVolumes;              // 0x24
	SetFloatPair m_setParameters;           // 0x28
	SetValue m_setMusicEnabled;             // 0x2c
	SetTwoWords m_unk0x30;                  // 0x30, ret 8
	AddAudioSource m_addSource;             // 0x34, ret 0xc
	Action m_unk0x38;                       // 0x38
	ReleaseSourceRecord m_releaseSource;    // 0x3c
	SetValue m_removeSource;                // 0x40
	FindAudioSource m_findSource;           // 0x44
	ValidateAudioSource m_validateSource;   // 0x48
	SetThreeWords m_unk0x4c;                // 0x4c, ret 0xc
	SetValue m_unk0x50;                     // 0x50
	SetValue m_unk0x54;                     // 0x54
	SetPathAndUnused m_setDigitalAudioPath; // 0x58; second stack word is ignored
	SetSignedValue m_playCdTrack;           // 0x5c
	Action m_stopCdTrack;                   // 0x60
	SetValue m_unk0x64;                     // 0x64
	SetMixerVolume m_setMixerVolume;        // 0x68
	GetFloatPair m_getMixerVolume;          // 0x6c
	AddListener m_addListener;              // 0x70
	SetValue m_removeListener;              // 0x74
	SetTwoWords m_setListenerField0x04;     // 0x78
	SetTwoWords m_setListenerField0x30;     // 0x7c
	SetListenerGains m_setListenerGains;    // 0x80
	SetPath m_initializeCommentary;         // 0x84
	SetFloat m_setCommentaryGain;           // 0x88
	SetOffsetAndLength m_queueCommentary;   // 0x8c
	union {
		SetValue m_stopCommentary; // 0x90
		SetValue m_unk0x90;        // compatibility name used by the commentary bridge
	};
	Action m_shutdownCommentary;         // 0x94
	Action m_unk0x98;                    // 0x98
	SetWaveFormat m_initializePcmBuffer; // 0x9c
	Action m_shutdownPcmBuffer;          // 0xa0
	AppendAudioData m_appendPcmData;     // 0xa4
};

DECOMP_SIZE_ASSERT(AudioServiceDispatch, 0xa8);
extern const AudioServiceDispatch g_audioServiceBaseDispatch;
extern const AudioServiceDispatch g_audioServiceOperationalDispatch;
extern const AudioSourceRecordDispatch g_audioSourceRecordBaseDispatch;
extern const AudioSourceRecordDispatch g_audioSourceRecordOperationalDispatch;

struct AudioBackendStateDispatch;

struct AudioBackendStateView {
	const AudioBackendStateDispatch* m_vtable; // 0x00
	IDirectSound8* m_device;                   // 0x04
	IDirectSoundBuffer* m_primaryBuffer;       // 0x08; temporary during initialization
	LegoU32 m_initializationStatus;            // 0x0c
};

struct AudioBackendStateDispatch {
	typedef AudioBackendStateView*(__fastcall* DeleteObject)(AudioBackendStateView*, void*, LegoU32);
	typedef void(__fastcall* Action)(AudioBackendStateView*, void*);
	typedef void(__fastcall* Initialize)(AudioBackendStateView*, void*, HWND);
	typedef IDirectSoundBuffer*(__fastcall* CreateSourceBuffer)(AudioBackendStateView*, void*, const void*, LegoU32);
	typedef void(__fastcall* ReleaseBufferPointer)(AudioBackendStateView*, void*, IDirectSoundBuffer**);
	typedef IDirectSoundBuffer*(__fastcall*
									DuplicatePlaybackBuffer)(AudioBackendStateView*, void*, IDirectSoundBuffer*);
	typedef void(__fastcall* ReleaseSourceBuffer)(AudioBackendStateView*, void*, AudioSourceRecord*);
	typedef LegoU32(__fastcall* GetSourceSampleRate)(AudioBackendStateView*, void*, const void*, LegoU32);
	typedef LegoBool32(__fastcall* StartPlayback)(AudioBackendStateView*, void*, IDirectSoundBuffer*, LegoU32);
	typedef LegoBool32(__fastcall* StopPlayback)(AudioBackendStateView*, void*, IDirectSoundBuffer*);
	typedef LegoBool32(__fastcall* IsPlaybackActive)(AudioBackendStateView*, void*, IDirectSoundBuffer*);
	typedef LegoBool32(__fastcall* SetPlaybackFrequency)(AudioBackendStateView*, void*, IDirectSoundBuffer*, LegoS32);
	typedef LegoBool32(__fastcall* PlaybackAction)(AudioBackendStateView*, void*, IDirectSoundBuffer*);
	typedef void(__fastcall* UpdateSource)(AudioBackendStateView*, void*, AudioSourceRecord*);

	DeleteObject m_destroy;                            // 0x00
	Initialize m_initialize;                           // 0x04
	Action m_releaseDeviceInterfaces;                  // 0x08
	CreateSourceBuffer m_createSourceBuffer;           // 0x0c
	ReleaseBufferPointer m_releaseBufferPointer;       // 0x10
	DuplicatePlaybackBuffer m_duplicatePlaybackBuffer; // 0x14
	ReleaseSourceBuffer m_releaseSourceBuffer;         // 0x18
	Action m_unk0x1c;                                  // 0x1c
	GetSourceSampleRate m_getSourceSampleRate;         // 0x20
	StartPlayback m_startPlayback;                     // 0x24
	StopPlayback m_stopPlayback;                       // 0x28
	IsPlaybackActive m_isPlaybackActive;               // 0x2c
	SetPlaybackFrequency m_setPlaybackFrequency;       // 0x30
	PlaybackAction m_resetPlaybackPosition;            // 0x34
	UpdateSource m_updateSource;                       // 0x38
};

DECOMP_STATIC_ASSERT(offsetof(AudioBackendStateView, m_initializationStatus) == 0x0c);
DECOMP_SIZE_ASSERT(AudioBackendStateView, 0x10);
DECOMP_STATIC_ASSERT(offsetof(AudioBackendStateDispatch, m_releaseSourceBuffer) == 0x18);
DECOMP_SIZE_ASSERT(AudioBackendStateDispatch, 0x3c);
extern const AudioBackendStateDispatch g_audioBackendBaseDispatch;

class AudioService {
public:
	AudioService(LegoU32 p_initialMode);
	void Initialize();
	void Shutdown();
	void Restart();
	void Update(LegoU32 p_frame);
	void GetQualityMode(LegoU32* p_mode);
	void SetQualityMode(LegoU32 p_mode);
	void SetVolumes(LegoFloat p_volumeA, LegoFloat p_volumeB);
	void GetVolumes(LegoFloat* p_volumeA, LegoFloat* p_volumeB);
	void SetMusicEnabled(LegoU32 p_enabled);
	void SetDigitalAudioPath(LPCSTR p_path);
	void SetCdPlayerMode(LegoU32 p_mode);
	void SetMixerVolume(LegoFloat p_volume, LegoU32 p_unused);
	void GetMixerVolume(LegoFloat* p_volumeA, LegoFloat* p_volumeB);
	void AddListener(const LegoU32* p_listenerData, LegoU32 p_listenerId);
	void RemoveListener(LegoU32 p_listenerId);
	void SetListenerField0x04(LegoU32 p_listenerId, LegoU32 p_value);
	void SetListenerField0x30(LegoU32 p_listenerId, LegoU32 p_value);
	void SetListenerGains(LegoU32 p_listenerId, LegoFloat p_gainA, LegoFloat p_gainB);
	void PlayCdTrack(LegoS32 p_track);

	const AudioServiceDispatch* m_vtable; // 0x00; construction and operational dispatch tables
	LegoU32 m_unk0x04;                    // 0x04
	LegoU32 m_shutdown;                   // 0x08
	LegoU32 m_qualityMode;                // 0x0c
	LegoU32 m_initializationFailed;       // 0x10
	LegoFloat m_elapsedTime;              // 0x14
	LegoS32 m_currentTrack;               // 0x18
	LegoU32 m_unk0x1c;                    // 0x1c
	LegoU32 m_unk0x20;                    // 0x20
	LegoFloat m_volumeA;                  // 0x24
	LegoFloat m_volumeB;                  // 0x28
	LegoFloat m_unk0x2c;                  // 0x2c
	LegoFloat m_unk0x30;                  // 0x30
	LegoU32 m_hasAudioParameters;         // 0x34
	LegoU32 m_musicEnabled;               // 0x38, stored as a normalized boolean
	AudioSourceRecord m_sources[512];     // 0x3c, stride 0xd8
	AudioChannelRecord m_channels[256];   // 0x1b03c, stride 0x98
	LegoU8 m_unk0x2483c[0x10];            // 0x2483c
	MCI_OPEN_PARMSA m_mciOpenParameters;  // 0x2484c
	LegoU32 m_currentCdTrack;             // 0x24860; low byte used as the MCI track number
	LegoU32 m_mciPlaybackStarted;         // 0x24864
	LegoU32 m_digitalPlaybackStarted;     // 0x24868
	LegoU32 m_usesDigitalAudio;           // 0x2486c
	LegoU32 m_unk0x24870;                 // 0x24870
	LegoU8 m_unk0x24874[0x180];           // 0x24874
	void* m_unk0x249f4;                   // 0x249f4, digital-audio control object
	LegoFloat m_unk0x249f8;
	AudioDeviceRecord m_devices[8]; // 0x249fc, stride 0x34
	LegoS32 m_listenerCount;        // 0x24b9c
	LegoU32 m_unk0x24ba0;           // 0x24ba0
	LegoU32 m_unk0x24ba4;           // 0x24ba4
	LegoU8 m_unk0x24ba8[0x5a7c];    // 0x24ba8 through the allocated object extent

private:
	AudioDeviceRecord* FindAvailableListener();
	AudioDeviceRecord* FindListenerById(LegoU32 p_listenerId);
};

DECOMP_SIZE_ASSERT(AudioService, 0x2a624);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_sources) == 0x3c);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_channels) == 0x1b03c);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_unk0x2483c) == 0x2483c);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_devices) == 0x249fc);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_listenerCount) == 0x24b9c);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_unk0x24ba0) == 0x24ba0);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_unk0x249f4) == 0x249f4);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_unk0x249f8) == 0x249f8);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_mciOpenParameters) == 0x2484c);
DECOMP_STATIC_ASSERT(offsetof(AudioService, m_usesDigitalAudio) == 0x2486c);

AudioService* GetAudioService(LegoBool32 p_create);
void DestroyAudioService();
LegoU32 __fastcall GetAudioServiceShutdownStatus(AudioService* p_service, void* p_edx);
AudioSourceRecord* __fastcall ConstructAudioSourceRecord(AudioSourceRecord* p_record, void* p_edx);
void __fastcall InitializeAudioSourceRecordFields(AudioSourceRecord* p_record, void* p_edx);
void __fastcall ReleaseAudioSourceRecordResources(AudioSourceRecord* p_record, void* p_edx);
void __fastcall DestroyAudioChannelRecordResources(AudioChannelRecord* p_record, void* p_edx);
AudioChannelRecord* __fastcall ConstructAudioChannelRecord(AudioChannelRecord* p_record, void* p_edx);
AudioDeviceRecord* __fastcall ConstructAudioDeviceRecord(AudioDeviceRecord* p_record, void* p_edx);
void __fastcall InitializeAudioDeviceRecord(AudioDeviceRecord* p_record, void* p_edx);
void __fastcall InitializeAudioSources(AudioService* p_service, void* p_edx);
void __fastcall InitializeAudioChannels(AudioService* p_service, void* p_edx);
void __fastcall InitializeAudioDevices(AudioService* p_service, void* p_edx);
extern AudioService* g_audioService;
extern AudioBackendStateView* g_audioBackend;
extern HWND g_audioWindow;
extern LegoU32 g_audioServiceExitCallbackRegistered;

#endif // LR2_AUDIOSERVICE_H
