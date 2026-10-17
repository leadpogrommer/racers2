# DirectInput service groundwork

The global pointer at `0x00549e80` points to a zero-initialized input service
object at `0x00653080`. Its retail constructor, `0x004aece0`, installs the
table at `0x00535914` and clears the fields at `+0x04` and `+0x08`. The table
contains the initializer at `+0x04` (`0x004aed30`), destructor at `+0x08`
(`0x004af050`), and device-list operations at `+0x0c` and `+0x10`. The
readiness gate at `0x004f6310` calls the `+0x04` method and proceeds only when
it returns nonzero.

The initializer is a larger DirectInput path. It calls `DirectInput8Create`
with version `0x0800`, creates the system keyboard and mouse devices, sets
their cooperative levels and data formats, allocates device wrappers through
the game's heap, registers them, and returns success only after setup. The
wrapper types have retail vtables at `0x00535928` and `0x0053598c`; their
shared fields include an availability DWORD at `+0x04`, type at `+0x08`,
product name at `+0x0c`, and `IDirectInputDevice8*` at `+0x8c`. The common
record prefix ends at `+0xbc`. The retail initializer allocates the type-2
wrapper with vtable `0x00535928` using `0xcc` bytes, and the type-1 wrapper
with vtable `0x0053598c` using `0x1bc` bytes. The `0x10` state reader at
`0x004ec470` is therefore the mouse reader; the `0x100` reader at `0x004ec590`
is keyboard state. These sizes support distinct mouse and keyboard projections
over the common prefix without asserting additional members after the observed
state buffers.

This batch recovers two closed list operations. `AcquireAvailableDevices`
walks the registered pointers and calls the real DirectInput `Acquire` method
when a record's availability word is nonzero. It saves each HRESULT to the
observed global at `0x00653114`. `FindDeviceByType(index, type)` returns the
pointer for the zero-based matching type occurrence, or null when absent. Its
caller at `0x004aed30` passes the per-type index before the type value. The
retail extents are 58 bytes (`0x004af130–0x004af169`) and 62 bytes
(`0x004af1a0–0x004af1dd`), including the complete return instructions.

`MouseInputDevice::ReadState` at `0x004ec470` asks DirectInput for a 16-byte
mouse state, retries after `Acquire` when `GetDeviceState` fails, stores each
HRESULT at `0x006e4554`, and returns true on every branch. Its retail extent is
89 bytes (`0x004ec470–0x004ec4c8`), and the isolated raw match is 98.46%; the
remaining difference is compiler branch layout and return placement. It is
marked `FUNCTION`.

`KeyboardInputDevice::ReadState` at `0x004ec590` requests all 256 keyboard
state bytes and follows the same acquire-and-retry pattern, using HRESULT
global `0x006e455c`. When the acquire attempt fails, retail clears all 256
state bytes; if retrying `GetDeviceState` fails, it leaves the buffer as-is.
It returns true on every branch. The function extent is 149 bytes
(`0x004ec590–0x004ec624`). The source captures this behavior, but measures
71.58% because the compiler emits different stack setup and zero-fill code;
it remains `STUB` under the current match threshold.

The isolated build measures `AcquireAvailableDevices` at 100% and
`FindDeviceByType` at 59.70%. The latter's explicit loop is semantically
equivalent to the retail array scan, but its compiled loop shape is not close
enough to promote under the current annotation policy, so it remains `STUB`.

The initial DirectInput setup and both specialized wrapper vtables are not
implemented in this batch. The service and state-reader operations therefore
provide behavior for a fully constructed input manager but do not by themselves
make the WinMain readiness gate pass. The code uses the real DirectInput 8
declarations from `util/directx8.h`; it does not define substitute COM
interfaces or wrapper vtables. Runtime execution is reserved for a larger
startup/rendering milestone.
