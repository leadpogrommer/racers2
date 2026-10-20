#include "core/strings/charbuffer128.h"
#include "frontend/resources/controllereventrecords.h"
#include "frontend/resources/controllerloadingprogress.h"
#include "game/controller/gamecontrollerconfig.h"
#include "render/core/renderer.h"

#include <stdio.h>
#include <string.h>

namespace
{
void SetLoadingTextureRegion(ControllerEventRecord* p_record, const LegoU32* p_words)
{
	memcpy(&p_record->m_u, p_words, 4 * sizeof(LegoU32));
}
} // namespace

// STUB: LEGORACERS2 0x00437430
void __fastcall LoadControllerLoadingTextures(void* p_storage, void*, LegoS32 p_mode)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_storage;
	if ((records->m_flags & 0x200) == 0) {
		records->m_flags |= 0x200;
		CharBuffer128 path;
		const LegoChar* world;
		switch (p_mode) {
		case 0:
			world = "Sandy bay";
			break;
		case 1:
			world = "Arctic";
			break;
		case 2:
			world = "Dino Valley";
			break;
		case 3:
			world = "Mars";
			break;
		case 4:
			world = "xalax";
			break;
		default:
			world = "Frontend";
			break;
		}
		sprintf(
			path.m_data,
			"Game Data\\Frontend\\Frontend bits\\Loading Screens\\Backgrounds\\%s\\Loading %s 01.tga",
			"PC",
			world
		);
		LegoS32 digit = strlen(path.m_data) - 5;
		LegoS32 index;
		for (index = 0; index < 4; ++index) {
			path.m_data[digit] = (LegoChar) ('1' + index);
			ControllerEventRecord* record = &records->m_records[270 + index];
			record->m_index = g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
			record->m_u = record->m_vEnd = 0.0f;
			record->m_v = record->m_uEnd = 1.0f;
		}
		sprintf(path.m_data, "Game Data\\Frontend\\Frontend bits\\Loading Screens\\Loading bar %s.tga", world);
		static const LegoU32 regions[3][4] = {
			{0x3bfae148, 0x3f7e0a3d, 0x3eec147b, 0x3cbeb852},
			{0x3efbeb85, 0x3f7e0a3d, 0x3efc147b, 0x3cbeb852},
			{0x3f05f5c3, 0x3f7e0a3d, 0x3f7a0a3d, 0x3cbeb852}
		};
		for (index = 0; index < 3; ++index) {
			records->m_records[274 + index].m_index =
				g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
			SetLoadingTextureRegion(&records->m_records[274 + index], regions[index]);
		}
		const LegoChar* language;
		switch (GetGameControllerSettings()->m_language) {
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
		sprintf(
			path.m_data,
			"Game Data\\Frontend\\Frontend bits\\Loading Screens\\Loading languages\\Loading word %s.tga",
			language
		);
		records->m_records[277].m_index = g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
		records->m_records[277].m_u = records->m_records[277].m_vEnd = 0.0f;
		records->m_records[277].m_v = records->m_records[277].m_uEnd = 1.0f;
		sprintf(path.m_data, "Game Data\\Frontend\\Frontend bits\\Loading Screens\\equaliser %s.tga", world);
		static const LegoU32 equalizerRegions[2][4] = {
			{0x3bfae148, 0x3f660a3d, 0x3f7a0a3d, 0x3f39f5c3},
			{0x3bfae148, 0x3f360a3d, 0x3f7a0a3d, 0x3f31f5c3}
		};
		for (index = 0; index < 2; ++index) {
			records->m_records[278 + index].m_index =
				g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
			SetLoadingTextureRegion(&records->m_records[278 + index], equalizerRegions[index]);
		}
	}
}

// FUNCTION: LEGORACERS2 0x00437770
void __fastcall ReleaseControllerLoadingTextures(void* p_storage, void*, LegoS32)
{
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) p_storage;
	if ((records->m_flags & 0x200) != 0) {
		records->m_flags &= ~0x200u;
		for (LegoS32 index = 279; index >= 270; --index) {
			g_displayRenderer->ReleaseTextureResourceReference(records->m_records[index].m_index, 1);
		}
	}
}
