# Ogg/Vorbis source

The audio commentary decoder reuses the Xiphophorus BSD-licensed reference implementation. Sources and notices are preserved from the official releases:

- [libogg 1.0rc2](https://downloads.xiph.org/releases/ogg/libogg-1.0rc2.tar.gz), SHA-256 `c5f5924f25402a59a2586c3d037d3e79dae97de30531b8dd8b8b4abc20d5f036`.
- [libvorbis 1.0rc2](https://downloads.xiph.org/releases/vorbis/libvorbis-1.0rc2.tar.gz), SHA-256 `bc9ddf648ae79ef65cff6199f998e65fd8c8f9dbe92e96731cb52ec2cfcbe5ce`.

This subset contains the core library compilation units and their headers, with upstream license files. Command-line examples, packaging files, encoder preset books, libvorbisenc and libvorbisfile are omitted. No upstream source code was changed. `iir.c` and `lookup.c` are also included by other upstream units for inlining, as in the original build.

The selected source produces many relocation-masked matches to the retail decoder with VC6 SP5. This establishes the library family and compatible source; it does not establish the exact original release, compiler flags, or byte identity for every upstream function. See [decoder identification](../docs/work-xiph-decoder.md).
