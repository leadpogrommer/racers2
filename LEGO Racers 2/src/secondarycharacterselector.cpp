#include "secondarycharacterselector.h"

#include "gamecontroller.h"
#include "globaloverlaymanager.h"
#include "npctextmanager.h"
#include "renderer.h"
#include "secondarymanagerleases.h"
#include "secondaryselectionwidgets.h"

// GLOBAL: LEGORACERS2 0x00543f50
static const SecondarySelectionSpriteDefinition g_characterSelectorSpriteDefinitions[25] = {
	{"b0",
	 {0.0f, 0.25f, 1.0f, 1.0f},
	 {0.100000001f, 0.125f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\red_peg_tile.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000002,
	 4},
	{"b1",
	 {0.0f, 0.0f, 1.0f, 0.129999995f},
	 {0.100000001f, 0.129999995f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\red_peg_tile.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000002,
	 4},
	{"racers_logo1",
	 {0.0f, 0.0f, 0.100000001f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\LR2 Logo\\Ch Builder logo 01.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 6},
	{"racers_logo2",
	 {0.100000001f, 0.0f, 0.200000003f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\LR2 Logo\\Ch Builder logo 02.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 6},
	{"racers_logo3",
	 {0.200000003f, 0.0f, 0.300000012f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\LR2 Logo\\Ch Builder logo 03.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 6},
	{"racers_logo4",
	 {0.300000012f, 0.0f, 0.400000006f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\LR2 Logo\\Ch Builder logo 04.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 6},
	{"racers_logo5",
	 {0.400000006f, 0.0f, 0.5f, 0.129999995f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\LR2 Logo\\Ch Builder logo 05.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000000,
	 0x00000000,
	 6},
	{"fig0",
	 {0.0500000007f, 0.330000013f, 0.0675000027f, 0.362500012f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig1",
	 {0.0675000027f, 0.330000013f, 0.382499993f, 0.362500012f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top mid.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig2",
	 {0.382499993f, 0.330000013f, 0.400000006f, 0.362500012f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig3",
	 {0.0500000007f, 0.362500012f, 0.0675000027f, 0.94749999f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig4",
	 {0.0675000027f, 0.362500012f, 0.382499993f, 0.94749999f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch middle.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig5",
	 {0.382499993f, 0.362500012f, 0.400000006f, 0.94749999f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig6",
	 {0.0500000007f, 0.94749999f, 0.0675000027f, 0.980000019f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig7",
	 {0.0675000027f, 0.94749999f, 0.382499993f, 0.980000019f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom mid.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"fig8",
	 {0.382499993f, 0.94749999f, 0.400000006f, 0.980000019f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part0",
	 {0.579999983f, 0.330000013f, 0.597499967f, 0.360800028f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part1",
	 {0.597499967f, 0.330000013f, 0.8125f, 0.360800028f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top mid.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part2",
	 {0.8125f, 0.330000013f, 0.829999983f, 0.360800028f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch top right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part3",
	 {0.579999983f, 0.360800028f, 0.597499967f, 0.739199996f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part4",
	 {0.597499967f, 0.360800028f, 0.8125f, 0.739199996f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch middle.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part5",
	 {0.8125f, 0.360800028f, 0.829999983f, 0.739199996f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part6",
	 {0.579999983f, 0.739199996f, 0.597499967f, 0.769999981f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom left.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part7",
	 {0.597499967f, 0.739199996f, 0.8125f, 0.769999981f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom mid.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
	{"part8",
	 {0.8125f, 0.739199996f, 0.829999983f, 0.769999981f},
	 {0.0f, 0.0f},
	 0xffffffff,
	 "Game Data\\Frontend\\Ch Builder Bits\\ch bottom right.tga",
	 {0.0f, 1.0f, 1.0f, 0.0f},
	 0x00000000,
	 0x00000007,
	 0x00000000,
	 5},
};
// GLOBAL: LEGORACERS2 0x005445f4
static LegoS32 g_characterSelectorSpriteCount = 25;
// GLOBAL: LEGORACERS2 0x00544b98
static const SecondarySelectionCategoryDefinition g_characterSelectorCategories[5] = {
	{{0x00000000, 0xffffffff, 0x00000053, 0x00000001}},
	{{0x00000001, 0x00000000, 0x00000054, 0x00000001}},
	{{0x00000002, 0x00000001, 0x00000055, 0x00000001}},
	{{0x00000003, 0x00000002, 0x00000056, 0x00000001}},
	{{0x00000004, 0x00000003, 0x00000057, 0x00000000}},
};
// GLOBAL: LEGORACERS2 0x00544be8
static LegoS32 g_characterSelectorCategoryCount = 5;
struct CharacterSelectionItemDefinition {
	LegoU32 m_prefix[3];
	const LegoChar* m_texturePath;
	LegoU32 m_suffix[13];
};
DECOMP_SIZE_ASSERT(CharacterSelectionItemDefinition, 0x44);
DECOMP_STATIC_ASSERT(offsetof(CharacterSelectionItemDefinition, m_texturePath) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(CharacterSelectionItemDefinition, m_suffix) == 0x10);

// GLOBAL: LEGORACERS2 0x00544bf0
static const CharacterSelectionItemDefinition g_characterSelectorItems[34] = {
	{{0x00000000, 0x00000001, 0x00000000},
	 "Game Data\\Frontend\\Body Parts\\head_goofy.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000001},
	 "Game Data\\Frontend\\Body Parts\\head_sanister.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000002},
	 "Game Data\\Frontend\\Body Parts\\head_rock_raider.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000003},
	 "Game Data\\Frontend\\Body Parts\\head_man.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000004},
	 "Game Data\\Frontend\\Body Parts\\head_jack.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000005},
	 "Game Data\\Frontend\\Body Parts\\head_girl2.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000006},
	 "Game Data\\Frontend\\Body Parts\\head_girl1.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000000, 0x00000001, 0x00000007},
	 "Game Data\\Frontend\\Body Parts\\head_ice_queen.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000000},
	 "Game Data\\Frontend\\Body Parts\\hat_workman.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000001},
	 "Game Data\\Frontend\\Body Parts\\hat_bush.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000002},
	 "Game Data\\Frontend\\Body Parts\\hat_cap.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000003},
	 "Game Data\\Frontend\\Body Parts\\hat_fireman.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000004},
	 "Game Data\\Frontend\\Body Parts\\hat_helmet1.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000005},
	 "Game Data\\Frontend\\Body Parts\\hat_helmet2.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000006},
	 "Game Data\\Frontend\\Body Parts\\hat_stunt.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000001, 0x00000002, 0x00000007},
	 "Game Data\\Frontend\\Body Parts\\hat_arctic.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000000},
	 "Game Data\\Frontend\\Body Parts\\torso_captain_ross.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000001},
	 "Game Data\\Frontend\\Body Parts\\torso_girl.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000002},
	 "Game Data\\Frontend\\Body Parts\\torso_goofy.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000003},
	 "Game Data\\Frontend\\Body Parts\\torso_pilot.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000004},
	 "Game Data\\Frontend\\Body Parts\\torso_rock_raider.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000005},
	 "Game Data\\Frontend\\Body Parts\\torso_sheriff.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000006},
	 "Game Data\\Frontend\\Body Parts\\torso_space_ranger.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000002, 0x00000003, 0x00000007},
	 "Game Data\\Frontend\\Body Parts\\torso_workman.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000000},
	 "Game Data\\Frontend\\Body Parts\\legs_yellow_black.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000001},
	 "Game Data\\Frontend\\Body Parts\\legs_red.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000002},
	 "Game Data\\Frontend\\Body Parts\\legs_orange.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000003},
	 "Game Data\\Frontend\\Body Parts\\legs_green.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000004},
	 "Game Data\\Frontend\\Body Parts\\legs_grey_grey.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000005},
	 "Game Data\\Frontend\\Body Parts\\legs_brown.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000006},
	 "Game Data\\Frontend\\Body Parts\\legs_blue_black.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000003, 0x00000004, 0x00000007},
	 "Game Data\\Frontend\\Body Parts\\legs_black_grey.tga",
	 {0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0x00000000,
	  0x00000000,
	  0x00000000,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff,
	  0xffffffff}},
	{{0x00000004, 0xffffffff, 0x00000000},
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
	{{0x00000004, 0x00000000, 0x00000001},
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
};
// GLOBAL: LEGORACERS2 0x005454f8
static LegoS32 g_characterSelectorItemCount = 34;

static void __fastcall InitializeSecondaryCharacterSelectorGrid(SecondaryCharacterSelector*, void*);
static void __fastcall ReleaseSecondaryCharacterPreview(SecondaryCharacterSelector*, void*);
static void __fastcall ReleaseSecondaryCharacterLighting(SecondaryCharacterSelector*, void*);
static void __fastcall ReleaseSecondaryCharacterCamera(SecondaryCharacterSelector*, void*);

// STUB: LEGORACERS2 0x00463f40
void __fastcall LoadSecondaryCharacterSelector(void* p_object, void*, BitmapFont* p_font)
{
	SecondaryCharacterSelector* selector = (SecondaryCharacterSelector*) p_object;
	selector->m_font = p_font;
	InitializeSecondarySpriteSet(
		selector->m_spriteSet,
		NULL,
		g_characterSelectorSpriteDefinitions,
		g_characterSelectorSpriteCount
	);
	InitializeSecondaryCharacterSelectorGrid(selector, NULL);

	// Retail leaves the initial text position fields untouched. These records
	// are cleared below and positioned when their text is selected for display.
	LegoU32 textParameters[13];
	textParameters[0] = (LegoU32) selector->m_font;
	textParameters[1] = 0xffffffff;
	textParameters[4] = 0;
	textParameters[5] = 0;
	textParameters[6] = 0x3f800000;
	textParameters[7] = 0x3f800000;
	textParameters[8] = 0;
	textParameters[9] = 0;
	textParameters[10] = 0x3f36db6e;
	textParameters[11] = 0;
	textParameters[12] = 0;
	selector->m_overlayFont = selector->m_font;
	selector->m_textRecords[0] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	selector->m_textRecords[0]->SetList(6);
	selector->m_textRecords[1] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	selector->m_textRecords[1]->SetList(5);
	LegoU32 shadowColor = 0x80000000;
	selector->m_textRecords[1]->SetColor(&shadowColor);
	selector->m_textRecords[2] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	selector->m_textRecords[2]->SetList(6);
	selector->m_textRecords[3] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	selector->m_textRecords[3]->SetList(5);
	selector->m_textRecords[3]->SetColor(&shadowColor);
	textParameters[10] = 0x3f2f8af9;
	selector->m_textRecords[4] = g_globalOverlayManager.AllocateRecord(textParameters, -1);
	LegoFloat hintPosition[2] = {0.225f, 0.9f};
	selector->m_textRecords[4]->SetPosition(hintPosition, 0xffffffff);
	selector->m_textRecords[4]->SetList(6);
	selector->m_textRecords[4]->SetScale(0.685714304f);

	// The no-op array constructors in the retail path leave tiling and texture
	// coordinates untouched; the sprite's initial texture is the -1 sentinel.
	LegoU32 spriteParameters[21];
	spriteParameters[0] = 0;
	spriteParameters[1] = 0;
	spriteParameters[2] = 0x3e800000;
	spriteParameters[3] = 0x3eaaaaab;
	spriteParameters[4] = 0;
	spriteParameters[5] = 0;
	spriteParameters[6] = 0x3f800000;
	spriteParameters[7] = 0x3f800000;
	spriteParameters[10] = 0xffffffff;
	spriteParameters[11] = 0xffffffff;
	spriteParameters[16] = 0;
	spriteParameters[17] = 0;
	spriteParameters[18] = 0;
	spriteParameters[19] = 0;
	spriteParameters[20] = 0;
	selector->m_sprites[0] = g_globalOverlayManager.AllocateSprite(spriteParameters);
	selector->m_sprites[1] = g_globalOverlayManager.AllocateSprite(spriteParameters);
	selector->m_sprites[0]->SetList(6);
	selector->m_sprites[1]->SetList(6);
	selector->m_backgroundTexture =
		g_displayRenderer->CreateTextureResource("Game Data\\Frontend\\Ch Builder Bits\\red_peg_tile.tga", 0, -1, 1, 1);
	FinishSecondaryCharacterSelection(selector, NULL);
}

// STUB: LEGORACERS2 0x00464700
void __fastcall FinishSecondaryCharacterSelection(void* p_object, void*)
{
	SecondaryCharacterSelector* p_selector = (SecondaryCharacterSelector*) p_object;
	p_selector->m_state = -1;
	if (((NpcTextManager*) g_gameController.m_npcTextManager)->m_selectedIndex == 0) {
		ClearSecondaryNpcTextMode(g_gameController.m_npcTextManager, NULL, 0);
	}
	HideSecondarySpriteSet(p_selector->m_spriteSet, NULL);
	HideSecondarySelectionGrid(p_selector->m_selectionGrid, NULL);
	ReleaseSecondaryCharacterPreview(p_selector, NULL);
	ReleaseSecondaryCharacterLighting(p_selector, NULL);
	ReleaseSecondaryCharacterCamera(p_selector, NULL);
	p_selector->m_sprites[0]->SetVisible(0);
	p_selector->m_sprites[1]->SetVisible(0);
	for (LegoS32 i = 0; i < 5; ++i) {
		p_selector->m_textRecords[i]->InitializeTextByIndex(-1, 0, 0.0f);
	}
	p_selector->m_state = -1;
}

// STUB: LEGORACERS2 0x004647c0
void __fastcall ReleaseSecondaryCharacterSelector(void* p_object, void*)
{
	SecondaryCharacterSelector* selector = (SecondaryCharacterSelector*) p_object;
	for (LegoS32 i = 4; i >= 0; --i) {
		selector->m_textRecords[i]->Release();
	}
	ReleaseSecondarySpriteSet(selector->m_spriteSet, NULL);
	ReleaseSecondarySelectionGrid(selector->m_selectionGrid, NULL);
	selector->m_sprites[0]->Release();
	selector->m_sprites[1]->Release();
	g_displayRenderer->ReleaseTextureResourceReference(selector->m_backgroundTexture, 1);
}

// STUB: LEGORACERS2 0x00464840
static void __fastcall InitializeSecondaryCharacterSelectorGrid(SecondaryCharacterSelector* p_selector, void*)
{
	InitializeSecondarySelectionGrid(p_selector->m_selectionGrid, NULL);
	for (LegoS32 i = 0; i < g_characterSelectorCategoryCount; ++i) {
		DefineSecondarySelectionCategory(p_selector->m_selectionGrid, NULL, &g_characterSelectorCategories[i]);
	}
	for (LegoS32 j = 0; j < g_characterSelectorItemCount; ++j) {
		AddSecondarySelectionItem(
			p_selector->m_selectionGrid,
			NULL,
			(const SecondarySelectionItemDefinition*) &g_characterSelectorItems[j],
			1
		);
	}
	SetSecondarySelectionGridFont(p_selector->m_selectionGrid, NULL, p_selector->m_font);
	FinalizeSecondarySelectionGrid(p_selector->m_selectionGrid, NULL);
}

// STUB: LEGORACERS2 0x00464a60
static void __fastcall ReleaseSecondaryCharacterPreview(SecondaryCharacterSelector* p_selector, void*)
{
	if (p_selector->m_characterLease != -1) {
		ReleaseMenuPreviewCharacter(
			(SecondaryManager*) g_gameController.m_secondaryManager,
			NULL,
			&p_selector->m_previewCharacter,
			p_selector->m_characterLease
		);
		p_selector->m_previewCharacter = NULL;
		p_selector->m_characterLease = -1;
	}
}

// STUB: LEGORACERS2 0x00464bb0
static void __fastcall ReleaseSecondaryCharacterLighting(SecondaryCharacterSelector* p_selector, void*)
{
	for (LegoS32 i = 1; i >= 0; --i) {
		if (p_selector->m_lightLeases[i] != -1) {
			ReleaseMenuPreviewLight(
				(SecondaryManager*) g_gameController.m_secondaryManager,
				NULL,
				&p_selector->m_previewLights[i],
				p_selector->m_lightLeases[i]
			);
			p_selector->m_lightLeases[i] = -1;
			p_selector->m_previewLights[i] = NULL;
		}
	}
	if (p_selector->m_ambientLease != -1) {
		ReleaseMenuPreviewAmbient(
			(SecondaryManager*) g_gameController.m_secondaryManager,
			NULL,
			&p_selector->m_previewAmbient,
			p_selector->m_ambientLease
		);
		p_selector->m_ambientLease = -1;
		p_selector->m_previewAmbient = NULL;
	}
}

// STUB: LEGORACERS2 0x00464d30
static void __fastcall ReleaseSecondaryCharacterCamera(SecondaryCharacterSelector* p_selector, void*)
{
	if (p_selector->m_cameraLease != -1) {
		ReleaseMenuPreviewCamera(
			(SecondaryManager*) g_gameController.m_secondaryManager,
			NULL,
			&p_selector->m_previewCamera,
			p_selector->m_cameraLease
		);
		p_selector->m_previewCamera = NULL;
		p_selector->m_cameraLease = -1;
	}
}
