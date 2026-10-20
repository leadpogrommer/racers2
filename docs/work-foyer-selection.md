# Foyer progress and scene selection

The canonical build now links the progress masks and counters needed by the
Foyer manager, its scene-selection queries, proximity/transition predicates and
NPC text cleanup. Full manager transitions and controller startup remain staged;
bootstrap is active and the menu has not launched.

## Recovered behavior

Track and bonus counters retain five worlds, four tracks and three bonuses per
world. Mode fields use the original five-bit stride and 1→0, 2→2, 3→1 mapping.
Scene selection reads the session state and the actual Foyer manager's +0xe4
state, then preserves the special mode-zero/mode-four decisions. Review corrected
an incorrect settings-object receiver for that gate and a call that bypassed
the special-mode helper.

The five threshold triplets at 0x52f9d0 are complete rebuilt storage, with the
verified twelve-byte row stride. Queries preserve strict threshold comparisons,
selection flag writes and scene indices. Progress fraction retains the 0.08 and
0.2 multipliers read from the PE. Other helpers retain the radius-squared
proximity test, transition states one through four and NPC textbox cleanup.
Names describe inferred behavior rather than recovered original symbols.

IsFoyerModeSelected (0x411c80) is a reviewed near match at 96%. Its only extra
instruction clears unused EDX before the mapped-progress helper. Branches,
receivers, mode-zero and mode-four results, return values and cleanup match.
It uses FUNCTION; the other new bodies remain STUB below the initial 95%
threshold. Instruction differences are retained without successive polishing.

## Validation

Both pinned SP5 builds link 372 units and agree on all 1,758 measured scores and
statuses: 512 exact functions, twenty-two reviewed near matches and 1,224 STUB
rows. Nineteen new function extents agree with raw control flow and Ghidra bodies.
The changed link layout also makes an existing spatial-grid initializer
measurable. All twelve dispatch verifiers and twelve complete folded aliases
pass in both PEs. Data comparison reports zero issues across 282 globals,
including all five threshold rows. Tooling tests were unchanged and passed in
the preceding reverb/factory checkpoint. No game behavior tests, game runs or
asset probes were added or performed.

Logs: `build/msvc/foyer-selection-build.log`,
`build/msvc/foyer-selection-final-compare.log`,
`build/msvc/reports/foyer-selection-datacmp.txt` and corresponding CMake paths;
`build/msvc/foyer-selection-dispatch.log`. Unlinked worker annotations can produce
missing-symbol notices outside the canonical measured count.
