#ifndef LR2_RENDERERSHADOWSHARED_H
#define LR2_RENDERERSHADOWSHARED_H

class DisplayRenderer;

void __fastcall EnsureSharedShadowGeometry(DisplayRenderer* p_renderer, void* p_unusedEdx);

void __fastcall ReleaseSharedShadowGeometry(DisplayRenderer* p_renderer, void* p_unusedEdx);

#endif // LR2_RENDERERSHADOWSHARED_H
