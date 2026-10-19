# Startup movie integration

The canonical 325-unit build links the three startup movie calls, MoviePlayer,
the MPEG DirectShow graph, the game texture renderer and its texture-format
query. Bootstrap remains the entry point. Movie playback and a rendered menu
have not been run; the larger reconstructions remain STUB under review.

The graph uses the genuine SDK interfaces and static BaseClasses library
described in [DirectShow setup](work-directshow.md). Raw stack analysis corrected
the previous automatic graph-rendering reconstruction. It now finds the
splitter's Input/Audio/Video pins, the source's Output pin and the custom
renderer's In pin. The audio renderer's input pin name is obtained by enumerating
its pins and reading PIN_INFO.achName. Three Connect calls join source to
splitter, splitter audio to audio input, and splitter video to the custom input.
The command-line `fol` option enables skipping; the helper at 4b4a60 is the
already reconstructed command-line search, not COM initialization.

Cleanup releases pin references before removing filters. The custom renderer
starts with zero references in CUnknown, so a successfully added renderer is
destroyed through RemoveFilter, without an extra direct Release. The source
is released after removal; the audio/splitter CoCreate references are released
separately. Additional failure guards and resource-creation ordering have not
been certified identical to every retail failure path.

The player wrapper now clears/presents four frames before and after playback,
unbinds the mapped texture, and enables MemoryHeap's CRT allocation mode only
around the graph lifetime. The receiver-only 4a4530 helper performs two passes
of local address arithmetic without dereferencing memory or changing the heap.
Its source is retained without instruction polishing. Its provenance remains
unclassified; it is not a replacement allocator implementation.

Raw review fixed pixel conversion that previously skipped 24-bit samples on
32-bit destinations. The branch now selects by destination format, advances
the source by its actual bytes per pixel, uses clipped row widths for pitch
skips, and maps BGR bytes to the corresponding color channels for 16-bit output.
The 32-bit tail preserves retail's preceding-byte alpha construction. Tiny-tile
and invalid-media edge cases remain uncertified; the large render callback's
low measured score does not establish equivalence.

The new static checker verifies every MoviePlayer slot and the three game-owned
SDK overrides: SetMediaType at +90, DoRenderSample at +ac, CheckMediaType at +b0.
It uses the complete decorated CUnknown table name to distinguish the class's
five interface tables. Inherited SDK method bodies are outside that pointer
check. The derived class size assertion is 278 hexadecimal bytes, with a 148
hexadecimal-byte SDK base prefix.

CMake now puts project include paths before external SDK paths, matching the
Python builder. This prevents the SDK d3dx8.h from shadowing util/d3dx8.h and
bypassing the existing Wine/VC6 bridge.

Both builders agree on 1,475 measured functions: 445 exact FUNCTION claims,
nineteen reviewed near matches and 1,011 STUB bodies. The texture-format query
matches exactly. Two complete folded leaves are now explicitly verified: the
return-zero/RET4 collision predicate and the one-byte movie exit callback.
The SDK link otherwise displaced the collision predicate's ordinary report
entry. Their entire bytes and exact map symbols pass in both PEs; eleven
folded aliases are now verified. Data comparison checks 256 globals with zero
issues, and all twelve dispatch checkers pass. No game behavioral tests or
runtime probes were added.
