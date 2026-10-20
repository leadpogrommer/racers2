#include "scene/scripts/foyersceneparserblocks.h"

#include "core/strings/charbuffer24.h"
#include "io/text/token.h"
#include "math/matrix3.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "scene/scripts/foyersceneactions.h"
#include "scene/scripts/foyersceneactionsbase.h"
#include "scene/scripts/foyersceneevents.h"
#include "scene/scripts/foyersceneparser.h"

#include <new.h>
#include <string.h>

void __fastcall SkipFoyerSceneDirective(FoyerSceneResource*, void*);
namespace
{
LegoU32 FloatBits(LegoFloat p_value)
{
	return *(LegoU32*) &p_value;
}

LegoU32* ActionWords(LegoU32* p_words, LegoU32 p_count)
{
	while (p_count-- != 0) {
		*p_words++ = 0;
	}
	return p_words;
}

void AppendAllocatedAction(
	FoyerSceneResource* p_resource,
	LegoU32 p_bytes,
	const LegoU32* p_words,
	void(__fastcall* p_initialize)(void*, void*, const LegoU32*)
)
{
	void* action = AllocateHeapMemory(p_bytes);
	if (action != NULL) {
		p_initialize(action, NULL, p_words);
	}
	AppendFoyerSceneAction(p_resource, NULL, action);
}
} // namespace

// STUB: LEGORACERS2 0x0047d100
void __fastcall ParseFoyerSceneActorBlock(FoyerSceneResource* p_resource, void*)
{
	LegoS32 eventIndex = ParseFoyerSceneInteger(p_resource, NULL);
	while (IsFoyerSceneParserAtEnd(p_resource, NULL)) {
		LegoU32 directive = GetFoyerSceneTokenId(p_resource, NULL, 1);
		LegoU32 words[11];
		switch (directive) {
		case 0x12: {
			ActionWords(words, 11);
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = (LegoU32) ((LegoU8*) p_resource->m_eventStorage + eventIndex * 0x94);
			FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, g_foyerSceneParserToken, FALSE, TRUE, TRUE);
			new ((CharBuffer24*) (words + 3)) CharBuffer24(g_foyerSceneParserToken);
			words[9] = FloatBits(ParseFoyerSceneNumber(p_resource));
			words[10] = FloatBits(ParseFoyerSceneNumber(p_resource));
			AppendAllocatedAction(p_resource, 0x34, words, InitializeFoyerSceneAction3);
			break;
		}
		case 0x17:
			ActionWords(words, 11);
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = (LegoU32) ((LegoU8*) p_resource->m_eventStorage + eventIndex * 0x94);
			words[3] = ParseFoyerSceneAssetType(p_resource, NULL, 4);
			words[4] = FloatBits(ParseFoyerSceneNumber(p_resource));
			AppendAllocatedAction(p_resource, 0x1c, words, InitializeFoyerSceneAction4);
			break;
		case 0x19:
		case 0x1a: {
			ActionWords(words, 11);
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = (LegoU32) ((LegoU8*) p_resource->m_eventStorage + eventIndex * 0x94);
			words[3] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			AppendAllocatedAction(
				p_resource,
				0x18,
				words,
				directive == 0x19 ? InitializeFoyerSceneAction5 : InitializeFoyerSceneAction6
			);
			break;
		}
		case 0x24: {
			ActionWords(words, 11);
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = (LegoU32) ((LegoU8*) p_resource->m_eventStorage + eventIndex * 0x94);
			GameObject* target = ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			words[3] = target != NULL ? (LegoU32) ((LegoU8*) target + 0x58) : 0;
			ParseFoyerSceneVector3(p_resource, NULL, (LegoFloat*) (words + 4));
			AppendAllocatedAction(p_resource, 0x24, words, InitializeFoyerSceneAction7);
			break;
		}
		default:
			SkipFoyerSceneDirective(p_resource, NULL);
			break;
		}
	}
}

// STUB: LEGORACERS2 0x0047d420
void __fastcall ParseFoyerSceneCinematicBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[32];
	LegoU32 persistentSecondTarget = 0;
	ActionWords(words, 32);
	words[0] = (LegoU32) p_resource;
	words[1] = *(LegoU32*) &g_foyerSceneParserTime;
	while (IsFoyerSceneParserAtEnd(p_resource, NULL)) {
		LegoU32 directive = GetFoyerSceneTokenId(p_resource, NULL, 1);
		switch (directive) {
		case 6:
			words[2] = ParseFoyerSceneAssetType(p_resource, NULL, 3);
			words[5] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 0);
			if (words[5] == 0) {
				words[5] = FindFoyerSceneEventIndex(p_resource, NULL);
			}
			words[6] = persistentSecondTarget;
			if (words[2] != 4) {
				words[6] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 0);
				if (words[6] == 0) {
					words[6] = FindFoyerSceneEventIndex(p_resource, NULL);
				}
			}
			persistentSecondTarget = words[6];
			break;
		case 7:
			words[3] = ParseFoyerSceneAssetType(p_resource, NULL, 2);
			if (words[3] == 6 || words[3] == 8) {
				words[5] = FloatBits(ParseFoyerSceneNumber(p_resource));
			}
			break;
		case 8:
			words[6] = FloatBits(ParseFoyerSceneNumber(p_resource));
			break;
		case 11:
			words[7] = 9;
			ParseFoyerSceneVector3(p_resource, NULL, (LegoFloat*) (words + 9));
			break;
		case 12:
			ParseFoyerSceneVector3(p_resource, NULL, (LegoFloat*) (words + 12));
			break;
		case 13:
			ParseFoyerSceneVector3(p_resource, NULL, (LegoFloat*) (words + 15));
			break;
		case 0x23:
			words[18] = FloatBits(ParseFoyerSceneNumber(p_resource));
			break;
		default:
			SkipFoyerSceneDirective(p_resource, NULL);
			break;
		}
	}
	AppendAllocatedAction(p_resource, 0x7c, words, InitializeFoyerSceneAction1);
}

// STUB: LEGORACERS2 0x0047d650
void __fastcall ParseFoyerSceneGlobalBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU8* bytes = (LegoU8*) p_resource;
	while (IsFoyerSceneParserAtEnd(p_resource, NULL)) {
		LegoU32 directive = GetFoyerSceneTokenId(p_resource, NULL, 1);
		switch (directive) {
		case 8:
			*(LegoFloat*) (bytes + 0x9c0) = ParseFoyerSceneNumber(p_resource);
			break;
		case 9: {
			GameObject* car = ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			*(LegoU32*) (bytes + 0x9e8) = 1;
			*(LegoU32*) (bytes + 0x9d4) = (LegoU32) car;
			Matrix3 rotation;
			rotation.FUN_004a4d30((LegoFloat*) ((LegoU8*) car + 0x64));
			memcpy(bytes + 0x9d8, &rotation, 3 * sizeof(LegoFloat));
			break;
		}
		case 10:
			*(LegoU32*) (bytes + 0x9ec) = 1;
			*(LegoU32*) (bytes + 0x9e4) = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			break;
		case 14: {
			GameObject* object = ParseFoyerSceneObjectReference(p_resource, NULL, 0);
			*(LegoU32*) (bytes + 0x68) =
				object != NULL ? (LegoU32) object : (LegoU32) FindFoyerSceneEventIndex(p_resource, NULL);
			break;
		}
		case 17: {
			LegoS32 index = p_resource->m_eventCount;
			FoyerSceneEventPrefix* event =
				(FoyerSceneEventPrefix*) ((LegoU8*) p_resource->m_eventStorage + index * 0x94);
			GameObject* target = ParseFoyerSceneObjectReference(p_resource, NULL, 0);
			SetFoyerSceneEventTarget(event, NULL, target);
			if (target == NULL) {
				event->m_unk0x04 = GetFoyerSceneTokenId(p_resource, NULL, 5);
			}
			FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, g_foyerSceneParserToken, FALSE, TRUE, TRUE);
			new (&event->m_firstBuffer) CharBuffer24(g_foyerSceneParserToken);
			++p_resource->m_eventCount;
			break;
		}
		case 24:
			FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, g_foyerSceneParserToken, TRUE, TRUE, TRUE);
			new ((CharBuffer24*) (bytes + 0xac0)) CharBuffer24(g_foyerSceneParserToken);
			break;
		case 28:
			*(LegoU32*) (bytes + 0x9f0) = 1;
			break;
		case 33:
			*(LegoU32*) (bytes + 0xad8) = ParseFoyerSceneAssetType(p_resource, NULL, 6);
			break;
		case 39:
			*(LegoU32*) (bytes + 0x9c8) = 0;
			break;
		case 42:
			*(LegoU32*) (bytes + 0xadc) = ParseFoyerSceneInteger(p_resource, NULL);
			break;
		case 43:
			*(LegoU32*) (bytes + 0x9cc) = 0;
			break;
		default:
			SkipFoyerSceneDirective(p_resource, NULL);
			break;
		}
	}
}

// STUB: LEGORACERS2 0x0047db60
void __fastcall ParseFoyerSceneSoundBlock(FoyerSceneResource* p_resource, void*)
{
	LegoChar inputName[24];
	FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, inputName, FALSE, TRUE, TRUE);
	for (LegoU32 i = 0; i < p_resource->m_unk0x5b4; ++i) {
		FoyerSceneInputState* state = &p_resource->m_inputStates[i];
		if (strcmp(state->m_text.m_data, inputName) == 0) {
			LegoU32 words[5];
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = i;
			AppendAllocatedAction(p_resource, 0x14, words, InitializeFoyerSceneAction0);
			return;
		}
	}
}

// STUB: LEGORACERS2 0x0047dc30
void __fastcall ParseFoyerSceneSubtitleBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[10];
	ActionWords(words, 10);
	words[0] = (LegoU32) p_resource;
	words[1] = *(LegoU32*) &g_foyerSceneParserTime;
	words[2] = ParseFoyerSceneAssetType(p_resource, NULL, 0);
	FUN_0045de20(&g_foyerSceneParserCursor, g_foyerSceneParserEnd, g_foyerSceneParserToken, FALSE, TRUE, TRUE);
	new ((CharBuffer24*) (words + 3)) CharBuffer24(g_foyerSceneParserToken);
	words[9] = FloatBits(ParseFoyerSceneNumber(p_resource));
	AppendAllocatedAction(p_resource, 0x30, words, InitializeFoyerSceneAction2);
}

// STUB: LEGORACERS2 0x0047dcf0
void __fastcall ParseFoyerSceneYesNoBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[2];
	words[0] = (LegoU32) p_resource;
	words[1] = *(LegoU32*) &g_foyerSceneParserTime;
	AppendAllocatedAction(p_resource, 0x10, words, InitializeFoyerSceneAction8);
}

// STUB: LEGORACERS2 0x0047dd40
void __fastcall ParseFoyerSceneSavePositionsBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 type = ParseFoyerSceneAssetType(p_resource, NULL, 1);
	LegoU32 words[4] = {(LegoU32) p_resource, *(LegoU32*) &g_foyerSceneParserTime, 0, 0};
	if (type == 30) {
		words[2] = ParseFoyerSceneAssetType(p_resource, NULL, 7);
		AppendAllocatedAction(p_resource, 0x14, words, InitializeFoyerSceneAction9);
	}
	else if (type == 31) {
		AppendAllocatedAction(p_resource, 0x10, words, InitializeFoyerSceneAction10);
	}
	else if (type == 32) {
		words[2] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
		AppendAllocatedAction(p_resource, 0x14, words, InitializeFoyerSceneAction11);
	}
}

// STUB: LEGORACERS2 0x0047de80
void __fastcall ParseFoyerSceneEffectBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[6] = {(LegoU32) p_resource, *(LegoU32*) &g_foyerSceneParserTime, 0, 0, 0, 0};
	words[2] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
	words[3] = FloatBits(ParseFoyerSceneNumber(p_resource));
	AppendAllocatedAction(p_resource, 0x18, words, InitializeFoyerSceneAction12);
}

// STUB: LEGORACERS2 0x0047dee0
void __fastcall ParseFoyerSceneMobileBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 firstValue = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
	while (IsFoyerSceneParserAtEnd(p_resource, NULL)) {
		LegoU32 directive = GetFoyerSceneTokenId(p_resource, NULL, 1);
		LegoU32 words[10];
		ActionWords(words, 10);
		if (directive == 0x19) {
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = firstValue;
			words[3] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			AppendAllocatedAction(p_resource, 0x18, words, InitializeFoyerSceneAction13);
		}
		else if (directive == 0x1a) {
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = ParseFoyerSceneInteger(p_resource, NULL);
			words[3] = firstValue;
			words[4] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
			words[5] = ParseFoyerSceneInteger(p_resource, NULL);
			words[6] = FloatBits(ParseFoyerSceneNumber(p_resource));
			AppendAllocatedAction(p_resource, 0x24, words, InitializeFoyerSceneAction14);
		}
		else if (directive == 0x1e) {
			words[0] = (LegoU32) p_resource;
			words[1] = *(LegoU32*) &g_foyerSceneParserTime;
			words[2] = firstValue;
			words[3] = ParseFoyerSceneInteger(p_resource, NULL);
			AppendAllocatedAction(p_resource, 0x18, words, InitializeFoyerSceneAction15);
		}
		else {
			SkipFoyerSceneDirective(p_resource, NULL);
		}
	}
}

// STUB: LEGORACERS2 0x0047e020
void __fastcall ParseFoyerSceneJibberBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[3] = {(LegoU32) p_resource, *(LegoU32*) &g_foyerSceneParserTime, 0};
	words[2] = ParseFoyerSceneAssetType(p_resource, NULL, 8);
	AppendAllocatedAction(p_resource, 0x14, words, InitializeFoyerSceneAction17);
}

// STUB: LEGORACERS2 0x0047e080
void __fastcall ParseFoyerSceneRenderSleepBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[4] = {(LegoU32) p_resource, *(LegoU32*) &g_foyerSceneParserTime, 0, 0};
	words[2] = (LegoU32) ParseFoyerSceneObjectReference(p_resource, NULL, 1);
	words[3] = ParseFoyerSceneInteger(p_resource, NULL);
	AppendAllocatedAction(p_resource, 0x18, words, InitializeFoyerSceneAction18);
}

// STUB: LEGORACERS2 0x0047e0e0
void __fastcall ParseFoyerSceneSparkyMenuBlock(FoyerSceneResource* p_resource, void*)
{
	LegoU32 words[2] = {(LegoU32) p_resource, *(LegoU32*) &g_foyerSceneParserTime};
	AppendAllocatedAction(p_resource, 0x10, words, InitializeFoyerSceneAction19);
}
