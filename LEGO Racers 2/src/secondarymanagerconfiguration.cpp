#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "secondarymanager.h"

#include <windows.h>

// STUB: LEGORACERS2 0x0042a870
void SecondaryManager::EnumerateDisplayModes()
{
	m_unk0x4bc = 0;
	LegoS32 available = ((const RendererMainDispatch*) g_displayRenderer->m_vtable)
							->m_getSelectedAdapterModeCount(g_displayRenderer, NULL);
	for (LegoS32 index = 0; index < available; ++index) {
		SecondaryDisplayMode mode;
		LegoU32 refreshRate;
		((const RendererMainDispatch*) g_displayRenderer->m_vtable)
			->m_getSelectedAdapterMode(
				g_displayRenderer,
				NULL,
				index,
				&mode.m_width,
				&mode.m_height,
				&mode.m_depth,
				&refreshRate
			);
		if (mode.m_depth != 16 && mode.m_depth != 32) {
			continue;
		}
		LegoS32 memory = ((const RendererMainDispatch*) g_displayRenderer->m_vtable)
							 ->m_getDeviceMemoryMegabytes(g_displayRenderer, NULL);
		LegoBool32 lowMemory = memory != 0 && memory <= 24;
		if (!((LegoS32) mode.m_width <= 640 && (LegoS32) mode.m_height <= 480) &&
			!(mode.m_width == 800 && mode.m_height == 600) && !(mode.m_width == 1024 && mode.m_height == 768) &&
			!(!lowMemory &&
			  ((mode.m_width == 1280 && mode.m_height == 1024) || (mode.m_width == 1600 && mode.m_height == 1200)))) {
			continue;
		}
		LegoBool32 shiftUp = TRUE;
		if (m_unk0x4bc >= 50 && (LegoS32) m_displayModes[0].m_width < 640) {
			shiftUp = FALSE;
		}
		LegoS32 position = 0;
		LegoBool32 duplicate = FALSE;
		for (; position < (LegoS32) m_unk0x4bc; ++position) {
			SecondaryDisplayMode* entry = &m_displayModes[position];
			if (mode.m_width == entry->m_width && mode.m_height == entry->m_height && mode.m_depth == entry->m_depth) {
				duplicate = TRUE;
				break;
			}
			if ((LegoS32) mode.m_width < (LegoS32) entry->m_width ||
				(mode.m_width == entry->m_width &&
				 ((LegoS32) mode.m_height < (LegoS32) entry->m_height ||
				  (mode.m_height == entry->m_height && (LegoS32) mode.m_depth < (LegoS32) entry->m_depth)))) {
				break;
			}
		}
		if (duplicate) {
			continue;
		}
		if (!shiftUp) {
			--position;
		}
		if (position < 0 || position >= 50) {
			continue;
		}
		if (shiftUp) {
			LegoS32 end = m_unk0x4bc < 50 ? m_unk0x4bc : 49;
			for (; end > position; --end) {
				m_displayModes[end] = m_displayModes[end - 1];
			}
		}
		else {
			for (LegoS32 entry = 0; entry < position; ++entry) {
				m_displayModes[entry] = m_displayModes[entry + 1];
			}
		}
		m_displayModes[position] = mode;
		if (++m_unk0x4bc > 50) {
			m_unk0x4bc = 50;
		}
	}
}

// STUB: LEGORACERS2 0x0042ab00
void SecondaryManager::DetectKeyboardLanguage()
{
	m_unk0x4cc = 2;
	LegoChar name[12];
	if (!GetKeyboardLayoutName(name)) {
		return;
	}
	LegoU32 layout = 0;
	for (LegoS32 index = 0; index < 8; ++index) {
		LegoS32 digit;
		if (name[index] >= '0' && name[index] <= '9') {
			digit = name[index] - '0';
		}
		else if (name[index] >= 'a' && name[index] <= 'f') {
			digit = name[index] - 'a' + 10;
		}
		else if (name[index] >= 'A' && name[index] <= 'F') {
			digit = name[index] - 'A' + 10;
		}
		else {
			return;
		}
		layout = layout * 16 + digit;
	}
	switch (layout & 0x3ff) {
	case 3:
	case 10:
	case 0x2d:
		m_unk0x4cc = 7;
		break;
	case 6:
		m_unk0x4cc = 0;
		break;
	case 7:
	case 8:
		m_unk0x4cc = 5;
		break;
	case 9:
		m_unk0x4cc = (layout & 0xfc00) == 0x400 ? 3 : 2;
		break;
	case 0x0c:
		m_unk0x4cc = 4;
		break;
	case 0x10:
		m_unk0x4cc = 6;
		break;
	case 0x13:
		m_unk0x4cc = 1;
		break;
	case 0x1d:
		m_unk0x4cc = 8;
		break;
	default:
		m_unk0x4cc = 2;
	}
}
