#include "core/strings/charbuffer128.h"
#include "frontend/resources/controllereventrecords.h"
#include "game/controller/gamecontrollerconfig.h"
#include "render/core/renderer.h"

#include <stdio.h>

struct PauseMenuTextureDefinition {
	const LegoChar* m_path;
	LegoFloat m_u, m_v, m_uEnd, m_vEnd;
};
static const PauseMenuTextureDefinition g_pauseMenuTextureDefinitions[14] = {
	{"Game Data\\HUD\\Pause menu system\\pa dial.tga", 0.0f, 1.0f, 1.0f, 0.0f},         // record164
	{"Game Data\\HUD\\Pause menu system\\pa dial pointer.tga", 0.0f, 1.0f, 1.0f, 0.0f}, // record165
	{"Game Data\\HUD\\Pause menu system\\pa dial low.tga", 0.0f, 1.0f, 1.0f, 0.0f},     // record166
	{"Game Data\\HUD\\Pause menu system\\pa dial high.tga", 0.0f, 1.0f, 1.0f, 0.0f},    // record167
	{"Game Data\\HUD\\Pause menu system\\pa adjust arrow left.tga",
	 0.257656246f,
	 0.789218724f,
	 0.695468724f,
	 0.226406246f}, // record168
	{"Game Data\\HUD\\Pause menu system\\pa adjust arrow right.tga",
	 0.273281246f,
	 0.789218724f,
	 0.711093724f,
	 0.226406246f},                                                                       // record169
	{"Game Data\\Frontend\\Frontend bits\\Misc\\sound icon.tga", 0.0f, 1.0f, 1.0f, 0.0f}, // record170
	{"Game Data\\Frontend\\Frontend bits\\Misc\\Slider Base.tga",
	 0.00765625015f,
	 0.992343724f,
	 0.289218754f,
	 0.0232812501f}, // record171
	{"Game Data\\Frontend\\Frontend bits\\Misc\\Slider Base.tga",
	 0.476406246f,
	 0.992343724f,
	 0.507968724f,
	 0.0232812501f}, // record172
	{"Game Data\\Frontend\\Frontend bits\\Misc\\Slider Base.tga",
	 0.695156276f,
	 0.992343724f,
	 0.976718724f,
	 0.0232812501f},                                                                           // record173
	{"Game Data\\Frontend\\Frontend bits\\Misc\\Slider Inactive.tga", 0.0f, 1.0f, 1.0f, 0.0f}, // record174
	{"Game Data\\Frontend\\Frontend bits\\Misc\\space bar.tga", 0.0f, 1.0f, 1.0f, 0.0f},       // record175
	{"Game Data\\Frontend\\Ch Builder Bits\\backspace.tga", 0.0f, 1.0f, 1.0f, 0.0f},           // record176
	{"Game Data\\Frontend\\Ch Builder bits\\tick.tga", 0.0f, 1.0f, 1.0f, 0.0f},                // record177
};

// STUB: LEGORACERS2 0x00435c40
void __fastcall LoadControllerPauseMenuTextures(void* p_storage, void*, LegoU32 p_mode)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 0x10) != 0) {
		return;
	}
	resources->m_flags |= 0x10;
	LegoU32 count = p_mode == 0 ? 14 : 7;
	for (LegoU32 i = 0; i < count; ++i) {
		const PauseMenuTextureDefinition* definition = &g_pauseMenuTextureDefinitions[i];
		ControllerEventRecord* record = &resources->m_records[164 + i];
		record->m_index = g_displayRenderer->CreateTextureResource(definition->m_path, 0, -1, 1, 1);
		record->m_u = definition->m_u;
		record->m_v = definition->m_v;
		record->m_uEnd = definition->m_uEnd;
		record->m_vEnd = definition->m_vEnd;
	}
	if (p_mode == 0) {
		const LegoChar* language;
		switch (g_gameControllerSettings->m_language) {
		case 0:
			language = "Danish";
			break;
		case 1:
			language = "Dutch";
			break;
		case 4:
			language = "French";
			break;
		case 5:
			language = "German";
			break;
		case 6:
			language = "Italian";
			break;
		case 7:
			language = "Spanish";
			break;
		case 8:
			language = "Swedish";
			break;
		default:
			language = "English";
			break;
		}
		CharBuffer128 path;
		sprintf(
			path.m_data,
			"Game Data\\Frontend\\Frontend bits\\Misc\\Loading languages\\Loading word %s.tga",
			language
		);
		resources->m_records[178].m_index = g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
		resources->SetFullTextureRegion(178);
	}
}

// FUNCTION: LEGORACERS2 0x00436070
void __fastcall ReleaseControllerPauseMenuTextures(void* p_storage, void*, LegoU32 p_mode)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 0x10) != 0) {
		resources->m_flags &= ~0x10u;
		if (p_mode == 0) {
			for (LegoS32 i = 178; i >= 171; --i) {
				g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[i].m_index, 1);
			}
		}
		for (LegoS32 j = 170; j >= 164; --j) {
			g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[j].m_index, 1);
		}
	}
}
