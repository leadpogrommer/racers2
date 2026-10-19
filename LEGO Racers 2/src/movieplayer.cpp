#include "movieplayer.h"

#include "decomp.h"
#include "heapaddresswalk.h"
#include "memoryheap.h"
#include "moviedirectshow.h"
#include "renderer.h"

#include <stdlib.h>

DECOMP_SIZE_ASSERT(MoviePlayerDispatch, 0x0c);
DECOMP_SIZE_ASSERT(MoviePlayer, 4);

// GLOBAL: LEGORACERS2 0x00578620
static LegoU8 g_moviePlayerInitialized;

// GLOBAL: LEGORACERS2 0x00578624
static MoviePlayer g_moviePlayerStorage;

static void __fastcall MoviePlayerPlay(MoviePlayer*, void*, const LegoChar* p_path, LegoBool32 p_allowSkip);
static void __fastcall MoviePlayerNoOp(MoviePlayer*, void*);
static LegoU32 __fastcall MoviePlayerZero(MoviePlayer*, void*);
static void __cdecl MoviePlayerStaticDestructor();

const MoviePlayerDispatch g_moviePlayerDispatch = {MoviePlayerPlay, MoviePlayerNoOp, MoviePlayerZero};

// STUB: LEGORACERS2 0x0049a390
MoviePlayer* GetMoviePlayer()
{
	if ((g_moviePlayerInitialized & 1) == 0) {
		g_moviePlayerStorage.m_vtable = &g_moviePlayerDispatch;
		g_moviePlayerInitialized |= 1;
		atexit(MoviePlayerStaticDestructor);
	}
	return &g_moviePlayerStorage;
}

// FUNCTION: LEGORACERS2 0x0049a3d0 FOLDED
static void __cdecl MoviePlayerStaticDestructor()
{
}

// STUB: LEGORACERS2 0x0049a3e0
static void __fastcall MoviePlayerPlay(MoviePlayer*, void*, const LegoChar* p_path, LegoBool32 p_allowSkip)
{
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	for (LegoS32 frameBefore = 0; frameBefore < 4; ++frameBefore) {
		renderer->m_vtable->m_clearBuffers(renderer, NULL, 7);
		renderer->m_vtable->m_present(renderer, NULL);
	}
	renderer->BindMappedTexture(-1);

	WalkHeapAddressRange(GetMemoryHeap(), NULL);
	MemoryHeap* heap = GetMemoryHeap();
	heap->m_useCRT = TRUE;
	PlayMpegMovie(p_path, p_allowSkip);
	GetMemoryHeap()->m_useCRT = FALSE;

	for (LegoS32 frameAfter = 0; frameAfter < 4; ++frameAfter) {
		renderer->m_vtable->m_clearBuffers(renderer, NULL, 7);
		renderer->m_vtable->m_present(renderer, NULL);
	}
}

static void __fastcall MoviePlayerNoOp(MoviePlayer*, void*)
{
}

static LegoU32 __fastcall MoviePlayerZero(MoviePlayer*, void*)
{
	return 0;
}
