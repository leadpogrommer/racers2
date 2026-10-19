#include "charbuffer128.h"
#include "controllereventrecords.h"
#include "menubackground.h"
#include "renderer.h"

#include <stdio.h>
#include <string.h>

// STUB: LEGORACERS2 0x00436e10
void __fastcall LoadControllerBackgroundTextures(ControllerEventRecordTable* p_records, void*)
{
	if ((p_records->m_flags & 0x40) == 0) {
		p_records->m_flags |= 0x40;
		CharBuffer128 path;
		const LegoChar* colors[4] = {"Blue", "Red", "Yellow", "Orange"};
		for (LegoS32 color = 0; color < 4; ++color) {
			sprintf(path.m_data, "Game Data\\Frontend\\Background\\%s\\Back%s1.tga", "PC", colors[color]);
			LegoS32 digit = strlen(path.m_data) - 5;
			for (LegoS32 quadrant = 0; quadrant < 4; ++quadrant) {
				path.m_data[digit] = (LegoChar) ('1' + quadrant);
				ControllerEventRecord* record = &p_records->m_records[233 + color * 4 + quadrant];
				record->m_index = g_displayRenderer->CreateTextureResource(path.m_data, 0, -1, 1, 1);
				record->m_u = record->m_vEnd = 0.0f;
				record->m_v = record->m_uEnd = 1.0f;
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x00436f20
void __fastcall ReleaseControllerBackgroundTextures(ControllerEventRecordTable* p_records, void*)
{
	if ((p_records->m_flags & 0x40) != 0) {
		p_records->m_flags &= ~0x40u;
		for (LegoS32 index = 0; index < 16; ++index) {
			g_displayRenderer->ReleaseTextureResourceReference(p_records->m_records[233 + index].m_index, 1);
		}
	}
}
