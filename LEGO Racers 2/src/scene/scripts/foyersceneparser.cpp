#include "scene/scripts/foyersceneparser.h"

#include "core/strings/charbuffer24.h"
#include "game/objects/gamemanager.h"
#include "io/files/winfile.h"
#include "io/text/token.h"
#include "memory/memoryheap.h"
#include "scene/effects/secondaryfxmanager.h"
#include "scene/scripts/foyersceneevents.h"

#include <stdlib.h>
#include <string.h>

GameObject* __cdecl FindGameObjectByName(const LegoChar*, LegoU32, const LegoU32*);

void __fastcall ParseFoyerSceneGlobalBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneTimeBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSoundBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSubtitleBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneActorBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneCinematicBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneYesNoBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSavePositionsBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneEffectBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneMobileBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneJibberBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneRenderSleepBlock(FoyerSceneResource*, void*);
void __fastcall ParseFoyerSceneSparkyMenuBlock(FoyerSceneResource*, void*);
void __fastcall SkipFoyerSceneDirective(FoyerSceneResource*, void*);

struct FoyerSceneTokenEntry {
	const LegoChar* m_text;
	LegoU32 m_value;
};

// GLOBAL: LEGORACERS2 0x00547ae0
static const FoyerSceneTokenEntry g_foyerSceneActorNames[] = {
	{"SPARKY", 0},
	{"PROF_VOLTAGE", 14},
	{"POSTMAN", 9},
	{"FIREMAN", 10},
	{"DOCTOR", 16},
	{"POLICEMAN", 11},
	{"NURSE", 17},
	{"FISHERMAN", 30},
	{"WORKMAN1", 12},
	{"WORKMAN2", 8},
	{"WORKMAN3", 13},
	{"LIGHTHOUSE_KEEPER", 18},
	{"BEACH_BABE1", 19},
	{"BEACH_BABE2", 20},
	{"BEACH_BABE3", 21},
	{"BEACH_DUDE", 22},
	{"GENERIC_MALE1", 23},
	{"GENERIC_MALE2", 24},
	{"GENERIC_MALE3", 25},
	{"GENERIC_FEMALE1", 26},
	{"GENERIC_FEMALE2", 27},
	{"GENERIC_FEMALE3", 28},
	{"COASTGUARD", 29},
	{"FOREMAN", 15},
	{"BB", 47},
	{"DOC", 48},
	{"VEGA", 49},
	{"ALTAIR", 50},
	{"RIGEL", 46},
	{"SCIENTIST", 51},
	{"ANTARIES", 52},
	{"SANISTER", 31},
	{"SLYBOOTS", 32},
	{"ACHU", 33},
	{"MORAT", 34},
	{"BUNGO", 35},
	{"ALEXIS", 36},
	{"MIKE", 37},
	{"BERG", 38},
	{"FROSTY", 39},
	{"CHILLY", 40},
	{"ROSS", 41},
	{"CRYSTAL", 42},
	{"COSMO", 43},
	{"SCOOTER", 44},
	{"ARCTIC_DOC", 45},
	{"ROCKET_RACER", 53},
	{"LIGHTOR", 54},
	{"WARRIOR", 55},
	{"GHOST", 56},
	{"CHILL", 57},
	{"DUSTER", 58},
	{"FREEZE", 59},
	{"SNAKE", 60},
	{"PLAYER", 0xffffffff},
	{"PIPPIN", 2},
	{"THUNDER", 4},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547cb0
static const FoyerSceneTokenEntry g_foyerSceneDirectives[] = {
	{"$global", 0},       {"$time", 1},          {"$sound", 2},       {"$subtitle", 3},       {"$actor", 4},
	{"$cinematic", 5},    {"$shot", 6},          {"$zoom", 7},        {"$length", 8},         {"$startrelative", 11},
	{"$endrelative", 12}, {"$targetoffset", 13}, {"$carposition", 9}, {"$figposition", 10},   {"$trigger", 14},
	{"DELAY", 16},        {"ONCE", 15},          {"$animation", 18},  {"$setactor", 17},      {"$setsound", 19},
	{"$sample", 20},      {"$bindto", 22},       {"$looped", 21},     {"$expression", 23},    {"$name", 24},
	{"$position", 25},    {"$motion", 26},       {"$yesno", 27},      {"$savepositions", 28}, {"$effect", 29},
	{"FADE", 30},         {"SHAKE", 31},         {"SMOKE", 32},       {"$story", 33},         {"$spinner", 34},
	{"$tracking", 35},    {"$linkto", 36},       {"$mobile", 37},     {"$jibber", 38},        {"$nocancel", 39},
	{"$rendersleep", 40}, {"$sparkymenu", 41},   {"$hotspot", 42},    {"$dontfade", 43},      {NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547e68
static const FoyerSceneTokenEntry g_foyerSceneViewModes[] = {
	{"DEFAULT_SIZE", 7},
	{"CUSTOM_SIZE", 8},
	{"VALUE", 6},
	{"ZOOM_IN_HOLD", 2},
	{"ZOOM_IN", 3},
	{"HOLD_ZOOM_OUT", 4},
	{"ZOOM_OUT", 5},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547e18
static const FoyerSceneTokenEntry g_foyerSceneBindings[] = {
	{"FIXED_TARGET", 1},
	{"FIXED_OBJECT", 2},
	{"FIXED_PATH", 3},
	{"OBJECT_STATIC", 4},
	{"OBJECT_OBJECT", 5},
	{"OBJECT_PATH", 6},
	{"PATH_TARGET", 7},
	{"PATH_OBJECT", 8},
	{"PATH_PATH", 9},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547ef8
static const FoyerSceneTokenEntry g_foyerSceneExpressions[] = {
	{"HAPPY1", 0},
	{"HAPPY2", 1},
	{"SAD1", 2},
	{"SAD2", 3},
	{"ANGRY1", 4},
	{"ANGRY2", 5},
	{"SURPRISED1", 6},
	{"SURPRISED2", 7},
	{"TALK", 8},
	{"TALK_SAD", 9},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547ea8
static const FoyerSceneTokenEntry g_foyerScenePlayers[] = {
	{"PLAYER", 0},
	{"SPARKY", 1},
	{"BOSS_ALIEN", 3},
	{"BOSS_MECH", 2},
	{"BERG", 5},
	{"ROCKET_RACER", 4},
	{"SANISTER", 6},
	{"2ND_PLACE", 7},
	{"3RD_PLACE", 8},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547f50
static const FoyerSceneTokenEntry g_foyerSceneStates[] = {
	{"INTRO", 0},
	{"PRE_TOWN_HALL", 1},
	{"TOWN_HALL", 2},
	{"WORKMAN_CHALLENGE", 3},
	{"POSTMAN_CHALLENGE", 4},
	{"FIREMAN_CHALLENGE", 5},
	{"POLICEMAN_CHALLENGE", 6},
	{NULL, 0}
};

// GLOBAL: LEGORACERS2 0x00547f90
static const FoyerSceneTokenEntry g_foyerSceneDirections[] = {{"UP", 2}, {"DOWN", 3}, {"IDLE", 0}, {NULL, 0}};

// GLOBAL: LEGORACERS2 0x00547fb0
static const FoyerSceneTokenEntry g_foyerSceneFaces[] = {{"NONE", 1}, {"DEFAULT", 0}, {"ANGRY", 2}, {NULL, 0}};

void __fastcall SkipFoyerSceneDirective(FoyerSceneResource* p_resource, void*)
{
	// The outer parser consumes the next token after dispatching a directive.
	(void) p_resource;
}

// GLOBAL: LEGORACERS2 0x00572f28
LegoChar g_foyerSceneParserToken[24];

// GLOBAL: LEGORACERS2 0x00572f84
LegoChar* g_foyerSceneParserBuffer;

// GLOBAL: LEGORACERS2 0x00572f88
LegoChar* g_foyerSceneParserCursor;

// GLOBAL: LEGORACERS2 0x00572f8c
LegoChar* g_foyerSceneParserEnd;

// GLOBAL: LEGORACERS2 0x00572f90
LegoFloat g_foyerSceneParserTime;

namespace
{
const FoyerSceneTokenEntry* GetTokenTable(LegoU32 p_table)
{
	switch (p_table) {
	case 0:
		return g_foyerSceneActorNames;
	case 1:
		return g_foyerSceneDirectives;
	case 2:
		return g_foyerSceneViewModes;
	case 3:
		return g_foyerSceneBindings;
	case 4:
		return g_foyerSceneExpressions;
	case 5:
		return g_foyerScenePlayers;
	case 6:
		return g_foyerSceneStates;
	case 7:
		return g_foyerSceneDirections;
	case 8:
		return g_foyerSceneFaces;
	default:
		return g_foyerSceneDirectives;
	}
}

void ReadFoyerSceneParserToken()
{
	FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, g_foyerSceneParserToken, FALSE, TRUE, TRUE);
}
} // namespace

// STUB: LEGORACERS2 0x0047cb60
LegoU32 __fastcall GetFoyerSceneTokenId(FoyerSceneResource*, void*, LegoU32 p_table)
{
	const FoyerSceneTokenEntry* entry = GetTokenTable(p_table);
	if (entry == NULL) {
		return 0xffffffff;
	}
	while (entry->m_text != NULL) {
		if (strcmp(g_foyerSceneParserToken, entry->m_text) == 0) {
			return entry->m_value;
		}
		++entry;
	}
	return 0xffffffff;
}

// FUNCTION: LEGORACERS2 0x0047ccd0
void __fastcall AppendFoyerSceneAction(FoyerSceneResource* p_resource, void*, void* p_action)
{
	p_resource->m_objects[p_resource->m_unk0x9b8] = (GameObject*) p_action;
	++p_resource->m_unk0x9b8;
}

// STUB: LEGORACERS2 0x0047cd00
void __fastcall ParseFoyerSceneResourceEvents(GameObject* p_object, void*)
{
	FoyerSceneResource* resource = (FoyerSceneResource*) p_object;
	WinFile file(resource->m_resourcePath, 0, NULL);
	LegoU32 length = file.GetManagedSize();
	MemoryHeap* heap = GetMemoryHeap();

	g_foyerSceneParserBuffer = (LegoChar*) heap->AllocateFromEnd(length, "cEventList: Build - temp mem buffer");
	file.Read(g_foyerSceneParserBuffer, length, 1);
	g_foyerSceneParserCursor = g_foyerSceneParserBuffer;
	g_foyerSceneParserEnd = g_foyerSceneParserBuffer + length - 1;
	g_foyerSceneParserTime = 0.0f;
	ReadFoyerSceneParserToken();

	while (g_foyerSceneParserToken[0] != '\0') {
		LegoU32 directive = GetFoyerSceneTokenId(resource, NULL, 1);
		switch (directive) {
		case 0:
			ParseFoyerSceneGlobalBlock(resource, NULL);
			break;
		case 1:
			ParseFoyerSceneTimeBlock(resource, NULL);
			break;
		case 2:
			ParseFoyerSceneSoundBlock(resource, NULL);
			break;
		case 3:
			ParseFoyerSceneSubtitleBlock(resource, NULL);
			break;
		case 4:
			ParseFoyerSceneActorBlock(resource, NULL);
			break;
		case 5:
			ParseFoyerSceneCinematicBlock(resource, NULL);
			break;
		case 27:
			ParseFoyerSceneYesNoBlock(resource, NULL);
			break;
		case 29:
			ParseFoyerSceneSavePositionsBlock(resource, NULL);
			break;
		case 34:
			ParseFoyerSceneEffectBlock(resource, NULL);
			break;
		case 37:
			ParseFoyerSceneMobileBlock(resource, NULL);
			break;
		case 38:
			ParseFoyerSceneJibberBlock(resource, NULL);
			break;
		case 40:
			ParseFoyerSceneRenderSleepBlock(resource, NULL);
			break;
		case 41:
			ParseFoyerSceneSparkyMenuBlock(resource, NULL);
			break;
		default:
			SkipFoyerSceneDirective(resource, NULL);
			break;
		}
		ReadFoyerSceneParserToken();
	}

	heap->Free(g_foyerSceneParserBuffer);
}

// STUB: LEGORACERS2 0x0047cee0
LegoBool32 __fastcall IsFoyerSceneParserAtEnd(FoyerSceneResource*, void*)
{
	ReadFoyerSceneParserToken();
	return strcmp(g_foyerSceneParserToken, "$end") != 0;
}

// STUB: LEGORACERS2 0x0047cf50
GameObject* __fastcall ParseFoyerSceneObjectReference(FoyerSceneResource*, void*, LegoU32 p_mode)
{
	ReadFoyerSceneParserToken();
	return FindGameObjectByName(g_foyerSceneParserToken, p_mode, NULL);
}

// STUB: LEGORACERS2 0x0047cfa0
LegoFloat __fastcall ParseFoyerSceneNumber(FoyerSceneResource*, void*)
{
	ReadFoyerSceneParserToken();
	return (LegoFloat) atof(g_foyerSceneParserToken);
}

// STUB: LEGORACERS2 0x0047cfd0
LegoS32 __fastcall ParseFoyerSceneInteger(FoyerSceneResource*, void*)
{
	ReadFoyerSceneParserToken();
	return atoi(g_foyerSceneParserToken);
}

// STUB: LEGORACERS2 0x0047d000
LegoU32 __fastcall ParseFoyerSceneAssetType(FoyerSceneResource* p_resource, void*, LegoU32 p_table)
{
	ReadFoyerSceneParserToken();
	return GetFoyerSceneTokenId(p_resource, NULL, p_table);
}

// STUB: LEGORACERS2 0x0047d040
void __fastcall ParseFoyerSceneVector3(FoyerSceneResource* p_resource, void*, LegoFloat* p_vector)
{
	p_vector[0] = ParseFoyerSceneNumber(p_resource, NULL);
	p_vector[1] = ParseFoyerSceneNumber(p_resource, NULL);
	p_vector[2] = ParseFoyerSceneNumber(p_resource, NULL);
}

// STUB: LEGORACERS2 0x0047d070
LegoS32 __fastcall FindFoyerSceneEventIndex(FoyerSceneResource* p_resource, void*)
{
	LegoU32 eventType = GetFoyerSceneTokenId(p_resource, NULL, 5);
	LegoS32 i;
	FoyerSceneEventPrefix* events = (FoyerSceneEventPrefix*) p_resource->m_eventStorage;

	for (i = 0; i < p_resource->m_eventCount; ++i) {
		if (events[i].m_unk0x04 == eventType) {
			return i;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x0047d0b0
LegoFloat __fastcall FindNextFoyerActionTime(
	FoyerSceneResource* p_resource,
	void*,
	LegoFloat p_afterTime,
	LegoU32 p_type
)
{
	LegoU32 i;
	for (i = 0; i < p_resource->m_unk0x9b8; ++i) {
		const LegoU8* action = (const LegoU8*) p_resource->m_objects[i];
		LegoU32 type = *(const LegoU32*) (action + 4);
		LegoFloat actionTime = *(const LegoFloat*) (action + 8);
		if (type == p_type && p_afterTime < actionTime) {
			return actionTime;
		}
	}
	return *(LegoFloat*) &p_resource->m_unk0x9c0;
}

// STUB: LEGORACERS2 0x0047dce0
void __fastcall ParseFoyerSceneTimeBlock(FoyerSceneResource* p_resource, void*)
{
	g_foyerSceneParserTime = ParseFoyerSceneNumber(p_resource, NULL);
}
