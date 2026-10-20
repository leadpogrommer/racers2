#ifndef LR2_MOVIEFILTER_H
#define LR2_MOVIEFILTER_H

#include "decomp.h"
#include "types.h"

#include <streams.h>

class MovieTextureRenderer : public CBaseVideoRenderer {
public:
	MovieTextureRenderer(LPUNKNOWN p_outer, HRESULT* p_result);
	virtual ~MovieTextureRenderer();

	virtual HRESULT CheckMediaType(const CMediaType* p_mediaType);
	virtual HRESULT SetMediaType(const CMediaType* p_mediaType);
	virtual HRESULT DoRenderSample(IMediaSample* p_sample);

private:
	LegoU8 m_movieData[0x130];
};

DECOMP_SIZE_ASSERT(MovieTextureRenderer, 0x278);

void __fastcall ClearMovieTextures(MovieTextureRenderer* p_renderer, void* p_edx);

#endif // LR2_MOVIEFILTER_H
