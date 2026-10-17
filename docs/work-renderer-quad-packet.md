# Renderer quad packet drawing

`DisplayRenderer::DrawQuadPacket` begins at `0x004e0db0`, uses one stack
argument, and returns with `ret 4`. The argument is a tagged variable-size
packet. The raw switch has eight accepted tag values and several layouts; the
header is not modeled as one fixed-size type.

The source now covers all eight accepted tags. Tags 0 and 2 draw one or two
triangle-strip primitives from 24-byte packet vertices. Tags 1 and 3 use
32-byte vertices and either bind two indexed textures or iterate the packet's
default-stage subrecords. Tags 4 and 6 build 28-byte scratch vertices from
packet values; tags 5 and 7 build 36-byte vertices and use either two mapped
textures or the same stage-subrecord loop. The signed 16-bit texture indices
in the latter packet families are sign-extended before indexing the renderer
map. The shared scratch arrays and cleanup-registration flags are mapped to
their observed data addresses.

This reconstruction remains `STUB`: the generated depth values use ordinary
float expressions, and their exact x87 intermediate rounding has not been
verified against the retail instruction sequence. The function body extends
through `0x004e1839` (`ret 4`); the jump table at `0x004e183c` is not part of
the function. No game or runtime test was run.
