# Joystick axis state

`JoystickAxis` is the repeated axis-state record embedded in the joystick
object. Its 32-entry array begins at joystick offset `0x1d3c`, advances by
`0x1c`, and ends at `0x20bc`, where the joystick's axis-count field begins.
The constructor writes through `+0x18`, confirming the record extent. This
supports `sizeof(JoystickAxis) == 0x1c`; it does not assert the joystick's
total size.

| Offset | Field | Evidence |
| --- | --- | --- |
| `+0x00` | DirectInput device pointer | Bound from joystick `+0x8c`; used for `SetProperty`. |
| `+0x04` | Device-object byte offset | Caller passes `0,4,...,20`; used as `DIPROPHEADER.dwObj`. |
| `+0x08` | Software-range flag | Zero when DirectInput accepts the range, one on failure. |
| `+0x0c` | Pointer to raw signed state | Binding stores state-buffer pointer plus the byte offset. |
| `+0x10` | Range span | Always recomputed as `max - min`. |
| `+0x14` | Minimum | Initial range's lower bound. |
| `+0x18` | Maximum | Initial range's upper bound. |

The parent constructor at `0x004ed8c0` constructs 32 records. Its input setup
at `0x004ed950` binds six axes: it passes the raw state buffer at joystick
`+0x20c4`, byte offsets `0,4,8,12,16,20`, and the DirectInput device at
`+0x8c`. The rest of the 32 records retain their constructor state until used.

`JoystickAxis::Bind` (`0x004edc10`, 49 bytes) stores those three values and
requests a default range of `-255.0f` through `255.0f`. The pointer calculation
is byte-based because the caller supplies byte offsets. `SetRange`
(`0x004edc50`, 159 bytes) builds a 24-byte `DIPROPRANGE`, sets its header's
object identifier to the stored offset and `dwHow` to `DIPH_BYOFFSET`, then calls
`IDirectInputDevice8A::SetProperty(DIPROP_RANGE, ...)`. It records that HRESULT
at `0x006e4634`. On success it stores the original float bounds and clears the
software-range flag. On failure it rounds each bound to a signed integer using
the current x87 `FISTP` mode, stores those rounded bounds as floats, and sets
the flag. The initializer only requests integral bounds; other callers would
observe the active x87 rounding mode as well.

`GetNormalizedValue` (`0x004edcf0`, 34 bytes) returns zero when DirectInput
accepted the range. In software mode it reads the signed raw state, subtracts
the minimum, divides by the range span, doubles the result and subtracts one.
`ExpandRange` (`0x004edfc0`, 90 bytes) uses the same software normalization. If
the value exceeds `+0.5`, it extends the maximum by one current span; if the
value is not greater than `-0.5`, it lowers the minimum by one span. It updates
one side per call and then recomputes the span. The lower test preserves the
retail x87 condition-code behavior: unordered comparisons also take that path.
The joystick state updater calls this for axes 1 through
the reported axis count minus one; axis zero is deliberately omitted by that
caller.

The constructor, binding, normalized-value, and expansion methods are
`FUNCTION` matches at 100% raw instruction accuracy. `SetRange` remains
`STUB` at 53.70%; its DirectInput setup is semantically recovered, but code
generation does not yet match closely enough to claim equivalence. The normal
initializer uses the nonzero `[-255,255]` range.
