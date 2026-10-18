# Renderer resource shutdown

`DisplayRenderer::ShutdownResources` at `0x004be330` spans 299 bytes, ending
at the return at `0x004be45a`. It is the renderer side of the platform shutdown
sequence. It visits all 800 texture records through virtual slot `+0x114`.
Live referenced textures are released through the reference manager; live
unreferenced textures receive their own `+0x0c` release callback and have their
in-use bit cleared. The method then releases each of the 125 auxiliary records,
invokes renderer shutdown through slot `+4`, frees the work-list storage, and
releases deferred, ordered and bucketed lookup arrays in that order. Finally it
frees the auxiliary pointer lists, animation-cache array, resource-reference
array and auxiliary-material array. It preserves retail's pointer re-reads
around callbacks and its lack of owner-field clearing after deallocation.

`ReleaseAuxiliaryResource` at `0x004c1050` spans 120 bytes through the `ret 4`
at `0x004c10c5`. It ignores index -1 or an inactive record, decrements the
16-bit reference count, and only clears the in-use byte when that count reaches
zero. Records with a nonempty path are removed from the animated-material list
by swapping in its last element. No extra index or underflow checks are added.

The integrated Python and CMake builds measure these methods at 61.24% and
45.65% raw similarity. Both retain STUB status; successful compilation does
not establish complete semantic equivalence. No game or shutdown probe ran.
