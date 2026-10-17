#ifndef LR2_D3DX8_H
#define LR2_D3DX8_H

#include "directx8.h"

// Narrow declarations from the verified DirectX 8.0 SDK d3dx8tex.h. Interface
// types come from the real DirectX headers, including in the Wine bridge.
// These functions link to the external retail static d3dx8.lib.
extern "C"
{
	HRESULT WINAPI D3DXLoadSurfaceFromSurface(
		LPDIRECT3DSURFACE8 pDestSurface,
		CONST PALETTEENTRY* pDestPalette,
		CONST RECT* pDestRect,
		LPDIRECT3DSURFACE8 pSrcSurface,
		CONST PALETTEENTRY* pSrcPalette,
		CONST RECT* pSrcRect,
		DWORD Filter,
		D3DCOLOR ColorKey
	);

	HRESULT WINAPI D3DXLoadSurfaceFromMemory(
		LPDIRECT3DSURFACE8 pDestSurface,
		CONST PALETTEENTRY* pDestPalette,
		CONST RECT* pDestRect,
		LPCVOID pSrcMemory,
		D3DFORMAT SrcFormat,
		UINT SrcPitch,
		CONST PALETTEENTRY* pSrcPalette,
		CONST RECT* pSrcRect,
		DWORD Filter,
		D3DCOLOR ColorKey
	);
}

#endif // LR2_D3DX8_H
