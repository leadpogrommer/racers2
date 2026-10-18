# Frame and frontend dependency integration

The Python and CMake builds now link the real message-loop implementation,
frame orchestration, GameObject lifecycle/render preparation, global pause,
contact-cache ownership, collision queries, RenderFrame, global overlay and
Menu Manager, audio service/providers/music support, and bitmap font loading.
The temporary entry point remains active. This is a static integration
checkpoint; the game has not opened a window or rendered its menu.

Two runtime contracts were corrected during integration. WinFile and
FileManager reads return the original success value, needed by audio channel
loading. The localization pointer at 0x679b70 is GlobalOverlayManager+0x24948,
not independent BSS storage. Its setter at 0x4b3880 and text lookup accessor now
share the manager field. Defining a separate pointer would leave indexed
frontend labels dereferencing null after initialization.

The audio restoration path now reloads channel files, rebuilds active provider
buffers and source duplicates, and restores digital music/commentary buffers.
Frame processing handles source fades, listener selection, distance culling,
priority selection and playback updates. Original DirectSound/WinMM interfaces
are used. The quality-mode shutdown condition, CD-player close command, mixer
source selection, provider destruction transitions and pure-call slots were
corrected from the original instructions/table bytes. The six complete audio
tables have 144 independently checked original targets/emitted symbol pointers
in `tools/check_audio_dispatch.py` and `reccmp/audio-dispatch.csv`. As with the
application tables, ABI adapters and their original methods are separate
functions; the table check does not establish behavioral equivalence.

The font path loads the TGA, scans transparency bounds, packs glyphs into
pages, creates/uploads textures and assigns glyph UVs. The font-set owner
loads two profiles and copies derived profiles. It remains STUB: compilation
and static recovery have not established rendered glyph equivalence.

Collision queries now cover dynamic broad phase, grid ray traversal, proxy
transforms, sphere rays, indexed tree candidates and terrain cell rays. Their
larger floating-point branches and encoded contact paths remain under review.
See [collision query notes](work-collision-queries.md) and
[font notes](work-bitmapfont.md).

The concrete remaining menu path is GameController initialization/update and
its scene-manager constructors, renderer callbacks, Secondary Manager's
media/input transition, and startup movie support. Those sources are staged
until their actual dependencies close.
Runtime testing remains reserved for a build credibly expected to render the
menu. Existing tooling/library-identification tests are retained; no behavioral
tests were added for recovered game functions.

Ten exact helpers were promoted after comparing both builders and checking
complete retail extents: terrain quad/cell dispatch (4d6050/4d60e0), dynamic
segment iteration (507e60), tree queue advancement (50a890), localization setter
(4b3880), audio vector length (4af2b0), named audio-object lookup/presence
(4f11a0/4f11c0), digital gain retrieval (4ef5a0), and font-set construction
(42ce60). Larger callers remain STUB where their semantics are unresolved.
