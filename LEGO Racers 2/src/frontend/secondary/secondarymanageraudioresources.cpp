#include "audio/service/audioservice.h"
#include "frontend/secondary/secondarymanager.h"
#include "frontend/secondary/secondarymanageraudionames.h"

#include <string.h>

struct SecondaryManagerAudioSourceDefinition {
	LegoChar m_name[24];
	LegoChar m_secondaryName[24];
	LegoChar m_path[128];
	LegoU8 m_unk0xb0[0x18];
	LegoFloat m_gainA;
	LegoFloat m_gainB;
	LegoFloat m_maximumGain;
	LegoU32 m_musicFlag;
	LegoU32 m_spatialFlag;
	LegoU32 m_distanceFlag;
	LegoU32 m_lowFlags;
	LegoFloat m_falloff;
	LegoFloat m_range;
	LegoU32 m_unk0xec;
	LegoU32 m_unk0xf0;
	LegoU32 m_unk0xf4;
	LegoU32 m_unk0xf8;
	LegoU32 m_audioValues[6];
	LegoU32 m_disableFalloff;
};

DECOMP_SIZE_ASSERT(SecondaryManagerAudioSourceDefinition, 0x118);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManagerAudioSourceDefinition, m_path) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManagerAudioSourceDefinition, m_gainA) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(SecondaryManagerAudioSourceDefinition, m_disableFalloff) == 0x114);

static const LegoChar* g_secondaryManagerAudioPaths[10] = {
	"Game Data\\sounds\\front end\\Select.vag",
	"Game Data\\sounds\\front end\\Cancel.vag",
	"Game Data\\sounds\\front end\\Invalid.vag",
	"Game Data\\sounds\\front end\\Horizont.vag",
	"Game Data\\sounds\\voices\\special\\1LAUGH1.vag",
	"Game Data\\sounds\\front end\\Updown.vag",
	"Game Data\\sounds\\front end\\Rotate.vag",
	"Game Data\\sounds\\front end\\Newbrick.vag",
	"Game Data\\sounds\\front end\\Delete.vag",
	"Game Data\\sounds\\front end\\Cycle.vag"
};

// STUB: LEGORACERS2 0x004276e0
void __fastcall LoadSecondaryManagerAudioResources(SecondaryManager* p_manager, void*, LegoU32 p_loadMode)
{
	AudioService* audio = GetAudioService(TRUE);
	LegoU32* resourceIds = p_manager->m_unk0x154;
	for (LegoU32 index = 0; index < 10; ++index) {
		if (p_loadMode == 0 && index >= 5) {
			resourceIds[index] = 0;
		}
		else {
			SecondaryManagerAudioSourceDefinition definition;
			memset(&definition, 0, sizeof(definition));
			memcpy(definition.m_name, (const LegoU8*) p_manager + 0x10, sizeof(definition.m_name));
			memcpy(
				definition.m_secondaryName,
				GetSecondaryManagerAudioNames()[index].m_data,
				sizeof(definition.m_secondaryName)
			);
			strcpy(definition.m_path, g_secondaryManagerAudioPaths[index]);
			strcpy((LegoChar*) definition.m_unk0xb0, "Empty");
			definition.m_gainA = 1.0f;
			definition.m_gainB = 1.0f;
			definition.m_maximumGain = 1.0f;
			definition.m_musicFlag = 0;
			definition.m_spatialFlag = 0;
			definition.m_distanceFlag = 1;
			definition.m_lowFlags = 5;
			definition.m_falloff = 30.0f;
			definition.m_range = 150.0f;
			definition.m_disableFalloff = 1;
			audio->m_vtable->m_addSource(audio, NULL, (const LegoU32*) &definition, (LegoU32) p_manager, 0);
			resourceIds[index] =
				(LegoU32) audio->m_vtable
					->m_findSource(audio, NULL, (LegoU32) p_manager, GetSecondaryManagerAudioNames()[index].m_data);
		}
	}
}

// STUB: LEGORACERS2 0x004278d0
void __fastcall ReleaseSecondaryManagerAudioResources(SecondaryManager* p_manager, void*)
{
	AudioService* audio = GetAudioService(TRUE);
	LegoS32 index;
	for (index = 9; index >= 0; --index) {
		if (p_manager->m_unk0x154[index] != 0) {
			AudioSourceRecord* source = (AudioSourceRecord*) p_manager->m_unk0x154[index];
			audio->m_vtable->m_releaseSource(audio, NULL, source);
			p_manager->m_unk0x154[index] = 0;
		}
	}
}
