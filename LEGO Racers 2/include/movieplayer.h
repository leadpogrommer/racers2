#ifndef LR2_MOVIEPLAYER_H
#define LR2_MOVIEPLAYER_H

#include "types.h"

struct MoviePlayer;
struct MoviePlayerDispatch {
	void(__fastcall* m_play)(MoviePlayer*, void*, const LegoChar*, LegoBool32);
	// These later slots are not needed by the startup movie call path.
	void(__fastcall* m_noOp)(MoviePlayer*, void*);
	LegoU32(__fastcall* m_zero)(MoviePlayer*, void*);
};
struct MoviePlayer {
	const MoviePlayerDispatch* m_vtable;
};
extern const MoviePlayerDispatch g_moviePlayerDispatch;
MoviePlayer* GetMoviePlayer();
LegoU32 __cdecl PlayMpegMovie(const LegoChar* p_path, LegoBool32 p_allowSkip);
#endif
