# Game frame rendering

`GameManager::RenderFrame` is the renderer-facing half of the frame loop at
`0x004aa8e0` (2,264 bytes through `ret 4`). The body is in
`src/gamerenderorchestration.cpp` and remains `STUB` while its context-owned
shadow state and broader frame dependencies are unresolved.

The disassembly refreshes object lifecycle hooks, profiles scene start and
rendering, updates components, and draws type-3 objects only when scene begin
succeeds. Objects with a nonzero field at `+0x1cc` take an additional pass:
all three draw lookup lists are cleared, type-2 objects receive the current
object position, and the renderer may draw one selected secondary object
under depth-write/stencil state. That path then draws the remaining secondary
objects and flushes lookup lists 1, 2, and 0. The zero-`+0x1cc` branch skips
that list pass. Both branches meet at the renderer callback at dispatch offset
`+0x18`, then conditionally submit a overlay pass and the inline diagnostic
overlay. The overlay block is now written inline in `RenderFrame`; it reuses
the retail stack arrangement for two 0x54-byte rectangles and a 0x28-byte
defaults record. The rectangle/default storage overlaps at one DWORD, matching
the observed stack addresses. Unwritten stack words are zeroed in the source,
so that part still needs byte-level review before the frame function is treated
as behaviorally complete. A later pass prepares remaining type-3 objects and
submits their shadow work. Finally, the method closes profiling, ends the scene, and presents.
It then checks renderer predicate slot `+0xa8`, requests device resource mode 7
when that predicate is false, and calls audio frame cleanup. The fallback
condition does not use the `Present` return value.

The shadow callback at `0x004b3540` (827 bytes through `ret 4`) receives the
distinct context at `0x00655228`; it is not the global pause controller. Its
body is in `src/globaloverlaymanager.cpp`. `globaloverlaymanager.h` models the observed
16-entry offset array at `+0x24900`, count at `+0x24940`, and list-head table
at `+0x6acdc`. The list scans stop after 300 and 200 nodes respectively. The
storage owner and full object size remain unresolved, so the context global is
still an external dependency.

Frame audio cleanup at `0x004e8830` (87 bytes through `ret`) is implemented in
`src/gamerenderaudio.cpp`. Its observed 0x20-byte records and source/destination
arrays map to zero-initialized BSS. The final pointer chain through
`0x006844d4` remains modeled with opaque pointers because its owner is not
identified.

No full worker build was run after these additions. Other lifecycle, object,
and context-owner dependencies still prevent linking this frame path. No
runtime or game behavior was tested.
