#include "frontend/overlay/textboxmanager.h"

#include "audio/service/audioservice.h"
#include "core/strings/charbuffer24.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectdispatch.h"
#include "game/session/gamecontrollerplayers.h"
#include "game/session/gamecontrollersession.h"
#include "game/state/gamerandom.h"
#include "memory/memoryheap.h"
#include "render/fonts/bitmapfontset.h"

#include <new>
#include <stdio.h>

void* __fastcall SelectPauseTargetData(GameObject*, void*, LegoU32 p_mode);

namespace
{
extern const LegoS32 g_textBoxVoiceCategories[63];
}

// GLOBAL: LEGORACERS2 0x0056c020
TextBoxFormattingState g_textBoxFormattingState;

namespace
{
LegoS32 ConvertTextBoxTimeToInteger(LegoFloat p_time)
{
	LegoS32 result;
	__asm {
		fld p_time
		fistp result
	}
	return result;
}
} // namespace

extern const TextBoxManagerDispatch g_textBoxManagerDispatch = {
	&TextBoxManagerDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&TextBoxManagerInitialize,
	&GameObjectBaseTrue,
	&GameObjectBaseZeroOneArgument,
	&TextBoxManagerRemove,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument,
	&TextBoxManagerGetObjectSize,
};

extern const NestedTextBoxManagerDispatch g_nestedTextBoxManagerDispatch = {
	&NestedTextBoxManagerDestroy,
	&GameObjectBaseApplyChildTransform,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&NestedTextBoxManagerInitialize,
	&GameObjectBaseZero,
	&NestedTextBoxManagerUpdate,
	&NestedTextBoxManagerRemove,
	&GameObjectBaseZero,
	&GameObjectBaseZero,
	&GameObjectBaseNoOp,
	&GameObjectBaseNoOp,
	&GameObjectBaseDrawChild,
	&GameObjectBaseNoOpOneArgument,
	&GameObjectBaseZero,
	&GameObjectBaseNoOpTwoArguments,
	&GameObjectBaseZero,
	&GameObjectBaseZeroTwoArguments,
	&GameObjectBaseUpdateRegion,
	&GameObjectBaseNoOpOneArgument,
	&NestedTextBoxManagerGetObjectSize,
};

// STUB: LEGORACERS2 0x00437970
NestedTextBoxManagerPrefix::NestedTextBoxManagerPrefix(GameObjectTemplate* p_template)
	: GameObject(0x900, p_template, NULL)
{
	m_active = 0;
	m_selected = -1;
	m_unk0x60 = 0;
	m_unk0x68 = 0;
	m_scale = 1.0f;
	m_unk0x70 = 0;
	m_selectedResource = -1;
	m_resourceElapsed = 0.0f;
	m_vtable = (const GameObjectDispatchView*) &g_nestedTextBoxManagerDispatch;
	LegoU32 i;
	for (i = 0; i < 11; ++i) {
		m_sprites0x7c[i] = NULL;
	}
	m_record0xa8 = NULL;
	for (i = 0; i < 4; ++i) {
		m_records0xac[i] = NULL;
	}
}

namespace
{
void PrepareNestedTextBoxManagerForDestruction(GameObject* p_object)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_nestedTextBoxManagerDispatch;
	p_object->GameObject::~GameObject();
}
} // namespace

// FUNCTION: LEGORACERS2 0x004379f0
LegoU32 __fastcall NestedTextBoxManagerGetObjectSize(GameObject*, void*)
{
	return 0x18e0;
}

// FUNCTION: LEGORACERS2 0x00437a00
GameObject* __fastcall NestedTextBoxManagerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	NestedTextBoxManagerDestructor(p_object, NULL);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00437a20
void __fastcall NestedTextBoxManagerDestructor(GameObject* p_object, void*)
{
	PrepareNestedTextBoxManagerForDestruction(p_object);
}

// STUB: LEGORACERS2 0x00437a30
void __fastcall NestedTextBoxManagerInitialize(GameObject* p_object, void*)
{
	p_object->GameObject::Initialize();
	*(LegoFloat*) ((LegoU8*) p_object + 0x18dc) = 0.1f;
}

// STUB: LEGORACERS2 0x00437a50
void __fastcall NestedTextBoxManagerRemove(GameObject* p_object, void*)
{
	ClearTextBoxManagerResources(p_object, NULL);
	p_object->GameObject::Remove();
}

// STUB: LEGORACERS2 0x00437ac0
LegoU32 __fastcall NestedTextBoxManagerUpdate(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	LegoU8* bytes = (LegoU8*) p_object;
	if (*(LegoU32*) (bytes + 0x58) == 0) {
		return 0;
	}

	UpdateTextBoxVoiceResource(p_object, NULL, p_elapsedSeconds);
	if (*(LegoU32*) (bytes + 0x60) != 0) {
		return 0;
	}

	LegoFloat* accumulatedTime = (LegoFloat*) (bytes + 0x18d4);
	LegoFloat* duration = (LegoFloat*) (bytes + 0x18d8);
	*accumulatedTime = p_elapsedSeconds * *(LegoFloat*) (bytes + 0x6c) + *accumulatedTime;
	if (*accumulatedTime >= 0.6f && HasActiveTextBoxPauseTarget(p_object, NULL)) {
		*accumulatedTime = 0.0f;
		*duration = 0.0f;
	}

	const LegoS32 numSlots = *(LegoS32*) (bytes + 0x18c0);
	const LegoS32 textCount = *(LegoS32*) (bytes + 0x18c4);
	const LegoS32 cursor = *(LegoS32*) (bytes + 0x18c8);
	if (cursor >= textCount && *accumulatedTime >= *duration) {
		*(LegoU32*) (bytes + 0x60) = 1;
		*(LegoFloat*) (bytes + 0x18dc) = 0.1f;
		return 1;
	}

	if (!(*accumulatedTime >= *duration)) {
		if (cursor < textCount) {
			const LegoS32 rounded = ConvertTextBoxTimeToInteger(*accumulatedTime * 100.0f);
			GlobalOverlaySprite* sprite = *(GlobalOverlaySprite**) (bytes + 0xa4);
			sprite->SetVisible(((LegoU32) rounded & 0x7f) < 0x40);
		}
		return 1;
	}

	const LegoS32 newSlot = ((*(LegoS32*) (bytes + 0xbc) + 1) % numSlots);
	*accumulatedTime -= *duration;
	BitmapFont* font = g_gameController.m_fontSet->m_fonts[0];
	const LegoFloat lineHeight = font->m_unk0x20 * font->m_scaleY;

	for (LegoS32 i = 1; i < numSlots; ++i) {
		const LegoS32 sourceIndex = cursor - i;
		if (sourceIndex < 0) {
			break;
		}
		LegoS32 recordIndex = newSlot - i;
		if (recordIndex < 0) {
			recordIndex += numSlots;
		}
		GlobalOverlayRecord* record = *(GlobalOverlayRecord**) (bytes + 0xac + recordIndex * 4);
		const LegoU16* text = (const LegoU16*) (bytes + 0xc0 + sourceIndex * 0x100);
		record->InitializeText(text, 0, 0.0f);
	}

	if (cursor >= numSlots) {
		for (LegoS32 i = 0; i < numSlots; ++i) {
			if (i != newSlot) {
				const LegoFloat position[2] = {0.0f, -lineHeight};
				GlobalOverlayRecord* record = *(GlobalOverlayRecord**) (bytes + 0xac + i * 4);
				record->SetPosition(position, 0xffffffff);
			}
		}
	}

	LegoS32 visibleLine = cursor;
	if (visibleLine >= numSlots) {
		visibleLine = numSlots - 1;
	}
	const LegoFloat position[2] = {
		g_textBoxFormattingState.m_horizontalOrigin + g_textBoxFormattingState.m_leftOffset +
			*(LegoFloat*) (bytes + 0x18cc) + g_textBoxFormattingState.m_rightOffset,
		(LegoFloat) (visibleLine + 1) * lineHeight + g_textBoxFormattingState.m_topOffset +
			*(LegoFloat*) (bytes + 0x18d0),
	};
	GlobalOverlayRecord* currentRecord = *(GlobalOverlayRecord**) (bytes + 0xac + newSlot * 4);
	currentRecord->SetPosition(position, 0xffffffff);

	const LegoU16* currentText = (const LegoU16*) (bytes + 0xc0 + cursor * 0x100);
	currentRecord->InitializeText(currentText, 0, 0.0f);
	const LegoFloat characterDuration = *(LegoFloat*) (bytes + 0x18dc);
	*duration = (LegoFloat) currentRecord->GetTextLength() * characterDuration;
	currentRecord->InitializeText(currentText, 0x10, *duration / *(LegoFloat*) (bytes + 0x6c));
	*(LegoS32*) (bytes + 0xbc) = newSlot;
	*(LegoS32*) (bytes + 0x18c8) = cursor + 1;
	if (cursor + 1 >= textCount) {
		(*(GlobalOverlaySprite**) (bytes + 0xa4))->SetVisible(0);
	}
	else {
		const LegoS32 rounded = ConvertTextBoxTimeToInteger(*accumulatedTime * 100.0f);
		(*(GlobalOverlaySprite**) (bytes + 0xa4))->SetVisible(((LegoU32) rounded & 0x7f) < 0x40);
	}
	return 1;
}

// STUB: LEGORACERS2 0x00438540
void __fastcall ClearTextBoxManagerResources(GameObject* p_object, void*)
{
	LegoU8* bytes = (LegoU8*) p_object;
	LegoS32 offset;
	*(GameObject**) (bytes + 0x58) = NULL;
	ReleaseTextBoxManagerResource(p_object, NULL);

	for (offset = 0xb8; offset >= 0xac; offset -= 4) {
		GlobalOverlayRecord** record = (GlobalOverlayRecord**) (bytes + offset);
		if (*record != NULL) {
			(*record)->Release();
			*record = NULL;
		}
	}
	GlobalOverlayRecord** record = (GlobalOverlayRecord**) (bytes + 0xa8);
	if (*record != NULL) {
		(*record)->Release();
		*record = NULL;
	}
	for (offset = 0xa4; offset >= 0x7c; offset -= 4) {
		GlobalOverlaySprite** sprite = (GlobalOverlaySprite**) (bytes + offset);
		if (*sprite != NULL) {
			(*sprite)->Release();
			*sprite = NULL;
		}
	}
}

// STUB: LEGORACERS2 0x00438be0
LegoBool32 __fastcall HasActiveTextBoxPauseTarget(GameObject*, void*)
{
	if (g_gameController.m_state != 1) {
		return FALSE;
	}

	LegoU8* session = (LegoU8*) GetGameControllerSession();
	const LegoS32 playerCount = *(LegoS32*) (session + 0x18);
	for (LegoS32 i = 0; i < playerCount; ++i) {
		if (*(LegoU32*) (session + 0x58 + i * 0x344) == 1) {
			GameObject* object = *(GameObject**) (session + 0x28 + i * sizeof(GameObject*));
			const LegoU8* pauseData = (const LegoU8*) SelectPauseTargetData(object, NULL, 0);
			if ((*(const LegoU32*) (pauseData + 8) & 8) != 0) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x00438c90
void __fastcall ReleaseTextBoxManagerResource(GameObject* p_object, void*)
{
	LegoU8* bytes = (LegoU8*) p_object;
	AudioSourceRecord* resource = *(AudioSourceRecord**) (bytes + 0x70);
	if (resource != NULL) {
		resource->m_vtable->m_clearSourceFlag(resource, NULL);
		*(AudioSourceRecord**) (bytes + 0x70) = NULL;
	}
	((NestedTextBoxManagerPrefix*) p_object)->m_resourceElapsed = 0.0f;
}

// STUB: LEGORACERS2 0x00438cb0
void __fastcall UpdateTextBoxVoiceResource(GameObject* p_object, void*, LegoFloat p_elapsedSeconds)
{
	LegoU8* bytes = (LegoU8*) p_object;
	const LegoS32 voiceIndex = *(LegoS32*) (bytes + 0x5c);
	if (*(LegoU32*) (bytes + 0x64) == 1 || voiceIndex < 0 || g_textBoxVoiceCategories[voiceIndex] == -1) {
		ReleaseTextBoxManagerResource(p_object, NULL);
		return;
	}

	AudioSourceRecord* resource = *(AudioSourceRecord**) (bytes + 0x70);
	if (resource != NULL) {
		NestedTextBoxManagerPrefix* manager = (NestedTextBoxManagerPrefix*) p_object;
		manager->m_resourceElapsed += p_elapsedSeconds;
		if (!(manager->m_resourceElapsed >= 0.05f)) {
			return;
		}

		const LegoU32 sourceId = resource->m_vtable->m_getIndex(resource, NULL);
		AudioService* audio = GetAudioService(1);
		if (audio->m_vtable->m_validateSource(audio, NULL, sourceId) != 0) {
			return;
		}
		ReleaseTextBoxManagerResource(p_object, NULL);
	}

	if (*(LegoU32*) (bytes + 0x60) != 0) {
		return;
	}

	AudioSourceRecord* selected =
		SelectTextBoxVoice((NestedTextBoxManagerPrefix*) p_object, NULL, voiceIndex, (LegoS32*) (bytes + 0x74));
	*(AudioSourceRecord**) (bytes + 0x70) = selected;
	selected->m_vtable->m_setMaximumGain(selected, NULL, *(LegoFloat*) (bytes + 0x6c));
	selected->m_vtable->m_resetPlaybackState(selected, NULL);
}

namespace
{
struct TextBoxVoiceCategory {
	LegoU32 m_variantCount;
	const LegoChar* m_voiceName;
};

// The selector uses only the first DWORD from each 0x14-byte row at 0x543730.
// Keep this as a compact projection; the remaining words are not modeled here.
const LegoS32 g_textBoxVoiceCategories[63] = {
	2, 2, 1, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2,
	2, 2, 2, 2, 1, 2, 0, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 3, 3, 3, 3, 2, 4, 4, 4, 4, 4, 4, 4, 2, 2,
};

const TextBoxVoiceCategory g_textBoxVoiceCategoryInfo[6] = {
	{7, "JibberBerg"},
	{10, "JibberGenFemale"},
	{8, "JibberGenMale"},
	{10, "JibberMartian"},
	{10, "JibberWarrior"},
	{6, "JibberAngry"},
};
} // namespace

// STUB: LEGORACERS2 0x00438d60
AudioSourceRecord* __fastcall SelectTextBoxVoice(
	NestedTextBoxManagerPrefix* p_manager,
	void*,
	LegoS32 p_voiceIndex,
	LegoS32* p_previousVoiceIndex
)
{
	LegoS32 category;
	if (p_manager->m_unk0x64 == 2) {
		category = 5;
	}
	else if (p_voiceIndex == 0x3d || p_voiceIndex == 0x3e) {
		const LegoU32 playerIndex = (LegoU32) (p_voiceIndex - 0x3d);
		const StartupPlayerSettings* player =
			(const StartupPlayerSettings*) ((const LegoU8*) GetGameControllerSession() + 0x4c +
											playerIndex * sizeof(StartupPlayerSettings));
		category = player->m_unk0x334 != 0 ? 2 : 1;
	}
	else {
		category = g_textBoxVoiceCategories[p_voiceIndex];
	}

	const TextBoxVoiceCategory* info = &g_textBoxVoiceCategoryInfo[category];
	LegoU32 count = info->m_variantCount;
	if (*p_previousVoiceIndex >= 0) {
		--count;
	}

	GameIntegerRandom* random = &GetGameRandomState()->m_integerStreams[1];
	random->m_state = random->m_state * random->m_multiplier + random->m_increment;
	const LegoS32 scaled = (LegoS32) (((random->m_state >> 8) * count));
	LegoS32 selected = scaled >> 24;
	if (*p_previousVoiceIndex >= 0 && selected >= *p_previousVoiceIndex) {
		++selected;
	}
	*p_previousVoiceIndex = selected;

	CharBuffer24 resourceName;
	sprintf(resourceName.m_data, "%s%d", info->m_voiceName, selected + 1);
	AudioService* audio = GetAudioService(1);
	return audio->m_vtable->m_findSource(audio, NULL, (LegoU32) g_gameController.m_npcTextManager, resourceName.m_data);
}

namespace
{
void PrepareTextBoxManagerForDestruction(GameObject* p_object)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_textBoxManagerDispatch;
	p_object->GameObject::~GameObject();
}
} // namespace

// STUB: LEGORACERS2 0x00438e50
TextBoxManager::TextBoxManager(GameObjectTemplate* p_template) : GameObject(0x900, p_template, NULL)
{
	m_nestedManager = NULL;
	m_vtable = (const GameObjectDispatchView*) &g_textBoxManagerDispatch;
}

// FUNCTION: LEGORACERS2 0x00438e80
LegoU32 __fastcall TextBoxManagerGetObjectSize(GameObject*, void*)
{
	return 0x5c;
}

// STUB: LEGORACERS2 0x00438e90
GameObject* __fastcall TextBoxManagerDestroy(GameObject* p_object, void*, LegoU32 p_flags)
{
	PrepareTextBoxManagerForDestruction(p_object);
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00438ec0
void TextBoxManager::Initialize()
{
	if ((m_flags & 0x800000) == 0) {
		GameObjectTemplate nestedTemplate;
		nestedTemplate.m_flags = 2;
		nestedTemplate.m_first = CharBuffer24("Text Box 0");
		nestedTemplate.m_second = CharBuffer24("");

		void* storage = AllocateHeapMemory(0x18e0);
		NestedTextBoxManagerPrefix* nested =
			storage == NULL ? NULL : new (storage) NestedTextBoxManagerPrefix(&nestedTemplate);
		m_nestedManager = nested;
		nested->m_vtable->m_initialize(nested, NULL);
		nested->m_flags |= 0x400000;
	}
	GameObject::Initialize();
}

void __fastcall TextBoxManagerInitialize(GameObject* p_object, void*)
{
	((TextBoxManager*) p_object)->Initialize();
}

// STUB: LEGORACERS2 0x00438fa0
void TextBoxManager::Remove()
{
	GameObject::Remove();
	GameObject* nested = m_nestedManager;
	if (nested != NULL) {
		nested->m_vtable->m_remove(nested, NULL);
		nested->m_vtable->m_destroy(nested, NULL, 1);
		m_nestedManager = NULL;
	}
}

void __fastcall TextBoxManagerRemove(GameObject* p_object, void*)
{
	((TextBoxManager*) p_object)->Remove();
}
