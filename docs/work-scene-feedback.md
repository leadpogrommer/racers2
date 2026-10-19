# Controller feedback and pooled scene effects

Two canonical units recover the 0x84 controller feedback object and the 0xac
scene circle pool. The feedback singleton constructs and virtually initializes
three sprites, a looping audio source and a textured pooled record. Removal
releases those resources; reset reloads the singleton after its Remove callback
before deciding whether to destroy it. Startup definitions cover the complete
0x118-byte audio definition, object template and angular frequency.

The pulse update retains the x87 FSIN path, bitwise absolute value and wrap
branches. Review corrected inline assembly that read local pointer storage
instead of the pointed-to elapsed/pulse floats. Allocation retains the retail
conditional construction followed by unconditional virtual initialization.

The pool owns 4,000 linked records of 0x68 bytes, an active list and a free list.
Its update forwards each child callback and returns one with one stack argument.
The random-vector chain dereferences the controller's +0xa4 owner before +0x5c.
FreeRecord preserves its null guard and list rewiring. Construction only writes
the evidenced fields; the compiler emits normal global initialization and atexit
destruction, so a second explicit exit callback would be incorrect.

Four pool methods match exactly: Remove, GetAllocationSize, AddObject and
RemoveObject. DestroyControllerSceneObject is a reviewed 95.65% near match; its
only extra instruction clears unused EDX before the destructor helper. FreeRecord
is a reviewed 95% near match: clearing the record's previous link is scheduled
before the free-head load. Pool records come from separate allocated storage,
so this reordering cannot alias the manager's free-head field. Both use FUNCTION;
other bodies remain STUB, with their actual scores visible.

Both pinned VC6 builds link 376 units and agree on all 1,803 measured raw scores
and statuses: 517 exact, twenty-four reviewed near matches and 1,262 STUB rows.
Twenty-four function extents were checked against raw control flow; Ghidra also
agrees where its automatic function body is present. One existing folded mapping
changes which annotated body is measurable. The complete 20/21-slot feedback and
pool dispatch tables are checked against both PEs and exact linker-map symbols.
The scene inventory now covers 501 pointers. All twelve dispatch verifiers and
twelve complete folded aliases pass in both builds. Data comparison reports zero
issues across 287 globals; all twenty-one tooling tests pass.

Logs: build/scene-feedback-build.log, build/scene-feedback-final-compare.log,
build/reports/scene-feedback-datacmp.txt and corresponding CMake paths;
build/scene-feedback-dispatch.log and build/scene-feedback-tooling-tests.log.
No game behavior tests, runtime runs or asset probes were added or performed.
Bootstrap remains active; menu startup still requires the Foyer and controller
manager closures.
