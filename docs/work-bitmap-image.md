# Bitmap image storage and TGA loading

`BitmapImage::ReleaseStorage` at `0x004f59c0` frees the pixel and palette
buffers through `MemoryHeap::Free`, then clears each pointer. The function
measures 95.24% raw; its only unmatched bytes are the retail `mov`/`cmp` of
`m_mipLevels` before the first null check. Those instructions only set flags,
which are immediately replaced by the pointer test, so they do not change
behavior. It is retained as a near-exact `FUNCTION` under the project's
reviewed-equivalence threshold.

`BitmapImage::LoadTga` at `0x004f54a0` remains `STUB`. The current source
recovers its file/header path, dimensions, metadata, palette and pixel
allocation, and short-read clamping, but raw matching is still low. Known
retail details preserved in the source include the 20-byte `AIF1` image-ID
metadata block, defaults `[AIF1, 1, 0, 1, 0]`, checking the image-ID length
byte for 20, `AllocateFromEnd` for image buffers, and allocating four bytes
per direct-color pixel while reading only `pixelDepth >> 3` bytes per pixel.
The method takes a second stack argument ignored by its body; its function
extent includes the `ret 8`.
