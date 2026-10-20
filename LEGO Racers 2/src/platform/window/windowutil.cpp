#include "platform/window/windowutil.h"

// GLOBAL: LEGORACERS2 0x00679b80
HWND g_unk0x00679b80 = NULL;

// FUNCTION: LEGORACERS2 0x004b4b80
LONG GetWindowWidth(HWND p_window)
{
	RECT rect;
	GetWindowRect(p_window, &rect);
	return rect.right - rect.left;
}

// FUNCTION: LEGORACERS2 0x004b4bb0
LONG GetWindowHeight(HWND p_window)
{
	RECT rect;
	GetWindowRect(p_window, &rect);
	return rect.bottom - rect.top;
}

// FUNCTION: LEGORACERS2 0x004b4be0
LONG GetWindowLeft(HWND p_window)
{
	RECT rect;
	GetWindowRect(p_window, &rect);
	return rect.left;
}

// FUNCTION: LEGORACERS2 0x004b4c00
LONG GetWindowTop(HWND p_window)
{
	RECT rect;
	GetWindowRect(p_window, &rect);
	return rect.top;
}

// FUNCTION: LEGORACERS2 0x004b4c20
HWND GetWindowOwner(HWND p_window)
{
	return (HWND) GetWindowLong(p_window, GWL_HWNDPARENT);
}

// FUNCTION: LEGORACERS2 0x004b4c30
LONG GetWindowStyle(HWND p_window)
{
	return GetWindowLong(p_window, GWL_STYLE);
}

// FUNCTION: LEGORACERS2 0x004b4c40
BOOL SetWindowWidth(HWND p_window, LONG p_width)
{
	RECT rect;
	if (GetWindowWidth(p_window) == p_width) {
		return FALSE;
	}
	GetWindowRect(p_window, &rect);
	if ((GetWindowStyle(p_window) & WS_CHILD) && GetWindowOwner(p_window) != NULL) {
		ScreenRectToClient(GetWindowOwner(p_window), &rect);
	}
	MoveWindow(p_window, rect.left, rect.top, p_width, rect.bottom - rect.top, TRUE);
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b4cd0
BOOL SetWindowHeight(HWND p_window, LONG p_height)
{
	RECT rect;
	if (GetWindowHeight(p_window) == p_height) {
		return FALSE;
	}
	GetWindowRect(p_window, &rect);
	if ((GetWindowStyle(p_window) & WS_CHILD) && GetWindowOwner(p_window) != NULL) {
		ScreenRectToClient(GetWindowOwner(p_window), &rect);
	}
	MoveWindow(p_window, rect.left, rect.top, rect.right - rect.left, p_height, TRUE);
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b4d60
BOOL CALLBACK ExpandChildWindowBounds(HWND p_window, LPARAM p_bounds)
{
	RECT rect;
	RECT* bounds = (RECT*) p_bounds;
	if (GetParent(p_window) == g_unk0x00679b80) {
		GetWindowRect(p_window, &rect);
		ScreenRectToClient(GetParent(p_window), &rect);
		if (rect.bottom > bounds->bottom) {
			bounds->bottom = rect.bottom;
		}
		if (rect.right > bounds->right) {
			bounds->right = rect.right;
		}
		if (rect.left < bounds->left) {
			bounds->left = rect.left;
		}
		if (rect.top < bounds->top) {
			bounds->top = rect.top;
		}
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004b4de0
void FitWindowToChildren(HWND p_window)
{
	RECT rect;
	rect.bottom = 0;
	rect.top = 0;
	rect.left = 0;
	rect.right = 0;
	g_unk0x00679b80 = p_window;
	if (EnumChildWindows(p_window, ExpandChildWindowBounds, (LPARAM) &rect)) {
		LONG style = GetWindowLong(p_window, GWL_STYLE);
		LONG exStyle = GetWindowLong(p_window, GWL_EXSTYLE);
		BOOL hasMenu = GetMenu(p_window) != NULL;
		AdjustWindowRectEx(&rect, style, hasMenu, exStyle);
		LONG width = rect.right - rect.left;
		LONG height = rect.bottom - rect.top;
		SetWindowWidth(p_window, width);
		SetWindowHeight(p_window, height);
	}
}

// FUNCTION: LEGORACERS2 0x004b4e80
BOOL ScreenRectToClient(HWND p_window, RECT* p_rect)
{
	POINT topLeft;
	POINT bottomRight;
	topLeft.x = p_rect->left;
	topLeft.y = p_rect->top;
	bottomRight.x = p_rect->right;
	bottomRight.y = p_rect->bottom;
	ScreenToClient(p_window, &topLeft);
	ScreenToClient(p_window, &bottomRight);
	p_rect->bottom = bottomRight.y;
	p_rect->right = bottomRight.x;
	p_rect->top = topLeft.y;
	p_rect->left = topLeft.x;
	return TRUE;
}
