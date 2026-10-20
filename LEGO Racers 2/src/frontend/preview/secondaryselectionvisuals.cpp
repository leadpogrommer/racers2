#include "frontend/preview/secondaryselectionvisuals.h"

#include "core/strings/charbuffer128.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/preview/cardisplay.h"
#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"
#include "game/objects/gameobjectmodelbinding.h"
#include "memory/memoryheap.h"
#include "render/core/renderer.h"

struct SelectionGridVisualStorage {
	LegoU8 m_prefix[0x1d0c];
	GlobalOverlaySprite* m_sprites[32];
	LegoU32 m_unk0x1d8c;
	GameObject* m_models[50];
	LegoU32 m_unk0x1e58;
	LegoU32 m_hasModels;
	CarDisplay* m_displays[8];
	LegoU32 m_unk0x1e80;
	LegoU32 m_hasText;
	CarDisplay* m_moreDisplays[8];
	LegoU32 m_unk0x1ea8;
	GlobalOverlaySprite* m_navigationArrows[2];
	LegoS32 m_arrowTextures[2];
	void* m_sharedModel;
};
DECOMP_SIZE_ASSERT(SelectionGridVisualStorage, 0x1ec0);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_models) == 0x1d90);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_hasModels) == 0x1e5c);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_displays) == 0x1e60);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_hasText) == 0x1e84);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_navigationArrows) == 0x1eac);
DECOMP_STATIC_ASSERT(offsetof(SelectionGridVisualStorage, m_sharedModel) == 0x1ebc);

// GLOBAL: LEGORACERS2 0x00543e4c
static const LegoChar* g_selectionGridInactiveArrowPath = "Game Data\\Frontend\\Ch Builder Bits\\Arrows Inactive.tga";
// GLOBAL: LEGORACERS2 0x00543e50
static const LegoChar* g_selectionGridActiveArrowPath = "Game Data\\Frontend\\Ch Builder Bits\\Arrows Active.tga";
// GLOBAL: LEGORACERS2 0x00570c60
static LegoFloat g_selectionGridArrowSize[2];

// FUNCTION: LEGORACERS2 0x00462190
static void InitializeSelectionGridArrowSize()
{
	g_selectionGridArrowSize[0] = 0.053700000047683716f;
	g_selectionGridArrowSize[1] = 0.12240000069141388f;
}

struct SelectionGridArrowSizeStartup {
	SelectionGridArrowSizeStartup() { InitializeSelectionGridArrowSize(); }
};
static SelectionGridArrowSizeStartup g_selectionGridArrowSizeStartup;

// STUB: LEGORACERS2 0x00462350
void __fastcall InitializeSelectionGridSprites(void* p_object, void*)
{
	SelectionGridVisualStorage* grid = (SelectionGridVisualStorage*) p_object;
	LegoU32 parameters[21];
	parameters[0] = 0;
	parameters[1] = 0;
	LegoFloat* values = (LegoFloat*) parameters;
	values[2] = 0.08f;
	values[3] = 1.3333333730697632f * 0.08f;
	parameters[4] = 0;
	parameters[5] = 0;
	parameters[6] = 0x3f800000;
	parameters[7] = 0x3f800000;
	// Retail leaves tile width and height untouched for this non-tiled sprite.
	parameters[10] = 0xffffffff;
	parameters[11] = 0xffffffff;
	parameters[12] = 0;
	parameters[13] = 0x3f800000;
	parameters[14] = 0x3f800000;
	parameters[15] = 0;
	parameters[16] = 0;
	parameters[17] = 7;
	parameters[18] = 1;
	parameters[19] = 0;
	parameters[20] = 0;
	for (LegoS32 i = 0; i < 32; ++i) {
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		sprite->SetList(2);
		sprite->SetVisible(0);
		sprite->SetTexture(0xffffffff);
		grid->m_sprites[i] = sprite;
	}
}

// STUB: LEGORACERS2 0x004624c0
void __fastcall InitializeSelectionGridModels(void* p_object, void*)
{
	SelectionGridVisualStorage* grid = (SelectionGridVisualStorage*) p_object;
	if (grid->m_hasModels != 0) {
		CharBuffer128 path("Game Data\\Objects\\bricks\\square\\1X1 red.MD2");
		for (LegoS32 i = 0; i < 50; ++i) {
			grid->m_models[i] = CreateCarDisplayPartForModel(path.m_data, 0x0e);
			grid->m_models[i]->m_flags |= 0x400000;
		}
		grid->m_sharedModel = ((GameObjectModelBinding*) grid->m_models[0]->m_unk0x48)->m_model;
	}
}

// STUB: LEGORACERS2 0x00462680
void __fastcall InitializeSelectionGridCarDisplays(void* p_object, void*)
{
	SelectionGridVisualStorage* grid = (SelectionGridVisualStorage*) p_object;
	if (grid->m_hasText != 0) {
		for (LegoS32 i = 0; i < 8; ++i) {
			CarDisplay* display = (CarDisplay*) AllocateHeapMemory(sizeof(CarDisplay));
			if (display != NULL) {
				display->m_activeCount = 0;
				display->m_resourceTable = ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x74;
				display->m_selectedChoice = 0;
			}
			grid->m_displays[i] = display;
			InitializeCarDisplay(display);
		}
	}
}

// STUB: LEGORACERS2 0x004635c0
void __fastcall InitializeSelectionGridNavigationArrows(void* p_object, void*)
{
	SelectionGridVisualStorage* grid = (SelectionGridVisualStorage*) p_object;
	grid->m_arrowTextures[0] = g_displayRenderer->CreateTextureResource(g_selectionGridInactiveArrowPath, 0, -1, 1, 1);
	grid->m_arrowTextures[1] = g_displayRenderer->CreateTextureResource(g_selectionGridActiveArrowPath, 0, -1, 1, 1);
	LegoU32 parameters[21];
	LegoFloat* values = (LegoFloat*) parameters;
	parameters[0] = 0;
	parameters[1] = 0;
	values[2] = g_selectionGridArrowSize[0];
	values[3] = g_selectionGridArrowSize[1] + 0.0f;
	parameters[4] = 0;
	parameters[5] = 0;
	parameters[6] = 0x3f800000;
	parameters[7] = 0x3f800000;
	// The retail packet leaves the unused tiling words uninitialized.
	parameters[10] = 0xffffffff;
	parameters[11] = grid->m_arrowTextures[0];
	parameters[16] = 0;
	parameters[17] = 0;
	parameters[18] = 1;
	parameters[19] = 0;
	parameters[20] = 0;
	for (LegoS32 i = 0; i < 2; ++i) {
		LegoFloat atlasStart = i == 0 ? 3.0f : 69.0f;
		values[12] = atlasStart * 0.0078125f;
		values[13] = 0.1328125f;
		values[14] = (atlasStart + 55.0f) * 0.0078125f;
		values[15] = 0.8671875f;
		grid->m_navigationArrows[i] = g_globalOverlayManager.AllocateSprite(parameters);
		grid->m_navigationArrows[i]->SetList(14);
		grid->m_navigationArrows[i]->SetVisible(0);
		LegoFloat offset = ((0.115f + 0.08f) * 0.5f + 0.07f) * 0.5f;
		LegoFloat position[2];
		position[0] = i == 0 ? 0.5f - offset : offset + 0.5f;
		position[1] = 0.19f + 0.002f;
		grid->m_navigationArrows[i]->SetPosition(position);
	}
}

// FUNCTION: LEGORACERS2 0x004637a0
void __fastcall ReleaseSelectionGridNavigationArrows(void* p_object, void*)
{
	SelectionGridVisualStorage* grid = (SelectionGridVisualStorage*) p_object;
	for (LegoS32 i = 1; i >= 0; --i) {
		grid->m_navigationArrows[i]->Release();
		grid->m_navigationArrows[i] = NULL;
	}
	g_displayRenderer->ReleaseTextureResourceReference(grid->m_arrowTextures[1], 1);
	g_displayRenderer->ReleaseTextureResourceReference(grid->m_arrowTextures[0], 1);
}
