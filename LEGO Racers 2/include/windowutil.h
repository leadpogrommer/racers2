#ifndef LR2_WINDOWUTIL_H
#define LR2_WINDOWUTIL_H

#include <windows.h>

LONG GetWindowWidth(HWND p_window);
LONG GetWindowHeight(HWND p_window);
LONG GetWindowLeft(HWND p_window);
LONG GetWindowTop(HWND p_window);
HWND GetWindowOwner(HWND p_window);
LONG GetWindowStyle(HWND p_window);
BOOL SetWindowWidth(HWND p_window, LONG p_width);
BOOL SetWindowHeight(HWND p_window, LONG p_height);
BOOL CALLBACK ExpandChildWindowBounds(HWND p_window, LPARAM p_bounds);
void FitWindowToChildren(HWND p_window);
BOOL ScreenRectToClient(HWND p_window, RECT* p_rect);

#endif // LR2_WINDOWUTIL_H
