#ifndef LR2_MOVIEDIRECTSHOW_H
#define LR2_MOVIEDIRECTSHOW_H

#include "types.h"

struct IMediaEvent;

LegoU32 __cdecl PlayMpegMovie(const LegoChar* p_path, LegoBool32 p_allowSkip);
LegoBool32 __stdcall IsMoviePlaybackIncomplete(IMediaEvent* p_eventSource);

#endif // LR2_MOVIEDIRECTSHOW_H
