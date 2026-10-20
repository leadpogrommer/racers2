#include "platform/window/nativewindow.h"

#include "core/application/application.h"
#include "core/application/commandline.h"
#include "core/debug/diagnostics.h"
#include "core/runtime/startup.h"
#include "platform/window/windowutil.h"
#include "render/device/displayconfig.h"
#include "types.h"

// GLOBAL: LEGORACERS2 0x0067a4a0
HWND g_mainWindow = NULL;

// GLOBAL: LEGORACERS2 0x0067a4a4
HWND g_viewWindow = NULL;

// The display initializer copies the application title to this buffer before
// creating the window. The following WNDPROC global begins 64 bytes later.
// GLOBAL: LEGORACERS2 0x0067a4ac
char g_windowTitle[64] = {0};

// GLOBAL: LEGORACERS2 0x0067a4f4
HACCEL g_windowAccelerators = NULL;

// STUB: LEGORACERS2 0x004b46c0
BOOL CreateNativeWindow(WNDPROC p_windowProcedure)
{
	HBRUSH backgroundBrush = (HBRUSH) GetStockObject(4);
	ATOM windowClass = RegisterNativeWindowClass(p_windowProcedure, backgroundBrush);
	if ((SHORT) windowClass == 0) {
		return FALSE;
	}

	LONG x = 0;
	LONG y = 0;
	if (FUN_004b4a60("windowx")) {
		x = FUN_004b4b30("windowx");
	}
	if (FUN_004b4a60("windowy")) {
		y = FUN_004b4b30("windowy");
	}

	return CreateNativeWindowEx(
		1,
		x,
		y,
		g_displayConfiguration->m_width,
		g_displayConfiguration->m_height,
		g_displayConfiguration->m_fullscreen
	);
}

// STUB: LEGORACERS2 0x004b4750
ATOM RegisterNativeWindowClass(WNDPROC p_windowProcedure, HBRUSH p_backgroundBrush)
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass, sizeof(windowClass));

	DWORD backgroundValue = (DWORD) p_backgroundBrush;
	if (backgroundValue <= 0x18) {
		p_backgroundBrush = (HBRUSH) (backgroundValue + 1);
	}

	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = p_windowProcedure;
	windowClass.hInstance = g_hInstance;
	windowClass.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE((WORD) GetApplication()->GetIconResourceId()));
	windowClass.hCursor = GetApplication()->IsDevelopmentMode() ? NULL : LoadCursor(NULL, MAKEINTRESOURCE(0x7f00));
	windowClass.lpszClassName = "Saracen II";
	windowClass.hbrBackground = p_backgroundBrush;

	return RegisterClassEx(&windowClass);
}

// STUB: LEGORACERS2 0x004b4800
BOOL CreateNativeWindowEx(LONG p_unused, LONG p_x, LONG p_y, LONG p_width, LONG p_height, LONG p_fullscreen)
{
	if (p_x == 0) {
		p_x = CW_USEDEFAULT;
	}
	if (p_y == 0) {
		p_y = CW_USEDEFAULT;
	}
	if (p_width == 0) {
		LONG screenWidth = GetSystemMetrics(SM_CXSCREEN);
		p_width = ((screenWidth + ((screenWidth >> 31) & 3)) >> 2) * 3;
	}
	if (p_height == 0) {
		LONG screenHeight = GetSystemMetrics(SM_CYSCREEN);
		p_height = ((screenHeight + ((screenHeight >> 31) & 3)) >> 2) * 3;
	}

	DWORD extendedStyle;
	DWORD style;
	LONG windowWidth;
	LONG windowHeight;
	if (p_fullscreen == 0) {
		RECT bounds;
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = p_width;
		bounds.bottom = p_height;
		AdjustWindowRectEx(&bounds, WS_OVERLAPPEDWINDOW, GetApplication()->IsDevelopmentMode(), 0x300);

		windowWidth = bounds.right - bounds.left;
		windowHeight = bounds.bottom - bounds.top;
		extendedStyle = 0x300;
		style = 0x00cb0000;
	}
	else {
		p_x = 0;
		p_y = 0;
		windowWidth = GetSystemMetrics(SM_CXSCREEN);
		windowHeight = GetSystemMetrics(SM_CYSCREEN);
		extendedStyle = WS_EX_TOPMOST;
		style = WS_POPUP;
	}

	HWND window = CreateWindowEx(
		extendedStyle,
		"Saracen II",
		g_windowTitle,
		style,
		p_x,
		p_y,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		g_hInstance,
		NULL
	);
	if (window == NULL) {
		return FALSE;
	}

	HWND viewWindow = window;
	if (GetApplication()->IsDevelopmentMode()) {
		viewWindow = CreateWindowEx(
			0,
			"static",
			"I love this editor",
			0x50000007,
			0,
			0,
			p_width,
			p_height,
			window,
			(HMENU) 1,
			g_hInstance,
			NULL
		);
		if (viewWindow == NULL) {
			ReportFatalError("Could not make view window");
		}
	}

	ShowWindow(window, SW_HIDE);
	SetForegroundWindow(window);
	SetFocus(window);
	ShowWindow(window, SW_SHOW);
	FitWindowToChildren(window);

	g_mainWindow = window;
	g_viewWindow = viewWindow;

	if (GetApplication()->IsDevelopmentMode()) {
		g_windowAccelerators =
			LoadAccelerators(g_hInstance, MAKEINTRESOURCE((WORD) GetApplication()->GetAcceleratorResourceId()));
	}

	return TRUE;
}
