# Screenshot capture

The real entry point calls 0x004bb210 on F10. The new source captures the desktop front buffer into a real Direct3D8 image surface, locks the selected rectangle, converts BGRA rows to bottom-up 24-bit BMP bytes, searches numbered Photo paths, writes via WinFile and restores the file-manager override flag. It uses genuine SDK interfaces and the existing renderer storage.

The retail function ignores CreateImageSurface/GetFrontBuffer failures, masks display/window dimensions to sixteen bits and writes unpadded three-byte rows. Those policies remain. The filename counter starts with the crop-top value held in EBX and stops after number 999. The full extent is 0x004bb210 through the final branch at 0x004bb632, including code after the first return.

The original compiler reuses several stack locations across file-header construction, path iteration and cleanup. Their surface-pointer aliasing at final UnlockRect remains under review; the source retains a typed pointer to the locked surface. CaptureScreenshot remains STUB and has no behavioral-equivalence claim. No screenshot, game or startup probe was run.
