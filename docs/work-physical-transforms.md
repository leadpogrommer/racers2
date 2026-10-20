# Physical parent transforms and preview timing

The canonical build now links physical lifecycle position/orientation getters,
setters, parent-relative conversions, captured-transform access and motion
clearing. The existing 0x128-byte state now names its saved position at +0x5c,
saved orientation at +0x68 and parameter pointer at +0x78, with layout assertions.
Parent callbacks retain their actual receiver and stack arguments. World getters
retain the shared static result storage, three initialization flags and empty
exit callbacks. This storage can be overwritten by later getter calls, as in
retail; it is not replaced by newly allocated objects or per-instance caches.

Nine callbacks match exactly. The world rotation getter is a reviewed 95% near
match: its pushed exit callback is a different linker alias of the same verified
one-byte RET body. The world orientation getter is a reviewed 95.08% near match:
the same alias difference and an extra clear of unused EDX account for the diff.
All three empty exit callbacks have complete-byte and exact linker-map checks.

The step helper at 0x4a6020 stores its input and its quotient by 331 in the
original separate globals. Review caught VC6 replacing literal division with
multiplication by a rounded reciprocal; a volatile constant now retains division.
The emitted operand load/divide order still differs, so the helper remains STUB
at 66.67%. Both PEs have the exact original 331.0f storage bytes verified
separately. The preview animation scale at 0x579128 is 30 / signed frame rate,
initialized after the existing timing values. Its initializer is an exact match.
This scale is separate from the movement simulation step at 0x5790d4.

The 0x460680 skeleton-record constructor is now a verified folded FUNCTION.
Its complete three-byte body is identical to the existing GameControllerItem
initializer at 0x409190. Both aliases are inventoried, preventing link folding
from incorrectly dropping the existing implementation claim. An existing menu
mode helper also becomes measurable with the changed link layout. Other new
bodies remain STUB and retain their actual scores without register polishing.

Both pinned VC6 builders link 379 units and agree on all 1,829 measured scores
and statuses: 531 exact, twenty-six reviewed near matches and 1,272 STUB rows.
Data comparison reports zero issues across 296 globals in both builds. All
twelve dispatch verifiers, seventeen complete folded aliases and twenty-one
tooling tests pass. Eighteen new function extents and three exit callback extents
were checked against raw control flow. No game behavior tests, game runs or
asset probes were added or performed. The original entry/main loop and remaining
Foyer/model/SecondaryManager closures still require integration; no menu launches.

Logs: build/msvc/physical-transforms-build.log,
build/msvc/physical-transforms-final-compare.log,
build/msvc/reports/physical-transforms-datacmp.txt and corresponding CMake paths;
build/msvc/physical-transforms-dispatch.log, build/msvc/physical-divisor-verification.log,
and build/msvc/physical-transforms-tooling-tests.log.
