# DirectInput service and joystick initialization

The input service now has a complete source dependency closure: the five-slot
service dispatch table, keyboard/mouse/joystick tables, device enumeration,
axis binding and normalization, and the seven force-feedback effect kinds.
This is a source and binary-wiring milestone. The temporary bootstrap still
does not call the initializer, create a window or render the menu. No game or
probe was executed during this recovery.

`InputDeviceService::Initialize` (`0x004aed30`, 678 bytes) activates the main
window and creates DirectInput 8 with the real SDK IID. It constructs and
registers keyboard and mouse wrappers, sets their cooperative levels and data
formats, enumerates mouse objects and attached game controllers, enables the
registered devices, and acquires them. Only the initial `DirectInput8Create`
failure returns false; later HRESULTs go through the recovered result helper.
The source retains the unchecked allocation/device failure behavior.

The service object at `0x00653080` is `0x94` bytes. Its static initializer at
`0x004aece0` writes the dispatch pointer, null DirectInput pointer and zero
device count. The remaining fields start in zero-initialized storage. The
selector pointer at `0x00549e80` refers to this service. The constructor and
all five methods are now connected through rebuilt callbacks; startup ordering
must provide the window and module handles before calling `Initialize`.

`JoystickInputDevice` has the observed `0x2124` allocation size. It contains
the common `0xbc`-byte prefix, 32 effect records (`0xe4` bytes each), 32 axis
records (`0x1c` each), two counts, the `0x50`-byte `DIJOYSTATE`, and four flags.
Compile-time assertions cover every boundary. Its constructor initializes
the observed fields and final table; the transient base purecall table has no
dispatch during the leaf member constructors and is not retained.

The joystick initializer at `0x004ed9a0` creates the selected GUID instance,
sets the joystick data format and exclusive/foreground cooperation, queries
capabilities, acquires the device and counts force-feedback actuators. It binds
six state axes and initializes effects only for `DIDC_FORCEFEEDBACK`. The
enumeration callback at `0x004edb50` uses the actual pool and service, preserving
the original null-allocation and failed-initialization paths. Shutdown releases
effects, restores autocenter, unacquires/releases the device and clears the
same retail flags and counts.

The effect kinds are constant force, friction, spring, sine, square, enveloped
constant and enveloped sine. They use the real DirectInput COM interfaces and
GUID library. Kind four's descriptor helper (`0x00473140`) really returns NULL;
the reconstructed call passes it to DirectInput, as retail does. Its three-byte
body folds with the existing zero-return leaf in the rebuild. This is a genuine
retail unsupported path, not a newly substituted descriptor.

Descriptor builders retain separate persistent workspaces and first-use
initialization flags. In particular, spring/friction axis counts are set on
first use only. Constant-force builders preserve the unusual upper-clamp test
that reads Y while assigning X. The enveloped-constant builder initializes an
envelope but leaves `lpEnvelope` null; enveloped sine attaches its envelope.
Small x87 `FISTP` primitives preserve the active rounding mode and float
store/reload boundaries before integer conversion. These builders remain
below the match threshold; their instruction scores are recorded separately
from the behavior review. No byte blobs or retail function pointers are used.

The linked SDK keyboard and joystick formats match retail directly. Wine's
current mouse format uses `GUID_Button` where retail has null button GUID
filters. `inputformats.cpp` expresses retail's seven entries using the real
SDK types, GUIDs, offsets and flags. `tools/check_input_formats.py` checks all
three headers and all 307 object entries, comparing GUID contents rather than
addresses. `tools/check_input_dispatch.py` checks all 80 original targets and
rebuilt pointers across the three device tables and service table. The
application table checker remains separate.

See [axis recovery](work-joystick-axis.md), [effect records](work-joystick-effect.md),
[periodic parameters](work-joystick-periodic.md), and
[state and queries](work-joystick-queries.md). Final combined measurements are
listed in [progress](progress.md). Renderer construction/initialization, audio,
the real entry path and the main loop still block a rendered menu.

Final Python/SP5 and CMake/NMake verification agrees on all 253 measured
functions: 150 exact, four reviewed near matches and 99 STUB bodies. This batch
adds 58 measured rows (33 exact, 25 STUB), plus the inventory-only folded
null helper. All 64 annotated globals match with zero issues in both builds.
Service Initialize measures 84.53%; joystick Initialize measures 67.47%.
The four nonperiodic parameter builders measure 3.30%, 5.71%, 5.71% and 2.63%;
they retain STUB annotations despite the behavior review and complete call
closure. Both builds pass the match guard, input/application dispatch checks
and data-format verification. The 13 existing tooling tests pass.
