#include "audio/service/audioservice.h"
#include "core/strings/charbuffer128.h"
#include "game/character/charactervisualbehavior.h"

#include <string.h>

struct CharacterFootstepAudioDefinition {
	CharBuffer24 m_ownerName;
	CharBuffer24 m_name;
	CharBuffer128 m_path;
	CharBuffer24 m_secondaryName;
	LegoFloat m_gainA;
	LegoFloat m_gainB;
	LegoFloat m_maximumGain;
	LegoU32 m_musicFlag;
	LegoU32 m_spatialFlag;
	LegoU32 m_distanceFlag;
	LegoU32 m_lowFlags;
	LegoFloat m_falloff;
	LegoFloat m_range;
	LegoU32 m_values[11];
};
DECOMP_SIZE_ASSERT(CharacterFootstepAudioDefinition, 0x118);

// GLOBAL: LEGORACERS2 0x0056ef08
static CharacterFootstepAudioDefinition g_characterFirstFootstepAudio;
// GLOBAL: LEGORACERS2 0x0056f418
static CharacterFootstepAudioDefinition g_characterSecondFootstepAudio;
// GLOBAL: LEGORACERS2 0x0056f40c
static LegoU32 g_characterFootstepAudioInitialized;

static void InitializeCharacterFootstepAudioDefinition(
	CharacterFootstepAudioDefinition* p_definition,
	GameObject* p_owner,
	const LegoChar* p_name,
	const LegoChar* p_path
)
{
	p_definition->m_ownerName = p_owner->m_first;
	p_definition->m_name = CharBuffer24(p_name);
	p_definition->m_path = CharBuffer128(p_path);
	p_definition->m_secondaryName = CharBuffer24("Empty");
	p_definition->m_gainA = p_definition->m_gainB = p_definition->m_maximumGain = 1.0f;
	p_definition->m_musicFlag = 0;
	p_definition->m_spatialFlag = p_definition->m_distanceFlag = 1;
	p_definition->m_lowFlags = 5;
	p_definition->m_falloff = 30.0f;
	p_definition->m_range = 150.0f;
	memset(p_definition->m_values, 0, sizeof(p_definition->m_values));
}

// STUB: LEGORACERS2 0x00450e70
void __fastcall InitializeCharacterVisualFootstepAudio(CharacterVisual* p_visual, void*)
{
	LegoU8* body = (LegoU8*) p_visual->m_body;
	*(LegoU32*) (body + 0x304) = 1;
	AudioService* audio = GetAudioService(1);
	if ((g_characterFootstepAudioInitialized & 1) == 0) {
		g_characterFootstepAudioInitialized |= 1;
		InitializeCharacterFootstepAudioDefinition(
			&g_characterFirstFootstepAudio,
			p_visual->m_body,
			"MFStep1",
			"game data\\sounds\\footstep\\grass04.vag"
		);
	}
	if ((g_characterFootstepAudioInitialized & 2) == 0) {
		g_characterFootstepAudioInitialized |= 2;
		InitializeCharacterFootstepAudioDefinition(
			&g_characterSecondFootstepAudio,
			p_visual->m_body,
			"MFStep2",
			"game data\\sounds\\footstep\\grass05.vag"
		);
	}
	audio->m_vtable
		->m_addSource(audio, NULL, (const LegoU32*) &g_characterFirstFootstepAudio, (LegoU32) p_visual->m_body, 0);
	audio->m_vtable
		->m_addSource(audio, NULL, (const LegoU32*) &g_characterSecondFootstepAudio, (LegoU32) p_visual->m_body, 0);
	AudioSourceRecord* first = audio->m_vtable->m_findSource(audio, NULL, (LegoU32) p_visual->m_body, "MFStep1");
	*(AudioSourceRecord**) (body + 0x30c) = first;
	AudioSourceRecord* second = audio->m_vtable->m_findSource(audio, NULL, (LegoU32) p_visual->m_body, "MFStep2");
	*(AudioSourceRecord**) (body + 0x310) = second;
	first->m_vtable->m_setPlaybackGains(first, NULL, 0.5f, 0.5f);
	first->m_vtable->m_setMaximumGain(first, NULL, 1.0f);
	first->m_vtable->m_setFlagsHigh(first, NULL, 0x80);
	first->m_vtable->m_clearSourceFlag(first, NULL);
	second->m_vtable->m_setPlaybackGains(second, NULL, 0.5f, 0.5f);
	second->m_vtable->m_setMaximumGain(second, NULL, 1.0f);
	second->m_vtable->m_setFlagsHigh(second, NULL, 0x80);
	second->m_vtable->m_clearSourceFlag(second, NULL);
	*(LegoU32*) (body + 0x308) = 2;
	*(LegoU32*) (body + 0x338) = 2;
	*(LegoFloat*) (body + 0x33c) = 6.0f;
	*(LegoU32*) (body + 0x340) = 0;
	*(LegoU32*) (body + 0x344) = 0;
	*(LegoFloat*) (body + 0x348) = 16.0f;
	*(LegoU32*) (body + 0x34c) = 0;
	*(LegoU32*) (body + 0x350) = 1;
}
