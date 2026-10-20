# Mouse and keyboard dispatch wiring

The mouse table at `0x00535928` and keyboard table at `0x0053598c` each
contain 25 pointers (`0x64` bytes). The following word at `0x005359f0` is a
floating-point constant, not another callback. `inputdispatch.h` records the
complete game table layout; the DirectInput COM interfaces remain the real
SDK types from `directx8.h`.

`inputruntime.cpp` emits both complete tables. Recovered member bodies use
bridges with the receiver in ECX, EDX unused, and explicit arguments on the
stack. Shared retail no-op and false callbacks reuse the existing application
leaf targets: `0x004038d0`, `0x004038e0`, `0x00403910`, and `0x00403930`.
The no-op targets leave EAX unspecified; their table entries have void return
types. In particular, the keyboard and mouse cleanup slot at `+0x20` is a
no-op in retail, not an inferred COM release.

The three-argument no-op at `0x004aefe0` is exactly a `ret 12`; the keyboard's
axis callback at `0x004aeff0` returns floating-point zero with `ret 4`.
Both measure 100% under the pinned SP5 toolchain in both build paths and are
marked `FUNCTION`. They are real retail targets, not placeholder success
callbacks. `GetButtonValue` measures 95.24%; its only extra instruction clears
EDX for the explicit ABI bridge. The callback contract ignores EDX, and the
retail predicate call, exact-one test, float constants and stack cleanup are
preserved. It is a reviewed near match.

The constructors are recovered from the inlined setup in `0x004aed30`.
They write the rebuilt table pointer, clear availability, and set device type
to 1 for keyboard or 2 for mouse. They leave the other fields uninitialized
until setup and state reads, matching retail. The observed allocation sizes
are `0x1bc` and `0xcc`, respectively; compile-time assertions verify both.
The record prefix is `0xbc` bytes, including the real `DIDEVCAPS` at `+0x90`.
Its `dwButtons` member is at record offset `+0xa0`.

`reccmp/input-dispatch.csv` records every original target and rebuilt bridge
symbol. Run `python tools/check_input_dispatch.py` (or pass
`--build-dir build/cmake`) to read both PEs and the linker map, verify the
original SHA-256, and check all 50 targets for these two tables, plus the
subsequently recovered joystick and service tables. Both builds pass. This
checks table wiring separately from instruction
matching; the bridge tables are not claimed as exact compiler-emitted C++
vtables. The existing application verifier still passes all 76 pointers.

The subsequent [service and joystick recovery](work-input-service.md) closes
the initializer at `0x004aed30`, its game-controller callback `0x004edb50`,
embedded records and complete joystick dispatch table. The verifier now checks
80 pointers across all three device tables and the five-slot service table.
The temporary bootstrap still does not invoke input setup.
No game or probe was run for this batch. See [setup](work-input-setup.md),
[queries](work-input-queries.md), and [lifecycle](work-input-lifecycle.md) for
the recovered bodies and remaining dependencies.
