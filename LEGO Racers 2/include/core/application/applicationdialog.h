#ifndef LR2_APPLICATIONDIALOG_H
#define LR2_APPLICATIONDIALOG_H

#include <windows.h>

INT_PTR ShowApplicationSettingsDialog();
INT_PTR CALLBACK ApplicationSettingsDialogProc(HWND p_window, UINT p_message, WPARAM p_wParam, LPARAM p_lParam);

#endif // LR2_APPLICATIONDIALOG_H
