# Menu widgets and text append

Eight six-slot widget tables now link: the common base, volume control, range
slider, two-choice selector, integer selector, toggle, label selector, and text
group. `check_menu_dispatch.py` checks every original target and all 48 emitted
callback pointers. Function names are descriptive reconstructions.

The 0x38-byte base owns two arrow sprites, font and bounds. It is not a
GameObject. The navigation receiver is SecondaryManager +0x7c; its separate
pointer record supplies coordinates and a five-word texture region. Keyboard
and mouse helpers preserve the evidenced thiscall stack arguments. Slider
callbacks retain dragging, snapping and display smoothing. Selector callbacks
retain wrapping, output result codes, persistent hover state and text colors.

Raw review corrected the 426xxx control to a MenuTextGroup containing six
GlobalOverlayRecord pointers. Those callbacks call text release, text clipping
and text bounds helpers, rather than similarly shaped sprite methods. The
choice selector's selected color index and stored hovered option were also
checked against the original arithmetic and output pointer stores before
integration. Mutable widget tuning and colors have actual startup initializers.

The text append path converts narrow bytes, restores glyph keywords before
concatenation, optionally inserts a space and reinitializes overlay text with
its existing animation flags/time. Its three shared scratch regions are
separate: append conversion at 654c20, text initialization at 654e20, and append
assembly at 655020. Each reserves the observed available 0x200-byte interval;
the original declarations' capacities remain unproven and copies remain
unchecked. Discovery of the third scratch corrected the previous oversized
reservation at 654e20. Overlay scale updates also preserve the retail promoted
float threshold and unordered FCOMP branch behavior.

Both builders verify 1,239 measured functions: 374 exact FUNCTION claims,
sixteen reviewed near matches, and 849 STUB reconstructions. Twenty-five new
exact claims include font setters, resets, clipping, tuning initialization,
narrow append and glyph keyword access. The two ten-byte font setters genuinely
fold; complete original/emitted bytes and exact linker-map symbols are checked
alongside the six earlier aliases. Larger callbacks remain STUB at their raw
scores; runtime behavior has not been validated.

Both 287-unit builds pass. Data comparison verifies 228 annotated globals with
zero issues, all ten dispatch inventories pass in each build, and the existing
21 tooling/library-identification tests pass. The Python builder now uses a
VC6 linker response file: the isolated build's direct command reached 33,385
characters and failed before producing diagnostics. The response file preserves
the same options and safely quotes paths containing spaces.

Menu resource orchestration and the remaining frontend controls are still being
connected. The bootstrap remains active; no game, startup, asset, or menu probe
was executed for this batch.
