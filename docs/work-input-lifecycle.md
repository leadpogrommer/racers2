# Input device service lifecycle

`InputDeviceService` stores its DirectInput interface at `+0x04`, the number of
records at `+0x08`, and record pointers beginning at `+0x0c`. The three methods
below use the existing game dispatch view for each record. Retail vtable calls
confirm `readState` at slot `+0x00`, `performAction` at `+0x18`, `cleanup` at
`+0x20`, and `isActive` at `+0x24`. The mouse and keyboard tables at `0x535928`
and `0x53598c` use `403910` for `isActive`, `403930` for `performAction`, and
`4038d0` for `cleanup`; these are shared game callbacks, not retail-address
function pointers to copy into rebuilt tables.

`PollDevices` (`0x004af000`, 74 bytes through the `ret 4`) walks the records
with a signed count comparison. For each record it calls `isActive()` and, only
when true, `performAction(p_inputState)`. It returns `TRUE` whether the list is
empty or after completing the loop. Current keyboard and mouse tables report
inactive and have a no-op action callback, while other device tables may use
these slots.
The isolated build scores 83.33% raw. The extra instructions clear the unused
EDX register required by the typed fastcall dispatch view; the original member
callbacks use ECX plus stack arguments and do not require that clear. The loop
retains the live signed count and reloads the record before the second call,
matching retail callback-mutation behavior.

`Shutdown` (`0x004af050`, 59 bytes through the final `ret`) walks the list with
an unsigned comparison, calls `cleanup()` for each record, then frees that
record through the pool free routine. After the loop it calls `Release()` on
the service's DirectInput interface. Retail does not clear the list, count, or
interface fields here, and the reconstruction preserves those writes as
absent. The function also does not test the interface pointer before release.
The isolated build scores 91.53% raw. The typed cleanup callback clears unused
EDX before dispatch; the remaining branch displacement differences follow
from that extra instruction. The record pointer is reloaded after cleanup
before freeing, as in retail.

`RefreshDevices` (`0x004af170`, 47 bytes through the final `ret`) walks the
same list with an unsigned comparison. It checks the record's `+0x04` flag and
calls `readState(1)` only when nonzero; it does not change the flag or return
the callback result. This is distinct from the `PollDevices` predicate/action
pair.
The isolated build scores 86.27% raw; the added instruction clears unused EDX
for the typed callback view. The source preserves the availability guard and
passes the literal value `1`.

All three implementations remain `STUB`-annotated because each raw score is
below the initial 95% threshold, despite the reviewed control flow and callback
arguments. No behavioral tests or game execution were performed.

## Joystick-enumeration closure

The enumeration callback `0x004edb50` receives the DirectInput device-instance
record and a context parameter. It allocates `0x2124` bytes through the game
pool, constructs a joystick object with `0x004ed8c0`, and initializes it from
the device-instance data through `0x004ed9a0`. If initialization succeeds it
registers the joystick with the service at `0x00653080` by calling
`0x004af090`, then returns `TRUE`. If initialization fails after allocation,
it invokes the constructor on all 32 embedded `0xe4`-stride records, frees the
object, and also returns `TRUE`. If the initial allocation fails, the callback
still calls the initializer with a null receiver; that path is not a safe
successful result.

The subsequent [service recovery](work-input-service.md) implements this
callback and its joystick dependencies. At the lifecycle checkpoint, these
were still open: The
joystick constructor installs a retail vtable at `0x00536e70`; its initialization
uses DirectInput COM calls, game property setup, and its own record callbacks.
The initializer at `0x004aed30` also creates keyboard and mouse devices,
sets device properties, enumerates class `4`, registers records, and acquires
available devices. Its full extent is 678 bytes (`0x004aed30–0x004aefd5`,
including `ret`). It also depends on the initialized application window/module
globals, DirectInput8 creation, device-record registration and property
helpers, keyboard/mouse tables, the external device-selection service at
`0x00549e80`, and the joystick callback closure above. The recovered service now includes those dependencies and rebuilt typed
tables, with runtime validation still deferred until the menu milestone. No retail vtable address
or fabricated success callback is used here. The static service pointer at
`0x00653080` is not constructed by these lifecycle methods. The callback
`0x004edb50` spans 121 bytes through its full `ret 8`; constructor
`0x004ed8c0` spans 118 bytes, and its device initializer `0x004ed9a0` spans 428
bytes.
