# Input query methods

The mouse and keyboard records share a 25-entry game dispatch table. In the
retail table, slot 1 points to `MouseInputDevice::IsButtonPressed` or
`KeyboardInputDevice::IsKeyPressed`, and slot 2 points to the common
`InputDeviceRecord::GetButtonValue` wrapper. Mouse slot 3 points to
`MouseInputDevice::GetAxisValue`. These functions use the input-device prefix
and state buffers documented in `inputdevice.h`, `inputmouse.h`, and
`inputkeyboard.h`.

`MouseInputDevice::IsButtonPressed` first compares the unsigned button index
against `DIDEVCAPS.dwButtons` at record offset `0xa0`. An out-of-range index
returns false; otherwise any nonzero byte in `DIMOUSESTATE.rgbButtons` returns
true. Its retail body is 40 bytes at `0x004ec4d0`.

`MouseInputDevice::GetAxisValue` maps indices 0, 1, and 2 to `lX`, `lY`, and
`lZ`; all other indices return zero. It multiplies the signed axis value by
the writable retail scale at `0x00536e5c` (`0.01568627543747425f`, bits
`0x3c808081`) and clamps the result to [-1, 1]. The lower comparison treats an
unordered x87 result like a value below -1, while the upper comparison leaves
an unordered value untouched, matching the two distinct retail condition
tests. Its retail extent is 110 bytes at `0x004ec500`.

`KeyboardInputDevice::IsKeyPressed` reads the byte at
`m_keyState[p_keyCode]` and returns its most significant bit. It performs no
range check, so the source preserves the retail direct indexed read. Its
retail body is 19 bytes at `0x004ec630`.

`InputDeviceRecord::GetButtonValue` invokes dispatch slot 1 and returns 1.0
only when the result equals exactly one; every other result returns 0.0. The
mouse and keyboard tables share this wrapper. Its retail body is 33 bytes at
`0x004ee050`.

Isolated VC6/reccmp results are 100% for both button/key predicates, 95.24%
for `GetButtonValue`, and 47.76% for `GetAxisValue`. The first three are
annotated `FUNCTION`; the axis method remains a `STUB` because its compiler
shape differs substantially even though its field selection, scaling,
clamping, and unordered comparisons were checked against the full retail body.
Exact extents above include each function's final `ret` instruction. The
dispatch declarations use the observed `__fastcall` bridges (receiver in ECX,
EDX unused, explicit argument
on the stack); no DirectInput COM vtable is substituted for the game table.
The sole instruction difference in `GetButtonValue` is the extra EDX clear
for that bridge. EDX is unused by its callback contract, so the extra clear
does not change the predicate, exact-one comparison, return value or cleanup.
