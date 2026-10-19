# Foyer model base and preview drawing

The canonical build now links the model base constructor at 0x4bce10,
destructor at 0x4bcea0, initialization at 0x4bced0, motion/force bridges,
lifecycle adjustor callbacks and region-key update. The constructor passes the
embedded physical state at object +0x58 into the original GameObject constructor,
constructs that state from the 0xf0-byte model template and binds its model with
the original orientation, position and child flag. A shared helper supports
already-constructed C++ derived bases without registering the object twice.
Destruction releases the model binding, resets the physical base dispatch and
runs the original object destructor. Removal reuses the existing canonical
0x4bcef0 implementation, now exported from the car-display unit.

Both complete ABI tables are inventoried: twenty physical slots at 0x536264 and
twenty-one object slots at 0x5362b4. They retain real transform getters/setters,
indexed property copying, physical integration and deleting destructor bridges.
They replace the staged partial table whose first twenty bytes were zero and
whose setters wrote unrelated owner fields. The original callbacks keep their
actual receivers and stack cleanup; no VTABLE/GLOBAL annotation is applied to
these explicit pointer tables. Both PEs and exact linker maps verify every slot.

The shared preview drawing helper at 0x4bcf60 retains its view filter, visibility
test, optional override, draw callback and visibility-only fallback. Its
carry-only comparison accepts unordered overrides as retail does. The fallback
at 0x4a9cb0 computes projection depth and updates model visibility without drawing.
These are available to the character/Foyer derived tables being recovered.

The deleting destructor at 0x473b10 is a reviewed 95.65% near match: the only
instruction difference is clearing EDX before a bridge whose second argument is
unused. All branches, optional free, return pointer and RET 4 match. Other new
functions remain STUB with raw measurements visible. Existing BeginMenuManagerMode
0x41dd50 folds out of the measurable STUB subset under the changed link layout.

A separate raw-backed correction reads bonus progress from the second word of
the progress record, matching the original [ECX+4] load. The threshold type and
existing table declaration are shared with the pending menu initialization work.
No storage was duplicated and existing FUNCTION claims remain verified.

Both pinned VC6 builders link 382 units and agree on all 1,846 measured scores
and statuses: 532 exact, twenty-seven reviewed near matches and 1,287 STUB rows.
Both data comparisons report zero issues across 297 globals. All twelve dispatch
verifiers pass, including 562 scene pointers; seventeen complete folded aliases
and twenty-one tooling tests pass. Twelve new function extents were checked
against raw control flow. No game behavior tests, game runs or asset probes were
performed. Higher-level Foyer/character and Secondary preview tables, plus the
actual entry/main loop, still require integration. The menu has not launched.

Logs: build/foyer-model-base-final-build.log,
build/foyer-model-base-final-compare.log,
build/reports/foyer-model-base-datacmp.txt and corresponding CMake paths;
build/foyer-model-base-dispatch.log and
build/foyer-model-base-tooling-tests.log.
