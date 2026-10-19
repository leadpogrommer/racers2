# Frontend selection and preview dependencies

The 313-unit combined build links eleven complete six-slot widget tables:
the previous eight controls, the track selector, network status widget and
character preview selector. All 66 original targets and emitted pointers pass
the independent menu-dispatch check. Initializers and menu orchestration are
still being connected; this is not a rendered-menu milestone.

The track selector now uses the catalog at session+1a6c, with mode/index/number
arrays at 1a78/1aa0/1ac8. Raw review corrected a localized-name index decrement,
a spurious Y offset and an initializer calling the inactive reset instead of
419740's active-state setup. Its twelve function extents were checked by CFG
and Ghidra, including 416b90=524 and 416fe0=37 bytes.

Network status uses float-one bit patterns in overlay descriptors, separately
verified BSS layout offsets, the correct label records, correct measured-width
arithmetic and the text's first UTF16 word for dash detection. The lobby status
helper returns its text pointer in EAX as well as writing readiness; the prior
void projection omitted that contract. Its scalar getters/reset/tuning match
exactly; larger routines remain STUB at their measured scores.

Character preview component indices select head model, face group, torso and
leg textures. The bodypart getters, model slot-index remapping, shared camera
leases, viewport updates, quaternion look-at path, actual D3D8 light upload and
light release now link. Light properties preserve retail RGB-only copying into
a zeroed D3DLIGHT8, and ambient RGBA uses the exact 1/255 float scale. The
56ba28..56bafc tuning interval has typed fields and thirteen real initializers.
Larger math, rendering and callback bodies remain STUB; rounding/aliasing and
remaining instruction differences have not been certified equivalent.

The character's fixed hidden-preview call inlines the complete effect of
44f6e0(5): freeze body/head, clear the embedded transform bit, enter state5 and
detach collision. This helper has no retail-address claim and does not pretend
to reconstruct the generic racing-mode dispatcher. Release also disables the
animation transform through the real 4c26e0 path. Startup idle preparation and full widget initialization were subsequently linked;
see [foyer event and initialization notes](work-foyer-events.md).

ReleaseMenuPreviewLight (429320, 95.45%) is a reviewed equivalent: retail checks
index<3 a second time after a matching element was found in the bounded 0..2
scan. The compiler removes that redundant comparison; key clearing, caller
pointer clearing, light disable and all exits agree. The genuine RET4 diagnostic
callback now folds into another emitted body; its complete three bytes and exact
linker-map symbol are verified in both PEs, bringing the alias inventory to nine.

Both builders agree on 1,441 measured functions: 434 exact FUNCTION claims,
nineteen reviewed near matches and 988 STUB bodies. Both data reports verify
252 globals with zero issues. All eleven dispatch inventories and the existing
21 tooling/library tests pass. No game, startup, asset or menu probe was run;
bootstrap remains the entry point and the menu has not launched.
