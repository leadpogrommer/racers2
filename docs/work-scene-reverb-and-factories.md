# Scene reverb and named object factories

The canonical build now links ReverbManager, terrain surface projection, camera
queries and the named-object registry with five real asset-class registrations.
Controller/Foyer startup and the real entry point remain staged; this build has
not launched a window or rendered a menu.

## Environment audio

The 0xbc-byte manager stores eight volume pointers, two selected volumes,
previous camera positions and camera pointers. Reset preserves the original
integer default of one, dirty flag and eight-update delay. Configuration captures
the active cameras. Update retains the controller-state gate, delay, large-move
rescan and segment-crossing path. Selection/reset sends the original parameter
pair through the real AudioService dispatch, preserving first-camera, force and
session-state rules. Disable/restore and the shared dirty setter also have source.

Volume lookup uses terrain projection and the original underground comparison.
The terrain helper transforms world/local coordinates, finds grid cells with
current-rounding-mode FISTP, selects the correct height-field triangle and
returns world-space surface/normal data. Its fallback and early-return paths
remain explicit. Camera queries walk the sorted GameObject registry with the
original type, inactive and enabled checks.

The named reverb factories construct real 0x8c-byte areas, 0xa8-byte gateways
and 0x58-byte initializer objects. Areas register with the manager; gateways
build quaternion-derived axes, ordered corners, reversed normals and squared
radii, then attach through the typed name/type-mask lookup. Initialization and
removal configure/reset the real manager/audio state. Review corrected two
cdecl callbacks that had been cast to fastcall and restored the original corner
addition/subtraction order. All callbacks now use the evidenced ABIs. Larger
geometry and lifecycle routines remain STUB pending further review.

## Named-object registry

Registration inserts class-name entries in case-insensitive order. Lookup retains
the retail unsigned binary-search bounds and midpoint behavior, including its
empty-registry assumption. The comparison uses the already identified VC6 CRT
`__stricmp` at 0x524eb0. Construction dispatches to rebuilt factory callbacks and
uses the original fatal-diagnostic path for unknown classes when requested.
Factory entry storage includes the full four-slot abstract dispatch table with
the actual CRT pure-call callback.

Five registrations now link: cSurfaceManager, cRaceStartPos, cReverbArea,
cReverbGateway and cReverbInit. The race-start factory preserves its template
flag, 0x74 allocation, position/quaternion fields and complete 21-slot derived
and intermediate-base tables, including both allocation-size callbacks.
The surface factory connects the previously recovered preview record owner.

A raw scan finds 76 calls to the factory-entry constructor; its ignored working
inventory is `data/factory-registration-inventory.csv`. The remaining asset
registrations are not provided by placeholder callbacks. Some other creator
bodies, such as spline factories, have source but still await registration.
This is an incomplete factory set, not a complete scene-loading path.

## Validation

Both pinned SP5 builds link 370 units and agree on all 1,738 measured scores and
statuses: 512 exact functions, twenty-one reviewed near matches and 1,205 STUB
rows. Thirty-seven new function extents were checked against raw control flow;
eight exact helpers were promoted. The larger routines remain STUB.
All twelve dispatch verifiers pass in both PEs. The scene/model inventory now
checks 460 pointers, including all factory/object tables above; menu widgets
remain at 84 pointers. Twelve complete folded aliases pass. Data comparison
reports zero issues across 281 globals, including the named registration
objects and integer default at 0x54225c.

Logs: `build/reverb-factory-build.log`,
`build/reverb-factory-final-compare.log`,
`build/reports/reverb-factory-datacmp.txt` and corresponding CMake paths;
`build/reverb-factory-dispatch.log` and `build/reverb-factory-tooltests.log`.
No game behavior tests, runtime runs or asset probes were added or performed.
Unlinked worker annotations can produce missing-symbol notices outside the
canonical measured count.
