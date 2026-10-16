# Deferred allocator research

These are exploratory reconstructions with unresolved provenance, not sources
in the main game build. See [library identification](../../docs/library-identification.md).

The default SP5 experiments measured allocation at `0x004a41d0` around 67.61%,
freeing at `0x004a4350` around 81.30%, cursor adjustment at `0x004a4480` around
95.83%, and forward search at `0x004a44c0` around 73.68%. Backward search,
header initialization and the flag setter had individual exact matches. None
are included in the main project's reported reconstructed-function count.

`/O1` and `/O2 /Os` experiments made the two searches exact but changed header
initialization and other code. They did not establish original compiler flags;
the main build settings were left unchanged.

Optional host behavioral checks were removed. This deferred source is not
included in the game build or progress counts.
