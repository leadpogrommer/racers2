# Renderer bump-map vectors

`DisplayRenderer::BuildBumpMapVectors` is the implementation at `0x004de460`
(1327 bytes, ending with `ret 4` at `0x004de98c`). The renderer dispatch calls
it with a request pointer. Its request prefix is physically shared with the
other renderer stage/draw request view: face count at `+0`, vertex count at
`+2`, cached output at `+8`, vertex-buffer object at `+0x0c`, index-buffer
object at `+0x10`, and the default-operation record at `+0x14`.

The function returns immediately when the selected adapter advertises
`D3DTEXOPCAPS_DOTPRODUCT3` (`TextureOpCaps` bit `0x00800000`), when the index
buffer's selector at `+0x0c` is nonzero, or when the request already has a
cached output pointer. Otherwise it allocates `vertexCount * 24` bytes under
the `Bump map vectors` tag and stores the allocation at `+8`; it separately
allocates temporary `faceCount * 24` bytes from the heap's other end using
the `FaceDirections` tag.

For each triangle, the routine reads three 16-bit indices from
`RendererIndexBuffer::m_data`, obtains the indexed vertex positions and the
texture coordinates for the first default slot whose value is zero (or stage
15 when none exists), then computes and conditionally normalizes a tangent
and bitangent. The normalization threshold is the literal `0.0001f`; vectors
at or below that magnitude are left unchanged. It stores temporary tangent
and bitangent arrays, then for each vertex sums the vectors for every matching
triangle corner and writes two `Vector3` values (24 bytes) to the cached
output. The per-vertex pass rereads the index buffer's `m_data` pointer on
each vertex, as the retail body does. The temporary block is freed; the cached
output remains owned by the request.

The isolated raw instruction score is 9.51%. The implementation remains
`STUB`, not a behaviorally accepted near-match. Its arithmetic and allocation
order follow the recovered control flow, but the compiler's x87
spill/rounding behavior and callback mutation effects have not been validated
sufficiently to call the body equivalent. The mesh/index selector is
preserved as the existing index-buffer field `m_unk0x0c`; its semantic name
remains unresolved.
