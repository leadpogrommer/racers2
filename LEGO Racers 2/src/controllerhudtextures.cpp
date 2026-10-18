#include "controllereventrecords.h"
#include "renderer.h"

// STUB: LEGORACERS2 0x00434bc0
void __fastcall LoadControllerHudTextures(void* p_storage, void*)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 2) != 0) {
		return;
	}
	resources->m_flags |= 2;
	LegoS32 texture = g_displayRenderer->CreateTextureResource("Game Data\\HUD\\Text Box\\text box.tga", 0, -1, 1, 1);
	resources->m_records[40].m_index = texture;
	resources->m_records[40].m_u = 0.0857812464f;
	resources->m_records[40].m_v = 0.914218724f;
	resources->m_records[40].m_uEnd = 0.414218754f;
	resources->m_records[40].m_vEnd = 0.585781276f;
	resources->m_records[41].m_index = texture;
	resources->m_records[41].m_u = 0.570156276f;
	resources->m_records[41].m_v = 0.914218724f;
	resources->m_records[41].m_uEnd = 0.898593724f;
	resources->m_records[41].m_vEnd = 0.585781276f;
	resources->m_records[42].m_index = texture;
	resources->m_records[42].m_u = 0.570156276f;
	resources->m_records[42].m_v = 0.429843754f;
	resources->m_records[42].m_uEnd = 0.898593724f;
	resources->m_records[42].m_vEnd = 0.101406246f;
	resources->m_records[43].m_index = texture;
	resources->m_records[43].m_u = 0.0857812464f;
	resources->m_records[43].m_v = 0.429843754f;
	resources->m_records[43].m_uEnd = 0.414218754f;
	resources->m_records[43].m_vEnd = 0.101406246f;
	resources->m_records[44].m_index = texture;
	resources->m_records[44].m_u = 0.476406246f;
	resources->m_records[44].m_v = 0.914218724f;
	resources->m_records[44].m_uEnd = 0.507968724f;
	resources->m_records[44].m_vEnd = 0.585781276f;
	resources->m_records[45].m_index = texture;
	resources->m_records[45].m_u = 0.570156276f;
	resources->m_records[45].m_v = 0.523593724f;
	resources->m_records[45].m_uEnd = 0.898593724f;
	resources->m_records[45].m_vEnd = 0.492031246f;
	resources->m_records[46].m_index = texture;
	resources->m_records[46].m_u = 0.476406246f;
	resources->m_records[46].m_v = 0.429843754f;
	resources->m_records[46].m_uEnd = 0.507968724f;
	resources->m_records[46].m_vEnd = 0.101406246f;
	resources->m_records[47].m_index = texture;
	resources->SetPixelTextureRegion(5, 30, 27, 33, 47, 0.015625f, 0.015625f);
	resources->m_records[48].m_index = texture;
	resources->SetPixelTextureRegion(21, 21, 42, 42, 48, 0.015625f, 0.015625f);
	resources->m_records[49].m_index =
		g_displayRenderer->CreateTextureResource("Game Data\\HUD\\Text Box\\text box arrow.tga", 0, -1, 1, 1);
	resources->SetFullTextureRegion(49);
}

// STUB: LEGORACERS2 0x00434d90
void ControllerEventRecordTable::SetPixelTextureRegion(
	LegoS32 p_left,
	LegoS32 p_top,
	LegoS32 p_right,
	LegoS32 p_bottom,
	LegoU32 p_index,
	LegoFloat p_scaleU,
	LegoFloat p_scaleV
)
{
	m_records[p_index].m_u = (p_left + 0.5f) * p_scaleU;
	m_records[p_index].m_v = 1.0f - (p_top + 0.5f) * p_scaleV;
	m_records[p_index].m_uEnd = (p_right - 0.5f) * p_scaleU;
	m_records[p_index].m_vEnd = 1.0f - (p_bottom - 0.5f) * p_scaleV;
}

// FUNCTION: LEGORACERS2 0x00434e00
void ControllerEventRecordTable::SetFullTextureRegion(LegoU32 p_index)
{
	m_records[p_index].m_u = 0.0f;
	m_records[p_index].m_v = 1.0f;
	m_records[p_index].m_uEnd = 1.0f;
	m_records[p_index].m_vEnd = 0.0f;
}

// FUNCTION: LEGORACERS2 0x00434e30
void __fastcall ReleaseControllerHudTextures(void* p_storage, void*)
{
	ControllerEventRecordTable* resources = (ControllerEventRecordTable*) p_storage;
	if ((resources->m_flags & 2) != 0) {
		resources->m_flags &= ~2u;
		g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[40].m_index, 1);
		g_displayRenderer->ReleaseTextureResourceReference(resources->m_records[49].m_index, 1);
	}
}
