# Application virtual ABI research

This note expands the singleton evidence in [work-application.md](work-application.md)
for the application vtables, their observed slot contracts, and the limits on
the object layout. Names such as `Application` and `VTable0xNN` are working
labels, not recovered source symbols.

## Table extents and transition

The constructor at `0x0049a610` installs `0x00534994` at `this+0`. The two
tables are adjacent in the retail `.rdata` image:

| Table | Start | Extent | Evidence |
| --- | ---: | ---: | --- |
| Primary | `0x00534994` | 38 DWORD slots (`0x98` bytes), offsets `+0x00..+0x94` | The second table begins at `0x00534a2c`, exactly `0x98` bytes later. The constructor writes this address as the object's vptr. |
| Destructor/exit-state | `0x00534a2c` | 38 DWORD slots (`0x98` bytes), offsets `+0x00..+0x94` | The next DWORD at `0x00534ac4` is data read as a float by primary slot `+0x0c`; it is not another function slot. Both the scalar deleting destructor and the atexit callback write this table address to the object's vptr. |

The table pairs and observed calling contracts are:

| Slot | Primary target | Destructor/exit target | Observed ABI and behavior |
| ---: | ---: | ---: | --- |
| `+0x00` | `0x0049b5d0` | `0x004038d0` | `void()`. Primary loads display configuration; teardown target is a one-byte no-op. |
| `+0x04` | `0x0049b780` | `0x004038d0` | `void()`. Primary saves display configuration; teardown target is a no-op. |
| `+0x08` | `0x0049a6b0` | `0x0049a6b0` | Scalar deleting destructor shape: returns `this`, accepts one 32-bit flags argument, tests bit 0, installs the second table, and frees `this` through `0x004a41b0` only when bit 0 is set. `ret 4`. |
| `+0x0c` | `0x0049adb0` | `0x004ba130` | `LegoFloat()`. Primary returns the float at `0x00534ac4` (`0.90100002f`); teardown target returns `0.0f`. Meaning is unknown. |
| `+0x10` | `0x004038d0` | `0x004038d0` | No-op, no stack arguments. |
| `+0x14` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x18` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x1c` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x20` | `0x0049a640` | `0x0049a640` | Returns one, accepts one pointer-sized argument (`ret 4`). WinMain supplies `MSG*` and dispatches the message when the result is nonzero. |
| `+0x24` | `0x00403920` | `0x00403920` | Returns one, no stack arguments. WinMain uses this result to choose blocking versus polling message retrieval. |
| `+0x28` | `0x004038d0` | `0x004038d0` | No-op, no stack arguments. |
| `+0x2c` | `0x0049a7d0` | `0x0049a6d0` | Returns `const LegoChar*`, no stack arguments. Primary returns `"LEGO Racers 2"`; teardown state returns `"NextGen Game"`. WinMain uses the primary result for `FindWindowA`. |
| `+0x30` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x34` | `0x0049a6a0` | `0x00403910` | Primary returns the DWORD at `this+0x10`; teardown returns zero. Field meaning is not established. |
| `+0x38` | `0x00493550` | `0x0049a6e0` | `LegoBool32(HWND, UINT, WPARAM, LPARAM)`, `ret 16`. Primary intercepts selected `WM_CHAR` messages and otherwise returns one. The WndProc at `0x004c32c0` passes the four original window-message arguments in this order and returns zero immediately if this slot returns zero. Teardown target returns one for every message. |
| `+0x3c` | `0x0049a7f0` | `0x00403910` | Primary returns `101`; teardown returns zero. Meaning unknown. |
| `+0x40` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x44` | `0x00479030` | `0x004038d0` | No-op, no stack arguments. Startup calls this primary slot without arguments. |
| `+0x48` | `0x00479030` | `0x004038d0` | No-op, no stack arguments. Startup calls this primary slot without arguments. |
| `+0x4c` | `0x0049a7e0` | `0x0049a6f0` | Returns `0x01900000` (25 MiB) in the primary table, `0x01c00000` (28 MiB) in teardown state. The primary value is used as the application's heap reserve policy. |
| `+0x50` | `0x0049a7b0` | `0x0049a700` | Returns `0x6d60` in the primary table, `0x2000` in teardown state. Meaning unknown. |
| `+0x54` | `0x00403920` | `0x00403920` | Returns one, no stack arguments. |
| `+0x58` | `0x004038d0` | `0x004038d0` | No-op, no stack arguments. |
| `+0x5c` | `0x0049a650` | `0x0049a650` | Returns `0xffff`, no stack arguments. A private-message path uses this slot on another object; that does not establish its Application meaning. |
| `+0x60` | `0x0049a660` | `0x0049a660` | Returns `0x8000`, no stack arguments. Meaning unknown. |
| `+0x64` | `0x0049a7c0` | `0x0049a710` | Returns `1500` in the primary table, `1024` in teardown state. Meaning unknown. |
| `+0x68` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x6c` | `0x00403910` | `0x00403910` | Returns zero, no stack arguments. |
| `+0x70` | `0x0049a690` | `0x0049a720` | Returns `30` in the primary table, `60` in teardown state. Slot `+0x78` computes the reciprocal of this result. The exact quantity is not named by observed call sites. |
| `+0x74` | `0x00403920` | `0x0049a730` | Returns one in the primary table, `2` in teardown state. WinMain treats the primary result as a Boolean readiness/loop policy. |
| `+0x78` | `0x0049a670` | `0x0049a670` | Returns floating-point `1.0f / slot(+0x70)`, no stack arguments. |
| `+0x7c` | `0x00473140` | `0x00403910` | Returns zero in both implementations, no stack arguments. |
| `+0x80` | `0x0040ec90` | `0x0040ec90` | Returns `this+4`, no stack arguments. This is the pointer to the three-word display-settings prefix. |
| `+0x84` | `0x0049b5a0` | `0x0049a740` | Accepts `(const ApplicationDisplaySettings*, LegoBool32)` and returns void (`ret 8`). Primary copies the three words to `this+4` and calls slot `+0x04` when the second argument is nonzero. Teardown implementation copies the values without that save call. |
| `+0x88` | `0x00403930` | `0x00403930` | No-op, one 32-bit stack argument (`ret 4`). Return register is not set by the target. |
| `+0x8c` | `0x004038e0` | `0x004038e0` | No-op, two 32-bit stack arguments (`ret 8`). Return register is not set by the target. |
| `+0x90` | `0x0049a800` | `0x004038d0` | Primary calls `DialogBoxParamA` for resource `0x66`, owner `NULL`, callback `0x0049a820`, and init parameter `NULL`; returns the dialog result. No explicit stack arguments. Teardown target is a no-op. |
| `+0x94` | `0x00403910` | `0x00403920` | Returns zero in the primary table and one in teardown state, no stack arguments. |

All rows are `__thiscall`-shaped entries: `ECX` is the receiver and any
explicit arguments are stack arguments cleaned by the target. For the
shared no-op targets, the return register is not always defined; the table
records the observed return/cleanup contract rather than assigning a
semantic result to those slots.

## Object extent and class-model limit

The constructor writes the vptr plus fields at `+0x04`, `+0x08`, `+0x0c`, and
`+0x10`; the getter at `0x0049a6a0` reads `+0x10`; the display accessor and
copy routines cover `+0x04..+0x0f`. This proves only a minimum object size of
`0x14` bytes. No observed application method reads or writes beyond `+0x10`.

The static object is at `0x00578648`, in the zero-initialized virtual tail of
`.data`. The atexit guard at `0x00578774` is `0x12c` bytes later, but that is
not an object-size proof. There are separate save-config globals between
these addresses, including the record at `0x00578760`; absolute global
references do not make those bytes application members. No allocation of the
static singleton supplies a size, and the optional delete branch in
`0x0049a6b0` calls a generic free routine without a size. Keep the exact class
size unknown; do not add `DECOMP_SIZE_ASSERT` from adjacency.

Two tables with the same 38-slot extent, plus the transition from the primary
table to the second table in the deleting destructor, are strong evidence for
a base/derived or equivalent destruction-state relationship. The title and
message-handler overrides also change back to generic-looking implementations
in the second table. This does not identify a base-class name or external
library, and the atexit callback only changes the vptr rather than calling a
visible destructor. Until a C++ inheritance hierarchy can be validated against
both generated tables, prefer an explicit complete callable ABI/table over a
partial list of C++ virtual declarations. Use neutral slot names where no
behavioral name is justified; map unknown actions to the actual recovered
retail target, not a made-up success callback.

## Construction and minimum startup dependencies

The lazy accessor `0x0049a760` sets its guard before calling constructor
`0x0049a610` on static object `0x00578648`. The constructor initializes the
display fields and vptr, then calls load-config target `0x0049b5d0` directly;
that config routine does not call the accessor, so this path does not recurse
into singleton construction. After construction, the accessor registers
`0x0049a7a0` with the CRT atexit routine `0x00519f5e`; that callback writes
the second vtable address to the singleton and performs no other observed
cleanup.

Load-config resets display fields, obtains the base directory through
`BuildUserDataPath(path, FALSE)`, then appends `Saved Games\\LR2.cfg`. It
checks for the file and, when present, opens and validates a 16-byte display
record before applying width, height, and depth. The paired save-config slot
uses the same path and writes the marker plus three display words. The path
helper and config existence/open/read/write bodies are now in the build,
though several remain below the matching threshold. The constructor,
accessor, exit callback, and both explicit 38-slot tables are also in the
runtime build. `tools/check_application_dispatch.py` verifies all 76 callback
pointers against the retail table targets in each PE. This is binary wiring
verification, not an exact vtable or method-body match. A runtime startup
check confirms config-path access and then archive metadata initialization;
it has not reached window creation or the main-loop managers in
[startup research](work-startup.md).
