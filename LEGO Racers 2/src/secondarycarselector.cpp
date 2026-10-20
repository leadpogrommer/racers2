#include "secondarycarselector.h"

#include "cardisplay.h"
#include "characterselectionposition.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"
#include "modelresourcecache.h"
#include "modelresourceloading.h"
#include "renderer.h"
#include "secondarybrickresources.h"
#include "secondarymanager.h"
#include "secondaryselectionwidgets.h"
#include "vector3.h"

#include <new.h>
#include <string.h>

extern LegoChar g_carDisplayModelPathStorage[14][128];
extern Vector3 g_characterBuilderPositionOffset;

struct SecondaryBrickBuilder : GameObject {
	LegoFloat m_unk0x58, m_unk0x5c;
	LegoU32 m_worldMask, m_nameCount, m_unk0x68;
	GameObject** m_names;
	LegoU8 m_cellStates[36][4];
	LegoS32 m_selectedName;
	void* m_selection;
	GameObject* m_parts[36];
	LegoS32 m_selectedPart;
	void* m_selectedPartRecord;
	SecondaryCarResourcePool* m_pool;
	LegoU32 m_unk0x1a4;
	GameControllerItemBlock m_car;
	LegoU32 m_selectionCount, m_selectionWords[37];
	Vector3 m_offset;
	LegoU32 m_unk0x540;
	LegoU8 m_gridDimensions[4];
	LegoS32 m_gridSize;
	LegoU8* m_grid;
};
DECOMP_SIZE_ASSERT(SecondaryBrickBuilder, 0x550);
DECOMP_STATIC_ASSERT(offsetof(SecondaryBrickBuilder, m_car) == 0x1a8);
DECOMP_STATIC_ASSERT(offsetof(SecondaryBrickBuilder, m_offset) == 0x534);

struct SecondaryCarSelector {
	LegoU32 m_worldMask;
	LegoS32 m_carSet, m_unk0x08;
	LegoU32 m_player, m_flags, m_unk0x14, m_unk0x18;
	BitmapFont* m_font;
	SecondaryBrickBuilder* m_builder;
	CarDisplay* m_display;
	undefined m_unk0x28[0x8c - 0x28];
	undefined m_spriteSet[0x404];
	undefined m_grid[0x2308];
	LegoU32 m_unk0x2798;
	LegoS32 m_texture;
	undefined m_unk0x27a0[0x27e0 - 0x27a0];
	ModelResource** m_wheelModels;
};
DECOMP_SIZE_ASSERT(SecondaryCarSelector, 0x27e4);

static void __fastcall InitializeSecondaryBrickBuilder(GameObject*, void*);
static void __fastcall ReleaseSecondaryBrickBuilder(GameObject*, void*);
static void __fastcall AllocateSecondaryBuilderGrid(SecondaryBrickBuilder*, void*);
static void __fastcall ClearSecondaryBuilderGrid(SecondaryBrickBuilder*, void*);
static void __fastcall FreeSecondaryBuilderGrid(SecondaryBrickBuilder*, void*);
static GameObject* __fastcall DestroySecondaryBrickBuilder(GameObject*, void*, LegoU32);
static LegoU32 __fastcall GetSecondaryBrickBuilderSize(GameObject*, void*);
static void __fastcall LoadSecondaryWheelModels(SecondaryCarSelector*, void*);
static void __fastcall ReleaseSecondaryWheelModels(SecondaryCarSelector*, void*);
void __fastcall BuildSecondaryCarSelectorGrid(SecondaryCarSelector*, void*);
static void InitializeSecondaryCarMenuItems();
static void InitializeSecondaryCarPanelSprites();
static void InitializeSecondaryBuilderGridDimensions();
static void InitializeSecondaryBuilderCellDimensions();
struct SecondaryBuilderTuningStartup {
	SecondaryBuilderTuningStartup()
	{
		InitializeSecondaryCarMenuItems();
		InitializeSecondaryCarPanelSprites();
		InitializeSecondaryBuilderGridDimensions();
		InitializeSecondaryBuilderCellDimensions();
	}
};
// GLOBAL: LEGORACERS2 0x005714a8
static LegoU8 g_secondaryBuilderGridDimensions[4];
// GLOBAL: LEGORACERS2 0x00571610
static LegoU8 g_secondaryBuilderCellDimensions[4];
static SecondaryBuilderTuningStartup g_secondaryBuilderTuningStartup;
struct SecondaryBrickBuilderDispatch {
	GameObjectDispatchView m_base;
	GameObjectDispatchView::Value m_getSize;
};
extern const SecondaryBrickBuilderDispatch g_secondaryBrickBuilderDispatch = {
	{&DestroySecondaryBrickBuilder,
	 &GameObjectBaseApplyChildTransform,
	 &GameObjectBaseNoOp,
	 &GameObjectBaseNoOp,
	 &InitializeSecondaryBrickBuilder,
	 &GameObjectBaseTrue,
	 &GameObjectBaseZeroOneArgument,
	 &ReleaseSecondaryBrickBuilder,
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
	 &GameObjectBaseNoOpOneArgument},
	&GetSecondaryBrickBuilderSize
};

// GLOBAL: LEGORACERS2 0x005461b8
static SecondarySelectionSpriteDefinition g_carSelectorSpriteDefinitions[26] = {
	{"BackBottom",
	 {0.0f, 0.25f, 1.0f, 1.0f},
	 {0.100000001f, 0.125f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\blue_peg_tile.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000002,
	 0x00000004},
	{"BackTop",
	 {0.0f, 0.0f, 1.0f, 0.129999995f},
	 {0.100000001f, 0.129999995f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\blue_peg_tile.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000002,
	 0x00000004},
	{"RacersLogo1",
	 {0.0f, 0.0f, 0.100000001f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\LR2 Logo\\Car Builder logo 01.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"RacersLogo2",
	 {0.100000001f, 0.0f, 0.200000003f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\LR2 Logo\\Car Builder logo 02.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"RacersLogo3",
	 {0.200000003f, 0.0f, 0.300000012f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\LR2 Logo\\Car Builder logo 03.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"RacersLogo4",
	 {0.300000012f, 0.0f, 0.400000006f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\LR2 Logo\\Car Builder logo 04.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"RacersLogo5",
	 {0.400000006f, 0.0f, 0.5f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\LR2 Logo\\Car builder logo 05.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"BricksLeft",
	 {0.0299999993f, 0.819999993f, 0.0799999982f, 0.887000024f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Car Builder Bits\\square brick logo.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000006},
	{"TopDown0",
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
	{NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 {0.0f, 0.0f},
	 0x00000000,
	 NULL,
	 {0.0f, 0.0f, 0.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 0x00000000},
};
// GLOBAL: LEGORACERS2 0x00545b70
static const SecondarySelectionCategoryDefinition g_carSelectorCategories[12] = {
	{{0x00000000, 0xffffffff, 0x00000003, 0x00000000}},
	{{0x00000001, 0x00000000, 0x00000059, 0x00000001}},
	{{0x00000009, 0x00000000, 0x00000059, 0x00000000}},
	{{0x00000002, 0x00000000, 0x0000005a, 0x00000000}},
	{{0x00000003, 0x00000002, 0x0000005b, 0x00000001}},
	{{0x00000004, 0x00000002, 0x0000005c, 0x00000001}},
	{{0x00000005, 0x00000002, 0x0000005c, 0x00000001}},
	{{0x00000006, 0x00000002, 0x0000005c, 0x00000001}},
	{{0x00000007, 0x00000002, 0x0000005d, 0x00000001}},
	{{0x00000008, 0x00000002, 0x0000005e, 0x00000001}},
	{{0x0000000a, 0xffffffff, 0x00000003, 0x00000000}},
	{{0x0000000b, 0x0000000a, 0x00000216, 0x00000001}},
};
// The first four grid records use two different pointer-bearing variants.
// Keep their true types in the contiguous retail catalogue; the grid API reads
// the shared 17-word packet only at the AddSecondarySelectionItem boundary.
struct SecondaryCarTextureItemDefinition {
	LegoU32 m_prefix[3];
	const LegoChar* m_texturePath;
	LegoU32 m_suffix[13];
};
struct SecondaryCarModelItemDefinition {
	LegoU32 m_prefix[4];
	const LegoChar* m_modelPath;
	LegoU32 m_suffix[12];
};
struct SecondaryCarItemCatalogue {
	SecondaryCarTextureItemDefinition m_chassis;
	SecondaryCarModelItemDefinition m_garage;
	SecondaryCarTextureItemDefinition m_accept;
	SecondaryCarTextureItemDefinition m_cancel;
	SecondaryCarModelItemDefinition m_carModel;
	SecondaryCarTextureItemDefinition m_categoryIcons[5];
	SecondaryCarTextureItemDefinition m_actions[3];
};
DECOMP_SIZE_ASSERT(SecondaryCarTextureItemDefinition, 0x44);
DECOMP_SIZE_ASSERT(SecondaryCarModelItemDefinition, 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarTextureItemDefinition, m_texturePath) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarModelItemDefinition, m_modelPath) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_garage) == 1 * 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_accept) == 2 * 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_cancel) == 3 * 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_carModel) == 4 * 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_categoryIcons) == 5 * 0x44);
DECOMP_STATIC_ASSERT(offsetof(SecondaryCarItemCatalogue, m_actions) == 10 * 0x44);
DECOMP_SIZE_ASSERT(SecondaryCarItemCatalogue, 13 * 0x44);

// GLOBAL: LEGORACERS2 0x00545c34
static const LegoChar* g_carSelectorHeaderPaths[6] = {
	"Game Data\\Frontend\\Car Builder Bits\\Meshes for headers\\car icon.MD2",
	"Game Data\\Frontend\\Car Builder Bits\\square brick logo PC.tga",
	"Game Data\\Frontend\\Car Builder Bits\\sloped brick logo PC.tga",
	"Game Data\\Frontend\\Car Builder Bits\\weird brick logo PC.tga",
	"Game Data\\Frontend\\Car Builder Bits\\windows logo PC.tga",
	"Game Data\\Frontend\\Car Builder Bits\\trim logo PC.tga"
};

// GLOBAL: LEGORACERS2 0x00545c50
static SecondaryCarItemCatalogue g_carSelectorItems = {
	{{0x00000000, 0x00000002, 0x00000001},
	 "Game Data\\Frontend\\Car Builder Bits\\Chassis logo.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0x0000005f,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000000, 0x00000000},
	 "Game Data\\Frontend\\Car Builder Bits\\Meshes for headers\\garage.MD2",
	 {0x00000000,
	  0x00000001,
	  0x00000060,
	  0xffffffff,
	  0x3fb33333,
	  0x000000e1,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000009, 0xffffffff, 0x00000000},
	 "Game Data\\Frontend\\Ch Builder Bits\\tick.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000057,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000009, 0xffffffff, 0x00000001},
	 "Game Data\\Frontend\\Ch Builder Bits\\cross2.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000058,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000000, 0x00000000},
	 NULL,
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0x00000000}},
	{{{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}}},
	{{{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}},
	 {{0x00000000, 0x00000000, 0x00000000},
	  NULL,
	  {0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000,
	   0x00000000}}}
};

extern const LegoS32 g_secondaryWorldCarStarts[5], g_secondaryWorldCarCounts[5];
static void __cdecl DefineSecondaryGridRowMarker(LegoU32, SecondarySelectionItemDefinition*);
static void __cdecl DefineSecondaryCarSelectionItem(LegoU32, LegoS32, void*, SecondarySelectionItemDefinition*);
static void __cdecl DefineSecondarySavedCarSelectionItem(LegoU32, void*, SecondarySelectionItemDefinition*);
static void __cdecl DefineSecondaryBrickSelectionItem(
	SecondaryCarResourcePool*,
	const CharBuffer128*,
	LegoS32,
	LegoS32,
	SecondarySelectionItemDefinition*
);
static LegoS32 __stdcall FindSecondaryChassisTag(const CharBuffer128*);

// GLOBAL: LEGORACERS2 0x005327d0
static const LegoU32 g_secondaryCarItemScales[38][2] = {
	{0x3f99999a, 0x00000000}, {0x3f800000, 0x3f000000}, {0x3f800000, 0x3f333333}, {0x3fb33333, 0x3e4ccccd},
	{0x3f666666, 0x00000000}, {0x3f800000, 0x00000000}, {0x3f666666, 0x3f99999a}, {0x3fa66666, 0x3f000000},
	{0x3fa66666, 0x3e4ccccd}, {0x3f800000, 0x00000000}, {0x3f800000, 0x00000000}, {0x3f800000, 0x3f19999a},
	{0x3f800000, 0x3f19999a}, {0x3f666666, 0x00000000}, {0x3f666666, 0x3fe66666}, {0x3fa66666, 0x3e4ccccd},
	{0x3fa66666, 0x3e99999a}, {0x3fa66666, 0x3ecccccd}, {0x3f800000, 0x3ecccccd}, {0x3f800000, 0x00000000},
	{0x3f8ccccd, 0x3f000000}, {0x3f8ccccd, 0x3f800000}, {0x3f800000, 0x00000000}, {0x3fb33333, 0x3e4ccccd},
	{0x3fc00000, 0x3f000000}, {0x3f99999a, 0x3e4ccccd}, {0x3f99999a, 0x00000000}, {0x3f8ccccd, 0xbe99999a},
	{0x3fc00000, 0x00000000}, {0x3f99999a, 0x3e99999a}, {0x3f99999a, 0x3ecccccd}, {0x3fa66666, 0x3f4ccccd},
	{0x3f99999a, 0x3f000000}, {0x3f8ccccd, 0xbdcccccd}, {0x3f8ccccd, 0x3e4ccccd}, {0x3f8ccccd, 0x3dcccccd},
	{0x3f800000, 0x3e99999a}, {0x3f4ccccd, 0xbecccccd},
};
// GLOBAL: LEGORACERS2 0x00546b94
static const LegoChar* g_secondaryChassisTags[41] = {
	"sandy police chassis",
	"Chassis beach bug",
	"Chassis CG4x4",
	"Chassis digger",
	"Chassis dumptruck",
	"Chassis firetruck",
	"Chassis postvan",
	"Sandy Bonus Car",
	"arctic utility chassis",
	"arctic skidoo chassis",
	"arctic buggy chassis",
	"arctic arm vehicle chassis",
	"arctic outpost chassis",
	"arctic small skidoo chassis",
	"arctic surfer",
	"arctic snow plough chassis",
	"adventures brown pickup chassis",
	"Chassis Bugatti",
	"Chassis Cattruck",
	"Chassis dino catcher",
	"chassis dino chasecar",
	"Chassis Jeep",
	"Chassis Tritruck",
	"Chassis Truck",
	"LOM buggy chassis",
	"LOM buggy2 chassis",
	"LOM buggy3 chassis",
	"LOM buggy4 chassis",
	"lom_hover chassis",
	"lom_hover2 chassis",
	"lom_hover3 chassis",
	"lom ATV chassis",
	"chill_chassis",
	"duster_chassis",
	"freeze_chassis",
	"ghost chassis",
	"lightor_chassis",
	"snake_chassis",
	"warrior_chassis",
	"Rocket Racer Chassis",
	NULL,
};

// STUB: LEGORACERS2 0x004011b0
static void __fastcall InitializeSecondaryBrickBuilder(GameObject* p_object, void*)
{
	SecondaryBrickBuilder* builder = (SecondaryBrickBuilder*) p_object;
	builder->m_pool = (SecondaryCarResourcePool*) ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x74;
	for (LegoS32 world = 0; world < 5; ++world) {
		if ((builder->m_worldMask & (1 << world)) != 0) {
			CharBuffer128 empty("");
			LoadSecondaryBuilderWorldMasks(
				builder->m_pool,
				NULL,
				world,
				&g_secondaryGenericBrickPath,
				&g_secondaryWorldBrickPaths[world],
				&empty
			);
		}
	}
	builder->m_unk0x68 = builder->m_nameCount = 0;
	builder->m_selectedName = builder->m_selectedPart = -1;
	builder->m_selection = builder->m_selectedPartRecord = NULL;
	builder->m_names = (GameObject**) GetMemoryHeap()->Allocate(0x90, "lego brick array in brickmanager");
	builder->m_unk0x1a4 = 0;
	for (LegoS32 index = 0; index < 36; ++index) {
		builder->m_names[builder->m_nameCount++] = CreateCarDisplayNameObject();
	}
	for (LegoS32 group = 0; group < 9; ++group) {
		for (LegoS32 item = 0; item < 4; ++item) {
			GameObject* object = CreateCarDisplayPartForModel(g_carDisplayModelPathStorage[group], 0x0e);
			builder->m_parts[group * 4 + item] = object;
			object->m_flags |= 0x400000;
		}
	}
	AllocateSecondaryBuilderGrid(builder, NULL);
	builder->GameObject::Initialize();
}

// STUB: LEGORACERS2 0x00401420
static void __fastcall ReleaseSecondaryBrickBuilder(GameObject* p_object, void*)
{
	SecondaryBrickBuilder* builder = (SecondaryBrickBuilder*) p_object;
	builder->GameObject::Remove();
	FreeSecondaryBuilderGrid(builder, NULL);
	for (LegoS32 part = 35; part >= 0; --part) {
		DestroyCarDisplayModelObject(builder->m_parts[part]);
	}
	for (LegoS32 name = 35; name >= 0; --name) {
		DestroyCarDisplayModelObject(builder->m_names[name]);
	}
	GetMemoryHeap()->Free(builder->m_names);
}

// STUB: LEGORACERS2 0x00402070
static void __fastcall AllocateSecondaryBuilderGrid(SecondaryBrickBuilder* p_builder, void*)
{
	p_builder->m_gridSize =
		p_builder->m_gridDimensions[2] * p_builder->m_gridDimensions[1] * p_builder->m_gridDimensions[0];
	p_builder->m_grid =
		(LegoU8*) GetMemoryHeap()->Allocate(p_builder->m_gridSize, "Voxel array for brick builder collision");
	ClearSecondaryBuilderGrid(p_builder, NULL);
}

// STUB: LEGORACERS2 0x004020c0
static void __fastcall ClearSecondaryBuilderGrid(SecondaryBrickBuilder* p_builder, void*)
{
	for (LegoS32 index = 0; index < p_builder->m_gridSize; ++index) {
		p_builder->m_grid[index] = 0;
	}
}

// STUB: LEGORACERS2 0x004020f0
static void __fastcall FreeSecondaryBuilderGrid(SecondaryBrickBuilder* p_builder, void*)
{
	GetMemoryHeap()->Free(p_builder->m_grid);
}

// STUB: LEGORACERS2 0x004654e0
static void InitializeSecondaryCarMenuItems()
{
	SecondaryCarModelItemDefinition car = {
		{2, 3, 0, 0},
		g_carSelectorHeaderPaths[0],
		{0, 1, 0xffffffff, 0xffffffff, 0x3fc00000, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
	};
	g_carSelectorItems.m_carModel = car;
	static const LegoU32 scales[5] = {0x3f800000, 0x40000000, 0x40000000, 0x3fc00000, 0x40000000};
	for (LegoU32 category = 0; category < 5; ++category) {
		SecondaryCarTextureItemDefinition icon = {
			{2, 4 + category, 1 + category},
			g_carSelectorHeaderPaths[1 + category],
			{0,
			 0,
			 0,
			 0xffffffff,
			 0xffffffff,
			 scales[category],
			 0,
			 0,
			 0xffffffff,
			 0xffffffff,
			 0xffffffff,
			 0xffffffff,
			 0xffffffff}
		};
		g_carSelectorItems.m_categoryIcons[category] = icon;
	}
	SecondaryCarTextureItemDefinition accept = {
		{2, 9, 999},
		"Game Data\\Frontend\\Ch Builder Bits\\tick.tga",
		{0, 0, 0, 0x57, 0xffffffff, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
	};
	SecondaryCarTextureItemDefinition save = {
		{10, 0xffffffff, 0},
		"Game Data\\Frontend\\Ch Builder Bits\\tick.tga",
		{0, 0, 0, 0x217, 0xffffffff, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
	};
	SecondaryCarTextureItemDefinition cancel = {
		{10, 0xffffffff, 1},
		"Game Data\\Frontend\\Ch Builder Bits\\cross2.tga",
		{0, 0, 0, 0x218, 0xffffffff, 0, 0, 0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff}
	};
	g_carSelectorItems.m_actions[0] = accept;
	g_carSelectorItems.m_actions[1] = save;
	g_carSelectorItems.m_actions[2] = cancel;
}

static void DefineSecondaryCarPanelSprites(
	LegoU32 p_first,
	const LegoChar* const* p_names,
	LegoFloat p_left,
	LegoFloat p_top,
	LegoFloat p_width,
	LegoFloat p_height,
	LegoFloat p_borderX,
	LegoFloat p_borderY
)
{
	static const LegoChar* const paths[9] = {
		"Game Data\\Frontend\\Ch Builder Bits\\ch top left.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch top mid.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch top right.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch left.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch middle.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch right.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch bottom left.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch bottom mid.tga",
		"Game Data\\Frontend\\Ch Builder Bits\\ch bottom right.tga"
	};
	// Retail keeps the unrounded sums in x87 while subtracting the borders.
	double right = (double) p_left + p_width;
	double bottom = (double) p_top + p_height;
	LegoFloat columns[4] = {p_left, p_left + p_borderX, (LegoFloat) (right - p_borderX), (LegoFloat) right};
	LegoFloat rows[4] = {p_top, p_top + p_borderY, (LegoFloat) (bottom - p_borderY), (LegoFloat) bottom};
	for (LegoU32 row = 0; row < 3; ++row) {
		for (LegoU32 column = 0; column < 3; ++column) {
			LegoU32 index = row * 3 + column;
			SecondarySelectionSpriteDefinition definition = {
				p_names[index],
				{columns[column], rows[row], columns[column + 1], rows[row + 1]},
				{0.0f, 0.0f},
				0xffffffff,
				paths[index],
				{0.0f, 1.0f, 1.0f, 0.0f},
				0,
				7,
				0,
				5
			};
			g_carSelectorSpriteDefinitions[p_first + index] = definition;
		}
	}
}

// STUB: LEGORACERS2 0x004658f0
static void InitializeSecondaryCarPanelSprites()
{
	static const LegoChar* const topDownNames[9] =
		{"TopDown0", "TopDown1", "TopDown2", "TopDown3", "TopDown4", "TopDown5", "TopDown6", "TopDown7", "TopDown8"};
	static const LegoChar* const threeDNames[9] =
		{"ThreeD0", "ThreeD1", "ThreeD2", "ThreeD3", "ThreeD4", "ThreeD5", "ThreeD6", "ThreeD7", "ThreeD8"};
	LegoFloat topDownBorder = 0.585f * 0.05f;
	DefineSecondaryCarPanelSprites(8, topDownNames, 0.015f, 0.315f, 0.585f, 0.5f, topDownBorder, topDownBorder);
	LegoFloat threeDBorderX = 0.365f * 0.07f;
	LegoFloat threeDBorderY = 0.455f * 0.07f;
	DefineSecondaryCarPanelSprites(17, threeDNames, 0.615f, 0.36f, 0.365f, 0.455f, threeDBorderX, threeDBorderY);
}

// STUB: LEGORACERS2 0x00467520
static void InitializeSecondaryBuilderGridDimensions()
{
	g_secondaryBuilderGridDimensions[0] = 10;
	g_secondaryBuilderGridDimensions[1] = 27;
	g_secondaryBuilderGridDimensions[2] = 17;
}

// STUB: LEGORACERS2 0x00467550
static void InitializeSecondaryBuilderCellDimensions()
{
	g_secondaryBuilderCellDimensions[0] = 5;
	g_secondaryBuilderCellDimensions[1] = 0;
	g_secondaryBuilderCellDimensions[2] = 8;
}

// STUB: LEGORACERS2 0x00467ba0
void __fastcall LoadSecondaryCarSelector(void* p_object, void*, BitmapFont* p_font, LegoS32 p_carSet)
{
	SecondaryCarSelector* selector = (SecondaryCarSelector*) p_object;
	selector->m_worldMask = p_carSet == -1 ? (LegoU32) -1 : 1 << (p_carSet & 31);
	selector->m_font = p_font;
	LoadSecondaryWheelModels(selector, NULL);
	struct BuilderTemplate {
		GameObjectTemplate m_base;
		LegoU32 m_worldMask;
		Vector3 m_offset;
		LegoU32 m_grid, m_cell;
	} data;
	data.m_base.m_flags = 2;
	data.m_base.m_first = CharBuffer24("NoName");
	data.m_base.m_second = CharBuffer24("");
	data.m_worldMask = selector->m_worldMask;
	data.m_offset = g_characterBuilderPositionOffset;
	memcpy(&data.m_grid, g_secondaryBuilderGridDimensions, 4);
	memcpy(&data.m_cell, g_secondaryBuilderCellDimensions, 4);
	void* storage = AllocateHeapMemory(sizeof(SecondaryBrickBuilder));
	SecondaryBrickBuilder* builder = NULL;
	if (storage != NULL) {
		builder = (SecondaryBrickBuilder*) new (storage) GameObject(0x900, &data.m_base, NULL);
		builder->m_unk0x58 = 0.4f;
		builder->m_unk0x5c = 0.16f;
		builder->m_worldMask = data.m_worldMask;
		for (LegoS32 cell = 0; cell < 36; ++cell) {
			builder->m_cellStates[cell][0] = builder->m_cellStates[cell][1] = builder->m_cellStates[cell][2] = 0;
		}
		for (LegoS32 item = 0; item < 36; ++item) {
			builder->m_car.m_items[item].Initialize();
		}
		builder->m_selectionCount = 0;
		builder->m_offset = data.m_offset;
		builder->m_unk0x540 = data.m_cell;
		memcpy(builder->m_gridDimensions, &data.m_grid, 4);
		builder->m_vtable = (const GameObjectDispatchView*) &g_secondaryBrickBuilderDispatch;
	}
	selector->m_builder = builder;
	builder->m_vtable->m_initialize(builder, NULL);
	selector->m_display = (CarDisplay*) AllocateHeapMemory(sizeof(CarDisplay));
	if (selector->m_display != NULL) {
		selector->m_display->m_activeCount = 0;
		selector->m_display->m_resourceTable = builder->m_pool;
		selector->m_display->m_selectedChoice = 0;
	}
	InitializeCarDisplay(selector->m_display);
	InitializeSecondarySpriteSet(selector->m_spriteSet, NULL, g_carSelectorSpriteDefinitions, 26);
	BuildSecondaryCarSelectorGrid(selector, NULL);
	selector->m_texture =
		g_displayRenderer
			->CreateTextureResource("Game Data\\Frontend\\Ch Builder Bits\\blue_peg_tile.tga", 0, -1, 1, 1);
}

// STUB: LEGORACERS2 0x00467da0
static LegoU32 __fastcall GetSecondaryBrickBuilderSize(GameObject*, void*)
{
	return 0x550;
}

// STUB: LEGORACERS2 0x00467db0
static GameObject* __fastcall DestroySecondaryBrickBuilder(GameObject* p_object, void*, LegoU32 p_flags)
{
	p_object->m_vtable = (const GameObjectDispatchView*) &g_secondaryBrickBuilderDispatch;
	p_object->GameObject::~GameObject();
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x00467de0
static void __fastcall LoadSecondaryWheelModels(SecondaryCarSelector* p_selector, void*)
{
	p_selector->m_wheelModels =
		(ModelResource**) GetMemoryHeap()->Allocate(14 * 4, "Stored wheel models for car builder");
	for (LegoS32 index = 0; index < 14; ++index) {
		p_selector->m_wheelModels[index] = g_carDisplayModelPathStorage[index][0] == 0
											   ? NULL
											   : LoadModelResource(g_carDisplayModelPathStorage[index], 0, 0, 0);
	}
}

// STUB: LEGORACERS2 0x00467e50
static void __fastcall ReleaseSecondaryWheelModels(SecondaryCarSelector* p_selector, void*)
{
	for (LegoS32 index = 13; index >= 0; --index) {
		ModelResource*& model = p_selector->m_wheelModels[index];
		if (model != NULL && g_modelResourceCache.Release(model)) {
			model = NULL;
		}
	}
	GetMemoryHeap()->Free(p_selector->m_wheelModels);
}

// STUB: LEGORACERS2 0x00468ee0
void __fastcall ReleaseSecondaryCarSelector(void* p_object, void*)
{
	SecondaryCarSelector* selector = (SecondaryCarSelector*) p_object;
	g_displayRenderer->ReleaseTextureResourceReference(selector->m_texture, 1);
	ReleaseSecondarySelectionGrid(selector->m_grid, NULL);
	ReleaseSecondarySpriteSet(selector->m_spriteSet, NULL);
	ReleaseCarDisplay(selector->m_display);
	FreeHeapMemory(selector->m_display);
	selector->m_builder->m_vtable->m_remove(selector->m_builder, NULL);
	if (selector->m_builder != NULL) {
		selector->m_builder->m_vtable->m_destroy(selector->m_builder, NULL, 1);
	}
	ReleaseSecondaryWheelModels(selector, NULL);
}

// STUB: LEGORACERS2 0x00468f40
void __fastcall BuildSecondaryCarSelectorGrid(SecondaryCarSelector* p_selector, void*)
{
	void* grid = p_selector->m_grid;
	InitializeSecondarySelectionGrid(grid, NULL);
	for (LegoS32 categoryIndex = 0; categoryIndex < 12; ++categoryIndex) {
		DefineSecondarySelectionCategory(grid, NULL, &g_carSelectorCategories[categoryIndex]);
	}
	const SecondarySelectionItemDefinition* initialItems =
		(const SecondarySelectionItemDefinition*) &g_carSelectorItems;
	for (LegoS32 initial = 0; initial < 13; ++initial) {
		AddSecondarySelectionItem(grid, NULL, &initialItems[initial], 1);
	}
	LegoS32 selectedWorld = -1;
	SecondarySelectionItemDefinition definition;
	for (LegoS32 world = 0; world < 5; ++world) {
		LegoU32 mask = 1 << world;
		if ((mask & p_selector->m_worldMask) == 0) {
			continue;
		}
		if (p_selector->m_worldMask != (LegoU32) -1) {
			mask = 1;
			selectedWorld = world;
		}
		LegoS32 start = g_secondaryWorldCarStarts[world], end = g_secondaryWorldCarCounts[world] - 1 + start;
		SecondaryManager* manager = (SecondaryManager*) g_gameController.m_secondaryManager;
		for (LegoS32 car = start; car <= end; ++car) {
			DefineSecondaryCarSelectionItem(car - start, car, manager->m_secondaryData[car], &definition);
			AddSecondarySelectionItem(grid, NULL, &definition, mask);
		}
		LegoS32 index = end - start;
		for (LegoS32 custom = 0; custom < 8; ++custom) {
			void* config = (LegoU8*) g_gameControllerSettings + world * 0x17a0 + custom * 0x2f4 + 0x94c;
			DefineSecondaryCarSelectionItem(++index, custom + 0x25, config, &definition);
			AddSecondarySelectionItem(grid, NULL, &definition, mask);
			DefineSecondarySavedCarSelectionItem(custom, config, &definition);
			AddSecondarySelectionItem(grid, NULL, &definition, mask);
		}
	}
	SecondaryCarResourcePool* pool = p_selector->m_builder->m_pool;
	LegoS32 brickCount = pool->m_entryCount;
	CharBuffer128 path;
	for (LegoS32 category = 0; category < 6; ++category) {
		LegoS32 count[5] = {0, 0, 0, 0, 0}, rows[5] = {1, 1, 1, 1, 1};
		LegoS32 pages[5] = {0, 0, 0, 0, 0}, perRow[5], remainder[5], remaining[5], distribution[5];
		for (LegoS32 countBrick = 0; countBrick < brickCount; ++countBrick) {
			LegoU32 mask = GetSecondaryBrickWorldMask(pool, NULL, countBrick);
			if ((mask & p_selector->m_worldMask) != 0) {
				LegoS32 brickCategory;
				memcpy(path.m_data, GetSecondaryBrickPath(pool, NULL, countBrick, &brickCategory), sizeof(path.m_data));
				if (brickCategory == category) {
					for (LegoS32 world = 0; world < 5; ++world) {
						if ((mask & p_selector->m_worldMask & (1 << world)) != 0) {
							++count[world];
						}
					}
				}
			}
		}
		for (LegoS32 world = 0; world < 5; ++world) {
			if (count[world] > 0) {
				rows[world] = (count[world] - 1) / 6 + 1;
			}
			perRow[world] = count[world] / rows[world];
			remainder[world] = count[world] - perRow[world] * rows[world];
			remaining[world] = perRow[world];
			distribution[world] = remainder[world];
			if (rows[world] <= remainder[world]) {
				remaining[world] = perRow[world] + 1;
				distribution[world] = remainder[world] - rows[world];
			}
		}
		LegoS32 unusedPageNumbers[5];
		for (LegoS32 brick = 0; brick < brickCount; ++brick) {
			LegoU32 mask = GetSecondaryBrickWorldMask(pool, NULL, brick) & p_selector->m_worldMask;
			if (mask == 0) {
				continue;
			}
			LegoS32 brickCategory;
			memcpy(path.m_data, GetSecondaryBrickPath(pool, NULL, brick, &brickCategory), sizeof(path.m_data));
			if (brickCategory != category) {
				continue;
			}
			DefineSecondaryBrickSelectionItem(pool, &path, brickCategory, brick, &definition);
			AddSecondarySelectionItem(grid, NULL, &definition, p_selector->m_worldMask == (LegoU32) -1 ? mask : 1);
			LegoU32 nextRowMask = 0;
			for (LegoS32 world = 0; world < 5; ++world) {
				if ((mask & (1 << world)) != 0 && --remaining[world] == 0) {
					nextRowMask |= p_selector->m_worldMask == (LegoU32) -1 ? 1 << world : 1;
					distribution[world] += remainder[world];
					remaining[world] = perRow[world];
					if (rows[world] <= distribution[world]) {
						remaining[world] = perRow[world] + 1;
						distribution[world] -= rows[world];
					}
				}
			}
			if (nextRowMask != 0 && category != 0) {
				DefineSecondaryGridRowMarker(brickCategory + 3, &definition);
				AddSecondarySelectionItem(grid, NULL, &definition, nextRowMask);
				for (LegoS32 world = 0; world < 5; ++world) {
					if ((nextRowMask & (1 << world)) != 0) {
						LegoS32 index = selectedWorld == -1 ? world : selectedWorld;
						LegoS32 page = ++pages[index];
						if ((page & 1) == 0 || page == rows[index]) {
							unusedPageNumbers[world] = page == rows[index] ? 1 : (page >> 1) + 1;
						}
					}
				}
			}
		}
	}
	SetSecondarySelectionGridFont(grid, NULL, p_selector->m_font);
	FinalizeSecondarySelectionGrid(grid, NULL);
}

// STUB: LEGORACERS2 0x00469450
static void __cdecl DefineSecondaryGridRowMarker(LegoU32 p_category, SecondarySelectionItemDefinition* p_result)
{
	LegoU32* values = p_result->m_values;
	values[0] = p_category;
	values[1] = 9;
	values[2] = 999;
	values[3] = (LegoU32) "Game Data\\Frontend\\Ch Builder Bits\\tick.tga";
	values[4] = values[5] = values[6] = 0;
	values[7] = 0x57;
	values[8] = -1;
	values[9] = values[10] = values[11] = 0;
	for (LegoS32 index = 12; index < 17; ++index) {
		values[index] = -1;
	}
}

// STUB: LEGORACERS2 0x004694b0
static void __cdecl DefineSecondaryCarSelectionItem(
	LegoU32 p_index,
	LegoS32 p_car,
	void* p_configuration,
	SecondarySelectionItemDefinition* p_result
)
{
	if (p_car > 37) {
		p_car = 37;
	}
	LegoU32* values = p_result->m_values;
	values[0] = 1;
	values[1] = 9;
	values[2] = p_index;
	values[3] = values[4] = 0;
	values[5] = (LegoU32) p_configuration;
	values[6] = 2;
	values[7] = values[8] = -1;
	values[9] = g_secondaryCarItemScales[p_car][0];
	values[10] = 0xe1;
	values[11] = g_secondaryCarItemScales[p_car][1];
	for (LegoS32 index = 12; index < 17; ++index) {
		values[index] = -1;
	}
}

// STUB: LEGORACERS2 0x00469530
static void __cdecl DefineSecondarySavedCarSelectionItem(
	LegoU32 p_index,
	void* p_configuration,
	SecondarySelectionItemDefinition* p_result
)
{
	LegoU32* values = p_result->m_values;
	values[0] = 11;
	values[1] = -1;
	values[2] = p_index;
	values[3] = values[4] = 0;
	values[5] = (LegoU32) p_configuration;
	values[6] = 2;
	values[7] = values[8] = -1;
	values[9] = g_secondaryCarItemScales[37][0];
	values[10] = 0xe1;
	values[11] = g_secondaryCarItemScales[37][1];
	for (LegoS32 index = 12; index < 17; ++index) {
		values[index] = -1;
	}
}

// STUB: LEGORACERS2 0x00469590
static void __cdecl DefineSecondaryBrickSelectionItem(
	SecondaryCarResourcePool* p_pool,
	const CharBuffer128* p_path,
	LegoS32 p_category,
	LegoS32 p_brick,
	SecondarySelectionItemDefinition* p_result
)
{
	LegoU32* values = p_result->m_values;
	values[0] = p_category + 3;
	values[1] = 9;
	values[2] = p_brick;
	values[3] = 0;
	values[4] = (LegoU32) p_path;
	values[5] = 0;
	values[6] = 1;
	values[7] = -1;
	LegoFloat scale = GetSecondaryBrickScale(p_pool, NULL, p_brick);
	memcpy(values + 9, &scale, 4);
	values[10] = GetSecondaryBrickOrientation(p_pool, NULL, p_brick);
	LegoFloat offset = GetSecondaryBrickOffset(p_pool, NULL, p_brick);
	memcpy(values + 11, &offset, 4);
	values[8] = p_category == 0 ? FindSecondaryChassisTag(p_path) : -1;
	for (LegoS32 index = 12; index < 17; ++index) {
		values[index] = -1;
	}
}

// STUB: LEGORACERS2 0x0046a200
static LegoS32 __stdcall FindSecondaryChassisTag(const CharBuffer128* p_path)
{
	for (LegoS32 index = 0; index < 41; ++index) {
		if (ContainsSeatPositionTag(g_secondaryChassisTags[index], p_path->m_data)) {
			return index;
		}
	}
	return -1;
}
