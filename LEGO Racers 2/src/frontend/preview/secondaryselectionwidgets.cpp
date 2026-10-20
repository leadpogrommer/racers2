#include "frontend/preview/secondaryselectionwidgets.h"

#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/preview/cardisplay.h"
#include "frontend/preview/secondaryselectionlabel.h"
#include "frontend/preview/secondaryselectionvisuals.h"
#include "frontend/secondary/secondarymanagerleases.h"
#include "game/controller/gamecontroller.h"
#include "math/vector3.h"
#include "memory/memoryheap.h"
#include "render/buffers/rendererbufferdispatch.h"
#include "render/core/renderer.h"
#include "render/models/modelresourcecache.h"
#include "render/models/modelresourceloading.h"

#include <float.h>
#include <string.h>

struct SecondarySpriteSet {
	GlobalOverlaySprite* m_sprites[128];
	const LegoChar* m_names[128];
	LegoS32 m_count;
};
DECOMP_SIZE_ASSERT(SecondarySpriteSet, 0x404);

struct SelectionGridEntry {
	SelectionGridEntry* m_previous[5];
	SelectionGridEntry* m_next[5];
	LegoS32 m_texture;
	ModelResource* m_model;
	const void* m_visualData;
	LegoU32 m_unk0x34, m_unk0x38, m_type, m_id, m_worldMask, m_unk0x48;
	LegoFloat m_scale;
	LegoU32 m_unk0x50;
	Vector3 m_offset;
	LegoU32 m_values[5];
};
DECOMP_SIZE_ASSERT(SelectionGridEntry, 0x74);
struct SelectionGridCategory {
	LegoU32 m_type;
	LegoS32 m_counts[5], m_totalCounts[5];
	LegoU32 m_unk0x2c;
	SelectionGridEntry* m_selected[5];
	SelectionGridEntry* m_first[5];
	SelectionGridEntry* m_last[5];
	LegoU32 m_unk0x6c, m_wrap;
};
DECOMP_SIZE_ASSERT(SelectionGridCategory, 0x74);
struct SelectionGrid {
	SelectionGridCategory m_categories[64];
	SelectionGridEntry* m_entries;
	LegoS32 m_entryCount;
	LegoU32 m_capacity;
	GlobalOverlaySprite* m_sprites[32];
	LegoU32 m_unk0x1d8c;
	GameObject* m_models[50];
	LegoU32 m_unk0x1e58;
	LegoU32 m_hasModels;
	void* m_displays[8];
	LegoU32 m_unk0x1e80, m_hasCarDisplays;
	void* m_moreDisplays[8];
	undefined m_unk0x1ea8[0x1eec - 0x1ea8];
	SecondarySelectionLabel* m_labels[2];
	LegoU32 m_unk0x1ef4;
	SecondarySpriteSet m_spriteSet;
	LegoU32 m_unk0x22fc;
	LegoS32 m_texture, m_oldTexture;
	LegoU32 m_unk0x2308;
};
DECOMP_SIZE_ASSERT(SelectionGrid, 0x230c);
DECOMP_STATIC_ASSERT(offsetof(SelectionGrid, m_spriteSet) == 0x1ef8);
static SelectionGridEntry* __fastcall AllocateSelectionGridEntry(SelectionGrid*, void*);
static void __fastcall RegisterSelectionGridEntry(SelectionGrid*, void*, LegoU32, SelectionGridEntry*, LegoU32);
void __cdecl MeasureSelectionGridModel(ModelResource*, SelectionGridEntry*);

static void __fastcall ReleaseSelectionGridCamera(SelectionGrid*, void*);
static void __fastcall SetSelectionGridTextVisible(GlobalOverlayRecord*, void*, LegoU32);
// GLOBAL: LEGORACERS2 0x005447f8
static const SecondarySelectionSpriteDefinition g_selectionGridSpriteDefinitions[3] = {
	{"icon_background",
	 {0.0f, 0.129999995f, 1.0f, 0.379999995f},
	 {0.100000001f, 0.125f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\red_peg_tile.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000002,
	 0x00000000},
	{"icon_background_trans0",
	 {0.0f, 0.129999995f, 1.0f, 0.25f},
	 {0.0f, 0.0f},
	 0xc0ffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch middle.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 0x00000001},
	{"string_background",
	 {0.0f, 0.25f, 1.0f, 0.300000012f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Common\\Textures\\bricktile_1_black.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000005},
};

// STUB: LEGORACERS2 0x00460690
void __fastcall InitializeSecondarySpriteSet(
	void* p_object,
	void*,
	const SecondarySelectionSpriteDefinition* p_definitions,
	LegoS32 p_count
)
{
	SecondarySpriteSet* set = (SecondarySpriteSet*) p_object;
	set->m_count = 0;
	LegoU32 parameters[21];
	memset(parameters, 0, sizeof(parameters));
	parameters[6] = parameters[7] = 0x3f800000;
	for (LegoS32 index = 0; index < p_count; ++index) {
		const SecondarySelectionSpriteDefinition& definition = p_definitions[index];
		memcpy(parameters, definition.m_rectangle, 4 * sizeof(LegoFloat));
		memcpy(parameters + 8, definition.m_tileSize, 2 * sizeof(LegoFloat));
		parameters[10] = definition.m_color;
		parameters[11] = g_displayRenderer->CreateTextureResource(definition.m_texturePath, 0, -1, 1, 1);
		memcpy(parameters + 12, definition.m_textureRegion, 4 * sizeof(LegoFloat));
		parameters[16] = definition.m_unk0x34;
		parameters[17] = definition.m_unk0x38;
		parameters[18] = definition.m_unk0x3c;
		GlobalOverlaySprite* sprite = g_globalOverlayManager.AllocateSprite(parameters);
		set->m_sprites[set->m_count] = sprite;
		sprite->SetList(definition.m_list);
		set->m_names[set->m_count] = definition.m_name;
		sprite->SetVisible(0);
		++set->m_count;
	}
}

// STUB: LEGORACERS2 0x004608a0
void __fastcall ShowSecondarySpriteSet(void* p_object, void*)
{
	SecondarySpriteSet* set = (SecondarySpriteSet*) p_object;
	for (LegoS32 index = 0; index < set->m_count; ++index) {
		set->m_sprites[index]->SetVisible(1);
	}
}

// STUB: LEGORACERS2 0x004608d0
void __fastcall HideSecondarySpriteSet(void* p_object, void*)
{
	SecondarySpriteSet* set = (SecondarySpriteSet*) p_object;
	for (LegoS32 index = 0; index < set->m_count; ++index) {
		set->m_sprites[index]->SetVisible(0);
	}
}

// STUB: LEGORACERS2 0x00460900
void __fastcall ReleaseSecondarySpriteSet(void* p_object, void*)
{
	SecondarySpriteSet* set = (SecondarySpriteSet*) p_object;
	for (LegoS32 index = 0; index < set->m_count; ++index) {
		set->m_names[index] = NULL;
		GlobalOverlaySprite* sprite = set->m_sprites[index];
		sprite->SetVisible(0);
		g_displayRenderer->ReleaseTextureResourceReference(sprite->m_texture, 1);
		sprite->Release();
		set->m_sprites[index] = NULL;
	}
	set->m_count = 0;
}

// STUB: LEGORACERS2 0x00460970
GlobalOverlaySprite* __fastcall FindSecondarySprite(void* p_object, void*, const LegoChar* p_name)
{
	SecondarySpriteSet* set = (SecondarySpriteSet*) p_object;
	for (LegoS32 index = 0; index < set->m_count; ++index) {
		if (strcmp(p_name, set->m_names[index]) == 0) {
			return set->m_sprites[index];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x00460a60
void __fastcall InitializeSecondarySelectionGrid(void* p_object, void*)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	grid->m_entries = (SelectionGridEntry*) GetMemoryHeap()->Allocate(
		grid->m_capacity * sizeof(SelectionGridEntry),
		"Memory for cScrollingMenu"
	);
	grid->m_entryCount = 0;
	grid->m_hasModels = grid->m_hasCarDisplays = 0;
	for (LegoS32 label = 0; label < 2; ++label) {
		void* storage = AllocateHeapMemory(sizeof(SecondarySelectionLabel));
		grid->m_labels[label] =
			storage == NULL ? NULL : (SecondarySelectionLabel*) ConstructSecondarySelectionLabel(storage, NULL);
		LoadSecondarySelectionLabel(grid->m_labels[label], NULL);
	}
	for (LegoS32 category = 0; category < 64; ++category) {
		for (LegoS32 world = 0; world < 5; ++world) {
			grid->m_categories[category].m_counts[world] = -1;
		}
	}
}

// STUB: LEGORACERS2 0x00460b10
void __fastcall DefineSecondarySelectionCategory(
	void* p_object,
	void*,
	const SecondarySelectionCategoryDefinition* p_definition
)
{
	LegoU32* words = (LegoU32*) ((LegoU8*) p_object + p_definition->m_values[0] * 0x74);
	for (LegoS32 index = 0; index < 5; ++index) {
		words[1 + index] = words[6 + index] = words[12 + index] = words[17 + index] = words[22 + index] = 0;
	}
	words[11] = 0;
	words[0] = p_definition->m_values[1];
	words[27] = p_definition->m_values[2];
	words[28] = p_definition->m_values[3];
}
// STUB: LEGORACERS2 0x00460b60
void __fastcall AddSecondarySelectionItem(
	void* p_object,
	void*,
	const SecondarySelectionItemDefinition* p_definition,
	LegoU32 p_worldMask
)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	const LegoU32* values = p_definition->m_values;
	SelectionGridEntry* entry = AllocateSelectionGridEntry(grid, NULL);
	RegisterSelectionGridEntry(grid, NULL, values[0], entry, p_worldMask);
	entry->m_texture =
		values[3] == 0 ? -1 : g_displayRenderer->CreateTextureResource((const LegoChar*) values[3], 0, -1, 1, 1);
	const LegoFloat* floats = (const LegoFloat*) values;
	if (values[4] != 0) {
		entry->m_model = LoadModelResource((const LegoChar*) values[4], 0, 0, 1);
		MeasureSelectionGridModel(entry->m_model, entry);
		LegoFloat multiplier = floats[9] != 0.0f ? 1.0f / floats[9] : 1.0f;
		entry->m_scale *= multiplier;
		entry->m_offset.m_y += floats[11];
		grid->m_hasModels = 1;
	}
	else {
		entry->m_model = NULL;
	}
	if (values[5] != 0) {
		entry->m_visualData = (const void*) values[5];
		entry->m_scale = floats[9] <= 0.001f ? 1.0f : 1.0f / floats[9];
		entry->m_offset.m_x = entry->m_offset.m_z = 0.0f;
		entry->m_offset.m_y = floats[11];
		grid->m_hasCarDisplays = 1;
	}
	else {
		entry->m_visualData = NULL;
	}
	entry->m_unk0x48 = values[8];
	entry->m_unk0x50 = values[10];
	entry->m_unk0x34 = values[6];
	entry->m_unk0x38 = values[7];
	entry->m_type = values[2];
	entry->m_id = values[1];
	entry->m_worldMask = p_worldMask;
	memcpy(entry->m_values, values + 12, 5 * 4);
}

// STUB: LEGORACERS2 0x00460cd0
void __fastcall FinalizeSecondarySelectionGrid(void* p_object, void*)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	for (LegoS32 category = 0; category < 64; ++category) {
		for (LegoS32 world = 0; world < 5; ++world) {
			grid->m_categories[category].m_selected[world] = grid->m_categories[category].m_first[world];
		}
	}
	grid->m_texture =
		g_displayRenderer->CreateTextureResource("Game Data\\Frontend\\Ch Builder Bits\\cross2.tga", 0, -1, 1, 1);
	InitializeSelectionGridSprites(grid, NULL);
	InitializeSelectionGridModels(grid, NULL);
	InitializeSelectionGridCarDisplays(grid, NULL);
	InitializeSelectionGridNavigationArrows(grid, NULL);
	InitializeSecondarySpriteSet(&grid->m_spriteSet, NULL, g_selectionGridSpriteDefinitions, 3);
	grid->m_oldTexture = FindSecondarySprite(&grid->m_spriteSet, NULL, "icon_background")->m_texture;
	LegoU32 white = -1;
	LegoFloat position[2] = {0.0f, 0.25200000405311584f};
	grid->m_labels[0]->m_record->SetColor(&white);
	grid->m_labels[1]->m_record->SetColor(&white);
	grid->m_labels[0]->m_record->SetPosition(position, -1);
	grid->m_labels[1]->m_record->SetPosition(position, -1);
	grid->m_labels[0]->m_record->SetList(6);
	grid->m_labels[1]->m_record->SetList(6);
	SetSelectionGridTextVisible(grid->m_labels[0]->m_record, NULL, 0);
	SetSelectionGridTextVisible(grid->m_labels[1]->m_record, NULL, 0);
}

// STUB: LEGORACERS2 0x00460f10
void __fastcall HideSecondarySelectionGrid(void* p_object, void*)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	HideSecondarySpriteSet(&grid->m_spriteSet, NULL);
	for (LegoS32 label = 0; label < 2; ++label) {
		SetSelectionGridTextVisible(grid->m_labels[label]->m_record, NULL, 0);
	}
	for (LegoS32 sprite = 0; sprite < 32; ++sprite) {
		grid->m_sprites[sprite]->SetVisible(0);
	}
	for (LegoS32 model = 0; model < 50; ++model) {
		GameObject* object = grid->m_models[model];
		if (object != NULL && (object->m_flags & 8) != 8) {
			object->m_flags |= 8;
		}
	}
	for (LegoS32 display = 0; display < 8; ++display) {
		if (grid->m_displays[display] != NULL) {
			ResetCarDisplay((CarDisplay*) grid->m_displays[display]);
		}
	}
	GlobalOverlaySprite** arrows = (GlobalOverlaySprite**) ((LegoU8*) grid + 0x1eac);
	arrows[0]->SetVisible(0);
	arrows[1]->SetVisible(0);
	ReleaseSelectionGridCamera(grid, NULL);
}

// STUB: LEGORACERS2 0x00461110
static void __fastcall ReleaseSelectionGridCamera(SelectionGrid* p_grid, void*)
{
	LegoS32& key = *(LegoS32*) ((LegoU8*) p_grid + 0x1ec4);
	if (key != -1) {
		void** camera = (void**) ((LegoU8*) p_grid + 0x1ec0);
		ReleaseMenuPreviewCamera((SecondaryManager*) g_gameController.m_secondaryManager, NULL, camera, key);
		*camera = NULL;
		key = -1;
	}
}

// STUB: LEGORACERS2 0x00461260
void __fastcall SetSecondarySelectionGridFont(void* p_object, void*, BitmapFont* p_font)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	SetSecondarySelectionLabelFont(grid->m_labels[0], NULL, p_font, 0.4f);
	SetSecondarySelectionLabelFont(grid->m_labels[1], NULL, p_font, 0.4f);
}

// STUB: LEGORACERS2 0x004612a0
void __fastcall ReleaseSecondarySelectionGrid(void* p_object, void*)
{
	SelectionGrid* grid = (SelectionGrid*) p_object;
	FindSecondarySprite(&grid->m_spriteSet, NULL, "icon_background")->SetTexture(grid->m_oldTexture);
	ReleaseSecondarySpriteSet(&grid->m_spriteSet, NULL);
	ReleaseSelectionGridNavigationArrows(grid, NULL);
	for (LegoS32 display = 7; display >= 0; --display) {
		if (grid->m_displays[display] != NULL) {
			ReleaseCarDisplay((CarDisplay*) grid->m_displays[display]);
			FreeHeapMemory(grid->m_displays[display]);
			grid->m_displays[display] = NULL;
		}
	}
	for (LegoS32 model = 49; model >= 0; --model) {
		GameObject* object = grid->m_models[model];
		if (object != NULL) {
			*(LegoU32*) ((LegoU8*) object->m_unk0x48 + 8) = *(LegoU32*) ((LegoU8*) grid + 0x1ebc);
			object->m_vtable->m_remove(object, NULL);
			if (object != NULL) {
				object->m_vtable->m_destroy(object, NULL, 1);
			}
			grid->m_models[model] = NULL;
		}
	}
	for (LegoS32 sprite = 0; sprite < 32; ++sprite) {
		grid->m_sprites[sprite]->Release();
	}
	g_displayRenderer->ReleaseTextureResourceReference(grid->m_texture, 1);
	for (LegoS32 index = grid->m_entryCount - 1; index >= 0; --index) {
		SelectionGridEntry& entry = grid->m_entries[index];
		if (entry.m_model != NULL && g_modelResourceCache.Release(entry.m_model)) {
			entry.m_model = NULL;
		}
		if (entry.m_texture >= 0) {
			g_displayRenderer->ReleaseTextureResourceReference(entry.m_texture, 1);
		}
	}
	for (LegoS32 label = 1; label >= 0; --label) {
		ReleaseSecondarySelectionLabel(grid->m_labels[label], NULL);
		if (grid->m_labels[label] != NULL) {
			FreeHeapMemory(grid->m_labels[label]);
		}
	}
	GetMemoryHeap()->Free(grid->m_entries);
}

// STUB: LEGORACERS2 0x004621b0
static SelectionGridEntry* __fastcall AllocateSelectionGridEntry(SelectionGrid* p_grid, void*)
{
	return &p_grid->m_entries[p_grid->m_entryCount++];
}

// STUB: LEGORACERS2 0x004621e0
static void __fastcall RegisterSelectionGridEntry(
	SelectionGrid* p_grid,
	void*,
	LegoU32 p_category,
	SelectionGridEntry* p_entry,
	LegoU32 p_worldMask
)
{
	SelectionGridCategory& category = p_grid->m_categories[p_category];
	for (LegoS32 world = 0; world < 5; ++world) {
		if ((p_worldMask & (1 << world)) == 0) {
			continue;
		}
		if (category.m_counts[world] == 0) {
			category.m_first[world] = category.m_last[world] = p_entry;
			if (category.m_wrap != 0) {
				p_entry->m_next[world] = p_entry->m_previous[world] = p_entry;
			}
			else {
				p_entry->m_next[world] = p_entry->m_previous[world] = NULL;
			}
		}
		else {
			SelectionGridEntry* first = category.m_first[world];
			SelectionGridEntry* last = category.m_last[world];
			last->m_next[world] = p_entry;
			p_entry->m_previous[world] = last;
			if (category.m_wrap != 0) {
				first->m_previous[world] = p_entry;
				p_entry->m_next[world] = first;
			}
			else {
				p_entry->m_next[world] = NULL;
			}
			category.m_last[world] = p_entry;
		}
		++category.m_counts[world];
		++category.m_totalCounts[world];
	}
}

// STUB: LEGORACERS2 0x00463390
void __cdecl MeasureSelectionGridModel(ModelResource* p_model, SelectionGridEntry* p_entry)
{
	Vector3 minimum = {FLT_MAX, FLT_MAX, FLT_MAX};
	Vector3 maximum = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
	ModelTextureGroup* group = p_model->m_groups;
	for (LegoS32 primitive = 0; primitive < group->m_primitiveCount; ++primitive) {
		RendererVertexBuffer* buffer = group->m_primitives[primitive].m_vertexBuffer;
		const RendererVertexBufferDispatch* dispatch = (const RendererVertexBufferDispatch*) buffer->m_vtable;
		LegoS32 count = dispatch->m_getCount(buffer, NULL);
		dispatch->m_lock(buffer, NULL, 0, count, 0, 0);
		for (LegoS32 vertex = 0; vertex < count; ++vertex) {
			dispatch->m_setCursor(buffer, NULL, vertex);
			const Vector3* position = dispatch->m_getCurrentPosition(buffer, NULL);
			if (position->m_x > maximum.m_x) {
				maximum.m_x = position->m_x;
			}
			if (position->m_x < minimum.m_x) {
				minimum.m_x = position->m_x;
			}
			if (position->m_y > maximum.m_y) {
				maximum.m_y = position->m_y;
			}
			if (position->m_y < minimum.m_y) {
				minimum.m_y = position->m_y;
			}
			if (position->m_z > maximum.m_z) {
				maximum.m_z = position->m_z;
			}
			if (position->m_z < minimum.m_z) {
				minimum.m_z = position->m_z;
			}
		}
		dispatch->m_unlock(buffer, NULL, 0);
	}
	LegoFloat height = maximum.m_y - minimum.m_y;
	p_entry->m_scale = height > 0.24f ? height * 4.1666669845581055f : 1.0f;
	p_entry->m_offset.m_x = (minimum.m_x + maximum.m_x) * -0.5f;
	p_entry->m_offset.m_y = (minimum.m_y + maximum.m_y) * -0.5f;
	p_entry->m_offset.m_z = (minimum.m_z + maximum.m_z) * -0.5f;
}
// STUB: LEGORACERS2 0x004b3950
static void __fastcall SetSelectionGridTextVisible(GlobalOverlayRecord* p_record, void*, LegoU32 p_visible)
{
	if (p_visible == 1) {
		p_record->m_flags |= 2;
	}
	else {
		p_record->m_flags &= ~2;
	}
}
