#ifndef LR2_NATIVEWINDOW_H
#define LR2_NATIVEWINDOW_H

#include <windows.h>

extern HWND g_mainWindow;
extern HWND g_viewWindow;
extern char g_windowTitle[64];
extern HACCEL g_windowAccelerators;

BOOL CreateNativeWindow(WNDPROC p_windowProcedure);
ATOM RegisterNativeWindowClass(WNDPROC p_windowProcedure, HBRUSH p_backgroundBrush);
BOOL CreateNativeWindowEx(LONG p_unused, LONG p_x, LONG p_y, LONG p_width, LONG p_height, LONG p_fullscreen);

#endif // LR2_NATIVEWINDOW_H
