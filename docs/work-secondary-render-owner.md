# Secondary preview owner and camera modes

The canonical build now includes the 0x350-byte preview owner constructed at
0x4b9e40, its physical state, view geometry, viewport lifecycle and five camera
modes. The complete twenty-slot physical table at 0x536000 and twenty-seven-slot
object table at 0x536050 are inventoried and verified against both PEs and linker
maps. The final six object slots include follow-mode queries and offset callbacks;
the table does not stop at the allocation-size callback. Its concrete 0x68-byte
creation record and 0x350-byte state have compile-time layout checks.

The constructor uses type 0x300, matching the original enum-wrapper shift. It
retains the original partially initialized state and array constructors that do
not clear point storage. Its 0x38-byte physical defaults remain BSS with a CRT
initializer at 0x4b9d50: zero position, identity orientation, friction/response
0.5, property index zero and mass/inertia one. Storage ends before the separate
current-view global at 0x679ef0. Viewport creation uses the actual display config
at 0x65307c and renderer at 0x549dfc. Destruction retains the original release
condition, clears the current view when needed and runs the base destructor.

Camera modes attach, follow, chase, fly and track actual lifecycle transforms.
The follow modes retain virtual setters, target callbacks, look-at/interpolation
and recorded-input access. The fly routine intentionally uses input byte two for
both angle updates: raw stack tracking confirmed that retail computes byte three
but never uses that scaled value. That original behavior is preserved.

Six leaf callbacks match exactly: size, category, zero-float query, removal and
the two offset getters. The deleting destructor is a reviewed 95.65% near match;
its only difference clears unused EDX before the destructor bridge. Other owner
bodies remain STUB with actual scores visible. The single-record skeleton
transform at 0x4f61a0 also links for the pending Foyer look-at path, retaining root
and parent-relative position handling and quaternion multiplication.

Raw review corrected all four orthographic side-normal initializations in the
existing view-geometry helper. Menu-choice and forwarding arguments now use the
actual position-first six-argument order at 0x423640/0x427e40. The existing slider
marker storage is exported for menu widget construction without duplication.
Application::GetDisplaySettings now has a complete four-byte folded alias check:
preview type-address callbacks can otherwise hide this existing implementation
from source-line matching. No game code was changed to defeat legitimate folding.

Both pinned VC6 builders link 383 units and agree on all 1,869 measured scores
and statuses: 538 exact, twenty-eight reviewed near matches and 1,303 STUB rows.
Both data comparisons report zero issues across 298 globals. All twelve dispatch
verifiers pass, including 609 scene pointers; eighteen complete folded aliases
and twenty-one tooling tests pass. Twenty-two new function extents were checked
against raw control flow. No game behavior tests, game runs or asset probes were
performed. The staged Foyer animation, character/menu/controller initialization
and actual entry path still need combined integration; the menu has not launched.

Logs: build/msvc/secondary-render-owner-final-build.log,
build/msvc/secondary-render-owner-final-compare.log,
build/msvc/reports/secondary-render-owner-datacmp.txt and corresponding CMake paths;
build/msvc/secondary-render-owner-dispatch.log and
build/msvc/secondary-render-owner-tooling-tests.log.
