#include "frontend/menu/menubackground.h"

#include "frontend/menu/menumanager.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/resources/controllereventrecords.h"
#include "game/controller/gamecontroller.h"
#include "game/session/gamecontrollersession.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x005304bc
LegoFloat g_menuBackgroundFadeDuration = 0.5f;

namespace
{
LegoU8 GetBackgroundAlpha(LegoFloat p_value)
{
	LegoFloat value = p_value * 255.0f;
	LegoS32 result;
	// Retail uses the current FPU rounding mode for alpha conversion.
	__asm {
		fld value
		fistp result
	}
	return (LegoU8) result;
}

void ApplyBackgroundQuadrants(
	GlobalOverlaySprite** p_sprites,
	ControllerEventRecordTable* p_records,
	LegoS32 p_category,
	LegoFloat p_alpha
)
{
	if (p_category == -1) {
		for (LegoS32 index = 0; index < 4; ++index) {
			p_sprites[index]->SetVisible(0);
		}
	}
	else {
		LegoU8 alpha = GetBackgroundAlpha(p_alpha);
		for (LegoS32 index = 0; index < 4; ++index) {
			memcpy(&p_sprites[index]->m_texture, &p_records->m_records[233 + p_category * 4 + index], 0x14);
			p_sprites[index]->SetAlpha(alpha);
			p_sprites[index]->SetVisible(1);
		}
	}
}
} // namespace

// FUNCTION: LEGORACERS2 0x004267c0
MenuBackground::MenuBackground()
{
	memset(m_sprites, 0, sizeof(m_sprites));
}

// STUB: LEGORACERS2 0x004267e0
void MenuBackground::Initialize()
{
	m_mode = 0;
	m_transition = 0;
	m_current = m_requested = m_previous = -1;
	m_loaded = 0;
}

// STUB: LEGORACERS2 0x00426800
MenuBackground::~MenuBackground()
{
	if (m_loaded != 0) {
		ReleaseControllerBackgroundTextures((ControllerEventRecordTable*) g_gameController.m_unk0x90, NULL);
		m_loaded = 0;
	}
}

// STUB: LEGORACERS2 0x00426820
void MenuBackground::Update(LegoFloat p_elapsed)
{
	if (m_mode == 0 || m_loaded == 0) {
		return;
	}
	if (m_mode == 2) {
		m_requested = GetMenuCategory();
	}
	if (m_requested != m_current) {
		m_previous = m_current;
		m_current = m_requested;
		m_transition = 1;
		m_remaining = g_menuBackgroundFadeDuration;
	}
	m_remaining -= p_elapsed;
	if (m_transition != 1) {
		return;
	}
	// The retail status-word test also takes the completed branch for NaN.
	if (!(m_remaining > 0.0f)) {
		m_remaining = 0.0f;
		m_transition = 0;
	}
	ControllerEventRecordTable* records = (ControllerEventRecordTable*) g_gameController.m_unk0x90;
	LegoFloat previousAlpha = 1.0f;
	if (m_previous != -1 && g_menuBackgroundFadeDuration >= m_remaining) {
		previousAlpha = (1.0f / g_menuBackgroundFadeDuration) * m_remaining;
	}
	ApplyBackgroundQuadrants(&m_sprites[4], records, m_previous, previousAlpha);
	LegoFloat currentAlpha = 1.0f;
	if (m_current != -1 && m_remaining >= 0.0f) {
		currentAlpha = (1.0f / g_menuBackgroundFadeDuration) * (g_menuBackgroundFadeDuration - m_remaining);
	}
	ApplyBackgroundQuadrants(m_sprites, records, m_current, currentAlpha);
}

// STUB: LEGORACERS2 0x00426a30
void MenuBackground::Load()
{
	if (m_loaded == 0) {
		m_loaded = 1;
		LoadControllerBackgroundTextures((ControllerEventRecordTable*) g_gameController.m_unk0x90, NULL);
		LegoU32 parameters[21];
		parameters[0] = parameters[1] = 0;
		parameters[2] = parameters[3] = 0x3f000000;
		parameters[4] = parameters[5] = 0;
		parameters[6] = parameters[7] = 0x3f800000;
		// Retail leaves tile size (8,9) and initial UVs (12..15) untouched.
		parameters[10] = 0xffffffff;
		parameters[11] = 0;
		parameters[16] = 0;
		parameters[17] = 7;
		parameters[18] = parameters[19] = parameters[20] = 0;
		for (LegoS32 index = 0; index < 8; ++index) {
			m_sprites[index] = g_globalOverlayManager.AllocateSprite(parameters);
			m_sprites[index]->SetVisible(0);
			LegoFloat position[2] = {(index & 1) ? 0.5f : 0.0f, (index & 2) ? 0.5f : 0.0f};
			m_sprites[index]->SetPosition(position);
		}
	}
}

// STUB: LEGORACERS2 0x00426b60
void MenuBackground::Release()
{
	if (m_loaded != 0) {
		for (LegoS32 index = 7; index >= 0; --index) {
			if (m_sprites[index] != NULL) {
				m_sprites[index]->Release();
				m_sprites[index] = NULL;
			}
		}
		m_loaded = 0;
		ReleaseControllerBackgroundTextures((ControllerEventRecordTable*) g_gameController.m_unk0x90, NULL);
	}
}

// FUNCTION: LEGORACERS2 0x00426bb0
void MenuBackground::SetMode(LegoU32 p_mode)
{
	m_mode = p_mode;
	if (p_mode == 0) {
		Hide();
		m_transition = 0;
	}
}

// FUNCTION: LEGORACERS2 0x00426bd0
void MenuBackground::Hide()
{
	for (LegoS32 index = 0; index < 8; ++index) {
		if (m_sprites[index] != NULL) {
			m_sprites[index]->SetVisible(0);
		}
	}
	m_current = -1;
}

// STUB: LEGORACERS2 0x00426c00
LegoS32 MenuBackground::GetMenuCategory()
{
	GameControllerSession* session = GetGameControllerSession();
	LegoU32 state = ((MenuManager*) g_gameController.m_menuManager)->m_unk0x1a0;
	if ((session->m_unk0x00 == 1 && state != 1) || session->m_unk0x04 != 0) {
		return 1;
	}
	switch (state) {
	case 1:
	case 2:
	case 0x19:
	case 0x1a:
		return 0;
	case 3:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
		return 1;
	case 4:
	case 0x18:
		return 2;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 0xb:
	case 0xc:
	case 0xd:
	case 0xe:
	case 0xf:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x2a:
	case 0x2b:
	case 0x2c:
		return 3;
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f:
	case 0x20:
	case 0x21:
	case 0x22:
		switch (session->m_mode) {
		case 0:
		case 2:
			return 0;
		case 3:
		case 4:
			return 3;
		case -1:
		case 5:
			return 2;
		default:
			return -1;
		}
	default:
		return -1;
	}
}
