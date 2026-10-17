#ifndef LR2_RENDERERQUADDRAW_H
#define LR2_RENDERERQUADDRAW_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// The rectangle packet is copied as 21 DWORDs by the retail renderer. Its
// coordinate and texture-coordinate words are understood; retain the other
// words as opaque fields until their callers establish narrower meanings.
struct RendererGuiRectangle {
	LegoFloat m_left;          // 0x00
	LegoFloat m_top;           // 0x04
	LegoFloat m_right;         // 0x08
	LegoFloat m_bottom;        // 0x0c
	LegoFloat m_clipLeft;      // 0x10
	LegoFloat m_clipTop;       // 0x14
	LegoFloat m_clipRight;     // 0x18
	LegoFloat m_clipBottom;    // 0x1c
	LegoU32 m_opaque0x20[2];   // 0x20
	LegoU32 m_color;           // 0x28, packed color consumed by the draw method
	LegoU32 m_textureIndex;    // 0x2c, written by the settings caller
	LegoFloat m_textureLeft;   // 0x30
	LegoFloat m_textureTop;    // 0x34
	LegoFloat m_textureRight;  // 0x38
	LegoFloat m_textureBottom; // 0x3c
	LegoFloat m_rotation;      // 0x40
	LegoU32 m_blendMode;       // 0x44
	LegoU32 m_opaque0x48[3];   // 0x48..0x53
};

DECOMP_SIZE_ASSERT(RendererGuiRectangle, 0x54);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_clipLeft) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_color) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_textureIndex) == 0x2c);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_textureLeft) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_rotation) == 0x40);
DECOMP_STATIC_ASSERT(offsetof(RendererGuiRectangle, m_blendMode) == 0x44);

LegoBool32 ClipGuiRectangle(RendererGuiRectangle* p_rectangle);

#endif // LR2_RENDERERQUADDRAW_H
