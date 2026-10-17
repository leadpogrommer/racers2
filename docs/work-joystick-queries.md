# Joystick query and state methods

The retail joystick dispatch table at `0x00536e70` has query/state entries at
slots 0 through 3: `ReadState` (`0x004edd50`), `IsButtonPressed`
(`0x004ee020`), the shared `GetButtonValue` wrapper (`0x004ee050`), and
`GetAxisValue` (`0x004ee080`). Slot 5 returns the configured axis count
(`0x004edf10`), and slot 9 reports whether the force-feedback axis count is
positive (`0x004ed940`). Table contents were read as 25 little-endian pointers
from the retail PE at `0x00536e70`; the complete table wiring is checked by the
input dispatch verifier.

`ReadState` refreshes the 44-byte `DIDEVCAPS` record and reads an 80-byte
`DIJOYSTATE` to `m_state` at `+0x20c4`. If capability retrieval fails or the
attached bit is absent, it clears the state. When both polled-device flags
are present, a failed first Poll triggers Acquire and a second Poll; failure
of either retry exits without clearing the state. A failed GetDeviceState
triggers Acquire and one state-read retry, again preserving the state on
Acquire failure. `ReadState` always invokes `UpdateAutoRange` and returns
true. The state
contains eight signed axis
values, four POV values, and 32 button bytes; this is why the button query
reads `rgbButtons` beginning at `+0x20f4`.

The button query rejects indices greater than or equal to
`DIDEVCAPS.dwButtons`, then treats any nonzero state byte as pressed. The axis
query takes a signed index, rejects negative values and values greater than
or equal to `m_axisCount`, and otherwise delegates to the indexed
`JoystickAxis::GetNormalizedValue`. The count getter returns `m_axisCount`;
the force-feedback predicate separately tests
`m_forceFeedbackAxisCount > 0`.

`Acquire` calls DirectInput `Acquire`, then sets `m_isAcquired` regardless of
the HRESULT, calls `CheckDirectInputResult` with the retail message text, and
returns whether the call succeeded. `Unacquire` clears the flag before
calling DirectInput `Unacquire`, performs the same result check, and returns
the status. `UpdateAutoRange` waits for a
nonzero value in one of the eight axis fields, marks auto-range initialized,
and expands axes starting at index 1 only when `dwAxes > 2` and the configured
axis count exceeds one. Axis normalization and range expansion themselves
remain in the `JoystickAxis` unit.

The two DirectInput diagnostic strings were read from the retail PE at
`0x0054b158` and `0x0054b174`; the C++ source uses their text instead of
hard-coded pointers. Retail extents including the final return instruction
are: `ReadState`, 369
bytes; `IsButtonPressed`, 40; `GetAxisValue`, 51; `GetAxisCount`, 7;
`HasForceFeedbackAxes`, 14; `Acquire`, 72; `Unacquire`, 66; and
`UpdateAutoRange`, 154. Isolated VC6/reccmp scores are 100% for the button
query, axis query, axis count getter, and force-feedback predicate; Acquire
matches at 95.24%, and Unacquire at 95.00%. The latter two retain the exact
HRESULT checks, flag-write order, and extracted diagnostic text; their scored
differences are string encoding/code-generation details. These six methods
are `FUNCTION`. `ReadState` (54.72%) and `UpdateAutoRange` (42.42%) remain
`STUB` while their code shapes differ substantially.

Final combined builds measure all six FUNCTION methods at 100%. Mapping the
extracted diagnostic strings resolves the earlier Acquire/Unacquire comparison
differences; no instruction-tuning change was needed. ReadState and
UpdateAutoRange retain the isolated scores above.
