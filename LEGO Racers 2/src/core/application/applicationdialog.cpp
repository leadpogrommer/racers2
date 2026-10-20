#include "core/application/applicationdialog.h"

#include "core/runtime/startup.h"
#include "render/device/displayconfig.h"

#include <winreg.h>

// GLOBAL: LEGORACERS2 0x00549780
static const DWORD g_dialogCheckboxStates[2] = {BST_UNCHECKED, BST_CHECKED};

namespace
{
struct DisplaySetting {
	LPCSTR m_registryName;
	int m_controlId;
	LegoU32 DisplayConfiguration::* m_member;
};

const DisplaySetting k_displaySettings[] = {
	{"Dithering", 1000, &DisplayConfiguration::m_dithering},
	{"AntiAliasing", 1001, &DisplayConfiguration::m_antiAliasing},
	{"WriteOnlyVBs", 1002, &DisplayConfiguration::m_writeOnlyVBs},
	{"LargeVBs", 1003, &DisplayConfiguration::m_largeVBs},
	{"HardwareTransformAndLighting", 1004, &DisplayConfiguration::m_hardwareTransformAndLighting},
	{"NVidia", 1005, &DisplayConfiguration::m_nvidia},
	{"VSync", 1006, &DisplayConfiguration::m_vSync},
	{"TripleBuffer", 1007, &DisplayConfiguration::m_tripleBuffer},
};

const char k_registryPath[] = "Software\\Attention To Detail\\Rendering Options";

HKEY OpenDisplaySettingsKey(HKEY p_initialKey)
{
	// Retail reuses the wParam slot for the output and ignores API failure.
	HKEY key = p_initialKey;
	DWORD disposition;
	RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		k_registryPath,
		0,
		"",
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&key,
		&disposition
	);
	return key;
}
} // namespace

// FUNCTION: LEGORACERS2 0x0049a800
INT_PTR ShowApplicationSettingsDialog()
{
	return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(0x66), NULL, ApplicationSettingsDialogProc, 0);
}

// STUB: LEGORACERS2 0x0049a820
INT_PTR CALLBACK ApplicationSettingsDialogProc(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam)
{
	if (p_message == WM_INITDIALOG) {
		HKEY key = OpenDisplaySettingsKey((HKEY) p_wParam);
		DWORD value = p_message;
		DWORD valueSize = sizeof(value);
		for (unsigned int index = 0; index < sizeof(k_displaySettings) / sizeof(k_displaySettings[0]); ++index) {
			const DisplaySetting& setting = k_displaySettings[index];
			if (RegQueryValueEx(key, setting.m_registryName, NULL, NULL, (LPBYTE) &value, &valueSize) ==
				ERROR_SUCCESS) {
				g_displayConfiguration->*(setting.m_member) = value;
			}
		}
		for (unsigned int stateIndex = 0; stateIndex < sizeof(k_displaySettings) / sizeof(k_displaySettings[0]);
			 ++stateIndex) {
			const DisplaySetting& setting = k_displaySettings[stateIndex];
			SendDlgItemMessage(
				p_window,
				setting.m_controlId,
				BM_SETCHECK,
				g_dialogCheckboxStates[g_displayConfiguration->*(setting.m_member)],
				0
			);
		}
		RegCloseKey(key);
		return TRUE;
	}

	if (p_message == WM_COMMAND) {
		switch (LOWORD(p_wParam)) {
		case IDOK: {
			HKEY key = OpenDisplaySettingsKey((HKEY) p_wParam);
			for (unsigned int index = 0; index < sizeof(k_displaySettings) / sizeof(k_displaySettings[0]); ++index) {
				const DisplaySetting& setting = k_displaySettings[index];
				LegoU32 value = SendDlgItemMessage(p_window, setting.m_controlId, BM_GETCHECK, 0, 0) == BST_CHECKED;
				g_displayConfiguration->*(setting.m_member) = value;
			}
			for (unsigned int valueIndex = 0; valueIndex < sizeof(k_displaySettings) / sizeof(k_displaySettings[0]);
				 ++valueIndex) {
				const DisplaySetting& setting = k_displaySettings[valueIndex];
				LegoU32 value = g_displayConfiguration->*(setting.m_member);
				RegSetValueEx(key, setting.m_registryName, 0, REG_DWORD, (const BYTE*) &value, sizeof(value));
			}
			RegCloseKey(key);
			EndDialog(p_window, IDOK);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(p_window, IDCANCEL);
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}
