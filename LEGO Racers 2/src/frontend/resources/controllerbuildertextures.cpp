#include "frontend/resources/controllereventrecords.h"
#include "render/core/renderer.h"

struct BuilderAtlasRegion {
	LegoFloat m_u, m_v, m_uEnd, m_vEnd;
};
static const BuilderAtlasRegion g_builderAtlasRegions[9] = {
	{0.00382812507f, 0.996171892f, 0.488359362f, 0.511640608f},
	{0.503828108f, 0.996171892f, 0.988359392f, 0.511640608f},
	{0.503828108f, 0.496171862f, 0.988359392f, 0.0116406251f},
	{0.00382812507f, 0.496171862f, 0.488359362f, 0.0116406251f},
	{0.488203138f, 0.996171892f, 0.503984392f, 0.511640608f},
	{0.503828108f, 0.511796892f, 0.988359392f, 0.496015638f},
	{0.488203138f, 0.496171862f, 0.503984392f, 0.0116406251f},
	{0.00382812507f, 0.511796892f, 0.488359362f, 0.496015638f},
	{0.488203138f, 0.511796892f, 0.503984392f, 0.496015638f},
};
static const LegoChar* g_builderPrimaryTexturePaths[27] = {
	"Game Data\\Frontend\\Ch Builder Bits\\ch top left.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch top right.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch bottom right.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch bottom left.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch top mid.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch right.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch bottom mid.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch left.tga",
	"Game Data\\Frontend\\Ch Builder Bits\\ch middle.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Top left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Top right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Bottom right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Bottom left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Top.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Bottom.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Blue Middle.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Top left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Top right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Bottom right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Bottom left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Top.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Bottom.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Tabs\\Red Middle.tga",
};
static const LegoChar* g_builderSecondaryTexturePaths[9] = {
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Top left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Top right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Bottom right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Bottom left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Top.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Right.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Bottom.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Left.tga",
	"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Pause\\Middle.tga",
};

static void AssignBuilderAtlas(ControllerEventRecordTable* p_resources, LegoU32 p_first, LegoS32 p_texture)
{
	for (LegoU32 i = 0; i < 9; ++i) {
		ControllerEventRecord* record = &p_resources->m_records[p_first + i];
		record->m_index = p_texture;
		record->m_u = g_builderAtlasRegions[i].m_u;
		record->m_v = g_builderAtlasRegions[i].m_v;
		record->m_uEnd = g_builderAtlasRegions[i].m_uEnd;
		record->m_vEnd = g_builderAtlasRegions[i].m_vEnd;
	}
}

// STUB: LEGORACERS2 0x004360d0
void __fastcall LoadControllerBuilderTextures(void* p_storage, void*, LegoU32 p_mode)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 0x20) != 0) {
		return;
	}
	resources->m_flags |= 0x20;
	LegoS32 texture = g_displayRenderer->CreateTextureResource(
		"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Standard\\Menu Box Blue.tga",
		0,
		-1,
		1,
		1
	);
	AssignBuilderAtlas(resources, 179, texture);
	if (p_mode == 1) {
		texture = g_displayRenderer->CreateTextureResource(
			"Game Data\\Frontend\\Frontend bits\\Menu Boxes\\Standard\\Menu Box Red.tga",
			0,
			-1,
			1,
			1
		);
		AssignBuilderAtlas(resources, 188, texture);
		for (LegoU32 i = 0; i < 27; ++i) {
			resources->m_records[206 + i].m_index =
				g_displayRenderer->CreateTextureResource(g_builderPrimaryTexturePaths[i], 0, -1, 1, 1);
			resources->SetFullTextureRegion(206 + i);
		}
	}
	else if (p_mode == 2) {
		for (LegoU32 j = 0; j < 9; ++j) {
			resources->m_records[197 + j].m_index =
				g_displayRenderer->CreateTextureResource(g_builderSecondaryTexturePaths[j], 0, -1, 1, 1);
			resources->SetFullTextureRegion(197 + j);
		}
	}
}

// STUB: LEGORACERS2 0x00436af0
void __fastcall ReleaseControllerBuilderTextures(void* p_storage, void*, LegoU32 p_mode)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 0x20) == 0) {
		return;
	}
	resources->m_flags &= ~0x20u;
	if (p_mode == 1) {
		for (LegoS32 i = 232; i >= 206; --i) {
			g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[i].m_index, 1);
		}
		g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[188].m_index, 1);
	}
	else if (p_mode == 2) {
		for (LegoS32 j = 205; j >= 197; --j) {
			g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[j].m_index, 1);
		}
	}
	g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[179].m_index, 1);
}
