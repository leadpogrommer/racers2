# F10 screen capture research

The helper at `0x004bb210`, called on the F10 key path in WinMain, captures
an image rather than toggling a window mode. Its full function is 1,059 bytes,
ending at `0x004bb632`; the late branch at `0x004bb626` rejoins the output path.
The capture entry remains research; no incomplete capture body was added.
The adjacent 31-byte directory setter at 0x004bb1f0 is recovered and matches
100%. It copies a CharBuffer128 into global 0x00679ef8 and sets the enable
flag at 0x00679f80 to TRUE. Both globals pass data comparison. The constructor
initializer at 0x004bb1e0 calls the already recovered buffer constructor; its
generated PDB name is not unique across translation units, so it is not
annotated by name.

The function creates an image surface through global interface pointer
`0x0058b8f0` (slot +0x6c), obtains the front buffer into it (slot +0x78), and
locks a rectangle on that surface (slot +0x24, flag 0x10). These contracts and
vtable slots agree with IDirect3DDevice8::CreateImageSurface/GetFrontBuffer
and IDirect3DSurface8::LockRect in the installed Wine Direct3D 8 header at
`/usr/include/wine/windows/d3d8.h`. The surface lock result is a signed pitch
and pixel pointer. The routine releases the surface on the observed failure
and cleanup paths. It should use the SDK interfaces when implemented, rather
than invent a game class for these COM objects.

It uses the window geometry helpers to crop windowed captures, or display
configuration dimensions when the configuration field at +0xc is nonzero.
Source pixels are read as 32-bit words and packed into three output bytes
per pixel, in reverse row order. It creates a BMP file header with signature
0x4d42, pixel offset 54, and a 40-byte information header with one plane,
24-bit pixels, no compression, and resolution values 0x0b12. Writes go through
the already recovered WinFile::WriteExact helper. The routine allocates and
frees the packed buffer with the confirmed CRT malloc/free rather than the
deferred engine pool allocator.

Capture naming uses global buffer `0x00679ef8` and a formatted numeric suffix.
It probes for an unused path, temporarily sets file-manager flag bit 0, opens
with file mode 1, writes the headers and pixels, closes, clears that flag bit,
and frees/unlocks/releases the capture resources. Dependencies still needing
source recovery include the mode/path WinFile constructor/open dispatch,
manager-aware path existence query, Direct3D 8 header/build integration, and
the display-record/window globals. The screenshot path therefore does not
justify replacing the temporary WinMain yet.
