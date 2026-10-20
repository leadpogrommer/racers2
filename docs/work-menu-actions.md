# Menu action and settings recovery

Two canonical units connect menu action-stack changes, choice labels and pointer
hover, value queries, display-mode fallback, language selection and graphics
quality updates. All 23 recovered extents were checked against raw control flow
and Ghidra bodies. One entry folds with an existing implementation, producing
22 additional measured rows. Names describe inferred behavior.

Review corrected the thirteen-word overlay allocation parameters, label ordering,
centered text geometry and separate header positions. The widget callback remains
a virtual dispatch. Settings sliders preserve the original float store and FISTP
rounding. Display fallback saves the old mode before attempting reconfiguration;
a successful request persists its mode through the application callback. The menu
action stack reloads its depth after callbacks that can change it.

Graphics quality passes the first quality value to the model clip-plane helper,
uses signed comparisons and the original positive 2^31 LOD threshold. Language
selection calls the localized string table and refreshes player names. These
larger reconstructions remain STUB pending further semantic/matching review.
UpdateMenuChoiceSelectorHover (0x41b6f0) is an exact FUNCTION match.

Both pinned VC6 builders link 374 source units and agree on all 1,780 measured
scores and statuses: 513 exact functions, twenty-two reviewed near matches and
1,245 STUB rows. Data comparison reports zero issues across 282 globals in both
PEs. All twelve dispatch verifiers and twelve complete folded aliases pass.
Tooling tests were unchanged and passed at the preceding reverb checkpoint.
No game behavior tests, runtime runs or asset probes were performed.

Logs: build/msvc/menu-actions-build.log, build/msvc/menu-actions-final-compare.log,
build/msvc/reports/menu-actions-datacmp.txt and corresponding CMake paths;
build/msvc/menu-actions-dispatch.log. Unlinked worker annotations may emit missing
symbols outside the measured canonical subset. The temporary bootstrap remains
active; a rendered menu still requires the Foyer and controller startup closures.
