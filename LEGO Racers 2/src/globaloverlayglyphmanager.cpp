#include "gamecontrollerconfig.h"
#include "gamecontrollerinitialization.h"
#include "globaloverlayglyph.h"
#include "globaloverlaymanager.h"
#include "localizedstringtable.h"
#include "memoryheap.h"
#include "renderer.h"

#include <string.h>

struct OverlayIconDefinition {
	LegoChar m_key[16];
	const LegoChar* m_texturePath;
	LegoFloat m_widthScale;
	LegoFloat m_heightScale;
};
struct OverlayStringDefinition {
	LegoChar m_key[16];
	LegoS32 m_stringIndex;
};
DECOMP_SIZE_ASSERT(OverlayIconDefinition, 0x1c);
DECOMP_SIZE_ASSERT(OverlayStringDefinition, 0x14);

// GLOBAL: LEGORACERS2 0x0053fcf0
const OverlayIconDefinition g_overlayIconDefinitions[10] = {
	{"~~X", "Game Data\\Frontend\\Ch Builder Bits\\enter.tga", 1.0f, 1.39999998f},
	{"~~T", "Game Data\\Frontend\\Ch Builder Bits\\escape.tga", 1.0f, 1.20000005f},
	{"~~FRENCHT", "Game Data\\Frontend\\Ch Builder Bits\\echap.tga", 1.0f, 1.20000005f},
	{"~~BACK", "Game Data\\Frontend\\Ch Builder Bits\\backspace.tga", 1.0f, 1.39999998f},
	{"~~UPDOWN", "Game Data\\Frontend\\Ch Builder Bits\\up_down.tga", 1.0f, 1.39999998f},
	{"~~LEFTRIGHT", "Game Data\\Frontend\\Ch Builder Bits\\left_right.tga", 1.0f, 1.39999998f},
	{"~~UDLR", "Game Data\\Frontend\\Ch Builder Bits\\up_down_left_right.tga", 1.0f, 1.39999998f},
	{"~~REG", "Game Data\\Frontend\\Frontend bits\\Misc\\registered.tga", 1.0f, 0.699999988f},
	{"~~UPDIR", "Game Data\\Frontend\\Ch Builder Bits\\d pad up.tga", 1.0f, 1.0f},
	{"~~DOWNDIR", "Game Data\\Frontend\\Ch Builder Bits\\d pad down.tga", 1.0f, 1.0f},
};
// GLOBAL: LEGORACERS2 0x0053fe08
LegoS32 g_overlayIconDefinitionCount = 10;
// GLOBAL: LEGORACERS2 0x0053fe10
const OverlayStringDefinition g_overlayStringDefinitions[10] = {
	{"~~1UP", 0},
	{"~~2UP", 0},
	{"~~O", 0},
	{"~~SQ", 0},
	{"~~L1", 520},
	{"~~R1", 521},
	{"~~L2", 522},
	{"~~R2", 523},
	{"~~START", 0},
	{"~~SELECT", 0},
};
// GLOBAL: LEGORACERS2 0x0053fed8
LegoS32 g_overlayStringDefinitionCount = 10;

static void __fastcall InitializeGlyphManager(GlobalOverlayGlyphReplacementManagerView* p_manager, void*)
{
	p_manager->Initialize();
}
static void __fastcall ReleaseGlyphManager(GlobalOverlayGlyphReplacementManagerView* p_manager, void*)
{
	p_manager->Release();
}
// Retail dispatch table at 0x0052f57c (two typed callback bridges).
const GlobalOverlayGlyphManagerDispatch g_overlayGlyphManagerDispatch = {InitializeGlyphManager, ReleaseGlyphManager};

void* ConstructControllerAuxiliaryState(void* p_storage)
{
	GlobalOverlayGlyphReplacementManagerView* manager = (GlobalOverlayGlyphReplacementManagerView*) p_storage;
	manager->m_vtable = &g_overlayGlyphManagerDispatch;
	manager->m_entryCount = 0;
	manager->m_stringCount = 0;
	manager->m_iconAllocation = NULL;
	manager->m_stringAllocation = NULL;
	return manager;
}

// STUB: LEGORACERS2 0x004334c0
void GlobalOverlayGlyphReplacementManagerView::Initialize()
{
	m_iconAllocation = (GlobalOverlayGlyphEntryView*) GetMemoryHeap()->Allocate(
		g_overlayIconDefinitionCount * sizeof(GlobalOverlayGlyphEntryView),
		"Special Char Icons"
	);
	m_stringAllocation = (GlobalOverlayStringEntry*) GetMemoryHeap()->Allocate(
		g_overlayStringDefinitionCount * sizeof(GlobalOverlayStringEntry),
		"Special Char Strings"
	);
	for (LegoS32 i = 0; i < g_overlayIconDefinitionCount; ++i) {
		GlobalOverlayGlyphEntryView* entry = &m_iconAllocation[i];
		const LegoU8* source = (const LegoU8*) g_overlayIconDefinitions[i].m_key;
		LegoU16* destination = entry->m_text;
		do {
			*destination++ = *source;
		} while (*source++ != 0);
		entry->m_texture = (LegoU32) -1;
		entry->m_widthScale = g_overlayIconDefinitions[i].m_widthScale * 0.75f;
		entry->m_heightScale = g_overlayIconDefinitions[i].m_heightScale;
		RegisterIcon(entry);
	}
	for (LegoS32 j = 0; j < g_overlayStringDefinitionCount; ++j) {
		GlobalOverlayStringEntry* entry = &m_stringAllocation[j];
		const LegoU8* source = (const LegoU8*) g_overlayStringDefinitions[j].m_key;
		LegoU16* destination = entry->m_key;
		do {
			*destination++ = *source;
		} while (*source++ != 0);
		const LegoU16* text;
		if (g_gameController.m_localizedStrings->GetString(g_overlayStringDefinitions[j].m_stringIndex, &text)) {
			destination = entry->m_text;
			do {
				*destination++ = *text;
			} while (*text++ != 0);
			RegisterString(entry);
		}
	}
	for (LegoS32 player = 0; player < 2; ++player) {
		SetPlayerName(&g_gameControllerSettings->m_names[player], player);
	}
	g_globalOverlayManager.SetGlyphReplacementManager(this);
}

// STUB: LEGORACERS2 0x00433640
void GlobalOverlayGlyphReplacementManagerView::Release()
{
	m_stringCount = 0;
	m_entryCount = 0;
	GetMemoryHeap()->Free(m_stringAllocation);
	GetMemoryHeap()->Free(m_iconAllocation);
}

// FUNCTION: LEGORACERS2 0x00433680
void GlobalOverlayGlyphReplacementManagerView::LoadIconTextures()
{
	GlobalOverlayIconTexture texture = {0, 0.0f, 1.0f, 1.0f, 0.0f};
	for (LegoS32 i = 0; i < g_overlayIconDefinitionCount; ++i) {
		texture.m_texture =
			g_displayRenderer->CreateTextureResource(g_overlayIconDefinitions[i].m_texturePath, 0, -1, 1, 1);
		SetIconTexture(i, &texture);
	}
}

// FUNCTION: LEGORACERS2 0x00433700
void GlobalOverlayGlyphReplacementManagerView::ReleaseIconTextures()
{
	GlobalOverlayIconTexture texture;
	texture.m_texture = -1;
	for (LegoS32 i = g_overlayIconDefinitionCount - 1; i >= 0; --i) {
		g_displayRenderer->ReleaseTextureResourceReference(GetIconTexture(i), 1);
		SetIconTexture(i, &texture);
	}
}

// FUNCTION: LEGORACERS2 0x00433750
void GlobalOverlayGlyphReplacementManagerView::SetPlayerName(const GameControllerName* p_name, LegoS32 p_index)
{
	GlobalOverlayStringEntry* entry = &m_stringAllocation[p_index];
	const LegoU16* source = p_name->m_text;
	LegoU16* destination = entry->m_text;
	do {
		*destination++ = *source;
	} while (*source++ != 0);
	SetStringSlot(p_index, entry);
}

// FUNCTION: LEGORACERS2 0x004b38b0
void GlobalOverlayManager::SetGlyphReplacementManager(GlobalOverlayGlyphReplacementManagerView* p_manager)
{
	m_unk0x2494c = (LegoU32) p_manager;
}

// FUNCTION: LEGORACERS2 0x004c4eb0
void GlobalOverlayGlyphReplacementManagerView::RegisterIcon(GlobalOverlayGlyphEntryView* p_entry)
{
	m_entries[m_entryCount++] = p_entry;
}

// FUNCTION: LEGORACERS2 0x004c4ed0
void GlobalOverlayGlyphReplacementManagerView::SetIconTexture(
	LegoU32 p_index,
	const GlobalOverlayIconTexture* p_texture
)
{
	memcpy(&m_entries[p_index]->m_texture, p_texture, sizeof(*p_texture));
}

// FUNCTION: LEGORACERS2 0x004c4ef0
LegoS32 GlobalOverlayGlyphReplacementManagerView::GetIconTexture(LegoU32 p_index)
{
	return m_entries[p_index]->m_texture;
}

// FUNCTION: LEGORACERS2 0x004c4f00
void GlobalOverlayGlyphReplacementManagerView::RegisterString(GlobalOverlayStringEntry* p_entry)
{
	m_strings[m_stringCount++] = p_entry;
}

// FUNCTION: LEGORACERS2 0x004c4f20
void GlobalOverlayGlyphReplacementManagerView::SetStringSlot(LegoS32 p_index, GlobalOverlayStringEntry* p_entry)
{
	m_strings[p_index] = p_entry;
}
