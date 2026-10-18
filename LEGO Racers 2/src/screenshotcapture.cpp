#include "displayconfig.h"
#include "filemanager.h"
#include "nativewindow.h"
#include "renderer.h"
#include "screenshot.h"
#include "windowutil.h"
#include "winfile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern CharBuffer128 g_screenshotDirectory;
extern LegoBool32 g_screenshotEnabled;

// STUB: LEGORACERS2 0x004bb210
void CaptureScreenshot()
{
	IDirect3DSurface8* surface = NULL;
	LegoS32 screenWidth = GetSystemMetrics(SM_CXSCREEN);
	LegoS32 screenHeight = GetSystemMetrics(SM_CYSCREEN);
	if (g_screenshotEnabled == FALSE) {
		return;
	}
	GetStaticDisplayRenderer()
		->m_device->CreateImageSurface(screenWidth & 0xffff, screenHeight & 0xffff, D3DFMT_A8R8G8B8, &surface);
	GetStaticDisplayRenderer()->m_device->GetFrontBuffer(surface);
	RECT lockBounds;
	LegoS32 firstX;
	LegoS32 firstY;
	LegoS32 lastX;
	LegoS32 lastY;
	if (g_displayConfiguration->m_fullscreen == 0) {
		RECT client;
		GetClientRect(g_mainWindow, &client);
		LegoS32 outerWidth = GetWindowWidth(g_mainWindow) & 0xffff;
		LegoS32 outerHeight = GetWindowHeight(g_mainWindow) & 0xffff;
		firstX = (outerWidth - client.right) / 2;
		firstY = outerHeight - client.bottom - firstX;
		lastX = client.right - firstX;
		lastY = firstY + client.bottom;
		lockBounds.left = (GetWindowLeft(g_mainWindow) & 0xffff) + client.left;
		lockBounds.right = (GetWindowLeft(g_mainWindow) & 0xffff) + client.right;
		lockBounds.bottom = (GetWindowTop(g_mainWindow) & 0xffff) + client.bottom;
		lockBounds.top = (GetWindowTop(g_mainWindow) & 0xffff) + client.top;
	}
	else {
		firstX = firstY = 0;
		lastX = g_displayConfiguration->m_width & 0xffff;
		lastY = g_displayConfiguration->m_height & 0xffff;
		lockBounds.left = lockBounds.top = 0;
		lockBounds.right = lastX;
		lockBounds.bottom = lastY;
	}
	D3DLOCKED_RECT locked;
	if (FAILED(surface->LockRect(&locked, &lockBounds, D3DLOCK_READONLY))) {
		if (surface != NULL) {
			surface->Release();
		}
		return;
	}
	LegoS32 width = lastX - firstX;
	LegoS32 height = lastY - firstY;
	LegoU8* pixels = (LegoU8*) malloc(width * height * 3);
	LegoU8* destination = pixels;
	for (LegoS32 y = lastY - 1; y >= firstY; --y) {
		const LegoU32* row = (const LegoU32*) ((const LegoU8*) locked.pBits + locked.Pitch * y) + firstX;
		for (LegoS32 x = firstX; x < lastX; ++x) {
			LegoU32 pixel = *row++;
			*(LegoU16*) destination = (LegoU16) pixel;
			destination[2] = (LegoU8) (pixel >> 16);
			destination += 3;
		}
	}
	BITMAPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4d42;
	fileHeader.bfSize = width * height * 3 + 0x36;
	fileHeader.bfReserved1 = fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = 0x36;
	BITMAPINFOHEADER info;
	info.biSize = 0x28;
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;
	info.biBitCount = 24;
	info.biCompression = info.biSizeImage = 0;
	info.biXPelsPerMeter = info.biYPelsPerMeter = 0xb12;
	info.biClrUsed = info.biClrImportant = 0;
	CreateDirectory(g_screenshotDirectory.m_data, NULL);
	GetFileManager()->SetFlags(1, TRUE);
	char path[128];
	char name[32];
	LegoBool32 available = FALSE;
	LegoU32 number = firstY;
	do {
		strcpy(path, g_screenshotDirectory.m_data);
		sprintf(name, "\\Photo%03d.bmp", number++);
		strcat(path, name);
		if (ManagedPathExists(path) == FALSE) {
			available = TRUE;
			break;
		}
	} while (number < 1000);
	if (available != FALSE) {
		BOOL opened;
		WinFile file(path, 1, &opened);
		if (opened != FALSE) {
			file.WriteExact(&fileHeader, 0x0e);
			file.WriteExact(&info, 0x28);
			file.WriteExact(pixels, width * height * 3);
		}
		file.CloseManaged();
	}
	GetFileManager()->SetFlags(1, FALSE);
	free(pixels);
	surface->UnlockRect();
	if (surface != NULL) {
		surface->Release();
	}
}
