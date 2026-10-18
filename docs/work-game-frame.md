# Frame loop and startup recovery

The next runtime milestone remains a rendered menu. No executable, startup probe or asset probe was run for this work.

## Linked support

`gameframepreparation.cpp` implements the component slot-0x40 preparation sweep at 0x004e6a00 and the controller frame assignment at 0x004e7ae0. Component preparation snapshots both the list pointer and unsigned count; its callbacks take no stack arguments. The controller frame setter takes one stack pointer with callee cleanup and only writes outside development mode.

`inputcontrollerpause.cpp` recovers 0x004b8f40: entering pause captures all six timing words directly into the manager's 0x6c..0x83 region. Leaving pause computes elapsed absolute time/frame and adjusts the origins, retaining the retail signed microsecond borrow and unsigned carry loops.

`profiler.cpp` recovers configuration at 0x0049adc0. It unconditionally writes disabled=0, scale bits 0x42855555 and the 128-byte font path record. Both profiler entrypoints immediately test that configuration before touching a profile name, entry, global graph storage or clock. Their enabled branches are unreachable in this executable; the source retains the configuration call and returns NULL/no effect accordingly. These reduced bodies remain STUB because their raw similarity is low. This is based on the unconditional configuration write and both branch instructions, rather than assuming profiling is unnecessary.

Component interpolation (0x004aa7b0) uses the by-type registry, snapshots its pointer/count, and calls the position/orientation getters without arguments. Pushes across those getters stage arguments for later lerp or object-transform calls. A zero or NaN interpolation delta selects the direct transform path; an ordered nonzero delta interpolates into separate position/quaternion outputs. Flags are reread after the component's transform-output callback.

## Object-only compiled work

The following units have passed VC6 object compilation but are deliberately absent from sources.txt pending genuine linkage dependencies:

- `gameentry.cpp`: actual WinMain policy at 0x004c3040, single-instance mutex, application/window/platform setup, message policy and frame/screenshot dispatch.
- `gamewindow.cpp`: window procedure, close/destroy policy, shutdown and private audio notification.
- `platformshutdown.cpp`: the actual heap, registry, audio, controller, input and renderer teardown order.
- `gameframe.cpp`: initialization, lazy scheduler, simulation passes, object updates, audio update, global pause update and frame rendering in retail order.
- `globalpause.cpp`: the Global Pause Mode object's observers, input/controller pause and audio synchronization, clock/deadline helpers. Its constructor and table-backed global storage still need recovery.
- `gamemovement.cpp`: physical object prepasses, contact service calls and four repeated category-1 update sweeps. The movement sequence starts at the PE's 0x40000000, not zero. The original tracing target 0x00489970 is a bare RET 8 with no effects.
- `collisionproxy.cpp`: lifecycle filter selection and root/child proxy transforms. Spatial-index registration/update methods remain genuine unresolved dependencies.

The frame routine still needs complete contact solving/cache ownership, the real RenderFrame body and audio service. Screenshot capture now links; its surface cleanup remains under review. GameObject base initialization, full emitted base dispatch and concrete scene/menu objects remain in recovery. The temporary entrypoint is unchanged.

## Callback evidence corrections

The 20-entry base table at `0x00535240` ends at offset `+0x4c`, immediately before separate float data at `+0x50`. A direct PE read establishes return-zero/no-argument slots +0x20,+0x24,+0x38,+0x40; no-op slots +0x28,+0x2c; visibility/draw behavior at +0x30; a one-argument no-op at +0x34; a two-argument no-op at +0x3c; return-zero/two-argument slot +0x44; and the one-argument region-selection helper at +0x48; and one-argument no-op at +0x4c. Earlier inferred offsets for these slots were wrong and were corrected before table emission. The lifecycle component table is a separate type: its +0x00 predicate takes one argument, and its +0x08 object field is a collision-proxy pointer, not a game-object flag word.


The collision proxy constructor now has source for the evidenced 0x98-byte object: identity transform, 0x34-byte index link, recursive subordinate proxies and model-derived ordering weight. Its destructor frees children in reverse order before detaching the link. The 0x10-byte spatial owner and its registration/update facade also compile as objects. Allocation/list/grid/axis backend implementations remain unresolved; none is replaced by a no-effect or arbitrary-success body. Collision service setup and its global selectors are owned by the object-service recovery unit.
